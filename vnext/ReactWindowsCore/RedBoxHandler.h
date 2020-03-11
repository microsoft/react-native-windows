// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.
#pragma once
#include <string>
#include <vector>

namespace Mso {
namespace React {

enum class JSExceptionType : int32_t {
  Fatal = 0,
  Soft = 1,
};

struct JSStackFrameInfo {
  std::string File;
  std::string Method;
  int Line;
  int Column;
};

struct JSExceptionInfo {
  std::string ExceptionMessage;
  uint32_t ExceptionId;
  std::vector<JSStackFrameInfo> Callstack;
};

struct IRedBoxHandler {
  virtual void showNewJSError(JSExceptionInfo &&, JSExceptionType) = 0;
  virtual bool isDevSupportEnabled() = 0;
  virtual void updateJSError(JSExceptionInfo &&) = 0;
  virtual void dismissRedbox() = 0;
};

} // namespace React
} // namespace Mso
