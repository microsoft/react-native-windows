// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include "Utils/Helpers.h"

#include <UI.Xaml.Controls.Primitives.h>
#include <UI.Xaml.Controls.h>

namespace Microsoft::ReactNative {

void FixProofingMenuCrashForXamlIsland(xaml::Controls::TextCommandBarFlyout const &flyout);

template <typename T>
inline void EnsureUniqueTextFlyoutForXamlIsland(T const &textView) {
  // This works around a XAML Islands bug where the XamlRoot of the first
  // window the flyout is shown on takes ownership of the flyout and attempts
  // to show the flyout on other windows cause the first window to get focus.
  // https://github.com/microsoft/microsoft-ui-xaml/issues/5341
  if (IsXamlIsland()) {
    xaml::Controls::TextCommandBarFlyout flyout;
    flyout.Placement(xaml::Controls::Primitives::FlyoutPlacementMode::BottomEdgeAlignedLeft);

    // This works around a XAML Islands bug where the Proofing sub-menu for
    // TextBox causes a crash while animating to open / close before 21H1.
    // https://github.com/microsoft/microsoft-ui-xaml/issues/3529
    if (!Is21H1OrHigher() && textView.try_as<xaml::Controls::TextBox>()) {
      FixProofingMenuCrashForXamlIsland(flyout);
    }

    textView.ContextFlyout(flyout);
    textView.SelectionFlyout(flyout);
  }
}

template <typename T>
inline void ClearUniqueTextFlyoutForXamlIsland(T const &textView) {
  textView.ClearValue(xaml::UIElement::ContextFlyoutProperty());
  if (textView.try_as<xaml::Controls::TextBlock>()) {
    textView.ClearValue(xaml::Controls::TextBlock::SelectionFlyoutProperty());
  } else if (textView.try_as<xaml::Controls::TextBox>()) {
    textView.ClearValue(xaml::Controls::TextBox::SelectionFlyoutProperty());
  }
}

} // namespace Microsoft::ReactNative
