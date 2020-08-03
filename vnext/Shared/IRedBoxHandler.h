// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once
#include <folly/dynamic.h>
#include <string>
#include <vector>

namespace Mso {
namespace React {

enum class ErrorType : int32_t {
  JSFatal = 0, // A JS Exception was thrown or otherwise fatal error
  JSSoft = 1, // An error coming from JS that isn't fatal, such as console.error
  Native = 2,
};

struct ErrorFrameInfo {
  std::string File;
  std::string Method;
  int Line{};
  int Column{};
  bool Collapse{false};
};

struct ErrorInfo {
  std::string Message;
  std::string OriginalMessage;
  std::string Name;
  std::string ComponentStack;
  std::vector<ErrorFrameInfo> Callstack;
  uint32_t Id{};
  folly::dynamic ExtraData;
};

struct IRedBoxHandler {
  virtual void showNewError(ErrorInfo &&, ErrorType) = 0;
  virtual bool isDevSupportEnabled() const = 0;
  virtual void updateError(ErrorInfo &&) = 0;
  virtual void dismissRedbox() = 0;
};

} // namespace React
} // namespace Mso
