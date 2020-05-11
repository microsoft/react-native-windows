// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include "RedBoxHelper.g.h"

namespace Mso::React {
struct IRedBoxHandler;
}

namespace winrt::Microsoft::ReactNative::implementation {

struct RedBoxHelper : RedBoxHelperT<RedBoxHelper> {
  RedBoxHelper() = default;

  static IRedBoxHandler CreateDefaultHandler(winrt::Microsoft::ReactNative::ReactNativeHost const &host) noexcept;
};

} // namespace winrt::Microsoft::ReactNative::implementation

namespace winrt::Microsoft::ReactNative::factory_implementation {

struct RedBoxHelper : RedBoxHelperT<RedBoxHelper, implementation::RedBoxHelper> {};

} // namespace winrt::Microsoft::ReactNative::factory_implementation
