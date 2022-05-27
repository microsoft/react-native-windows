// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "NapiJsiV8RuntimeHolder.h"

using namespace facebook::jsi;
using namespace facebook::react;

using std::shared_ptr;
using std::unique_ptr;

namespace Microsoft::JSI {

struct NapiTask {
  NapiTask(
      napi_env env,
      napi_ext_task_callback taskCallback,
      void *taskData,
      napi_finalize finalizeCallback,
      void *finalizeHint) noexcept
      : m_env{env},
        m_taskCallback{taskCallback},
        m_taskData{taskData},
        m_finalizeCallback{finalizeCallback},
        m_finalizeHint{finalizeHint} {}

  NapiTask(const NapiTask &) = delete;
  NapiTask &operator=(const NapiTask &) = delete;

  ~NapiTask() {
    if (m_finalizeCallback) {
      m_finalizeCallback(m_env, m_taskData, m_finalizeHint);
    }
  }

  void operator()() noexcept {
    m_taskCallback(m_env, m_taskData);
  }

 private:
  napi_env m_env;
  napi_ext_task_callback m_taskCallback;
  void *m_taskData;
  napi_finalize m_finalizeCallback;
  void *m_finalizeHint;
};

// See napi_ext_schedule_task_callback definition.
/*static*/ void NapiJsiV8RuntimeHolder::ScheduleTaskCallback(
    napi_env env,
    napi_ext_task_callback taskCallback,
    void *taskData,
    uint32_t /*delayInMsec*/,
    napi_finalize finalizeCallback,
    void *finalizeHint) {
  NapiJsiV8RuntimeHolder *holder;
  auto result = napi_get_instance_data(env, (void **)&holder);
  if (result != napi_status::napi_ok) {
    std::terminate();
  }

  auto task = std::make_shared<NapiTask>(env, taskCallback, taskData, finalizeCallback, finalizeHint);
  holder->m_jsQueue->runOnQueue([task = std::move(task)]() { task->operator()(); });
}

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
  settings.foreground_scheduler = &NapiJsiV8RuntimeHolder::ScheduleTaskCallback;

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
