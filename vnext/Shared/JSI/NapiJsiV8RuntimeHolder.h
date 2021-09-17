// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include <DevSettings.h>
#include <JSI/NodeApiJsiRuntime.h>
#include "RuntimeHolder.h"
#include "ScriptStore.h"

namespace Microsoft::JSI {

class NapiJsiV8RuntimeHolder : public facebook::jsi::RuntimeHolderLazyInit {
 public:
  std::shared_ptr<facebook::jsi::Runtime> getRuntime() noexcept override;

  NapiJsiV8RuntimeHolder(
      std::shared_ptr<facebook::react::DevSettings> devSettings,
      std::shared_ptr<facebook::react::MessageQueueThread> jsQueue,
      std::unique_ptr<facebook::jsi::ScriptStore> &&scriptStore,
      std::unique_ptr<facebook::jsi::PreparedScriptStore> &&preparedScriptStore) noexcept;

 private:
  static void ScheduleTaskCallback(
      napi_env env,
      napi_ext_task_callback taskCb,
      void *taskData,
      uint32_t delayMs,
      napi_finalize finalizeCb,
      void *finalizeHint);

  void InitRuntime() noexcept;

  std::shared_ptr<facebook::jsi::Runtime> m_runtime;
  std::shared_ptr<facebook::react::MessageQueueThread> m_jsQueue;

  std::unique_ptr<facebook::jsi::ScriptStore> m_scriptStore;
  std::unique_ptr<facebook::jsi::PreparedScriptStore> m_preparedScriptStore;

  std::once_flag m_onceFlag;
  std::thread::id m_ownThreadId;

  uint16_t m_debuggerPort;
  bool m_useDirectDebugger{false};
  bool m_debuggerBreakOnNextLine{false};
  std::string m_debuggerRuntimeName;
};

} // namespace Microsoft::JSI
