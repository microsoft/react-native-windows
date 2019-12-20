// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

// In Microsoft.ReactNative project the DynamicAutomationProperties.g.h file
// is generated with the 'react.uwp.' prefix.
#include "react.uwp.DynamicAutomationProperties.g.h"

namespace winrt::react::uwp {
  // The AccessibilityAction must be in the project namespace to be
  // usable by Xaml dependency properties.
  // The rest of the code uses it from the winrt::react::uwp namespace.
  // Thus, we add the AccessibilityAction to the winrt::react::uwp namespace.
  using AccessibilityAction = winrt::Microsoft::ReactNative::AccessibilityAction;
}
