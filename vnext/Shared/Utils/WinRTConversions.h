// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

// Windows API
#include <winrt/base.h>

// Standard Library
#include <sstream>

namespace Microsoft::React::Utilities {

std::string HResultToString(winrt::hresult_error const &e);

std::string HResultToString(winrt::hresult &&result);

} // namespace Microsoft::React::Utilities
