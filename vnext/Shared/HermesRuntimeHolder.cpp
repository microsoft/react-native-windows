// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"

#include "HermesRuntimeHolder.h"

#include <ApiLoaders/HermesApi.h>
#include <JSI/decorator.h>
#include <NodeApiJsiRuntime.h>
#include <crash/verifyElseCrash.h>
#include <cxxreact/SystraceSection.h>
#include <jsinspector-modern/InspectorInterfaces.h>
#include <mutex>
#include "SafeLoadLibrary.h"

#define CRASH_ON_ERROR(result) VerifyElseCrash(result == napi_ok);

namespace React {
using namespace winrt::Microsoft::ReactNative;
}

using namespace Microsoft::NodeApiJsi;

namespace Microsoft::ReactNative {

React::ReactPropertyId<React::ReactNonAbiValue<std::shared_ptr<HermesRuntimeHolder>>>
HermesRuntimeHolderProperty() noexcept {
  static React::ReactPropertyId<React::ReactNonAbiValue<std::shared_ptr<HermesRuntimeHolder>>> propId{
      L"ReactNative.HermesRuntimeHolder", L"HermesRuntimeHolder"};
  return propId;
}

namespace {

int32_t NAPI_CDECL addInspectorPage(const char *title, const char *vm, void *connectFunc) noexcept;
void NAPI_CDECL removeInspectorPage(int32_t pageId) noexcept;

class HermesFuncResolver : public IFuncResolver {
 public:
  HermesFuncResolver() : libHandle_(LoadLibraryAsPeerFirst(L"hermes.dll")) {}

  FuncPtr getFuncPtr(const char *funcName) override {
    return reinterpret_cast<FuncPtr>(GetProcAddress(libHandle_, funcName));
  }

 private:
  HMODULE libHandle_;
};

HermesApi &initHermesApi() noexcept {
  static HermesFuncResolver funcResolver;
  static HermesApi s_hermesApi(&funcResolver);
  HermesApi::setCurrent(&s_hermesApi);
  CRASH_ON_ERROR(s_hermesApi.hermes_set_inspector(&addInspectorPage, &removeInspectorPage));
  return s_hermesApi;
}

HermesApi &getHermesApi() noexcept {
  static HermesApi &s_hermesApi = initHermesApi();
  return s_hermesApi;
}

class HermesTask {
 public:
  HermesTask(
      void *taskData,
      jsr_task_run_cb taskRunCallback,
      jsr_data_delete_cb taskDataDeleteCallback,
      void *deleterData)
      : taskData_(taskData),
        taskRunCallback_(taskRunCallback),
        taskDataDeleteCallback_(taskDataDeleteCallback),
        deleterData_(deleterData) {}

  HermesTask(const HermesTask &other) = delete;
  HermesTask &operator=(const HermesTask &other) = delete;

  ~HermesTask() {
    if (taskDataDeleteCallback_ != nullptr) {
      taskDataDeleteCallback_(taskData_, deleterData_);
    }
  }

  void Run() const {
    if (taskRunCallback_ != nullptr) {
      taskRunCallback_(taskData_);
    }
  }

 private:
  void *taskData_;
  jsr_task_run_cb taskRunCallback_;
  jsr_data_delete_cb taskDataDeleteCallback_;
  void *deleterData_;
};

class HermesTaskRunner {
 public:
  static void Create(jsr_config config, std::shared_ptr<facebook::react::MessageQueueThread> queue) {
    CRASH_ON_ERROR(getHermesApi().jsr_config_set_task_runner(
        config, new HermesTaskRunner(std::move(queue)), &PostTask, &Delete, nullptr));
  }

 private:
  HermesTaskRunner(std::shared_ptr<facebook::react::MessageQueueThread> queue) : queue_(std::move(queue)) {}

  static void NAPI_CDECL PostTask(
      void *taskRunnerData,
      void *taskData,
      jsr_task_run_cb taskRunCallback,
      jsr_data_delete_cb taskDataDeleteCallback,
      void *deleterData) {
    auto task = std::make_shared<HermesTask>(taskData, taskRunCallback, taskDataDeleteCallback, deleterData);
    reinterpret_cast<HermesTaskRunner *>(taskRunnerData)->queue_->runOnQueue([task = std::move(task)] { task->Run(); });
  }

