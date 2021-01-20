// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"

#include <TraceLoggingProvider.h>
#include <jsi/jsi.h>
#include "tracing/fbsystrace.h"

#include <array>
#include <string>

// Define the GUID to use in TraceLoggingProviderRegister
// {910FB9A1-75DD-4CF4-BEEC-DA21341F20C8}
TRACELOGGING_DEFINE_PROVIDER(
    g_hTraceLoggingProvider,
    "Microsoft.ReactNativeWindows",
    (0x910fb9a1, 0x75dd, 0x4cf4, 0xbe, 0xec, 0xda, 0x21, 0x34, 0x1f, 0x20, 0xc8));

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

  TraceLoggingWrite(
      g_hTraceLoggingProvider,
      "SystraceNativeAsyncFlow",
      TraceLoggingString("begin", "op"),
      TraceLoggingString(name, "profile_name"),
      TraceLoggingUInt64(tag, "tag"),
      TraceLoggingInt64(cookie, "cookie"));
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

  TraceLoggingWrite(
      g_hTraceLoggingProvider,
      "SystraceNativeAsyncFlow",
      TraceLoggingString("end", "op"),
      TraceLoggingString(name, "profile_name"),
      TraceLoggingUInt64(tag, "tag"),
      TraceLoggingInt64(cookie, "cookie"));
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
  TraceLoggingWrite(
      g_hTraceLoggingProvider,
      "SystraceNativeSection",
      TraceLoggingString("begin", "op"),
      TraceLoggingString(profile_name.c_str(), "profile_name"),
      TraceLoggingUInt64(tag, "tag"),
      TraceLoggingString(args[0].c_str(), "arg0"),
      TraceLoggingString(args[1].c_str(), "arg1"),
      TraceLoggingString(args[2].c_str(), "arg2"),
      TraceLoggingString(args[3].c_str(), "arg3"),
      TraceLoggingString(args[4].c_str(), "arg4"),
      TraceLoggingString(args[5].c_str(), "arg5"),
      TraceLoggingString(args[6].c_str(), "arg6"),
      TraceLoggingString(args[7].c_str(), "arg7"));
}

void trace_end_section(uint64_t id, uint64_t tag, const std::string &profile_name, double duration) {
  TraceLoggingWrite(
      g_hTraceLoggingProvider,
      "SystraceNativeSection",
      TraceLoggingString("end", "op"),
      TraceLoggingString(profile_name.c_str(), "profile_name"),
      TraceLoggingUInt64(tag, "tag"),
      TraceLoggingFloat64(duration, "duration"));
}

void syncSectionBeginJSHook(uint64_t tag, const std::string &profile_name, const std::string &args) {
  TraceLoggingWrite(
      g_hTraceLoggingProvider,
      "SystraceJSSection",
      TraceLoggingString("begin", "op"),
      TraceLoggingUInt64(tag, "tag"),
      TraceLoggingString(profile_name.c_str(), "profile_name"),
      TraceLoggingString(args.c_str(), "arg"));
}

void syncSectionEndJSHook(uint64_t tag) {
  TraceLoggingWrite(
      g_hTraceLoggingProvider, "SystraceJSSection", TraceLoggingString("end", "op"), TraceLoggingUInt64(tag, "tag"));
}

void asyncSectionBeginJSHook(uint64_t tag, const std::string &profile_name, int cookie) {
  TraceLoggingWrite(
      g_hTraceLoggingProvider,
      "SystraceJSAsyncSection",
      TraceLoggingString("begin", "op"),
      TraceLoggingString(profile_name.c_str(), "profile_name"),
      TraceLoggingUInt64(tag, "tag"),
      TraceLoggingInt64(cookie, "cookie"));
}

void asyncSectionEndJSHook(uint64_t tag, const std::string &profile_name, int cookie) {
  TraceLoggingWrite(
      g_hTraceLoggingProvider,
      "SystraceJSAsyncSection",
      TraceLoggingString("end", "op"),
      TraceLoggingString(profile_name.c_str(), "profile_name"),
      TraceLoggingUInt64(tag, "tag"),
      TraceLoggingInt64(cookie, "cookie"));
}

void asyncFlowBeginJSHook(uint64_t tag, const std::string &profile_name, int cookie) {
  TraceLoggingWrite(
      g_hTraceLoggingProvider,
      "SystraceJSAsyncFlow",
      TraceLoggingString("begin", "op"),
      TraceLoggingString(profile_name.c_str(), "profile_name"),
      TraceLoggingUInt64(tag, "tag"),
      TraceLoggingInt64(cookie, "cookie"));
}

void asyncFlowEndJSHook(uint64_t tag, const std::string &profile_name, int cookie) {
  TraceLoggingWrite(
      g_hTraceLoggingProvider,
      "SystraceJSAsyncFlow",
      TraceLoggingString("end", "op"),
      TraceLoggingString(profile_name.c_str(), "profile_name"),
      TraceLoggingUInt64(tag, "tag"),
      TraceLoggingInt64(cookie, "cookie"));
}

void counterJSHook(uint64_t tag, const std::string &profile_name, int value) {
  TraceLoggingWrite(
      g_hTraceLoggingProvider,
      "SystraceCounter",
      TraceLoggingString(profile_name.c_str(), "profile_name"),
      TraceLoggingUInt64(tag, "tag"),
      TraceLoggingInt64(value, "value"));
}

void initializeJSHooks(jsi::Runtime &runtime) {
  // TODO:: Assess the performance impact of hooking up the JS trace events. Especially when the tracing is not enabled.
  // If significant, this should be put under flag based on devsettings

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
  // Register the provider
  TraceLoggingRegister(g_hTraceLoggingProvider);
}

} // namespace tracing
} // namespace react
} // namespace facebook
