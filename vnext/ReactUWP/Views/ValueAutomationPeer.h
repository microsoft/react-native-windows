// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include <winrt/Windows.Foundation.h>

#include "ValueAutomationPeer.g.h"

namespace winrt::PROJECT_ROOT_NAMESPACE::implementation {

struct ValueAutomationPeer : ValueAutomationPeerT<ValueAutomationPeer> {
  using Super = ValueAutomationPeerT<ValueAutomationPeer>;

  ValueAutomationPeer() = delete;
  ValueAutomationPeer(xaml::UIElement const &owner);

  // IValueProvider
  winrt::hstring Value();
  bool IsReadOnly();
  void SetValue(winrt::hstring const &value);

 private:
  winrt::hstring GetAccessibilityValue(winrt::PROJECT_ROOT_NAMESPACE::AccessibilityValue value) const;
  xaml::UIElement m_uiElement;
};
} // namespace winrt::PROJECT_ROOT_NAMESPACE::implementation

namespace winrt::PROJECT_ROOT_NAMESPACE::factory_implementation {
struct ValueAutomationPeer : ValueAutomationPeerT<ValueAutomationPeer, implementation::ValueAutomationPeer> {};
} // namespace winrt::PROJECT_ROOT_NAMESPACE::factory_implementation
