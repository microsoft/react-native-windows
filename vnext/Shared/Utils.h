// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include <winrt/Windows.Foundation.h>
#include <string>

namespace Microsoft::React {

// string formatting
std::string FormatString(_Printf_format_string_ const char *format, ...);

} // namespace Microsoft::React
