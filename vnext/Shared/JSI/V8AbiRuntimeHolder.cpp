// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "V8AbiRuntimeHolder.h"

#include <crash/verifyElseCrash.h>
#include <jsi_abi/JsiAbiRuntime.h>
#include <jsi_abi/v8_jsi_abi.h>
#include <v8jsi_names.h>
#include "SafeLoadLibrary.h"

namespace Microsoft::ReactNative {
namespace {

// The V8 JSI ABI functions exported by v8jsi2.dll from Microsoft.JavaScript.V8.
// Resolve them with GetProcAddress to keep the legacy v8jsi.dll import unchanged.
struct V8AbiApi {
#define V8_ABI_FUNC(name) name##_fn name{};
#define V8_ABI_FUNCS                                            \
  V8_ABI_FUNC(v8_jsi_abi_runtime_create)                        \
  V8_ABI_FUNC(v8_jsi_abi_config_set_debugger_runtime_name_utf8) \
  V8_ABI_FUNC(v8_jsi_abi_config_set_inspector_enabled)          \
  V8_ABI_FUNC(v8_jsi_abi_config_set_inspector_port)             \
  V8_ABI_FUNC(v8_jsi_abi_config_set_inspector_break_on_start)   \
  V8_ABI_FUNC(v8_jsi_abi_config_set_multi_thread_enabled)       \
  V8_ABI_FUNC(v8_jsi_abi_config_set_task_runner)                \
  V8_ABI_FUNC(v8_jsi_abi_config_set_script_cache)
  V8_ABI_FUNCS
#undef V8_ABI_FUNC

  // Returns the name of the first missing function, or nullptr if all are resolved.
  const char *Resolve(HMODULE module) noexcept {
#define V8_ABI_FUNC(name)                                              \
  name = reinterpret_cast<name##_fn>(::GetProcAddress(module, #name)); \
  if (!name)                                                           \
    return #name;
    V8_ABI_FUNCS
#undef V8_ABI_FUNC
    return nullptr;
  }
#undef V8_ABI_FUNCS
};

// Posts the tasks that V8 schedules for the JS thread to the React Native JS queue.
class V8AbiTaskRunner {
 public:
  static jsi_status
  Create(const V8AbiApi &api, jsi_config config, std::shared_ptr<facebook::react::MessageQueueThread> queue) {
    // The config owns the runner from this call on, even if the call fails, and frees it with Delete.
    return api.v8_jsi_abi_config_set_task_runner(
        config, &PostTask, new V8AbiTaskRunner(std::move(queue)), &Delete, /*user_data_delete_data*/ nullptr);
  }

  explicit V8AbiTaskRunner(std::shared_ptr<facebook::react::MessageQueueThread> queue) noexcept
      : m_queue(std::move(queue)) {}

 private:
  class Task {
   public:
    Task(void *taskData, v8_jsi_abi_task_run_cb runCb, jsi_data_delete_cb deleteCb, void *deleteData) noexcept
        : m_taskData(taskData), m_runCb(runCb), m_deleteCb(deleteCb), m_deleteData(deleteData) {}

    Task(const Task &) = delete;
    Task &operator=(const Task &) = delete;

    ~Task() {
      if (m_deleteCb) {
        m_deleteCb(m_deleteData, m_taskData);
      }
    }

    void Run() const {
      if (m_runCb) {
        m_runCb(m_taskData);
      }
    }

   private:
    void *m_taskData;
    v8_jsi_abi_task_run_cb m_runCb;
    jsi_data_delete_cb m_deleteCb;
    void *m_deleteData;
  };

  // Exceptions must not unwind into v8jsi2.dll, and the task data must be deleted exactly once.
  static void JSI_ABI_CDECL PostTask(
      void *userData,
      void *taskData,
      v8_jsi_abi_task_run_cb runCb,
      jsi_data_delete_cb deleteCb,
      void *deleteData) noexcept {
    std::shared_ptr<Task> task;
    try {
      task = std::make_shared<Task>(taskData, runCb, deleteCb, deleteData);
    } catch (...) {
      if (deleteCb) {
        deleteCb(deleteData, taskData);
      }
      return;
    }

    try {
      reinterpret_cast<V8AbiTaskRunner *>(userData)->m_queue->runOnQueue([task] { task->Run(); });
    } catch (...) {
      // The task is dropped: its destructor deletes the task data.
    }
  }

  static void JSI_ABI_CDECL Delete(void * /*deleteData*/, void *userData) {
    delete reinterpret_cast<V8AbiTaskRunner *>(userData);
  }

  std::shared_ptr<facebook::react::MessageQueueThread> m_queue;
};

// Owns a buffer that v8jsi2.dll handed over together with its delete callback.
class V8AbiBuffer final : public facebook::jsi::Buffer {
 public:
  V8AbiBuffer(const uint8_t *data, size_t size, jsi_data_delete_cb deleteCb, void *deleteData) noexcept
      : m_data(data), m_size(size), m_deleteCb(deleteCb), m_deleteData(deleteData) {}

