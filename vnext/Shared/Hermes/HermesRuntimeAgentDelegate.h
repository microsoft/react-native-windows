// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include "HermesRuntimeTargetDelegate.h"

#include <ReactCommon/RuntimeExecutor.h>
#include <jsinspector-modern/ReactCdp.h>
#include "HermesRuntimeHolder.h"

namespace Microsoft::ReactNative {

// A RuntimeAgentDelegate that handles requests from the Chrome DevTools
// Protocol for an instance of Hermes, using the modern CDPAgent API.
class HermesRuntimeAgentDelegate : public facebook::react::jsinspector_modern::RuntimeAgentDelegate {
 public:
  HermesRuntimeAgentDelegate(
      facebook::react::jsinspector_modern::FrontendChannel frontendChannel,
      facebook::react::jsinspector_modern::SessionState &sessionState,
      std::unique_ptr<facebook::react::jsinspector_modern::RuntimeAgentDelegate::ExportedState> previouslyExportedState,
      const facebook::react::jsinspector_modern::ExecutionContextDescription &executionContextDescription,
      hermes_runtime runtime,
      HermesRuntimeTargetDelegate &runtimeTargetDelegate,
      facebook::react::RuntimeExecutor runtimeExecutor);

  ~HermesRuntimeAgentDelegate() override;

 public: // RuntimeAgentDelegate implementation
  bool handleRequest(const facebook::react::jsinspector_modern::cdp::PreparsedRequest &req) override;
  std::unique_ptr<RuntimeAgentDelegate::ExportedState> getExportedState() override;

 private:
  facebook::hermes::HermesUniqueCdpAgent hermesCdpAgent_;
};

} // namespace Microsoft::ReactNative