  static void NAPI_CDECL Delete(void *taskRunner, void * /*deleterData*/) {
    delete reinterpret_cast<HermesTaskRunner *>(taskRunner);
  }

 private:
  std::shared_ptr<facebook::react::MessageQueueThread> queue_;
};

struct HermesJsiBuffer : facebook::jsi::Buffer {
  static std::shared_ptr<const facebook::jsi::Buffer>
  Create(const uint8_t *buffer, size_t bufferSize, jsr_data_delete_cb bufferDeleteCallback, void *deleterData) {
    return std::shared_ptr<const facebook::jsi::Buffer>(
        new HermesJsiBuffer(buffer, bufferSize, bufferDeleteCallback, deleterData));
  }

  HermesJsiBuffer(
      const uint8_t *buffer,
      size_t bufferSize,
      jsr_data_delete_cb bufferDeleteCallback,
      void *deleterData) noexcept
      : buffer_(buffer),
        bufferSize_(bufferSize),
        bufferDeleteCallback_(bufferDeleteCallback),
        deleterData_(deleterData) {}

  ~HermesJsiBuffer() override {
    if (bufferDeleteCallback_) {
      bufferDeleteCallback_(const_cast<uint8_t *>(buffer_), deleterData_);
    }
  }

  const uint8_t *data() const override {
    return buffer_;
  }

  size_t size() const override {
    return bufferSize_;
  }

 private:
  const uint8_t *buffer_;
  size_t bufferSize_;
  jsr_data_delete_cb bufferDeleteCallback_;
  void *deleterData_;
};

class HermesScriptCache {
 public:
  static void Create(jsr_config config, std::shared_ptr<facebook::jsi::PreparedScriptStore> scriptStore) {
    CRASH_ON_ERROR(getHermesApi().jsr_config_set_script_cache(
        config, new HermesScriptCache(std::move(scriptStore)), &LoadScript, &StoreScript, &Delete, nullptr));
  }

 private:
  HermesScriptCache(std::shared_ptr<facebook::jsi::PreparedScriptStore> scriptStore)
      : scriptStore_(std::move(scriptStore)) {}

  static void NAPI_CDECL LoadScript(
      void *scriptCache,
      const char *sourceUrl,
      uint64_t sourceHash,
      const char *runtimeName,
      uint64_t runtimeVersion,
      const char *cacheTag,
      const uint8_t **buffer,
      size_t *bufferSize,
      jsr_data_delete_cb *bufferDeleteCallback,
      void **deleterData) {
    auto &scriptStore = reinterpret_cast<HermesScriptCache *>(scriptCache)->scriptStore_;
    std::shared_ptr<const facebook::jsi::Buffer> preparedScript = scriptStore->tryGetPreparedScript(
        facebook::jsi::ScriptSignature{sourceUrl, sourceHash},
        facebook::jsi::JSRuntimeSignature{runtimeName, runtimeVersion},
        cacheTag);
    if (preparedScript) {
      *buffer = preparedScript->data();
      *bufferSize = preparedScript->size();
      *bufferDeleteCallback = [](void * /*data*/, void *deleterData) noexcept {
        delete reinterpret_cast<std::shared_ptr<const facebook::jsi::Buffer> *>(deleterData);
      };
      *deleterData = new std::shared_ptr<const facebook::jsi::Buffer>(std::move(preparedScript));
    } else {
      *buffer = nullptr;
      *bufferSize = 0;
      *bufferDeleteCallback = nullptr;
      *deleterData = nullptr;
    }
  }

  static void NAPI_CDECL StoreScript(
      void *scriptCache,
      const char *sourceUrl,
      uint64_t sourceHash,
      const char *runtimeName,
      uint64_t runtimeVersion,
      const char *cacheTag,
      const uint8_t *buffer,
      size_t bufferSize,
      jsr_data_delete_cb bufferDeleteCallback,
      void *deleterData) {
    auto &scriptStore = reinterpret_cast<HermesScriptCache *>(scriptCache)->scriptStore_;
    scriptStore->persistPreparedScript(
        HermesJsiBuffer::Create(buffer, bufferSize, bufferDeleteCallback, deleterData),
        facebook::jsi::ScriptSignature{sourceUrl, sourceHash},
        facebook::jsi::JSRuntimeSignature{runtimeName, runtimeVersion},
        cacheTag);
  }

