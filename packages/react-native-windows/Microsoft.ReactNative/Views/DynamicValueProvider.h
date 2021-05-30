// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include <winrt/Windows.Foundation.h>

#include "DynamicAutomationPeer.g.h"
#include "DynamicValueProvider.g.h"

namespace winrt::Microsoft::ReactNative::implementation {

struct DynamicValueProvider : DynamicValueProviderT<DynamicValueProvider> {
  using Super = DynamicValueProviderT<DynamicValueProvider>;

  DynamicValueProvider() = delete;
  DynamicValueProvider(xaml::Automation::Peers::FrameworkElementAutomationPeer peer);

  // IValueProvider
  winrt::hstring Value();
  bool IsReadOnly();
  void SetValue(winrt::hstring const &value);

 private:
  winrt::hstring GetAccessibilityValue() const;
  xaml::Automation::Peers::FrameworkElementAutomationPeer m_peer{nullptr};
};
} // namespace winrt::Microsoft::ReactNative::implementation

namespace winrt::Microsoft::ReactNative::factory_implementation {
struct DynamicValueProvider : DynamicValueProviderT<DynamicValueProvider, implementation::DynamicValueProvider> {};
} // namespace winrt::Microsoft::ReactNative::factory_implementation
