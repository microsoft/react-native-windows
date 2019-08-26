#pragma once
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "Bridge.MethodInfo.g.h"

namespace winrt::Microsoft::ReactNative::Bridge::implementation {
struct MethodInfo : MethodInfoT<MethodInfo> {
  MethodInfo(
      hstring const &methodName,
      ReturnType const &type,
      Method const &method)
      : m_methodName(methodName), m_type(type), m_method(method) {
    if (methodName.empty())
      throw hresult_invalid_argument(L"name");
    if (method == nullptr)
      throw hresult_null_argument(L"method");
  }

  hstring Name() {
    return m_methodName;
  }

  ReturnType ReturnType() {
    return m_type;
  }
  Method Method() {
    return m_method;
  }

 private:
  hstring m_methodName{};
  Bridge::ReturnType m_type{ReturnType::Promise};
  Bridge::Method m_method{nullptr};
};
} // namespace winrt::Microsoft::ReactNative::Bridge::implementation

namespace winrt::Microsoft::ReactNative::Bridge::factory_implementation {
struct MethodInfo : MethodInfoT<MethodInfo, implementation::MethodInfo> {};
} // namespace winrt::Microsoft::ReactNative::Bridge::factory_implementation
