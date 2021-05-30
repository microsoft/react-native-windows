// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.
#pragma once

#include <NativeModules.h>

namespace Microsoft::ReactNative {

REACT_MODULE(Clipboard)
struct Clipboard {
  REACT_INIT(Initialize)
  void Initialize(winrt::Microsoft::ReactNative::ReactContext const &reactContext) noexcept;

  REACT_METHOD(getString)
  void getString(React::ReactPromise<React::JSValue> result) noexcept;

  REACT_METHOD(setString)
  void setString(std::string content) noexcept;

  winrt::Microsoft::ReactNative::ReactContext m_reactContext;
};

} // namespace Microsoft::ReactNative
