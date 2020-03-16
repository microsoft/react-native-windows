// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"

#include <windows.h>
#include "etw/react_native_windows.h"

#include "tracing/fbsystrace.h"

#include <jsi/jsi.h>

#include <array>
#include <string>

using namespace facebook;

namespace fbsystrace {

/*static */ uint64_t FbSystraceSection::s_id_counter = 0;

/*static */ std::unordered_map<int, std::chrono::high_resolution_clock::time_point> FbSystraceAsyncFlow::s_tracker_;
/*static */ std::mutex FbSystraceAsyncFlow::s_tracker_mutex_;

std::mutex g_pages_mutex;
/*static*/ void FbSystraceAsyncFlow::begin(uint64_t tag, const char *name, int cookie) {
  {
    std::scoped_lock lock{s_tracker_mutex_};
    s_tracker_[cookie] = std::chrono::high_resolution_clock::now();
  }

  EventWriteNATIVE_ASYNC_BEGIN_FLOW(tag, name, cookie, 0);
}

/*static */ void FbSystraceAsyncFlow::end(uint64_t tag, const char *name, int cookie) {
  double duration = -1;
  {
    std::scoped_lock lock{s_tracker_mutex_};
    auto search = s_tracker_.find(cookie);

    if (search != s_tracker_.end()) {
      duration = std::chrono::duration_cast<std::chrono::duration<double>>(
                     std::chrono::high_resolution_clock::now() - search->second)
                     .count();

      // Flow has ended. Clear the cookie tracker.
      s_tracker_.erase(cookie);
    }
  }

  EventWriteNATIVE_ASYNC_END_FLOW(tag, name, cookie, duration);
}

} // namespace fbsystrace

void fbsystrace_end_async_flow(uint64_t tag, const char *name, int callId) {
  fbsystrace::FbSystraceAsyncFlow::end(tag, name, callId);
}

