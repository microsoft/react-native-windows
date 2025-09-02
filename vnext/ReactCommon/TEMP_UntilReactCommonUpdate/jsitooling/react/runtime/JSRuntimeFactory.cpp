/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "JSRuntimeFactory.h"
#include <jsinspector-modern/ConsoleMessage.h>
#include <jsinspector-modern/FallbackRuntimeAgentDelegate.h>

namespace facebook::react {

jsi::Runtime& JSIRuntimeHolder::getRuntime() noexcept {
  return *runtime_;
}

JSIRuntimeHolder::JSIRuntimeHolder(std::unique_ptr<jsi::Runtime> runtime)
    : runtime_(std::move(runtime)) {
  assert(runtime_ != nullptr);
}

void JSIRuntimeHolder::addConsoleMessage(jsi::Runtime& runtime, jsinspector_modern::ConsoleMessage message) {
  return;
}

bool JSIRuntimeHolder::supportsConsole() const{
  return false;
}

std::unique_ptr<jsinspector_modern::RuntimeAgentDelegate>
JSIRuntimeHolder::createAgentDelegate(
    jsinspector_modern::FrontendChannel frontendChannel,
    jsinspector_modern::SessionState& sessionState,
    std::unique_ptr<jsinspector_modern::RuntimeAgentDelegate::ExportedState>,
    const jsinspector_modern::ExecutionContextDescription&
        executionContextDescription,
    RuntimeExecutor runtimeExecutor) {
  (void)executionContextDescription;
  (void)runtimeExecutor;
  return std::make_unique<jsinspector_modern::FallbackRuntimeAgentDelegate>(
      std::move(frontendChannel), sessionState, runtime_->description());
}

//windows base implementation needed as getRuntimeTargetDelegate is not pure virtual
jsinspector_modern::RuntimeTargetDelegate& JSRuntime::getRuntimeTargetDelegate() {
  if (!runtimeTargetDelegate_) {
    runtimeTargetDelegate_.emplace("JSRuntime");
  }
  return *runtimeTargetDelegate_;
}

} // namespace facebook::react