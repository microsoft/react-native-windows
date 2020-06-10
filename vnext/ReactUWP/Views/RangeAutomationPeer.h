// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include <winrt/Windows.Foundation.h>

#include "RangeAutomationPeer.g.h"

namespace winrt::PROJECT_ROOT_NAMESPACE::implementation {

struct RangeAutomationPeer : RangeAutomationPeerT<RangeAutomationPeer> {
  using Super = RangeAutomationPeerT<RangeAutomationPeer>;

  RangeAutomationPeer() = delete;
  RangeAutomationPeer(xaml::UIElement const &owner);

  // IRangeValueProvider
  double Minimum();
  double Maximum();
  double Value();
  double SmallChange();
  double LargeChange();
  bool IsReadOnly();

  void SetValue(double value);

 private:
  double GetAccessibilityValueRange(winrt::PROJECT_ROOT_NAMESPACE::AccessibilityValue value) const;
  xaml::UIElement m_uiElement;
};
} // namespace winrt::PROJECT_ROOT_NAMESPACE::implementation

namespace winrt::PROJECT_ROOT_NAMESPACE::factory_implementation {
struct RangeAutomationPeer : RangeAutomationPeerT<RangeAutomationPeer, implementation::RangeAutomationPeer> {};
} // namespace winrt::PROJECT_ROOT_NAMESPACE::factory_implementation
