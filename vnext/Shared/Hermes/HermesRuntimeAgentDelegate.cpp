// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "HermesRuntimeAgentDelegate.h"

#include <jsinspector-modern/ReactCdp.h>

using namespace facebook::react::jsinspector_modern;
using namespace facebook::hermes;

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
    : hermesCdpAgent_(HermesApi2().createCdpAgent(
          runtimeTargetDelegate.getCdpDebugger(),
          executionContextDescription.id,
          // RuntimeTask takes a HermesRuntime whereas our RuntimeExecutor takes a jsi::Runtime.
          AsFunctor<hermes_enqueue_runtime_task_functor>([runtimeExecutor = std::move(runtimeExecutor),
                                                          runtime](hermes_runtime_task_functor runtimeTask) {
            OutputDebugStringA("[RNW] Enqueuing runtime task for CDP operation\n");
            runtimeExecutor([runtime, fn = std::make_shared<FunctorWrapper<hermes_runtime_task_functor>>(runtimeTask)](
                                auto &) { 
              OutputDebugStringA("[RNW] Executing runtime task on JS thread\n");
              (*fn)(runtime); 
            });
          }),
          AsFunctor<hermes_enqueue_frontend_message_functor>(
              [frontendChannel = std::move(frontendChannel)](const char *json_utf8, size_t json_size) {
                std::string message(json_utf8, json_size);
                std::string logMsg = "[RNW] Sending CDP message to DevTools: " + message.substr(0, std::min(size_t(100), json_size)) + "\n";
                OutputDebugStringA(logMsg.c_str());
                frontendChannel(std::string_view(json_utf8, json_size));
              }),
          HermesStateWrapper::unwrapDestructively(previouslyExportedState.get()).release())) {
  
  // enable domain debbuger only once 
  bool runtimeAlreadyEnabled = sessionState.isRuntimeDomainEnabled;
  bool debuggerAlreadyEnabled = sessionState.isDebuggerDomainEnabled;
  
  if (runtimeAlreadyEnabled) {
    OutputDebugStringA("WARNING: Runtime domain was ALREADY enabled - skipping re-enable\n");
  } else {
    OutputDebugStringA("Enabling Runtime domain for first time\n");
    HermesApi2().enableRuntimeDomain(hermesCdpAgent_.get());
    OutputDebugStringA("Runtime domain enabled successfully\n");
  }
  
  if (debuggerAlreadyEnabled) {
    OutputDebugStringA("WARNING: Debugger domain was ALREADY enabled - skipping re-enable\n");
  } else {
    OutputDebugStringA("Enabling Debugger domain for first time\n");
    HermesApi2().enableDebuggerDomain(hermesCdpAgent_.get());
    OutputDebugStringA("Debugger domain enabled successfully\n");
  }
  
  OutputDebugStringA("HermesRuntimeAgentDelegate constructor COMPLETE\n");
}

HermesRuntimeAgentDelegate::~HermesRuntimeAgentDelegate() {
  OutputDebugStringA("[RNW] HermesRuntimeAgentDelegate destructor called - CDP agent will be released\n");
}

bool HermesRuntimeAgentDelegate::handleRequest(const cdp::PreparsedRequest &req) {
  if (req.method.starts_with("Log.")) {
    // Hermes doesn't do anything useful with Log messages.
    return false;
  }

  std::string json = req.toJson();
  HermesApi2().handleCommand(hermesCdpAgent_.get(), json.c_str(), json.size());
  return true;
}

std::unique_ptr<RuntimeAgentDelegate::ExportedState> HermesRuntimeAgentDelegate::getExportedState() {
  return std::make_unique<HermesStateWrapper>(HermesApi2().getCdpState(hermesCdpAgent_.get()));
}

} // namespace Microsoft::ReactNative
