// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include "CustomUserControlViewManagerCPP.h"

#include "CustomUserControlCPP.h"

using namespace winrt;
using namespace Microsoft::ReactNative::Bridge;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;

using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Controls;

namespace winrt::SampleLibraryCPP::implementation {

hstring CustomUserControlViewManagerCPP::Name() noexcept {
  return L"CustomUserControlCPP";
}

FrameworkElement CustomUserControlViewManagerCPP::CreateView() noexcept {
  auto view = winrt::SampleLibraryCPP::CustomUserControlCPP();
  return view;
}

IMapView<hstring, IInspectable>
CustomUserControlViewManagerCPP::ExportedViewConstants() noexcept {
  auto constants = winrt::single_threaded_map<hstring, IInspectable>();
  return constants.GetView();
}

IMapView<hstring, IInspectable>
CustomUserControlViewManagerCPP::Commands() noexcept {
  auto commands = winrt::single_threaded_map<hstring, IInspectable>();
  return commands.GetView();
}

IMapView<hstring, ViewManagerPropertyType>
CustomUserControlViewManagerCPP::NativeProps() noexcept {
  auto nativeProps =
      winrt::single_threaded_map<hstring, ViewManagerPropertyType>();

  nativeProps.Insert(L"label", ViewManagerPropertyType::String);
  nativeProps.Insert(L"color", ViewManagerPropertyType::Color);
  nativeProps.Insert(L"backgroundColor", ViewManagerPropertyType::Color);

  return nativeProps.GetView();
}

void CustomUserControlViewManagerCPP::UpdateProperties(
    FrameworkElement const &view,
    IMapView<hstring, IInspectable> const &propertyMap) {
  if (auto control =
          view.try_as<winrt::SampleLibraryCPP::CustomUserControlCPP>()) {
    for (auto const &pair : propertyMap) {
      auto const &propertyName = pair.Key();
      auto const &propertyValue = pair.Value();

      if (propertyName == L"label") {
        if (propertyValue != nullptr) {
          auto value = winrt::unbox_value<hstring>(propertyValue);
          control.SetValue(
              winrt::SampleLibraryCPP::CustomUserControlCPP::LabelProperty(),
              propertyValue);
        } else {
          control.ClearValue(
              winrt::SampleLibraryCPP::CustomUserControlCPP::LabelProperty());
        }
      } else if (propertyName == L"color") {
        if (auto value = propertyValue.try_as<Brush>()) {
          control.SetValue(Control::ForegroundProperty(), propertyValue);
        } else {
          control.ClearValue(Control::ForegroundProperty());
        }
      } else if (propertyName == L"backgroundColor") {
        if (auto value = propertyValue.try_as<Brush>()) {
          control.SetValue(Control::BackgroundProperty(), propertyValue);
        } else {
          control.ClearValue(Control::BackgroundProperty());
        }
      }
    }
  }
}

} // namespace winrt::SampleLibraryCPP::implementation
