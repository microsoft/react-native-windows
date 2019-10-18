// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include "cppwinrt/AccessibilityAction.g.h"

namespace winrt::react::uwp::implementation {

//
// AccessibilityAction translates the values of the attached properties in
// DynamicAutomationProperties into the equivalent UIA AutomationPeer
//
struct AccessibilityAction : AccessibilityActionT<AccessibilityAction> {
  using Super = AccessibilityActionT<AccessibilityAction>;

  AccessibilityAction(){};

  hstring Name();
  void Name(hstring const &value);

  hstring Label();
  void Label(hstring const &value);

private:
  hstring m_name{};
  hstring m_label{};
};
} // namespace winrt::react::uwp::implementation

namespace winrt::react::uwp::factory_implementation {
struct AccessibilityAction : AccessibilityActionT<
                                 AccessibilityAction,
                                 implementation::AccessibilityAction> {};
} // namespace winrt::react::uwp::factory_implementation
