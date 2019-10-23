// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include "ReactInstance.h"
#if __has_include("Bridge.ReactInstance.g.cpp")
#include "Bridge.ReactInstance.g.cpp"
#endif

#include "ReactSupport.h"

namespace winrt::Microsoft::ReactNative::Bridge::implementation {
void ReactInstance::InvokeFunction(
    hstring const &moduleName,
    hstring const &method,
    IVectorView<IInspectable> const &arguments) {
  folly::dynamic args = Microsoft::ReactNative::Bridge::ConvertToDynamic(arguments);

  m_instance->CallJsFunction(to_string(moduleName), to_string(method), std::move(args));
}

} // namespace winrt::Microsoft::ReactNative::Bridge::implementation
