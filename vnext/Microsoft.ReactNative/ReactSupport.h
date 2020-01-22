// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include <folly/dynamic.h>

namespace winrt::Microsoft::ReactNative {

// Convert a WinRT IInspectable into a folly::dynamic object
folly::dynamic ConvertToDynamic(Windows::Foundation::IInspectable const &object);

} // namespace winrt::Microsoft::ReactNative
