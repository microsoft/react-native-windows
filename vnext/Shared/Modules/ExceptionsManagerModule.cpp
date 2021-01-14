// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "ExceptionsManagerModule.h"
#include <assert.h>
#include <cxxreact/JsArgumentHelpers.h>
#include <sstream>

namespace facebook {
namespace react {

namespace {
std::string RetrieveOptionalStringFromMap(const folly::dynamic &map, const std::string &key) noexcept {
  assert(map.type() == folly::dynamic::OBJECT);
  auto iterator = map.find(key);
  if (iterator != map.items().end()) {
    if (iterator->second.isNull()) {
      return {};
    }
    return iterator->second.asString();
  }
  return {};
}

int RetrieveIntFromMap(const folly::dynamic &map, const std::string &key) noexcept {
  assert(map.type() == folly::dynamic::OBJECT);

  auto iterator = map.find(key);
  if (iterator != map.items().end()) {
    if (iterator->second.isNull()) {
      return -1;
    }
    assert(iterator->second.isNumber());
    return static_cast<int>(iterator->second.asDouble());
  }
  assert(false);
  return -1;
}

bool RetrieveOptionalBoolFromMap(const folly::dynamic &map, const std::string &key) noexcept {
  assert(map.type() == folly::dynamic::OBJECT);

  auto iterator = map.find(key);
  if (iterator != map.items().end()) {
    if (iterator->second.isNull()) {
      return false;
    }
    assert(iterator->second.isBool());
    return iterator->second.asBool();
  }
  return false;
}

Mso::React::ErrorInfo CreateErrorInfo(const folly::dynamic &args) noexcept {
  // Parameter args is a dynamic array containing 3 objects:
  // 1. an exception message string.
  // 2. an array containing stack information.
  // 3. an exceptionID int.
  assert(args.isArray());
  assert(args.size() == 3);
  assert(args[0].isString());
  assert(args[1].isArray());
  assert(args[2].isNumber());
  assert(facebook::xplat::jsArgAsInt(args, 2) <= std::numeric_limits<uint32_t>::max());

  Mso::React::ErrorInfo errorInfo;
  errorInfo.Message = facebook::xplat::jsArgAsString(args, 0);
  errorInfo.Id = static_cast<uint32_t>(facebook::xplat::jsArgAsInt(args, 2));

  folly::dynamic stackAsFolly = facebook::xplat::jsArgAsArray(args, 1);

  // Construct a string containing the stack frame info in the following format:
  // <method> Line:<Line Number>  Column:<ColumnNumber> <Filename>
  for (const auto &stackFrame : stackAsFolly) {
    // Each dynamic object is a map containing information about the stack
    // frame: method (string), arguments (array), filename(string), line number
    // (int) and column number (int).
    assert(stackFrame.isObject());
    assert(stackFrame.size() >= 4); // 4 in 0.57, 5 in 0.58+ (arguments added)

    errorInfo.Callstack.push_back(Mso::React::ErrorFrameInfo{
        RetrieveOptionalStringFromMap(stackFrame, "file"),
        RetrieveOptionalStringFromMap(stackFrame, "methodName"),
        RetrieveIntFromMap(stackFrame, "lineNumber"),
        RetrieveIntFromMap(stackFrame, "column"),
        false});
  }

  return errorInfo;
}

Mso::React::ErrorInfo CreateErrorInfo2(const folly::dynamic &args) noexcept {
  /*
   Parameter args is an object:
      message: string,
      originalMessage: ?string,
      name: ?string,
      componentStack: ?string,
      stack: Array<StackFrame>,
      id: number,
      isFatal: boolean,
      // flowlint-next-line unclear-type:off
      extraData?: Object,
  */
  assert(args.isObject());

  Mso::React::ErrorInfo errorInfo;

  errorInfo.Message = RetrieveOptionalStringFromMap(args, "message");
  errorInfo.OriginalMessage = RetrieveOptionalStringFromMap(args, "originalMessage");
  errorInfo.Name = RetrieveOptionalStringFromMap(args, "name");
  errorInfo.ComponentStack = RetrieveOptionalStringFromMap(args, "componentStack");
  errorInfo.Id = RetrieveIntFromMap(args, "id");

  auto iterator = args.find("extraData");
  if (iterator != args.items().end()) {
    if (iterator->second.isObject()) {
      errorInfo.ExtraData = iterator->second;
    }
  }

  iterator = args.find("stack");
  if (iterator != args.items().end()) {
    if (iterator->second.isArray()) {
      auto stackAsFolly = iterator->second;
      for (const auto &stackFrame : stackAsFolly) {
        // Each dynamic object is a map containing information about the stack
        // frame: method (string), arguments (array), filename(string), line number
        // (int) and column number (int).
        assert(stackFrame.isObject());
        assert(stackFrame.size() >= 4); // 4 in 0.57, 5 in 0.58+ (arguments added)

        errorInfo.Callstack.push_back(Mso::React::ErrorFrameInfo{
            RetrieveOptionalStringFromMap(stackFrame, "file"),
            RetrieveOptionalStringFromMap(stackFrame, "methodName"),
            RetrieveIntFromMap(stackFrame, "lineNumber"),
            RetrieveIntFromMap(stackFrame, "column"),
            RetrieveOptionalBoolFromMap(stackFrame, "collapse")});
      }
    }
  }

  return errorInfo;
}

} // namespace

ExceptionsManagerModule::ExceptionsManagerModule(std::shared_ptr<Mso::React::IRedBoxHandler> redboxHandler)
    : m_redboxHandler(std::move(redboxHandler)) {}

std::string ExceptionsManagerModule::getName() {
  return name;
}

std::map<std::string, folly::dynamic> ExceptionsManagerModule::getConstants() {
  return std::map<std::string, folly::dynamic>();
}

std::vector<facebook::xplat::module::CxxModule::Method> ExceptionsManagerModule::getMethods() {
  auto redboxHandler = m_redboxHandler;
  return {
      Method(
          "reportFatalException",
          [redboxHandler](folly::dynamic args) noexcept {
            if (redboxHandler && redboxHandler->isDevSupportEnabled()) {
              redboxHandler->showNewError(std::move(CreateErrorInfo(args)), Mso::React::ErrorType::JSFatal);
            }
            /*
            // TODO - fatal errors should throw if there is no redbox handler
            else {
              throw Exception();
            } */
          }),

      Method(
          "reportSoftException",
          [redboxHandler](folly::dynamic args) noexcept {
            if (redboxHandler && redboxHandler->isDevSupportEnabled()) {
              redboxHandler->showNewError(std::move(CreateErrorInfo(args)), Mso::React::ErrorType::JSSoft);
            }
          }),

      Method(
          "reportException",
          [redboxHandler](folly::dynamic args) noexcept {
            if (redboxHandler && redboxHandler->isDevSupportEnabled()) {
              auto isFatal = RetrieveOptionalBoolFromMap(args[0], "isFatal");
              redboxHandler->showNewError(
                  std::move(CreateErrorInfo2(args[0])),
                  isFatal ? Mso::React::ErrorType::JSFatal : Mso::React::ErrorType::JSSoft);
            }
          }),

      Method(
          "updateExceptionMessage",
          [redboxHandler](folly::dynamic args) noexcept {
            if (redboxHandler && redboxHandler->isDevSupportEnabled()) {
              redboxHandler->updateError(std::move(CreateErrorInfo(args)));
            }
          }),

      Method("dismissRedbox", [redboxHandler](folly::dynamic /*args*/) noexcept {
        if (redboxHandler)
          redboxHandler->dismissRedbox();
      })};
}

} // namespace react
} // namespace facebook
