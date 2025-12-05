// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

// This file must match the code in React Native folder:
// ReactCommon/hermes/inspector-modern/chrome/HermesRuntimeTargetDelegate.h
// Unlike the code in React Native sources, this class delegates calls to Hermes C-based API.
// We use different namespace for this class comparing with the RN code.

#pragma once

#include <ReactCommon/RuntimeExecutor.h>

#include <cxxreact/MessageQueueThread.h>
#include <jsinspector-modern/ReactCdp.h>

#include <memory>
#include "HermesRuntimeHolder.h"

namespace Microsoft::ReactNative {

/**
 * A RuntimeTargetDelegate that enables debugging a Hermes runtime over CDP.
 */
class HermesRuntimeTargetDelegate : public facebook::react::jsinspector_modern::RuntimeTargetDelegate {
 public:
  explicit HermesRuntimeTargetDelegate(std::shared_ptr<HermesRuntimeHolder> &&hermesRuntimeHolder);

  ~HermesRuntimeTargetDelegate() override;

  // Our C-API specific helper method to be used internally instead of RN getCDPDebugAPI() private function.
  hermes_cdp_debug_api getCdpDebugApi();

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

  /**
   * Start sampling profiler.
   */
  void enableSamplingProfiler() override;

  /**
   * Stop sampling profiler.
   */
  void disableSamplingProfiler() override;

  /**
   * Return recorded sampling profile for the previous sampling session.
   */
  facebook::react::jsinspector_modern::tracing::RuntimeSamplingProfile collectSamplingProfile() override;

 private:
  std::shared_ptr<HermesRuntimeHolder> hermesRuntimeHolder_;
  const HermesUniqueCdpDebugApi hermesCdpDebugApi_;
};

} // namespace Microsoft::ReactNative
