/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "JSExecutor.h"

#include "RAMBundleRegistry.h"

#include <folly/Conv.h>
#include <jsinspector-modern/ReactCdp.h>
#include <jsinspector-modern/tracing/InstanceTracingProfile.h>
#include <react/timing/primitives.h>

#include <chrono>

namespace facebook::react {

std::string JSExecutor::getSyntheticBundlePath(
    uint32_t bundleId,
    const std::string& bundlePath) {
  if (bundleId == RAMBundleRegistry::MAIN_BUNDLE_ID) {
    return bundlePath;
  }
  return folly::to<std::string>("seg-", bundleId, ".js");
}

double JSExecutor::performanceNow() {
  return chronoToDOMHighResTimeStamp(std::chrono::steady_clock::now());
}

void JSExecutor::addConsoleMessage(jsi::Runtime& runtime, jsinspector_modern::ConsoleMessage message){
    return;
}

bool JSExecutor::supportsConsole() const {
    return false;
}

std::unique_ptr<facebook::react::jsinspector_modern::StackTrace> JSExecutor::captureStackTrace(
    facebook::jsi::Runtime &runtime,
    size_t framesToSkip) {
  return std::make_unique<facebook::react::jsinspector_modern::StackTrace>();
}

void JSExecutor::enableSamplingProfiler() {
  return; // [Windows TODO: stubbed implementation #14700]
}

void JSExecutor::disableSamplingProfiler()  {
  return; // [Windows TODO: stubbed implementation #14700]
}

facebook::react::jsinspector_modern::tracing::RuntimeSamplingProfile JSExecutor::collectSamplingProfile() {
  return facebook::react::jsinspector_modern::tracing::RuntimeSamplingProfile(
      "stubbed_impl", {}); // [Windows TODO: stubbed implementation #14700]
}

std::unique_ptr<jsinspector_modern::RuntimeAgentDelegate>
JSExecutor::createAgentDelegate(
    jsinspector_modern::FrontendChannel frontendChannel,
    jsinspector_modern::SessionState& sessionState,
    std::unique_ptr<jsinspector_modern::RuntimeAgentDelegate::ExportedState>,
    const jsinspector_modern::ExecutionContextDescription&
        executionContextDescription,
    RuntimeExecutor runtimeExecutor) {
  (void)executionContextDescription;
  (void)runtimeExecutor;
  return std::make_unique<jsinspector_modern::FallbackRuntimeAgentDelegate>(
      std::move(frontendChannel), sessionState, getDescription());
}

} // namespace facebook::react