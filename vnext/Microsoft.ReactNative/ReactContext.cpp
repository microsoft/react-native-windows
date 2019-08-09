// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include "ReactContext.h"
#include "Bridge.ReactContext.g.cpp"

using namespace winrt;
using namespace Microsoft::ReactNative::Bridge;
using namespace Windows::Foundation::Collections;

namespace winrt::Microsoft::ReactNative::Bridge::implementation {
IReactInstance ReactContext::ReactInstance() {
  if (m_reactInstance == nullptr) {
    throw hresult_invalid_operation(L"ReactInstance has not yet been set.");
  }

  return m_reactInstance;
}

void ReactContext::InitializeWithInstance(
    Bridge::ReactInstance const &instance) {
  if (instance == nullptr) {
    throw hresult_null_argument(L"instance");
  }

  if (m_reactInstance != nullptr) {
    throw hresult_invalid_argument(L"ReactInstance has already been set.");
  }
  m_reactInstance = instance;
}

void ReactContext::CallJSFunction(
    hstring const &moduleName,
    hstring const &method,
    IVectorView<IInspectable> const &params) {
  if (m_reactInstance == nullptr) {
    ReactInstance().InvokeFunction(moduleName, method, params);
  }
}
} // namespace winrt::Microsoft::ReactNative::Bridge::implementation
