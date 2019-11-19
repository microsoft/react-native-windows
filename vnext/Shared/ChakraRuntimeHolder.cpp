// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"

#include "ChakraRuntimeHolder.h"

// TODO (yicyao): Due to the current structure of our msbuild projects, we can't
// move ChakraRuntimeHolder into a platform specific project. For now, we use
// conditional compilation in this file. We ought to clean this up.
#ifdef CHAKRACORE
#include "JSI/Shared/ChakraCoreRuntimeArgs.h"
#include "JSI/Shared/ChakraCoreRuntimeFactory.h"
#else
#include "JSI/Shared/ChakraRuntimeArgs.h"
#include "JSI/Universal/ChakraRtRuntimeFactory.h"
#endif

namespace Microsoft::JSI {

std::shared_ptr<facebook::jsi::Runtime> ChakraRuntimeHolder::getRuntime() noexcept {
  std::call_once(m_init_runtime_flag, [this]() { InitRuntime(); });

  if (!m_runtime) {
    std::terminate();
  }

  // ChakraCoreRuntime and ChakraRtRuntime are not thread safe as of right now.
  if (m_own_thread_id != std::this_thread::get_id()) {
    std::terminate();
  }

  return m_runtime;
}

ChakraRuntimeHolder::ChakraRuntimeHolder(
    std::shared_ptr<facebook::react::DevSettings> devSettings,
    std::shared_ptr<facebook::react::MessageQueueThread> jsQueue,
    std::unique_ptr<facebook::jsi::ScriptStore> &&scriptStore,
    std::unique_ptr<facebook::jsi::PreparedScriptStore> &&preparedScriptStore) noexcept {
#ifdef CHAKRACORE
  auto args = std::make_shared<ChakraCoreRuntimeArgs>();
#else
  auto args = std::make_shared<ChakraRtRuntimeArgs>();
#endif

  if (!devSettings->useJITCompilation) {
    args->executablePageAllocationPolicy = ChakraRuntimeArgs::ExecutablePageAllocationPolicy::DisableAll;
  }

  if (devSettings->useDirectDebugger) {
    args->enableDebugging = true;
  }

  args->memoryTracker = devSettings->memoryTracker;

  args->scriptStore = std::move(scriptStore);
  args->preparedScriptStore = std::move(preparedScriptStore);

#ifdef CHAKRACORE
  args->jsQueue = std::move(jsQueue);

  args->debuggerBreakOnNextLine = devSettings->debuggerBreakOnNextLine;
  args->debuggerPort = devSettings->debuggerPort;
  args->debuggerRuntimeName = devSettings->debuggerRuntimeName;

  // clang-format off
  args->debuggerLoggingCallback =
    [loggingCallback = std::move(devSettings->loggingCallback)](const char *message, LogLevel logLevel) {
      loggingCallback(static_cast<facebook::react::RCTLogLevel>(logLevel), message);
    };
  // clange-format on
#endif

  m_args = std::move(args);
}

void ChakraRuntimeHolder::InitRuntime() noexcept {
#ifdef CHAKRACORE
  m_runtime = makeChakraCoreRuntime(std::static_pointer_cast<ChakraCoreRuntimeArgs>(m_args));
#else
  m_runtime = makeChakraRtRuntime(std::shared_ptr<ChakraRtRuntimeArgs>(m_args));
#endif
  m_own_thread_id = std::this_thread::get_id();
}

} // namespace Microsoft::JSI