  V8AbiBuffer(const V8AbiBuffer &) = delete;
  V8AbiBuffer &operator=(const V8AbiBuffer &) = delete;

  ~V8AbiBuffer() override {
    if (m_deleteCb) {
      m_deleteCb(m_deleteData, const_cast<uint8_t *>(m_data));
    }
  }

  const uint8_t *data() const override {
    return m_data;
  }

  size_t size() const override {
    return m_size;
  }

 private:
  const uint8_t *m_data;
  size_t m_size;
  jsi_data_delete_cb m_deleteCb;
  void *m_deleteData;
};

// Connects the V8 code cache to the React Native PreparedScriptStore.
class V8AbiScriptCache {
 public:
  static jsi_status
  Create(const V8AbiApi &api, jsi_config config, std::shared_ptr<facebook::jsi::PreparedScriptStore> scriptStore) {
    // The config owns the cache from this call on, even if the call fails, and frees it with Delete.
    return api.v8_jsi_abi_config_set_script_cache(
        config,
        &LoadScript,
        &StoreScript,
        new V8AbiScriptCache(std::move(scriptStore)),
        &Delete,
        /*user_data_delete_data*/ nullptr);
  }

  explicit V8AbiScriptCache(std::shared_ptr<facebook::jsi::PreparedScriptStore> scriptStore) noexcept
      : m_scriptStore(std::move(scriptStore)) {}

 private:
  static void JSI_ABI_CDECL LoadScript(
      void *userData,
      const char *sourceUrl,
      uint64_t sourceHash,
      const char *runtimeName,
      uint64_t runtimeVersion,
      const char *cacheTag,
      const uint8_t **outData,
      size_t *outSize,
      jsi_data_delete_cb *outDeleteCb,
      void **outDeleteData) noexcept {
    *outData = nullptr;
    *outSize = 0;
    *outDeleteCb = nullptr;
    *outDeleteData = nullptr;

    try {
      auto &scriptStore = reinterpret_cast<V8AbiScriptCache *>(userData)->m_scriptStore;
      std::shared_ptr<const facebook::jsi::Buffer> preparedScript = scriptStore->tryGetPreparedScript(
          facebook::jsi::ScriptSignature{sourceUrl, sourceHash},
          facebook::jsi::JSRuntimeSignature{runtimeName, runtimeVersion},
          cacheTag);
      if (preparedScript) {
        auto holder = std::make_unique<std::shared_ptr<const facebook::jsi::Buffer>>(std::move(preparedScript));
        *outData = (*holder)->data();
        *outSize = (*holder)->size();
        *outDeleteCb = [](void *deleteData, void * /*data*/) {
          delete reinterpret_cast<std::shared_ptr<const facebook::jsi::Buffer> *>(deleteData);
        };
        *outDeleteData = holder.release();
      }
    } catch (...) {
      // Report a cache miss: V8 compiles the script instead.
    }
  }

  static void JSI_ABI_CDECL StoreScript(
      void *userData,
      const char *sourceUrl,
      uint64_t sourceHash,
      const char *runtimeName,
      uint64_t runtimeVersion,
      const char *cacheTag,
      const uint8_t *data,
      size_t size,
      jsi_data_delete_cb deleteCb,
      void *deleteData) noexcept {
    // The data must be deleted exactly once, either here or by the V8AbiBuffer.
    std::shared_ptr<V8AbiBuffer> buffer;
    try {
      buffer = std::make_shared<V8AbiBuffer>(data, size, deleteCb, deleteData);
    } catch (...) {
      if (deleteCb) {
        deleteCb(deleteData, const_cast<uint8_t *>(data));
      }
      return;
    }

    try {
      auto &scriptStore = reinterpret_cast<V8AbiScriptCache *>(userData)->m_scriptStore;
      scriptStore->persistPreparedScript(
          std::move(buffer),
          facebook::jsi::ScriptSignature{sourceUrl, sourceHash},
          facebook::jsi::JSRuntimeSignature{runtimeName, runtimeVersion},
          cacheTag);
    } catch (...) {
      // The script is not cached.
    }
  }

  static void JSI_ABI_CDECL Delete(void * /*deleteData*/, void *userData) {
    delete reinterpret_cast<V8AbiScriptCache *>(userData);
  }

