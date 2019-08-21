// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"

#include "rnwtrace.h"

#include "windows.h"
#include "etw/react_native_windows.h"

#include <fstream>

using namespace facebook;

void nativeTraceBeginSection(
    uint64_t tag,
    const std::string &profile_name,
    const std::string &args) {

  EventWriteJS_BEGIN_SECTION(tag, profile_name.c_str(), args.c_str());
  
  /*std::ofstream stream;
  stream.open(
      "C:\\Users\\anandrag.REDMOND\\AppData\\Local\\Packages\\59b7e31b-a8fe-4fc7-85d6-a0a2475ad32e_kc2bncckyf4ap\\LocalState\\jstrace.txt",
      std::ios_base::app);

  stream << "nativeTraceBeginSection"
         << " ###### " << tag << " ####### " << profile_name << " ####### "
         << args << std::endl;
  stream.close();*/
}

void nativeTraceEndSection(uint64_t tag) {
  EventWriteJS_END_SECTION(tag);
  /*std::ofstream stream;
  stream.open(
      "C:\\Users\\anandrag.REDMOND\\AppData\\Local\\Packages\\59b7e31b-a8fe-4fc7-85d6-a0a2475ad32e_kc2bncckyf4ap\\LocalState\\jstrace.txt",
      std::ios_base::app);

  stream << "nativeTraceEndSection"
         << " ###### " << tag << std::endl;
  stream.close();*/
}

void nativeTraceBeginAsyncSection(
    uint64_t tag,
    const std::string &profile_name,
    int cookie) {

  EventWriteJS_ASYNC_BEGIN_SECTION(tag, profile_name.c_str(), cookie);

  /*std::ofstream stream;
  stream.open(
      "C:\\Users\\anandrag.REDMOND\\AppData\\Local\\Packages\\59b7e31b-a8fe-4fc7-85d6-a0a2475ad32e_kc2bncckyf4ap\\LocalState\\jstrace.txt",
      std::ios_base::app);

  stream << "nativeTraceBeginAsyncSection"
         << " ###### " << tag << " ####### " << profile_name << " ####### "
         << cookie << std::endl;
  stream.close();*/
}

void nativeTraceEndAsyncSection(
    uint64_t tag,
    const std::string &profile_name,
    int cookie) {
  EventWriteJS_ASYNC_END_SECTION(tag, profile_name.c_str(), cookie);
  /*std::ofstream stream;
  stream.open(
      "C:\\Users\\anandrag.REDMOND\\AppData\\Local\\Packages\\59b7e31b-a8fe-4fc7-85d6-a0a2475ad32e_kc2bncckyf4ap\\LocalState\\jstrace.txt",
      std::ios_base::app);

  stream << "nativeTraceEndAsyncSection"
         << " ###### " << tag << " ####### " << profile_name << " ####### "
         << cookie << std::endl;
  stream.close();*/
}

void nativeTraceBeginAsyncFlow(
    uint64_t tag,
    const std::string &profile_name,
    int cookie) {
  EventWriteJS_ASYNC_BEGIN_FLOW(tag, profile_name.c_str(), cookie);
  /*std::ofstream stream;
  stream.open(
      "C:\\Users\\anandrag.REDMOND\\AppData\\Local\\Packages\\59b7e31b-a8fe-4fc7-85d6-a0a2475ad32e_kc2bncckyf4ap\\LocalState\\jstrace.txt",
      std::ios_base::app);

  stream << "nativeTraceBeginAsyncFlow"
         << " ###### " << tag << " ####### " << profile_name << " ####### "
         << cookie << std::endl;
  stream.close();*/
}

