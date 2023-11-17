// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include "jsi/jsi.h"
#include "winrt/Microsoft.ReactNative.h"

namespace winrt::Microsoft::ReactNative::implementation {

struct JsiNonAbiHostObject : winrt::implements<JsiNonAbiHostObject, IJsiNonAbiHostObject> {
 public: // IReactNonAbiHostObject
  JsiNonAbiHostObject(std::shared_ptr<facebook::jsi::HostObject> const &hostObject) noexcept
      : m_hostObject{hostObject} {}

  std::shared_ptr<facebook::jsi::HostObject> HostObject() const noexcept {
    return m_hostObject;
  }

 private:
  std::shared_ptr<facebook::jsi::HostObject> m_hostObject;
};

} // namespace winrt::Microsoft::ReactNative::implementation
