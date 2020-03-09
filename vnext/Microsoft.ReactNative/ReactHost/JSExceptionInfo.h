// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include <functional/functor.h>
#include <string>
#include <vector>
#include "DevSettings.h"

namespace Mso::React {

enum class JSExceptionType : int32_t {
  Fatal = 0,
  Soft = 1,
};

using JSStackFrameInfo = facebook::react::JSStackFrameInfo;

struct JSExceptionInfo {
  JSExceptionType ExceptionType{};
  std::string ExceptionMessage;
  uint32_t ExceptionId{};
  std::vector<JSStackFrameInfo> Callstack;

  std::string ToString() const noexcept;
};

using OnJSExceptionCallback = Mso::Functor<void(JSExceptionInfo &&jsExceptionInfo)>;

} // namespace Mso::React
