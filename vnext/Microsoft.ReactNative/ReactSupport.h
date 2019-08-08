#pragma once
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include <cxxreact/CxxModule.h>
#include <folly/dynamic.h>
#include <folly/json.h>

using namespace winrt;
using namespace Windows::Foundation;

namespace winrt::Microsoft::ReactNative::Bridge {

// Convert a WinRT IInspectable into a folly::dynamic object
folly::dynamic ConvertToDynamic(IInspectable const &object);

} // namespace winrt::Microsoft::ReactNative::Bridge
