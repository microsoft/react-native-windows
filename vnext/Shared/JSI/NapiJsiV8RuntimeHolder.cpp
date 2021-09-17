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
  napi_env env{};
  napi_ext_env_settings settings{};
  settings.this_size = sizeof(settings);
  settings.flags.enable_gc_api = true;
  if (m_debuggerPort > 0)
    settings.inspector_port = m_debuggerPort;

  settings.flags.enable_inspector = m_useDirectDebugger;
  settings.flags.wait_for_debugger = m_debuggerBreakOnNextLine;
  settings.foreground_scheduler = &NapiJsiV8RuntimeHolder::ScheduleTaskCallback;

  napi_ext_create_env(&settings, &env);
  // Associate environment to holder.
  napi_set_instance_data(env, this, nullptr /*finalize_cb*/, nullptr /*finalize_hint*/);

  m_runtime = MakeNodeApiJsiRuntime(env);
  m_ownThreadId = std::this_thread::get_id();
}

#pragma region facebook::jsi::RuntimeHolderLazyInit

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

#pragma endregion facebook::jsi::RuntimeHolderLazyInit

} // namespace Microsoft::JSI
