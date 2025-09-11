// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include <ReactCommon/RuntimeExecutor.h>

#include <cxxreact/MessageQueueThread.h>
#include <jsinspector-modern/ReactCdp.h>

#include <memory>
#include "HermesRuntimeHolder.h"

namespace Microsoft::ReactNative {

// A RuntimeTargetDelegate that enables debugging a Hermes runtime over CDP.
class HermesRuntimeTargetDelegate : public facebook::react::jsinspector_modern::RuntimeTargetDelegate {
 public:
  explicit HermesRuntimeTargetDelegate(std::shared_ptr<HermesRuntimeHolder> hermesRuntimeHolder);
  ~HermesRuntimeTargetDelegate() override;
  hermes_cdp_debugger getCdpDebugger();

 public: // RuntimeTargetDelegate implementation
  std::unique_ptr<facebook::react::jsinspector_modern::RuntimeAgentDelegate> createAgentDelegate(
      facebook::react::jsinspector_modern::FrontendChannel frontendChannel,
      facebook::react::jsinspector_modern::SessionState &sessionState,
      std::unique_ptr<facebook::react::jsinspector_modern::RuntimeAgentDelegate::ExportedState> previouslyExportedState,
      const facebook::react::jsinspector_modern::ExecutionContextDescription &executionContextDescription,
      facebook::react::RuntimeExecutor runtimeExecutor) override;

  void addConsoleMessage(facebook::jsi::Runtime &runtime, facebook::react::jsinspector_modern::ConsoleMessage message)
      override;

  bool supportsConsole() const override;

  std::unique_ptr<facebook::react::jsinspector_modern::StackTrace> captureStackTrace(
      facebook::jsi::Runtime &runtime,
      size_t framesToSkip) override;

 private:
  std::shared_ptr<HermesRuntimeHolder> hermesRuntimeHolder_;
  const facebook::hermes::HermesUniqueCdpDebugger hermesCdpDebugger_;
};

} // namespace Microsoft::ReactNative
