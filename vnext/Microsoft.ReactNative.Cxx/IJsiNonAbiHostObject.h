// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include "winrt/Microsoft.ReactNative.h"
#include "jsi/jsi.h"

namespace winrt::Microsoft::ReactNative::implementation {

struct JsiNonAbiHostObject : winrt::implements<JsiNonAbiHostObject, IJsiNonAbiHostObject> {
 public: // IReactNonAbiHostObject
  JsiNonAbiHostObject(std::shared_ptr<facebook::jsi::HostObject> const &hostObject) noexcept
      : m_hostObject{hostObject} {}

  std::shared_ptr<facebook::jsi::HostObject> HostObject() const noexcept {
    return m_hostObject;
  }

 public: // IJsiNonAbiHostObject
  bool HasValue() const noexcept {
    return !!m_hostObject;
  }

 private:
  std::shared_ptr<facebook::jsi::HostObject> m_hostObject;
};

} // namespace winrt::Microsoft::ReactNative::implementation
