#pragma once

#include <stdint.h>
#include <string.h>
#include <array>
#include <chrono>
#include <mutex>
#include <string>
#include <unordered_map>

#define TRACE_TAG_REACT_CXX_BRIDGE 1 << 10
#define TRACE_TAG_REACT_APPS 1 << 11

// Assuming maximum 8 arguments.
// TODO :: We assume this magic number "8" at a couple of other places without
// proper static verfications.
#define SYSTRACE_SECTION_MAX_ARGS 8

namespace facebook {
namespace react {
namespace tracing {

enum class TraceTask : uint16_t {
  EvaluateScript,
  CallJSFunction,
  CallNativeModules,
  Unknown
};

void trace_begin_section(
    uint64_t tag,
    const std::string &profile_name,
    std::array<std::string, SYSTRACE_SECTION_MAX_ARGS> &&args,
    uint8_t size,
    TraceTask task);

void trace_end_section(
    uint64_t tag,
    const std::string &profile_name,
    double duration,
    TraceTask task);
} // namespace tracing
} // namespace react
} // namespace facebook

void fbsystrace_end_async_flow(uint64_t tag, const char *name, int callId);

namespace fbsystrace {

class FbSystraceSection {
 public:
  void begin_section() {
    facebook::react::tracing::trace_begin_section(
        tag_, profile_name_, std::move(args_), index_, task_);
  }

  void end_section() {
    facebook::react::tracing::trace_end_section(
        tag_,
        profile_name_,
        std::chrono::duration_cast<std::chrono::duration<double>>(
            std::chrono::high_resolution_clock::now() - start_)
            .count(),
        task_);
  }

  void init(std::string &&v) {
    args_[index_++] = std::move(v);
    begin_section();
  }

  void init(const std::string &v) {
    args_[index_++] = v; // copy
    begin_section();
  }

  void init() {
    begin_section();
  }

  template <typename... ConvertsToStringPiece>
  void init(std::string &&v, ConvertsToStringPiece &&... rest) {
    args_[index_++] = std::move(v);
    init(std::forward<ConvertsToStringPiece>(rest)...);
  }

  template <typename... ConvertsToStringPiece>
  void init(const std::string &v, ConvertsToStringPiece &&... rest) {
    args_[index_++] = v; // copy
    init(std::forward<ConvertsToStringPiece>(rest)...);
  }

  template <typename... ConvertsToStringPiece>
  FbSystraceSection(
      uint64_t tag,
      std::string &&v,
      ConvertsToStringPiece &&... rest)
      : tag_(tag), profile_name_(std::move(v)) {
    // Note : We don't want to add any fuzzy text search here as they are
    // usually a lot more expensive ..
    if (profile_name_.compare("JSIExecutor::loadApplicationScript") == 0) {
      task_ = facebook::react::tracing::TraceTask::EvaluateScript;
    } else if (profile_name_.compare("JSIExecutor::callFunction") == 0) {
      task_ = facebook::react::tracing::TraceTask::CallJSFunction;
    } else if (profile_name_.compare("JSIExecutor::callNativeModules") == 0) {
      task_ = facebook::react::tracing::TraceTask::CallNativeModules;
    }

    init(std::forward<ConvertsToStringPiece>(rest)...);
  }

  ~FbSystraceSection() {
    end_section();
  }

 private:
  std::array<std::string, SYSTRACE_SECTION_MAX_ARGS> args_;
  uint64_t tag_{0};
  std::string profile_name_;
  uint8_t index_{0};
  facebook::react::tracing::TraceTask task_{
      facebook::react::tracing::TraceTask::Unknown};

  std::chrono::high_resolution_clock::time_point start_{
      std::chrono::high_resolution_clock::now()};
};

struct FbSystraceAsyncFlow {
  static void begin(uint64_t tag, const char *name, int cookie);
  static void end(uint64_t tag, const char *name, int cookie);

  static std::mutex s_tracker_mutex_;
  static std::unordered_map<int, std::chrono::high_resolution_clock::time_point>
      s_tracker_;
};
} // namespace fbsystrace