  static void NAPI_CDECL Delete(void *scriptCache, void * /*deleterData*/) {
    delete reinterpret_cast<HermesScriptCache *>(scriptCache);
  }

 private:
  std::shared_ptr<facebook::jsi::PreparedScriptStore> scriptStore_;
};

class HermesLocalConnection : public facebook::react::jsinspector_modern::ILocalConnection {
 public:
  HermesLocalConnection(
      std::unique_ptr<facebook::react::jsinspector_modern::IRemoteConnection> remoteConnection,
      void *connectFunc) noexcept {
    CRASH_ON_ERROR(getHermesApi().hermes_create_local_connection(
        connectFunc,
        reinterpret_cast<hermes_remote_connection>(remoteConnection.release()),
        &OnRemoteConnectionSendMessage,
        &OnRemoteConnectionDisconnect,
        &OnRemoteConnectionDelete,
        nullptr,
        &localConnection_));
  }

  ~HermesLocalConnection() override {
    CRASH_ON_ERROR(getHermesApi().hermes_delete_local_connection(localConnection_));
  }

  void sendMessage(std::string message) {
    CRASH_ON_ERROR(getHermesApi().hermes_local_connection_send_message(localConnection_, message.c_str()));
  }

  void disconnect() {
    CRASH_ON_ERROR(getHermesApi().hermes_local_connection_disconnect(localConnection_));
  }

 private:
  static void NAPI_CDECL OnRemoteConnectionSendMessage(hermes_remote_connection remoteConnection, const char *message) {
    reinterpret_cast<facebook::react::jsinspector_modern::IRemoteConnection *>(remoteConnection)->onMessage(message);
  }

  static void NAPI_CDECL OnRemoteConnectionDisconnect(hermes_remote_connection remoteConnection) {
    reinterpret_cast<facebook::react::jsinspector_modern::IRemoteConnection *>(remoteConnection)->onDisconnect();
  }

  static void NAPI_CDECL OnRemoteConnectionDelete(void *remoteConnection, void * /*deleterData*/) {
    delete reinterpret_cast<facebook::react::jsinspector_modern::IRemoteConnection *>(remoteConnection);
  }

