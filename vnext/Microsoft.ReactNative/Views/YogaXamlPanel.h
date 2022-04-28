// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include "YogaXamlPanel.g.h"
#include <react/renderer/core/LayoutConstraints.h>

namespace winrt::Microsoft::ReactNative::implementation {

//
// YogaXamlPanel is our custom Panel used to bridge yoga and xaml layouts in fabric
//
struct YogaXamlPanel : YogaXamlPanelT<YogaXamlPanel> {
  using Super = YogaXamlPanelT<YogaXamlPanel>;

 public:
  // Constructors
  YogaXamlPanel(
      std::function<void(facebook::react::Size size)> &&onMeasured,
      std::function<const facebook::react::LayoutConstraints &()> &&getConstraints);

  // Overrides
  winrt::Windows::Foundation::Size MeasureOverride(winrt::Windows::Foundation::Size availableSize);
  winrt::Windows::Foundation::Size ArrangeOverride(winrt::Windows::Foundation::Size finalSize);

 private:
  std::function<void(facebook::react::Size size)> m_onMeasured;
  std::function<const facebook::react::LayoutConstraints &()> m_getConstraints;
};

} // namespace winrt::Microsoft::ReactNative::implementation
