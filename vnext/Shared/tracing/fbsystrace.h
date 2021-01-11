// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include <stdint.h>
#include <string.h>
#include <array>
#include <chrono>
#include <mutex>
#include <string>
#include <type_traits>
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

void trace_begin_section(
    uint64_t id,
    uint64_t tag,
    const std::string &profile_name,
    std::array<std::string, SYSTRACE_SECTION_MAX_ARGS> &&args,
    uint8_t size);

void trace_end_section(uint64_t id, uint64_t tag, const std::string &profile_name, double duration);
} // namespace tracing
} // namespace react
} // namespace facebook

void fbsystrace_end_async_flow(uint64_t tag, const char *name, int callId);

namespace fbsystrace {

class FbSystraceSection {
 public:
  void begin_section() {
    facebook::react::tracing::trace_begin_section(id_, tag_, profile_name_, std::move(args_), index_);
  }

  void end_section() {
    facebook::react::tracing::trace_end_section(
        id_,
        tag_,
        profile_name_,
        std::chrono::duration_cast<std::chrono::duration<double>>(std::chrono::high_resolution_clock::now() - start_)
            .count());
  }

  template <typename... RestArg>
  FbSystraceSection(uint64_t tag, std::string &&profileName, RestArg &&... rest)
      : tag_(tag), profile_name_(std::move(profileName)) {
    id_ = s_id_counter++;
    init(std::forward<RestArg>(rest)...);
  }

  ~FbSystraceSection() {
    end_section();
  }

 private:
  void init() {
    begin_section();
  }

  template <typename Arg, typename... RestArg>
  void init(Arg &&arg, RestArg &&... rest) {
    if constexpr (std::is_convertible_v<Arg, std::string>) {
      args_[index_++] = std::forward<Arg>(arg);
    } else {
      args_[index_++] = std::to_string(std::forward<Arg>(arg));
    }

    init(std::forward<RestArg>(rest)...);
  }

  std::array<std::string, SYSTRACE_SECTION_MAX_ARGS> args_;
  uint64_t tag_{0};

  static uint64_t s_id_counter;
  uint64_t id_;

  std::string profile_name_;
  uint8_t index_{0};

  std::chrono::high_resolution_clock::time_point start_{std::chrono::high_resolution_clock::now()};
};

struct FbSystraceAsyncFlow {
  static void begin(uint64_t tag, const char *name, int cookie);
  static void end(uint64_t tag, const char *name, int cookie);

  static std::mutex s_tracker_mutex_;
  static std::unordered_map<int, std::chrono::high_resolution_clock::time_point> s_tracker_;
};
} // namespace fbsystrace