void nativeTraceEndAsyncFlow(
    uint64_t tag,
    const std::string &profile_name,
    int cookie) {
  EventWriteJS_ASYNC_END_FLOW(tag, profile_name.c_str(), cookie);
  /*std::ofstream stream;
  stream.open(
      "C:\\Users\\anandrag.REDMOND\\AppData\\Local\\Packages\\59b7e31b-a8fe-4fc7-85d6-a0a2475ad32e_kc2bncckyf4ap\\LocalState\\jstrace.txt",
      std::ios_base::app);

  stream << "nativeTraceEndAsyncFlow"
         << " ###### " << tag << " ####### " << profile_name << " ####### "
         << cookie << std::endl;
  stream.close();*/
}

void nativeTraceCounter(
    uint64_t tag,
    const std::string &profile_name,
    int value) {
  EventWriteJS_COUNTER(tag, profile_name.c_str(), value);
  /*std::ofstream stream;
  stream.open(
      "C:\\Users\\anandrag.REDMOND\\AppData\\Local\\Packages\\59b7e31b-a8fe-4fc7-85d6-a0a2475ad32e_kc2bncckyf4ap\\LocalState\\jstrace.txt",
      std::ios_base::app);

  stream << "nativeTraceCounter"
         << " ###### " << tag << " ####### " << profile_name << " ####### "
         << value << std::endl;
  stream.close();*/
}

void init_tracing(jsi::Runtime &runtime) {
  runtime.global().setProperty(runtime, "__RCTProfileIsProfiling", true);

  runtime.global().setProperty(
      runtime,
      "nativeTraceBeginSection",
      jsi::Function::createFromHostFunction(
          runtime,
          jsi::PropNameID::forAscii(runtime, "nativeTraceBeginSection"),
          3,
          [](jsi::Runtime &runtime,
             const jsi::Value &,
             const jsi::Value *jsargs,
             size_t count) -> jsi::Value {
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

              nativeTraceBeginSection(tag, profile_name, args);
            } else {
              throw std::runtime_error(
                  "nativeTraceBeginSection called without any arguments.");
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
          [](jsi::Runtime &runtime,
             const jsi::Value &,
             const jsi::Value *args,
             size_t count) -> jsi::Value {
            if (count >= 1) {
              uint64_t tag = static_cast<uint64_t>(args[0].getNumber());
              nativeTraceEndSection(tag);
            } else {
              throw std::runtime_error(
                  "nativeTraceEndSection called without any arguments.");
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
          [](jsi::Runtime &runtime,
             const jsi::Value &,
             const jsi::Value *args,
             size_t count) -> jsi::Value {
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

              nativeTraceBeginAsyncSection(tag, profile_name, cookie);
            } else {
              throw std::runtime_error(
                  "nativeTraceBeginAsyncSection called without any arguments.");
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
          [](jsi::Runtime &runtime,
             const jsi::Value &,
             const jsi::Value *args,
             size_t count) -> jsi::Value {
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

              nativeTraceEndAsyncSection(tag, profile_name, cookie);
            } else {
              throw std::runtime_error(
                  "nativeTraceEndAsyncSection called without any arguments.");
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
          [](jsi::Runtime &runtime,
             const jsi::Value &,
             const jsi::Value *args,
             size_t count) -> jsi::Value {
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

              nativeTraceBeginAsyncFlow(tag, profile_name, cookie);
            } else {
              throw std::runtime_error(
                  "nativeTraceBeginAsyncFlow called without any arguments.");
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
          [](jsi::Runtime &runtime,
             const jsi::Value &,
             const jsi::Value *args,
             size_t count) -> jsi::Value {
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

              nativeTraceEndAsyncFlow(tag, profile_name, cookie);
            } else {
              throw std::runtime_error(
                  "nativeTraceEndAsyncFlow called without any arguments.");
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
          [](jsi::Runtime &runtime,
             const jsi::Value &,
             const jsi::Value *args,
             size_t count) -> jsi::Value {
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

              nativeTraceCounter(tag, profile_name, value);
            } else {
              throw std::runtime_error(
                  "nativeTraceEndAsyncFlow called without any arguments.");
            }

            return jsi::Value::undefined();
          }));

  EventRegisterReact_Native_Windows_Provider();
}
