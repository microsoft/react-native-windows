// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "NapiJsiV8RuntimeHolder.h"

// V8-JSI
#include <V8JsiRuntime.h>

using namespace facebook::jsi;
using namespace facebook::react;

using std::shared_ptr;
using std::unique_ptr;

namespace Microsoft::JSI {

// See napi_ext_schedule_task_callback definition.
/*static*/ void NapiJsiV8RuntimeHolder::ScheduleTaskCallback(
    napi_env env,
    napi_ext_task_callback taskCb,
    void *taskData,
    uint32_t delayMs,
    napi_finalize finalizeCb,
    void *finalizeint) {

  NapiJsiV8RuntimeHolder *holder;
  auto result = napi_get_instance_data(env, (void **)&holder);
  if (result != napi_status::napi_ok) {
    //TODO: Signal error
    return;
  }

  //TODO: ABI-safe?
  auto sharedTask = shared_ptr<v8runtime::JSITask>(static_cast<v8runtime::JSITask *>(taskData));
  holder->m_jsQueue->runOnQueue([sharedTask = std::move(sharedTask)]() { sharedTask->run();
  });
}

NapiJsiV8RuntimeHolder::NapiJsiV8RuntimeHolder(
    shared_ptr<DevSettings> devSettings,
    shared_ptr<MessageQueueThread> jsQueue,
    unique_ptr<ScriptStore> &&scriptStore,
    unique_ptr<PreparedScriptStore> &&preparedScritpStore) noexcept
    : m_useDirectDebugger{devSettings->useDirectDebugger},
      m_debuggerBreakOnNextLine{devSettings->debuggerBreakOnNextLine},
      m_debuggerPort{devSettings->debuggerPort},
      m_debuggerRuntimeName{devSettings->debuggerRuntimeName},
      m_jsQueue{std::move(jsQueue)},
      m_scriptStore{std::move(scriptStore)},
      m_preparedScriptStore{std::move(preparedScritpStore)} {}

void NapiJsiV8RuntimeHolder::InitRuntime() noexcept {
  napi_env env{};
  napi_ext_env_settings settings{};
  settings.this_size = sizeof(napi_ext_env_settings);
  settings.flags.enable_gc_api = true;
  if (m_debuggerPort > 0)
    settings.inspector_port = m_debuggerPort;

  settings.flags.enable_inspector = m_useDirectDebugger;
  settings.flags.wait_for_debugger = m_debuggerBreakOnNextLine;
  // TODO: debuggerRuntimeName?

  settings.foreground_scheduler = &NapiJsiV8RuntimeHolder::ScheduleTaskCallback;
  // TODO: scriptStore?

  napi_ext_create_env(&settings, &env);
  // Associate environment to holder.
  napi_set_instance_data(env, this, nullptr, nullptr); // TODO: Finalize

  m_runtime = MakeNapiJsiRuntime(env);
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
    std::terminate();

  return m_runtime;
}

#pragma endregion facebook::jsi::RuntimeHolderLazyInit

} // namespace Microsoft::JSI