 private:
  hermes_local_connection localConnection_{};
};

int32_t NAPI_CDECL addInspectorPage(const char *title, const char *vm, void *connectFunc) noexcept {
  return facebook::react::jsinspector_modern::getInspectorInstance().addPage(
      title,
      vm,
      [connectFunc, hermesApi = HermesApi::current()](
          std::unique_ptr<facebook::react::jsinspector_modern::IRemoteConnection> remoteConnection) {
        HermesApi::Scope apiScope(hermesApi);
        return std::make_unique<HermesLocalConnection>(std::move(remoteConnection), connectFunc);
      });
}

void NAPI_CDECL removeInspectorPage(int32_t pageId) noexcept {
  facebook::react::jsinspector_modern::getInspectorInstance().removePage(pageId);
}

} // namespace

HermesRuntimeHolder::HermesRuntimeHolder(
    std::shared_ptr<facebook::react::DevSettings> devSettings,
    std::shared_ptr<facebook::react::MessageQueueThread> jsQueue,
    std::shared_ptr<facebook::jsi::PreparedScriptStore> preparedScriptStore) noexcept
    : m_weakDevSettings(devSettings),
      m_jsQueue(std::move(jsQueue)),
      m_preparedScriptStore(std::move(preparedScriptStore)) {}

HermesRuntimeHolder::~HermesRuntimeHolder() {
  if (m_runtime) {
    CRASH_ON_ERROR(getHermesApi().jsr_delete_runtime(m_runtime));
  }
}

void HermesRuntimeHolder::initRuntime() noexcept {
  facebook::react::SystraceSection s("HermesExecutorFactory::makeHermesRuntimeSystraced");
  std::shared_ptr<facebook::react::DevSettings> devSettings = m_weakDevSettings.lock();
  VerifyElseCrash(devSettings);

  HermesApi &api = getHermesApi();
  HermesApi::setCurrent(&api);
  jsr_config config{};
  CRASH_ON_ERROR(api.jsr_create_config(&config));
  CRASH_ON_ERROR(api.hermes_config_enable_default_crash_handler(config, devSettings->enableDefaultCrashHandler));
  CRASH_ON_ERROR(api.jsr_config_enable_inspector(config, devSettings->useDirectDebugger));
  CRASH_ON_ERROR(api.jsr_config_set_inspector_runtime_name(config, devSettings->debuggerRuntimeName.c_str()));
  CRASH_ON_ERROR(api.jsr_config_set_inspector_port(config, devSettings->debuggerPort));
  CRASH_ON_ERROR(api.jsr_config_set_inspector_break_on_start(config, devSettings->debuggerBreakOnNextLine));
  if (m_jsQueue) {
    HermesTaskRunner::Create(config, m_jsQueue);
  }
  if (m_preparedScriptStore) {
    HermesScriptCache::Create(config, m_preparedScriptStore);
  }
  jsr_runtime runtime{};
  CRASH_ON_ERROR(api.jsr_create_runtime(config, &runtime));
  CRASH_ON_ERROR(api.jsr_delete_config(config));

  napi_env env{};
  CRASH_ON_ERROR(api.jsr_runtime_get_node_api_env(runtime, &env));

  m_jsiRuntime = makeNodeApiJsiRuntime(
      env, &api, [runtime]() { CRASH_ON_ERROR(HermesApi::current()->jsr_delete_runtime(runtime)); });
  m_ownThreadId = std::this_thread::get_id();

  // Add JS engine information to Error.prototype so in error reporting we
  // can send this information.
  auto errorPrototype = m_jsiRuntime->global()
                            .getPropertyAsObject(*m_jsiRuntime, "Error")
                            .getPropertyAsObject(*m_jsiRuntime, "prototype");
  errorPrototype.setProperty(*m_jsiRuntime, "jsEngine", "hermes");
}

facebook::react::JSIEngineOverride HermesRuntimeHolder::getRuntimeType() noexcept {
  return facebook::react::JSIEngineOverride::Hermes;
}

std::shared_ptr<facebook::jsi::Runtime> HermesRuntimeHolder::getRuntime() noexcept {
  std::call_once(m_onceFlag, [this]() { initRuntime(); });
  VerifyElseCrash(m_jsiRuntime);
  return m_jsiRuntime;
}

void HermesRuntimeHolder::crashHandler(int fileDescriptor) noexcept {
  CRASH_ON_ERROR(getHermesApi().hermes_dump_crash_data(m_runtime, fileDescriptor));
}

void HermesRuntimeHolder::teardown() noexcept {
  // TODO: (@vmoroz) Implement
}

std::shared_ptr<HermesRuntimeHolder> HermesRuntimeHolder::loadFrom(
    React::ReactPropertyBag const &propertyBag) noexcept {
  return *(propertyBag.Get(HermesRuntimeHolderProperty()));
}

void HermesRuntimeHolder::storeTo(
    React::ReactPropertyBag const &propertyBag,
    std::shared_ptr<HermesRuntimeHolder> const &holder) noexcept {
  propertyBag.Set(HermesRuntimeHolderProperty(), holder);
}

void HermesRuntimeHolder::addToProfiling() const noexcept {
  CRASH_ON_ERROR(getHermesApi().hermes_sampling_profiler_add(m_runtime));
}

void HermesRuntimeHolder::removeFromProfiling() const noexcept {
  CRASH_ON_ERROR(getHermesApi().hermes_sampling_profiler_remove(m_runtime));
}

/*static*/ void HermesRuntimeHolder::enableSamplingProfiler() noexcept {
  CRASH_ON_ERROR(getHermesApi().hermes_sampling_profiler_enable());
}

/*static*/ void HermesRuntimeHolder::disableSamplingProfiler() noexcept {
  CRASH_ON_ERROR(getHermesApi().hermes_sampling_profiler_disable());
}

/*static*/ void HermesRuntimeHolder::dumpSampledTraceToFile(const std::string &fileName) noexcept {
  CRASH_ON_ERROR(getHermesApi().hermes_sampling_profiler_dump_to_file(fileName.c_str()));
}

} // namespace Microsoft::ReactNative