namespace facebook {
namespace react {
namespace tracing {

void trace_begin_section(
    uint64_t id,
    uint64_t tag,
    const std::string &profile_name,
    std::array<std::string, SYSTRACE_SECTION_MAX_ARGS> &&args,
    uint8_t size) {
  EventWriteNATIVE_BEGIN_SECTION(
      id,
      tag,
      profile_name.c_str(),
      args[0].c_str(),
      args[1].c_str(),
      args[2].c_str(),
      args[3].c_str(),
      args[4].c_str(),
      args[5].c_str(),
      args[6].c_str(),
      args[7].c_str());
}

void trace_end_section(uint64_t id, uint64_t tag, const std::string &profile_name, double duration) {
  EventWriteNATIVE_END_SECTION(id, tag, profile_name.c_str(), duration);
}

void syncSectionBeginJSHook(uint64_t tag, const std::string &profile_name, const std::string &args) {
  EventWriteJS_BEGIN_SECTION(
      0, tag, profile_name.c_str(), args.c_str(), nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr);
}

void syncSectionEndJSHook(uint64_t tag) {
  EventWriteJS_END_SECTION(0, tag, "", 0);
}

void asyncSectionBeginJSHook(uint64_t tag, const std::string &profile_name, int cookie) {
  EventWriteJS_ASYNC_BEGIN_SECTION(tag, profile_name.c_str(), cookie, 0);
}

void asyncSectionEndJSHook(uint64_t tag, const std::string &profile_name, int cookie) {
  EventWriteJS_ASYNC_END_SECTION(tag, profile_name.c_str(), cookie, 0);
}

void asyncFlowBeginJSHook(uint64_t tag, const std::string &profile_name, int cookie) {
  EventWriteJS_ASYNC_BEGIN_FLOW(tag, profile_name.c_str(), cookie, 0);
}

void asyncFlowEndJSHook(uint64_t tag, const std::string &profile_name, int cookie) {
  EventWriteJS_ASYNC_END_FLOW(tag, profile_name.c_str(), cookie, 0);
}

void counterJSHook(uint64_t tag, const std::string &profile_name, int value) {
  EventWriteJS_COUNTER(tag, profile_name.c_str(), value);
}

void initializeJSHooks(jsi::Runtime &runtime) {
  // Don't hook up unless the provider is enabled.
  if (!EventEnabledJS_ASYNC_BEGIN_FLOW())
    return;

  runtime.global().setProperty(runtime, "__RCTProfileIsProfiling", true);

  runtime.global().setProperty(
      runtime,
      "nativeTraceBeginSection",
      jsi::Function::createFromHostFunction(
          runtime,
          jsi::PropNameID::forAscii(runtime, "nativeTraceBeginSection"),
          3,
          [](jsi::Runtime &runtime, const jsi::Value &, const jsi::Value *jsargs, size_t count) -> jsi::Value {
            if (count >= 1) {
              uint64_t tag = static_cast<uint64_t>(jsargs[0].getNumber());

              std::string profile_name = "unknown";
              if (count >= 2) {
                profile_name = jsargs[1].getString(runtime).utf8(runtime);
              }

              std::string args;
              if (count >= 3) {
                const jsi::Value &val = jsargs[2];
                if (val.isString())
                  args = val.getString(runtime).utf8(runtime);
                else if (!val.isUndefined())
                  args = "<unhandled>";
                else
                  args = "<undefined>";
              }

              syncSectionBeginJSHook(tag, profile_name, args);
            } else {
              throw std::runtime_error("nativeTraceBeginSection called without any arguments.");
            }
            return jsi::Value::undefined();
          }));

  runtime.global().setProperty(
      runtime,
      "nativeTraceEndSection",
      jsi::Function::createFromHostFunction(
          runtime,
          jsi::PropNameID::forAscii(runtime, "nativeTraceEndSection"),
          2,
          [](jsi::Runtime &runtime, const jsi::Value &, const jsi::Value *args, size_t count) -> jsi::Value {
            if (count >= 1) {
              uint64_t tag = static_cast<uint64_t>(args[0].getNumber());
              syncSectionEndJSHook(tag);
            } else {
              throw std::runtime_error("nativeTraceEndSection called without any arguments.");
            }
            return jsi::Value::undefined();
          }));

  runtime.global().setProperty(
      runtime,
      "nativeTraceBeginAsyncSection",
      jsi::Function::createFromHostFunction(
          runtime,
          jsi::PropNameID::forAscii(runtime, "nativeTraceBeginAsyncSection"),
          2,
          [](jsi::Runtime &runtime, const jsi::Value &, const jsi::Value *args, size_t count) -> jsi::Value {
            if (count >= 1) {
              uint64_t tag = static_cast<uint64_t>(args[0].getNumber());

              std::string profile_name = "unknown";
              if (count >= 2) {
                profile_name = args[1].getString(runtime).utf8(runtime);
              }

              int cookie = -1;
              if (count >= 3 && args[2].isNumber()) {
                cookie = static_cast<int>(args[2].asNumber());
              }

              asyncSectionBeginJSHook(tag, profile_name, cookie);
            } else {
              throw std::runtime_error("nativeTraceBeginAsyncSection called without any arguments.");
            }

            return jsi::Value::undefined();
          }));

  runtime.global().setProperty(
      runtime,
      "nativeTraceEndAsyncSection",
      jsi::Function::createFromHostFunction(
          runtime,
          jsi::PropNameID::forAscii(runtime, "nativeTraceEndAsyncSection"),
          2,
          [](jsi::Runtime &runtime, const jsi::Value &, const jsi::Value *args, size_t count) -> jsi::Value {
            if (count >= 1) {
              uint64_t tag = static_cast<uint64_t>(args[0].getNumber());

              std::string profile_name = "unknown";
              if (count >= 2) {
                profile_name = args[1].getString(runtime).utf8(runtime);
              }

              int cookie = -1;
              if (count >= 3 && args[2].isNumber()) {
                cookie = static_cast<int>(args[2].asNumber());
              }

              asyncSectionEndJSHook(tag, profile_name, cookie);
            } else {
              throw std::runtime_error("nativeTraceEndAsyncSection called without any arguments.");
            }

            return jsi::Value::undefined();
          }));
  runtime.global().setProperty(
      runtime,
      "nativeTraceBeginAsyncFlow",
      jsi::Function::createFromHostFunction(
          runtime,
          jsi::PropNameID::forAscii(runtime, "nativeTraceBeginAsyncFlow"),
          2,
          [](jsi::Runtime &runtime, const jsi::Value &, const jsi::Value *args, size_t count) -> jsi::Value {
            if (count >= 1) {
              uint64_t tag = static_cast<uint64_t>(args[0].getNumber());

              std::string profile_name = "unknown";
              if (count >= 2) {
                profile_name = args[1].getString(runtime).utf8(runtime);
              }

              int cookie = -1;
              if (count >= 3 && args[2].isNumber()) {
                cookie = static_cast<int>(args[2].asNumber());
              }

              asyncFlowBeginJSHook(tag, profile_name, cookie);
            } else {
              throw std::runtime_error("nativeTraceBeginAsyncFlow called without any arguments.");
            }

            return jsi::Value::undefined();
          }));
  runtime.global().setProperty(
      runtime,
      "nativeTraceEndAsyncFlow",
      jsi::Function::createFromHostFunction(
          runtime,
          jsi::PropNameID::forAscii(runtime, "nativeTraceEndAsyncFlow"),
          2,
          [](jsi::Runtime &runtime, const jsi::Value &, const jsi::Value *args, size_t count) -> jsi::Value {
            if (count >= 1) {
              uint64_t tag = static_cast<uint64_t>(args[0].getNumber());

              std::string profile_name = "unknown";
              if (count >= 2) {
                profile_name = args[1].getString(runtime).utf8(runtime);
              }

              int cookie = -1;
              if (count >= 3 && args[2].isNumber()) {
                cookie = static_cast<int>(args[2].asNumber());
              }

              asyncFlowEndJSHook(tag, profile_name, cookie);
            } else {
              throw std::runtime_error("nativeTraceEndAsyncFlow called without any arguments.");
            }

            return jsi::Value::undefined();
          }));

  runtime.global().setProperty(
      runtime,
      "nativeTraceCounter",
      jsi::Function::createFromHostFunction(
          runtime,
          jsi::PropNameID::forAscii(runtime, "nativeTraceCounter"),
          2,
          [](jsi::Runtime &runtime, const jsi::Value &, const jsi::Value *args, size_t count) -> jsi::Value {
            if (count >= 1) {
              uint64_t tag = static_cast<uint64_t>(args[0].getNumber());

              std::string profile_name = "unknown";
              if (count >= 2) {
                profile_name = args[1].getString(runtime).utf8(runtime);
              }

              int value = -1;
              if (count >= 3 && args[2].isNumber()) {
                value = static_cast<int>(args[2].asNumber());
              }

              counterJSHook(tag, profile_name, value);
            } else {
              throw std::runtime_error("nativeTraceEndAsyncFlow called without any arguments.");
            }

            return jsi::Value::undefined();
          }));
}

void initializeETW() {
  EventRegisterReact_Native_Windows_Provider();
}

} // namespace tracing
} // namespace react
} // namespace facebook
