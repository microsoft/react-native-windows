// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

// This file must match the code in React Native folder:
// ReactCommon/hermes/inspector-modern/chrome/HermesRuntimeTargetDelegate.cpp
// Unlike the code in React Native sources, this class delegates calls to Hermes C-based API.
// We use different namespace for this class comparing with the RN code.

#include "HermesRuntimeTargetDelegate.h"
#include <jsinspector-modern/RuntimeTarget.h>
#include <utility>
#include "HermesRuntimeAgentDelegate.h"

using namespace facebook::react::jsinspector_modern;

namespace Microsoft::ReactNative {

namespace {

const uint16_t HERMES_SAMPLING_FREQUENCY_HZ = 10000;

class HermesStackTraceWrapper : public StackTrace {
 public:
  explicit HermesStackTraceWrapper(HermesUniqueStackTrace &&hermesStackTrace)
      : hermesStackTrace_{std::move(hermesStackTrace)} {}

  HermesUniqueStackTrace &operator*() {
    return hermesStackTrace_;
  }

  HermesUniqueStackTrace *operator->() {
    return &hermesStackTrace_;
  }

 private:
  HermesUniqueStackTrace hermesStackTrace_;
};

} // namespace

HermesRuntimeTargetDelegate::HermesRuntimeTargetDelegate(std::shared_ptr<HermesRuntimeHolder> &&hermesRuntimeHolder)
    : hermesRuntimeHolder_(std::move(hermesRuntimeHolder)),
      hermesCdpDebugApi_(HermesInspectorApi::createCdpDebugApi(hermesRuntimeHolder_->getHermesRuntime())) {}

HermesRuntimeTargetDelegate::~HermesRuntimeTargetDelegate() = default;

hermes_cdp_debug_api HermesRuntimeTargetDelegate::getCdpDebugApi() {
  return hermesCdpDebugApi_.get();
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

void HermesRuntimeTargetDelegate::addConsoleMessage(facebook::jsi::Runtime &runtime, ConsoleMessage message) {
  // Convert ConsoleAPIType to hermes_console_api_type
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

  // Create a jsi::Array from the vector of jsi::Values
  facebook::jsi::Array argsArray(runtime, message.args.size());
  for (size_t i = 0; i < message.args.size(); ++i) {
    argsArray.setValueAtIndex(runtime, i, std::move(message.args[i]));
  }

  // Store array as a temporary global property
  // Using a property name that's unlikely to collide with user code
  const char *propName = "__rnw_cdp_console_args";
  runtime.global().setProperty(runtime, propName, argsArray);

  // Convert stack trace to HermesUniqueStackTrace if available
  HermesUniqueStackTrace hermesStackTrace{};
  if (auto hermesStackTraceWrapper = dynamic_cast<HermesStackTraceWrapper *>(message.stackTrace.get())) {
    hermesStackTrace = std::move(**hermesStackTraceWrapper);
  }

  // Call C API with property name instead of serialized args
  // The property will be cleaned up by the Hermes side
  HermesInspectorApi::addConsoleMessage(
      hermesCdpDebugApi_.get(), message.timestamp, type, propName, hermesStackTrace.get());
}

bool HermesRuntimeTargetDelegate::supportsConsole() const {
  return true;
}

std::unique_ptr<StackTrace> HermesRuntimeTargetDelegate::captureStackTrace(
    facebook::jsi::Runtime & /*runtime*/,
    size_t /*framesToSkip*/) {
  return std::make_unique<HermesStackTraceWrapper>(
      HermesInspectorApi::captureStackTrace(hermesRuntimeHolder_->getHermesRuntime()));
}

void HermesRuntimeTargetDelegate::enableSamplingProfiler() {
  HermesInspectorApi::enableSamplingProfiler(hermesRuntimeHolder_->getHermesRuntime());
}

void HermesRuntimeTargetDelegate::disableSamplingProfiler() {
  HermesInspectorApi::disableSamplingProfiler(hermesRuntimeHolder_->getHermesRuntime());
}

namespace {

// Helper class to hold state while reading the sampling profile
struct SamplingProfileReaderState {
  std::vector<tracing::RuntimeSamplingProfile::Sample> samples;
  std::vector<tracing::RuntimeSamplingProfile::SampleCallStackFrame> frames;
  uint64_t timestamp;
  uint64_t threadId;
  bool hasCurrentSample;
};

static void NAPI_CDECL onInfo(void *cb_data, size_t sample_count) {
  SamplingProfileReaderState *readerState = reinterpret_cast<SamplingProfileReaderState *>(cb_data);
  readerState->samples.reserve(sample_count);
}

// Callback invoked for each sample
static void NAPI_CDECL onSample(void *cb_data, uint64_t timestamp, uint64_t threadId, size_t frame_count) {
  SamplingProfileReaderState *readerState = reinterpret_cast<SamplingProfileReaderState *>(cb_data);
  if (readerState->hasCurrentSample) {
    // Save the previous sample
    readerState->samples.emplace_back(readerState->timestamp, readerState->threadId, std::move(readerState->frames));
  }
  std::vector<tracing::RuntimeSamplingProfile::SampleCallStackFrame> frames;
  frames.reserve(frame_count);
  readerState->frames = std::move(frames);
  readerState->timestamp = timestamp;
  readerState->threadId = threadId;
  readerState->hasCurrentSample = true;
}

// Callback invoked for each frame within a sample
static void NAPI_CDECL onFrame(
    void *cb_data,
    hermes_call_stack_frame_kind kind,
    uint32_t scriptId,
    const char *functionName,
    size_t functionNameSize,
    const char *scriptUrl,
    size_t scriptUrlSize,
    uint32_t lineNumber,
    uint32_t columnNumber) {
  SamplingProfileReaderState *readerState = reinterpret_cast<SamplingProfileReaderState *>(cb_data);

  using Kind = tracing::RuntimeSamplingProfile::SampleCallStackFrame::Kind;

  Kind frameKind;
  switch (kind) {
    case hermes_call_stack_frame_kind_js_function:
      frameKind = Kind::JSFunction;
      break;
    case hermes_call_stack_frame_kind_native_function:
      frameKind = Kind::NativeFunction;
      break;
    case hermes_call_stack_frame_kind_host_function:
      frameKind = Kind::HostFunction;
      break;
    case hermes_call_stack_frame_kind_gc:
      frameKind = Kind::GarbageCollector;
      break;
    default:
      return; // Unknown frame kind, skip
  }

  std::string funcName(functionName, functionNameSize);
  std::optional<std::string> url = scriptUrl ? std::optional{std::string(scriptUrl, scriptUrlSize)} : std::nullopt;
  std::optional<uint32_t> line = lineNumber > 0 ? std::optional{lineNumber} : std::nullopt;
  std::optional<uint32_t> column = columnNumber > 0 ? std::optional{columnNumber} : std::nullopt;

  readerState->frames.emplace_back(frameKind, scriptId, std::move(funcName), std::move(url), line, column);
}

} // namespace

tracing::RuntimeSamplingProfile HermesRuntimeTargetDelegate::collectSamplingProfile() {
  // Create a readerState state to gather samples and frames
  SamplingProfileReaderState readerState{};

  // Collect the profile from Hermes
  HermesUniqueSamplingProfile profile = HermesInspectorApi::collectSamplingProfile(
      hermesRuntimeHolder_->getHermesRuntime(), &readerState, onInfo, onSample, onFrame);

  if (readerState.hasCurrentSample) {
    // Save the last sample
    readerState.samples.emplace_back(readerState.timestamp, readerState.threadId, std::move(readerState.frames));
  }

  // Return the complete profile with samples. Wrap the raw profile since it owns the strings.
  return tracing::RuntimeSamplingProfile("Hermes", std::move(readerState.samples));
}

} // namespace Microsoft::ReactNative
