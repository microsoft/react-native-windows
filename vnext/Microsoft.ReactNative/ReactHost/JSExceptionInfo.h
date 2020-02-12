// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include <functional/functor.h>
#include <string>
#include <vector>

namespace Mso::React {

enum class JSExceptionType : int32_t {
  Fatal = 0,
  Soft = 1,
};

struct JSExceptionInfo {
  JSExceptionType ExceptionType{};
  std::string ExceptionMessage;
  uint32_t ExceptionId{};
  std::vector<std::string> Callstack;

  std::string ToString() const noexcept;
};

using OnJSExceptionCallback = Mso::Functor<void(JSExceptionInfo &&jsExceptionInfo)>;

} // namespace Mso::React
