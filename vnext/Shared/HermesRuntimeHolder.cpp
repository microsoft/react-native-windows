// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"

#include "HermesRuntimeHolder.h"

#include <ApiLoaders/HermesApi.h>
#include <JSI/decorator.h>
#include <NodeApiJsiRuntime.h>
#include <crash/verifyElseCrash.h>
#include <cxxreact/TraceSection.h>
#include <jsinspector-modern/ConsoleMessage.h>
#include <jsinspector-modern/InspectorInterfaces.h>
#include <jsinspector-modern/tracing/InstanceTracingProfile.h>
#include <react/featureflags/ReactNativeFeatureFlags.h>
#include <cstdio>
#include <mutex>
#include "Hermes/HermesRuntimeTargetDelegate.h"
#include "SafeLoadLibrary.h"

#define CRASH_ON_ERROR(result) VerifyElseCrash(result == napi_ok);

namespace React {
using namespace winrt::Microsoft::ReactNative;
}

using namespace Microsoft::NodeApiJsi;

namespace Microsoft::ReactNative {

/*static*/ const hermes_inspector_vtable *HermesInspectorApi::vtable = nullptr;

void setHermesInspectorVTable(const hermes_inspector_vtable *vtable) {
  HermesInspectorApi::vtable = vtable;
}

React::ReactPropertyId<React::ReactNonAbiValue<std::shared_ptr<HermesRuntimeHolder>>>
HermesRuntimeHolderProperty() noexcept {
  static React::ReactPropertyId<React::ReactNonAbiValue<std::shared_ptr<HermesRuntimeHolder>>> propId{
      L"ReactNative.HermesRuntimeHolder", L"HermesRuntimeHolder"};
  return propId;
}

namespace {

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
  const hermes_inspector_vtable *inspectorVTable{};
  s_hermesApi.hermes_get_inspector_vtable(&inspectorVTable);
  setHermesInspectorVTable(inspectorVTable);
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

} // namespace

//==============================================================================
// HermesRuntimeHolder implementation
//==============================================================================

HermesRuntimeHolder::HermesRuntimeHolder(
    std::shared_ptr<facebook::react::DevSettings> devSettings,
    std::shared_ptr<facebook::react::MessageQueueThread> jsQueue,
    std::shared_ptr<facebook::jsi::PreparedScriptStore> preparedScriptStore) noexcept
    : m_weakDevSettings(devSettings),
      m_jsQueue(std::move(jsQueue)),
      m_preparedScriptStore(std::move(preparedScriptStore)) {}

HermesRuntimeHolder::~HermesRuntimeHolder() {
  // if (m_runtime) {
  //   CRASH_ON_ERROR(getHermesApi().jsr_delete_runtime(m_runtime));
  // }
}

void HermesRuntimeHolder::initRuntime() noexcept {
  facebook::react::TraceSection s("HermesExecutorFactory::makeHermesRuntimeSystraced");
  std::shared_ptr<facebook::react::DevSettings> devSettings = m_weakDevSettings.lock();
  VerifyElseCrash(devSettings);

  HermesApi &api = getHermesApi();
  HermesApi::setCurrent(&api);
  jsr_config config{};
  CRASH_ON_ERROR(api.jsr_create_config(&config));
  CRASH_ON_ERROR(api.hermes_config_enable_default_crash_handler(config, devSettings->enableDefaultCrashHandler));
  CRASH_ON_ERROR(api.jsr_config_enable_inspector(config, devSettings->useDirectDebugger));
  CRASH_ON_ERROR(api.jsr_config_set_explicit_microtasks(
      config, facebook::react::ReactNativeFeatureFlags::enableBridgelessArchitecture()));

  if (m_jsQueue) {
    HermesTaskRunner::Create(config, m_jsQueue);
  }
  if (m_preparedScriptStore) {
    HermesScriptCache::Create(config, m_preparedScriptStore);
  }
  jsr_runtime runtime{};
  CRASH_ON_ERROR(api.jsr_create_runtime(config, &runtime));
  CRASH_ON_ERROR(api.jsr_delete_config(config));
  m_runtime = runtime;

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

std::shared_ptr<facebook::react::jsinspector_modern::RuntimeTargetDelegate>
HermesRuntimeHolder::createRuntimeTargetDelegate() {
  return std::make_shared<Microsoft::ReactNative::HermesRuntimeTargetDelegate>(shared_from_this());
}

void HermesRuntimeHolder::crashHandler(int fileDescriptor) noexcept {
  CRASH_ON_ERROR(getHermesApi().hermes_dump_crash_data(m_runtime, fileDescriptor));
}

hermes_runtime HermesRuntimeHolder::getHermesRuntime() noexcept {
  return reinterpret_cast<hermes_runtime>(m_runtime);
}

//==============================================================================
// HermesJSRuntime implementation
//==============================================================================

HermesJSRuntime::HermesJSRuntime(std::shared_ptr<Microsoft::JSI::RuntimeHolderLazyInit> hermesRuntimeHolder)
    : m_holder(std::move(hermesRuntimeHolder)) {}

facebook::jsi::Runtime &HermesJSRuntime::getRuntime() noexcept {
  return *m_holder->getRuntime();
}

facebook::react::jsinspector_modern::RuntimeTargetDelegate &HermesJSRuntime::getRuntimeTargetDelegate() {
  if (!m_runtimeTargetDelegate) {
    m_runtimeTargetDelegate = m_holder->createRuntimeTargetDelegate();
  }
  return *m_runtimeTargetDelegate;
}

} // namespace Microsoft::ReactNative
