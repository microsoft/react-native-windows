// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

// This file must match the code in React Native folder:
// ReactCommon/hermes/inspector-modern/chrome/HermesRuntimeAgentDelegate.cpp
// Unlike the code in React Native sources, this class delegates calls to Hermes C-based API.

#include "HermesRuntimeAgentDelegate.h"

#include <jsinspector-modern/ReactCdp.h>

using namespace facebook::react::jsinspector_modern;

namespace Microsoft::ReactNative {

namespace {

struct HermesStateWrapper : public RuntimeAgentDelegate::ExportedState {
  explicit HermesStateWrapper(HermesUniqueCdpState &&hermesCdpState) : hermesCdpState_(std::move(hermesCdpState)) {}

  static HermesUniqueCdpState unwrapDestructively(ExportedState *wrapper) {
    if (!wrapper) {
      return {};
    }
    if (auto *typedWrapper = dynamic_cast<HermesStateWrapper *>(wrapper)) {
      return std::move(typedWrapper->hermesCdpState_);
    }
    return {};
  }

 private:
  HermesUniqueCdpState hermesCdpState_;
};

} // namespace

HermesRuntimeAgentDelegate::HermesRuntimeAgentDelegate(
    FrontendChannel frontendChannel,
    SessionState &sessionState,
    std::unique_ptr<RuntimeAgentDelegate::ExportedState> previouslyExportedState,
    const ExecutionContextDescription &executionContextDescription,
    hermes_runtime runtime,
    HermesRuntimeTargetDelegate &runtimeTargetDelegate,
    facebook::react::RuntimeExecutor runtimeExecutor)
    : hermesCdpAgent_(HermesInspectorApi::createCdpAgent(
          runtimeTargetDelegate.getCdpDebugApi(),
          executionContextDescription.id,
          // Adapt std::function<void(std::function<void(jsi::Runtime& runtime)>&& callback)>
          // to hermes_enqueue_runtime_task_functor
          AsFunctor<hermes_enqueue_runtime_task_functor>(
              [runtimeExecutor = std::move(runtimeExecutor), runtime](hermes_run_runtime_task_functor runtimeTask) {
                // Adapt std::function<void(jsi::Runtime& runtime)> to hermes_runtime_task_functor
                runtimeExecutor(
                    [runtime, fn = std::make_shared<FunctorWrapper<hermes_run_runtime_task_functor>>(runtimeTask)](
                        facebook::jsi::Runtime &rt) { (*fn)(runtime); });
              }),
          // Adapt void(const char *json_utf8, size_t json_size) to std::function<void(std::string_view)>
          AsFunctor<hermes_enqueue_frontend_message_functor>(
              [frontendChannel = std::move(frontendChannel)](const char *json_utf8, size_t json_size) {
                frontendChannel(std::string_view(json_utf8, json_size));
              }),
          HermesStateWrapper::unwrapDestructively(previouslyExportedState.get()).release())) {
  // Enable domains conditionally based on session state
  // This matches the iOS/Android implementation pattern:
  // Domains are enabled in response to Chrome DevTools sending Runtime.enable/Debugger.enable
  if (sessionState.isRuntimeDomainEnabled) {
    HermesInspectorApi::enableRuntimeDomain(hermesCdpAgent_.get());
  }
  if (sessionState.isDebuggerDomainEnabled) {
    HermesInspectorApi::enableDebuggerDomain(hermesCdpAgent_.get());
  }
}

HermesRuntimeAgentDelegate::~HermesRuntimeAgentDelegate() = default;

bool HermesRuntimeAgentDelegate::handleRequest(const cdp::PreparsedRequest &req) {
  if (req.method.starts_with("Log.")) {
    // Since we know Hermes doesn't do anything useful with Log messages,
    // but our containing HostAgent will, bail out early.
    return false;
  }

  std::string json = req.toJson();
  HermesInspectorApi::handleCommand(hermesCdpAgent_.get(), json.c_str(), json.size());
  return true;
}

std::unique_ptr<RuntimeAgentDelegate::ExportedState> HermesRuntimeAgentDelegate::getExportedState() {
  return std::make_unique<HermesStateWrapper>(HermesInspectorApi::getCdpState(hermesCdpAgent_.get()));
}

} // namespace Microsoft::ReactNative
