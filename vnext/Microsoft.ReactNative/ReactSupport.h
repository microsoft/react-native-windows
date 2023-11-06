// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include <folly/dynamic.h>

namespace winrt::Microsoft::ReactNative {

// Convert a WinRT IInspectable into a folly::dynamic object
folly::dynamic ConvertToDynamic(winrt::Windows::Foundation::IInspectable const &object);

} // namespace winrt::Microsoft::ReactNative
