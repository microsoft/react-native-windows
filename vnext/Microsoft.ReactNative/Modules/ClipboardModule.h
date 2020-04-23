// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.
#pragma once

#include <NativeModules.h>

namespace Microsoft::ReactNative {

REACT_MODULE(Clipboard)
struct Clipboard {
  REACT_METHOD(getString)
  static winrt::fire_and_forget getString(React::ReactPromise<React::JSValue> result) noexcept;

  REACT_METHOD(setString)
  static void setString(std::string content) noexcept;
};

} // namespace Microsoft::ReactNative
