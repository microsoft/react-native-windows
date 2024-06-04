// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include "App.YogaXamlPanel.g.h"
#include <functional>

namespace winrt::PlaygroundApp::implementation {

//
// YogaXamlPanel is our custom Panel used to bridge yoga and xaml layouts in fabric
//
struct YogaXamlPanel : YogaXamlPanelT<YogaXamlPanel> {
  using Super = YogaXamlPanelT<YogaXamlPanel>;

 public:
  // Constructors
  YogaXamlPanel(std::function<void(winrt::Windows::Foundation::Size size)> &&onMeasured);

  // Overrides
  winrt::Windows::Foundation::Size MeasureOverride(winrt::Windows::Foundation::Size availableSize);
  winrt::Windows::Foundation::Size ArrangeOverride(winrt::Windows::Foundation::Size finalSize);

 private:
  std::function<void(winrt::Windows::Foundation::Size size)> m_onMeasured;
};

} // namespace winrt::PlaygroundApp::implementation
