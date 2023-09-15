// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "NapiJsiV8RuntimeHolder.h"

using namespace facebook::jsi;
using namespace facebook::react;

using std::shared_ptr;
using std::unique_ptr;

namespace Microsoft::JSI {

class NapiTask {
 public:
  NapiTask(void *task, v8_task_run_cb onTaskRun, v8_task_release_cb onTaskRelease)
      : task_(task), onTaskRun_(onTaskRun), onTaskRelease_(onTaskRelease) {}

  NapiTask(NapiTask &&other)
      : task_(std::exchange(other.task_, nullptr)),
        onTaskRun_(std::exchange(other.onTaskRun_, nullptr)),
        onTaskRelease_(std::exchange(other.onTaskRelease_, nullptr)) {}

  NapiTask &operator=(NapiTask &&other) {
    if (this != &other) {
      NapiTask taskToDelete(std::move(*this));
      task_ = std::exchange(other.task_, nullptr);
      onTaskRun_ = std::exchange(other.onTaskRun_, nullptr);
      onTaskRelease_ = std::exchange(other.onTaskRelease_, nullptr);
    }
    return *this;
  }

  NapiTask(const NapiTask &other) = delete;
  NapiTask &operator=(const NapiTask &other) = delete;

  ~NapiTask() {
    if (task_ != nullptr) {
      onTaskRelease_(task_);
    }
  }

  void Run() const {
    if (task_ != nullptr) {
      onTaskRun_(task_);
    }
  }

 private:
  void *task_;
  v8_task_run_cb onTaskRun_;
  v8_task_release_cb onTaskRelease_;
};

class NapiTaskRunner {
 public:
  NapiTaskRunner(std::shared_ptr<facebook::react::MessageQueueThread> jsQueue) : m_jsQueue(std::move(jsQueue)) {}

  static v8_task_runner_t Create(std::shared_ptr<facebook::react::MessageQueueThread> jsQueue) {
    NapiTaskRunner *taskRunner = new NapiTaskRunner(std::move(jsQueue));
    return v8_create_task_runner(reinterpret_cast<void *>(taskRunner), &PostTask, &Release);
  }

 private:
  static void __cdecl PostTask(
      void *taskRunner,
      void *task,
      v8_task_run_cb onTaskRun,
      v8_task_release_cb onTaskRelease) {
    auto napiTask = std::make_shared<NapiTask>(task, onTaskRun, onTaskRelease);
    reinterpret_cast<NapiTaskRunner *>(taskRunner)->m_jsQueue->runOnQueue([napiTask = std::move(napiTask)] {
      napiTask->Run();
    });
  }

  static void __cdecl Release(void *taskRunner) {
    delete reinterpret_cast<NapiTaskRunner *>(taskRunner);
  }

 private:
  std::shared_ptr<facebook::react::MessageQueueThread> m_jsQueue;
};

NapiJsiV8RuntimeHolder::NapiJsiV8RuntimeHolder(
    shared_ptr<DevSettings> devSettings,
    shared_ptr<MessageQueueThread> jsQueue,
    unique_ptr<ScriptStore> &&scriptStore,
    unique_ptr<PreparedScriptStore> &&preparedScriptStore) noexcept
    : m_useDirectDebugger{devSettings->useDirectDebugger},
      m_debuggerBreakOnNextLine{devSettings->debuggerBreakOnNextLine},
      m_debuggerPort{devSettings->debuggerPort},
      m_debuggerRuntimeName{devSettings->debuggerRuntimeName},
      m_jsQueue{jsQueue},
      m_scriptStore{std::move(scriptStore)},
      m_preparedScriptStore{std::move(preparedScriptStore)} {}

void NapiJsiV8RuntimeHolder::InitRuntime() noexcept {
  napi_ext_env_settings settings{};
  settings.this_size = sizeof(settings);
  if (m_debuggerPort > 0)
    settings.inspector_port = m_debuggerPort;

  settings.flags.enable_inspector = m_useDirectDebugger;
  settings.flags.wait_for_debugger = m_debuggerBreakOnNextLine;
  //  TODO: args.debuggerRuntimeName = debuggerRuntimeName_;
  settings.foreground_task_runner = NapiTaskRunner::Create(m_jsQueue);

  napi_ext_script_cache scriptCache = InitScriptCache(std::move(m_preparedScriptStore));
  settings.script_cache = &scriptCache;

  napi_env env{};
  napi_ext_create_env(&settings, &env);
  // Associate environment to holder.
  napi_set_instance_data(env, this, nullptr /*finalize_cb*/, nullptr /*finalize_hint*/);

  m_runtime = MakeNodeApiJsiRuntime(env);
  m_ownThreadId = std::this_thread::get_id();
}

struct NodeApiJsiBuffer : facebook::jsi::Buffer {
  static std::shared_ptr<const facebook::jsi::Buffer> CreateJsiBuffer(const napi_ext_buffer *buffer) {
    if (buffer && buffer->data) {
      return std::shared_ptr<const facebook::jsi::Buffer>(new NodeApiJsiBuffer(buffer));
    } else {
      return {};
    }
  }

