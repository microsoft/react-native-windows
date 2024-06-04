// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "JSExecutorFactorySettings.h"

namespace Microsoft::JSI::JSExecutorFactorySettings {

winrt::Microsoft::ReactNative::ReactPropertyId<
    winrt::Microsoft::ReactNative::ReactNonAbiValue<facebook::react::JSExecutorFactoryDelegate>>
JSExecutorFactoryDelegateProperty() noexcept {
  winrt::Microsoft::ReactNative::ReactPropertyId<
      winrt::Microsoft::ReactNative::ReactNonAbiValue<facebook::react::JSExecutorFactoryDelegate>>
      propId{L"ReactNative.JSI", L"JSExecutorFactory"};
  return propId;
}
void SetJSExecutorFactoryDelegate(
    winrt::Microsoft::ReactNative::ReactPropertyBag properties,
    facebook::react::JSExecutorFactoryDelegate const &value) noexcept {
  properties.Set(JSExecutorFactoryDelegateProperty(), value);
}

const facebook::react::JSExecutorFactoryDelegate GetJSExecutorFactoryDelegate(
    winrt::Microsoft::ReactNative::ReactPropertyBag properties) noexcept {
  const auto jsExecutorFactoryDelegate = properties.Get(JSExecutorFactoryDelegateProperty());
  if (!jsExecutorFactoryDelegate)
    return nullptr;
  return jsExecutorFactoryDelegate.Value();
}

} // namespace Microsoft::JSI::JSExecutorFactorySettings
