// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include <winrt/Windows.Foundation.h>

#include "DynamicAutomationPeer.g.h"
#include "DynamicValueProvider.g.h"

namespace winrt::PROJECT_ROOT_NAMESPACE::implementation {

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
} // namespace winrt::PROJECT_ROOT_NAMESPACE::implementation

namespace winrt::PROJECT_ROOT_NAMESPACE::factory_implementation {
struct DynamicValueProvider : DynamicValueProviderT<DynamicValueProvider, implementation::DynamicValueProvider> {};
} // namespace winrt::PROJECT_ROOT_NAMESPACE::factory_implementation
