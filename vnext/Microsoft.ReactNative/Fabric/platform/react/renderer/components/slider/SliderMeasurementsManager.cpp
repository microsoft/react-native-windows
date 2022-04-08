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

#include "ReactContext.h"
#include "ReactHost/React.h"
#include "ReactPropertyBag.h"
#include "winrt/Microsoft.ReactNative.h"

namespace facebook::react {

void InitSliderMeasurements(const Mso::React::IReactContext &reactContext) {
  auto slider = xaml::Controls::Slider();
  xaml::Style sliderStyle;
  xaml::Application::Current()
      .Resources()
      .TryLookup(winrt::box_value(winrt::xaml_typename<xaml::Controls::Slider>()))
      .as(sliderStyle);
  slider.Style(sliderStyle);

  slider.Measure({std::numeric_limits<float>::infinity(), std::numeric_limits<float>::infinity()});
  auto size = slider.DesiredSize();
  winrt::Microsoft::ReactNative::ReactPropertyBag(reactContext.Properties())
      .Set(
          winrt::Microsoft::ReactNative::ReactPropertyId<winrt::Windows::Foundation::Size>(L"SliderMeasurements"),
          size);
}

Size SliderMeasurementsManager::measure(SurfaceId surfaceId, LayoutConstraints layoutConstraints) const {
  /*
  auto context = m_contextContainer->at<winrt::Microsoft::ReactNative::ReactContext>("MSRN.ReactContext");
  auto size = *context.Properties().Get(
      winrt::Microsoft::ReactNative::ReactPropertyId<winrt::Windows::Foundation::Size>(L"SliderMeasurements"));

  return {static_cast<float>(size.Width), static_cast<float>(size.Height)};
  */
  return {50, 50}; // TEMP
}

} // namespace facebook::react
