// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include "ExceptionsManagerModule.h"
#include <assert.h>
#include <cxxreact/JsArgumentHelpers.h>

namespace facebook {
namespace react {

ExceptionsManagerModule::ExceptionsManagerModule(
    std::function<void(JSExceptionInfo)> &&jsExceptionCallback)
    : m_jsExceptionCallback(std::move(jsExceptionCallback)) {}

std::string ExceptionsManagerModule::getName() {
  return name;
}

std::map<std::string, folly::dynamic> ExceptionsManagerModule::getConstants() {
  return std::map<std::string, folly::dynamic>();
}

std::vector<facebook::xplat::module::CxxModule::Method>
ExceptionsManagerModule::getMethods() {
  return {
      Method(
          "reportFatalException",
          [this](folly::dynamic args) noexcept {
            if (m_jsExceptionCallback) {
              m_jsExceptionCallback(
                  std::move(CreateExceptionInfo(args, JSExceptionType::Fatal)));
            }
          }),

      Method(
          "reportSoftException",
          [this](folly::dynamic args) noexcept {
            if (m_jsExceptionCallback) {
              m_jsExceptionCallback(
                  std::move(CreateExceptionInfo(args, JSExceptionType::Soft)));
            }
          }),

      Method(
          "updateExceptionMessage",
          [](folly::dynamic /*args*/) noexcept {
              // For every JS exception, react native first calls
              // reportFatalException or reportSoftException.
              // Then it attempts to Symbolicate the stack trace and if it
              // succeeds, calls this method (updateExceptionMessage),
              // As a result every JS exception is propagated across the bridge
              // to this native module twice.
              // We only need to expose the exception info to the exception
              // callback once, and in the case of Win32, stacks coming through
              // reportFatalException and reportSoftException already have
              // symbol information, so there is nothing we need to do here.
          }),

  };
}

JSExceptionInfo ExceptionsManagerModule::CreateExceptionInfo(
    const folly::dynamic &args,
    JSExceptionType jsExceptionType) const noexcept {
  // Parameter args is a dynamic array containing 3 objects:
  // 1. an exception message string.
  // 2. an array containing stack information.
  // 3. an exceptionID int.
  assert(args.isArray());
  assert(args.size() == 3);
  assert(args[0].isString());
  assert(args[1].isArray());
  assert(args[2].isNumber());
  assert(
      facebook::xplat::jsArgAsInt(args, 2) <=
      std::numeric_limits<uint32_t>::max());

  JSExceptionInfo jsExceptionInfo;
  jsExceptionInfo.exceptionMessage = facebook::xplat::jsArgAsString(args, 0);
  jsExceptionInfo.exceptionId =
      static_cast<uint32_t>(facebook::xplat::jsArgAsInt(args, 2));
  jsExceptionInfo.exceptionType = jsExceptionType;

  folly::dynamic stackAsFolly = facebook::xplat::jsArgAsArray(args, 1);

  // Construct a string containing the stack frame info in the following format:
  // <method> Line:<Line Number>  Column:<ColumnNumber> <Filename>
  for (const auto &stackFrame : stackAsFolly) {
    // Each dynamic object is a map containing information about the stack
    // frame: method (string), arguments (array), filename(string), line number
    // (int) and column number (int).
    assert(stackFrame.isObject());
    assert(stackFrame.size() >= 4); // 4 in 0.57, 5 in 0.58+ (arguments added)

    std::stringstream stackFrameInfo;

    stackFrameInfo << RetrieveValueFromMap(
                          stackFrame, "methodName", folly::dynamic::STRING)
                   << ' ';
    stackFrameInfo << "Line: "
                   << RetrieveValueFromMap(
                          stackFrame, "lineNumber", folly::dynamic::INT64)
                   << ' ';
    stackFrameInfo << "Column: "
                   << RetrieveValueFromMap(
                          stackFrame, "column", folly::dynamic::INT64)
                   << ' ';
    stackFrameInfo << RetrieveValueFromMap(
        stackFrame, "file", folly::dynamic::STRING);

    jsExceptionInfo.callstack.push_back(stackFrameInfo.str());
  }

  return jsExceptionInfo;
}

std::string ExceptionsManagerModule::RetrieveValueFromMap(
    const folly::dynamic &map,
    const std::string &key,
    folly::dynamic::Type type) const noexcept {
  assert(type == folly::dynamic::INT64 || type == folly::dynamic::STRING);
  assert(map.type() == folly::dynamic::OBJECT);

  std::string value;
  auto iterator = map.find(key);
  if (iterator != map.items().end()) {
    if (type == folly::dynamic::STRING) {
      value = iterator->second.asString();
    } else {
      if (iterator->second.isNull()) {
        return "<Unknown>";
      }
      assert(iterator->second.isNumber());
      std::stringstream stream;
      stream << static_cast<int64_t>(iterator->second.asDouble());
      value = stream.str();
    }
  } else {
    assert(false);
  }

  return value;
}

} // namespace react
} // namespace facebook
