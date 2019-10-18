// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"

#include "AccessibilityAction.h"

namespace winrt::react::uwp::implementation {

hstring AccessibilityAction::Name() {
  return m_name;
}
void AccessibilityAction::Name(hstring const &value) {
  m_name = value;
}

hstring AccessibilityAction::Label() {
  return m_label;
}

void AccessibilityAction::Label(hstring const &value) {
  m_label = value;
}

} // namespace winrt::react::uwp::implementation
