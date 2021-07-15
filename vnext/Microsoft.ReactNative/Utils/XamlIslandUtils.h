// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include "Utils/Helpers.h"

#include <UI.Xaml.Controls.Primitives.h>
#include <UI.Xaml.Controls.h>

namespace Microsoft::ReactNative {

template <typename T>
inline void FixTextFlyoutForXamlIsland(T const &textView) {
  // This works around a XAML Islands bug where the XamlRoot of the first
  // window the flyout is shown on takes ownership of the flyout and attempts
  // to show the flyout on other windows cause the first window to get focus.
  // https://github.com/microsoft/microsoft-ui-xaml/issues/5341
  if (IsXamlIsland()) {
    xaml::Controls::TextCommandBarFlyout flyout;
    flyout.Placement(xaml::Controls::Primitives::FlyoutPlacementMode::BottomEdgeAlignedLeft);
    textView.ContextFlyout(flyout);
    textView.SelectionFlyout(flyout);
  }
}

} // namespace Microsoft::ReactNative