  std::shared_ptr<facebook::jsi::PreparedScriptStore> m_scriptStore;
};

struct V8AbiConfigureContext {
  const V8AbiApi &api;
  V8AbiRuntimeArgs &args;
};

jsi_status JSI_ABI_CDECL ConfigureRuntime(void *userData, jsi_config config) {
  try {
    auto &context = *reinterpret_cast<V8AbiConfigureContext *>(userData);
    const V8AbiApi &api = context.api;
    V8AbiRuntimeArgs &args = context.args;

#define RETURN_IF_FAILED(expr)                             \
  if (jsi_status status = (expr); status != jsi_status_ok) \
    return status;

    RETURN_IF_FAILED(api.v8_jsi_abi_config_set_debugger_runtime_name_utf8(
        config, args.debuggerRuntimeName.data(), args.debuggerRuntimeName.size()));
    RETURN_IF_FAILED(api.v8_jsi_abi_config_set_inspector_enabled(config, args.enableInspector));
    if (args.inspectorPort > 0) {
      RETURN_IF_FAILED(api.v8_jsi_abi_config_set_inspector_port(config, args.inspectorPort));
    }
    RETURN_IF_FAILED(api.v8_jsi_abi_config_set_inspector_break_on_start(config, args.inspectorBreakOnStart));
    RETURN_IF_FAILED(api.v8_jsi_abi_config_set_multi_thread_enabled(config, args.enableMultiThreading));
    if (args.jsQueue) {
      RETURN_IF_FAILED(V8AbiTaskRunner::Create(api, config, args.jsQueue));
    }
    if (args.preparedScriptStore) {
      RETURN_IF_FAILED(V8AbiScriptCache::Create(api, config, args.preparedScriptStore));
    }

#undef RETURN_IF_FAILED
    return jsi_status_ok;
  } catch (...) {
    return jsi_status_native_error;
  }
}

} // namespace

std::unique_ptr<facebook::jsi::Runtime>
MakeV8AbiRuntime(HMODULE v8jsiModule, V8AbiRuntimeArgs &&args, std::string &errorMessage) noexcept {
  try {
    if (!v8jsiModule) {
      errorMessage = "V8 JSI ABI is unavailable: " V8JSI_DLL_NAME " could not be loaded.";
      return nullptr;
    }

    V8AbiApi api;
    if (const char *missingFunc = api.Resolve(v8jsiModule)) {
      errorMessage = std::string("V8 JSI ABI is unavailable: " V8JSI_DLL_NAME " does not export ") + missingFunc +
          ". The V8 JSI ABI requires " V8JSI_DLL_NAME " from the Microsoft.JavaScript.V8 package.";
      return nullptr;
    }

    V8AbiConfigureContext context{api, args};
    return ::jsi::abi::makeJsiAbiRuntime(api.v8_jsi_abi_runtime_create, &ConfigureRuntime, &context);
  } catch (const std::exception &ex) {
    errorMessage = std::string("V8 JSI ABI runtime creation failed: ") + ex.what();
  } catch (...) {
    errorMessage = "V8 JSI ABI runtime creation failed with an unknown error.";
  }
  return nullptr;
}

V8AbiRuntimeHolder::V8AbiRuntimeHolder(
    std::shared_ptr<facebook::react::DevSettings> devSettings,
    std::shared_ptr<facebook::react::MessageQueueThread> jsQueue,
    std::shared_ptr<facebook::jsi::PreparedScriptStore> preparedScriptStore,
    bool enableMultiThreadingSupport) noexcept
    : m_weakDevSettings(devSettings),
      m_jsQueue(std::move(jsQueue)),
      m_preparedScriptStore(std::move(preparedScriptStore)),
      m_enableMultiThreadingSupport(enableMultiThreadingSupport) {}

void V8AbiRuntimeHolder::initRuntime() noexcept {
  std::shared_ptr<facebook::react::DevSettings> devSettings = m_weakDevSettings.lock();
  VerifyElseCrash(devSettings);

  V8AbiRuntimeArgs args;
  args.jsQueue = m_jsQueue;
  args.preparedScriptStore = m_preparedScriptStore;
  args.debuggerRuntimeName = devSettings->debuggerRuntimeName;
  args.inspectorPort = devSettings->debuggerPort;
  args.enableInspector = devSettings->useDirectDebugger;
  args.inspectorBreakOnStart = devSettings->debuggerBreakOnNextLine;
  args.enableMultiThreading = m_enableMultiThreadingSupport;

  static HMODULE s_v8jsiModule = LoadLibraryAsPeerFirst(V8JSI_DLL_NAME_W);

  std::string errorMessage;
  m_jsiRuntime = MakeV8AbiRuntime(s_v8jsiModule, std::move(args), errorMessage);
  if (!m_jsiRuntime) {
    // The loggingCallback and OutputDebugString are synchronous; the errorCallback may report asynchronously, after
    // the crash below.
    if (devSettings->loggingCallback) {
      devSettings->loggingCallback(facebook::react::RCTLogLevel::Error, errorMessage.c_str());
    }
    ::OutputDebugStringA((errorMessage + "\n").c_str());
    if (devSettings->errorCallback) {
      devSettings->errorCallback(errorMessage);
    }
  }
  VerifyElseCrashSz(m_jsiRuntime, errorMessage.c_str());
}

facebook::react::JSIEngineOverride V8AbiRuntimeHolder::getRuntimeType() noexcept {
  return facebook::react::JSIEngineOverride::V8;
}

std::shared_ptr<facebook::jsi::Runtime> V8AbiRuntimeHolder::getRuntime() noexcept {
  std::call_once(m_onceFlag, [this]() { initRuntime(); });
  VerifyElseCrash(m_jsiRuntime);
  return m_jsiRuntime;
}

} // namespace Microsoft::ReactNative
