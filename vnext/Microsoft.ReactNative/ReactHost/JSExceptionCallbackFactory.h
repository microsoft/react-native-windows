// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include <DevSettings.h>
#include "JSExceptionInfo.h"

namespace Mso::React {

std::function<void(facebook::react::JSExceptionInfo &&)> CreateExceptionCallback(
    OnJSExceptionCallback &&jsExceptionCallback) noexcept;

} // namespace Mso::React
