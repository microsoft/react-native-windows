// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"
#include "JSExceptionCallbackFactory.h"

namespace Mso::React {

std::function<void(facebook::react::JSExceptionInfo &&)> CreateExceptionCallback(
    OnJSExceptionCallback &&jsExceptionCallback) noexcept {
  return [exceptionCallback = std::move(jsExceptionCallback)](
      facebook::react::JSExceptionInfo && jsExceptionInfo) noexcept {
    Mso::React::JSExceptionInfo exceptionInfo;
    exceptionInfo.Callstack = std::move(jsExceptionInfo.callstack);
    exceptionInfo.ExceptionId = jsExceptionInfo.exceptionId;
    exceptionInfo.ExceptionMessage = std::move(jsExceptionInfo.exceptionMessage);

    static_assert(
        static_cast<int32_t>(facebook::react::JSExceptionType::Fatal) ==
            static_cast<int32_t>(Mso::React::JSExceptionType::Fatal),
        "Exception type enums don't match");
    static_assert(
        static_cast<int32_t>(facebook::react::JSExceptionType::Soft) ==
            static_cast<int32_t>(Mso::React::JSExceptionType::Soft),
        "Exception type enums don't match");
    exceptionInfo.ExceptionType = static_cast<Mso::React::JSExceptionType>(jsExceptionInfo.exceptionType);

    exceptionCallback(std::move(exceptionInfo));
  };
}

} // namespace Mso::React
