/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "SliderMeasurementsManager.h"

#include <react/renderer/core/conversions.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.UI.Xaml.Interop.h>

namespace facebook::react {

Size SliderMeasurementsManager::measure(SurfaceId surfaceId, LayoutConstraints layoutConstraints) const {
  /*
  if (!m_slider) {
    m_slider = xaml::Controls::Slider();
    xaml::Style sliderStyle;
    xaml::Application::Current()
        .Resources()
        .TryLookup(winrt::box_value(winrt::xaml_typename<xaml::Controls::Slider>()))
        .as(sliderStyle);
    m_slider.Style(sliderStyle);
  }

  winrt::Windows::Foundation::Size availiableSize(
      layoutConstraints.maximumSize.width, layoutConstraints.maximumSize.height);

  m_slider.Measure(availiableSize);
  auto size = m_slider.DesiredSize();
  return {static_cast<float>(size.Width), static_cast<float>(size.Height)};
  */
  return {50, 50}; // TEMP
}

} // namespace facebook::react