  NodeApiJsiBuffer(const napi_ext_buffer *buffer) noexcept : buffer_(*buffer) {}

  ~NodeApiJsiBuffer() override {
    if (buffer_.buffer_object.finalize_cb) {
      buffer_.buffer_object.finalize_cb(nullptr, buffer_.buffer_object.data, buffer_.buffer_object.finalize_hint);
    }
  }

  const uint8_t *data() const override {
    return buffer_.data;
  }

  size_t size() const override {
    return buffer_.byte_size;
  }

 private:
  napi_ext_buffer buffer_;
};

napi_ext_script_cache NapiJsiV8RuntimeHolder::InitScriptCache(
    unique_ptr<PreparedScriptStore> &&preparedScriptStore) noexcept {
  napi_ext_script_cache scriptCache{};
  scriptCache.cache_object = NativeObjectWrapper<unique_ptr<PreparedScriptStore>>::Wrap(std::move(preparedScriptStore));
  scriptCache.load_cached_script = [](napi_env env,
                                      napi_ext_script_cache *script_cache,
                                      napi_ext_cached_script_metadata *script_metadata,
                                      napi_ext_buffer *result) -> napi_status {
    PreparedScriptStore *scriptStore = reinterpret_cast<PreparedScriptStore *>(script_cache->cache_object.data);
    std::shared_ptr<const facebook::jsi::Buffer> buffer = scriptStore->tryGetPreparedScript(
        ScriptSignature{script_metadata->source_url, script_metadata->source_hash},
        JSRuntimeSignature{script_metadata->runtime_name, script_metadata->runtime_version},
        script_metadata->tag);
    if (buffer) {
      result->buffer_object = NativeObjectWrapper<std::shared_ptr<const facebook::jsi::Buffer>>::Wrap(
          std::shared_ptr<const facebook::jsi::Buffer>{buffer});
      result->data = buffer->data();
      result->byte_size = buffer->size();
    } else {
      *result = napi_ext_buffer{};
    }
    return napi_ok;
  };
  scriptCache.store_cached_script = [](napi_env env,
                                       napi_ext_script_cache *script_cache,
                                       napi_ext_cached_script_metadata *script_metadata,
                                       const napi_ext_buffer *buffer) -> napi_status {
    PreparedScriptStore *scriptStore = reinterpret_cast<PreparedScriptStore *>(script_cache->cache_object.data);
    scriptStore->persistPreparedScript(
        NodeApiJsiBuffer::CreateJsiBuffer(buffer),
        ScriptSignature{script_metadata->source_url, script_metadata->source_hash},
        JSRuntimeSignature{script_metadata->runtime_name, script_metadata->runtime_version},
        script_metadata->tag);
    return napi_ok;
  };
  return scriptCache;
}

#pragma region Microsoft::JSI::RuntimeHolderLazyInit

facebook::react::JSIEngineOverride NapiJsiV8RuntimeHolder::getRuntimeType() noexcept {
  return facebook::react::JSIEngineOverride::V8NodeApi;
}

shared_ptr<Runtime> NapiJsiV8RuntimeHolder::getRuntime() noexcept /*override*/
{
  std::call_once(m_onceFlag, [this]() { InitRuntime(); });

  if (!m_runtime)
    std::terminate();

  // V8 NapiJsiRuntime is not known to be thread safe.
  if (m_ownThreadId != std::this_thread::get_id())
    __fastfail(FAST_FAIL_INVALID_THREAD);

  return m_runtime;
}

#pragma endregion Microsoft::JSI::RuntimeHolderLazyInit

} // namespace Microsoft::JSI
