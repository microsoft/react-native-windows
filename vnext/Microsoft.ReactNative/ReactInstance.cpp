// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include "ReactInstance.h"
#include "ReactInstance.g.cpp"
#include "ReactSupport.h"

namespace winrt::Microsoft::ReactNative::implementation {
void ReactInstance::InvokeFunction(
    hstring const &moduleName,
    hstring const &method,
    Windows::Foundation::Collections::IVectorView<IInspectable> const &arguments) {
  folly::dynamic args{ConvertToDynamic(arguments)};
  m_instance->CallJsFunction(to_string(moduleName), to_string(method), std::move(args));
}

} // namespace winrt::Microsoft::ReactNative::implementation
