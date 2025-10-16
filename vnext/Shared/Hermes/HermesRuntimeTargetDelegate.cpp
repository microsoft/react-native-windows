// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "HermesRuntimeTargetDelegate.h"
#include <jsinspector-modern/RuntimeTarget.h>
#include <utility>
#include "HermesRuntimeAgentDelegate.h"

using namespace facebook::react::jsinspector_modern;
using namespace facebook::hermes;

namespace Microsoft::ReactNative {

namespace {

class HermesStackTraceWrapper : public StackTrace {
 public:
  explicit HermesStackTraceWrapper(HermesUniqueStackTrace &&hermesStackTrace)
      : hermesStackTrace_{std::move(hermesStackTrace)} {}

  HermesUniqueStackTrace &operator*() {
    return hermesStackTrace_;
  }

 private:
  HermesUniqueStackTrace hermesStackTrace_;
};

} // namespace

HermesRuntimeTargetDelegate::HermesRuntimeTargetDelegate(std::shared_ptr<HermesRuntimeHolder> hermesRuntimeHolder)
    : hermesRuntimeHolder_(std::move(hermesRuntimeHolder)),
      hermesCdpDebugger_(HermesApi2().createCdpDebugger(hermesRuntimeHolder_->getHermesRuntime())) {}

HermesRuntimeTargetDelegate::~HermesRuntimeTargetDelegate() = default;

hermes_cdp_debugger HermesRuntimeTargetDelegate::getCdpDebugger() {
  return hermesCdpDebugger_.get();
}

std::unique_ptr<RuntimeAgentDelegate> HermesRuntimeTargetDelegate::createAgentDelegate(
    FrontendChannel frontendChannel,
    SessionState &sessionState,
    std::unique_ptr<RuntimeAgentDelegate::ExportedState> previouslyExportedState,
    const ExecutionContextDescription &executionContextDescription,
    facebook::react::RuntimeExecutor runtimeExecutor) {
  return std::unique_ptr<RuntimeAgentDelegate>(new HermesRuntimeAgentDelegate(
      frontendChannel,
      sessionState,
      std::move(previouslyExportedState),
      executionContextDescription,
      hermesRuntimeHolder_->getHermesRuntime(),
      *this,
      std::move(runtimeExecutor)));
}

void HermesRuntimeTargetDelegate::addConsoleMessage(facebook::jsi::Runtime & /*runtime*/, ConsoleMessage message) {
  hermes_console_api_type type{};
  switch (message.type) {
    case ConsoleAPIType::kLog:
      type = hermes_console_api_type_log;
      break;
    case ConsoleAPIType::kDebug:
      type = hermes_console_api_type_debug;
      break;
    case ConsoleAPIType::kInfo:
      type = hermes_console_api_type_info;
      break;
    case ConsoleAPIType::kError:
      type = hermes_console_api_type_error;
      break;
    case ConsoleAPIType::kWarning:
      type = hermes_console_api_type_warning;
      break;
    case ConsoleAPIType::kDir:
      type = hermes_console_api_type_dir;
      break;
    case ConsoleAPIType::kDirXML:
      type = hermes_console_api_type_dir_xml;
      break;
    case ConsoleAPIType::kTable:
      type = hermes_console_api_type_table;
      break;
    case ConsoleAPIType::kTrace:
      type = hermes_console_api_type_trace;
      break;
    case ConsoleAPIType::kStartGroup:
      type = hermes_console_api_type_start_group;
      break;
    case ConsoleAPIType::kStartGroupCollapsed:
      type = hermes_console_api_type_start_group_collapsed;
      break;
    case ConsoleAPIType::kEndGroup:
      type = hermes_console_api_type_end_group;
      break;
    case ConsoleAPIType::kClear:
      type = hermes_console_api_type_clear;
      break;
    case ConsoleAPIType::kAssert:
      type = hermes_console_api_type_assert;
      break;
    case ConsoleAPIType::kTimeEnd:
      type = hermes_console_api_type_time_end;
      break;
    case ConsoleAPIType::kCount:
      type = hermes_console_api_type_count;
      break;
    default:
      throw std::logic_error{"Unknown console message type"};
  }

  HermesUniqueStackTrace hermesStackTrace{};
  if (auto hermesStackTraceWrapper = dynamic_cast<HermesStackTraceWrapper *>(message.stackTrace.get())) {
    hermesStackTrace = std::move(**hermesStackTraceWrapper);
  }

  // TODO: Implement this
  // HermesApi2().addConsoleMessage(
  //    hermesCdpDebugger_.get(),
  //    message.timestamp,
  //    type,
  //    std::move(message.args),
  //    hermesStackTrace.release());
}

bool HermesRuntimeTargetDelegate::supportsConsole() const {
  return true;
}

std::unique_ptr<StackTrace> HermesRuntimeTargetDelegate::captureStackTrace(
    facebook::jsi::Runtime & /*runtime*/,
    size_t /*framesToSkip*/) {
  return std::make_unique<HermesStackTraceWrapper>(
      HermesApi2().captureStackTrace(hermesRuntimeHolder_->getHermesRuntime()));
}

void HermesRuntimeTargetDelegate::enableSamplingProfiler() {
  return; // [Windows TODO: stubbed implementation #14700]
}

void HermesRuntimeTargetDelegate::disableSamplingProfiler() {
  return; // [Windows TODO: stubbed implementation #14700]
}

facebook::react::jsinspector_modern::tracing::RuntimeSamplingProfile HermesRuntimeTargetDelegate::collectSamplingProfile() {
  return facebook::react::jsinspector_modern::tracing::RuntimeSamplingProfile(
      "stubbed_impl", {}); // [Windows TODO: stubbed implementation #14700]
}

} // namespace Microsoft::ReactNative
