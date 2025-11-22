// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

// This file must match the code in React Native folder:
// ReactCommon/hermes/inspector-modern/chrome/HermesRuntimeAgentDelegate.h
// Unlike the code in React Native sources, this class delegates calls to Hermes C-based API.
// We use different namespace for this class comparing with the RN code.

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
  /**
   * \param frontendChannel A channel used to send responses and events to the
   * frontend.
   * \param sessionState The state of the current CDP session. This will only
   * be accessed on the main thread (during the constructor, in handleRequest,
   * etc).
   * \param previouslyExportedState The exported state from a previous instance
   * of RuntimeAgentDelegate (NOT necessarily HermesRuntimeAgentDelegate).
   * This may be nullptr, and if not nullptr it may be of any concrete type that
   * implements RuntimeAgentDelegate::ExportedState.
   * \param executionContextDescription A description of the execution context
   * represented by this runtime. This is used for disambiguating the
   * source/destination of CDP messages when there are multiple runtimes
   * (concurrently or over the life of a Host).
   * \param hermes_runtime The Hermes runtime that this agent is attached to. The caller
   * is responsible for keeping this object alive for the duration of the
   * \c HermesRuntimeAgentDelegate lifetime.
   * \param runtimeTargetDelegate The \c HermesRuntimeTargetDelegate object
   * object for the passed runtime.
   * \param runtimeExecutor A callback for scheduling work on the JS thread.
   * \c runtimeExecutor may drop scheduled work if the runtime is destroyed
   * first.
   */
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
  /**
   * Handle a CDP request. The response will be sent over the provided
   * \c FrontendChannel synchronously or asynchronously.
   * \param req The parsed request.
   * \returns true if this agent has responded, or will respond asynchronously,
   * to the request (with either a success or error message). False if the
   * agent expects another agent to respond to the request instead.
   */
  bool handleRequest(const facebook::react::jsinspector_modern::cdp::PreparsedRequest &req) override;

  std::unique_ptr<RuntimeAgentDelegate::ExportedState> getExportedState() override;

 private:
  HermesUniqueCdpAgent hermesCdpAgent_;
};

} // namespace Microsoft::ReactNative
