// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include "CustomUserControlViewManagerCPP.h"

#include "CustomUserControlCPP.h"
#include "DebugHelpers.h"

using namespace winrt;
using namespace Microsoft::ReactNative::Bridge;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;

using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Controls;

namespace winrt::SampleLibraryCPP::implementation {

CustomUserControlViewManagerCPP::CustomUserControlViewManagerCPP(IReactContext const &reactContext)
    : m_reactContext{reactContext} {}

IReactContext CustomUserControlViewManagerCPP::ReactContext() noexcept {
  return m_reactContext;
}

// IViewManager
hstring CustomUserControlViewManagerCPP::Name() noexcept {
  return L"CustomUserControlCPP";
}

FrameworkElement CustomUserControlViewManagerCPP::CreateView() noexcept {
  auto const &view = winrt::SampleLibraryCPP::CustomUserControlCPP();

  view.RegisterPropertyChangedCallback(
      winrt::SampleLibraryCPP::CustomUserControlCPP::LabelProperty(),
      [this](
          winrt::Windows::UI::Xaml::DependencyObject obj, winrt::Windows::UI::Xaml::DependencyProperty prop) noexcept {
        if (auto c = obj.try_as<winrt::SampleLibraryCPP::CustomUserControlCPP>()) {
          ReactContext().DispatchEvent(c, L"topLabelChanged", box_value(c.Label()));
        }
      });

  return view;
}

// IViewManagerWithNativeProperties
IMapView<hstring, ViewManagerPropertyType> CustomUserControlViewManagerCPP::NativeProps() noexcept {
  auto nativeProps = winrt::single_threaded_map<hstring, ViewManagerPropertyType>();

  nativeProps.Insert(L"label", ViewManagerPropertyType::String);
  nativeProps.Insert(L"color", ViewManagerPropertyType::Color);
  nativeProps.Insert(L"backgroundColor", ViewManagerPropertyType::Color);

  return nativeProps.GetView();
}

void CustomUserControlViewManagerCPP::UpdateProperties(
    FrameworkElement const &view,
    IMapView<hstring, IInspectable> const &propertyMap) {
  if (auto control = view.try_as<winrt::SampleLibraryCPP::CustomUserControlCPP>()) {
    for (auto const &pair : propertyMap) {
      auto const &propertyName = pair.Key();
      auto const &propertyValue = pair.Value();

      if (propertyName == L"label") {
        if (propertyValue != nullptr) {
          auto value = winrt::unbox_value<hstring>(propertyValue);
          control.SetValue(winrt::SampleLibraryCPP::CustomUserControlCPP::LabelProperty(), propertyValue);
        } else {
          control.ClearValue(winrt::SampleLibraryCPP::CustomUserControlCPP::LabelProperty());
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

// IViewManagerWithCommands
IMapView<hstring, int64_t> CustomUserControlViewManagerCPP::Commands() noexcept {
  auto commands = winrt::single_threaded_map<hstring, int64_t>();
  commands.Insert(L"CustomCommand", 0);
  return commands.GetView();
}

void CustomUserControlViewManagerCPP::DispatchCommand(
    FrameworkElement const &view,
    int64_t commandId,
    IVectorView<IInspectable> commandArgs) noexcept {
  if (auto control = view.try_as<winrt::SampleLibraryCPP::CustomUserControlCPP>()) {
    if (commandId == 0) {
      std::string arg = std::to_string(winrt::unbox_value<int64_t>(view.Tag()));
      arg.append(", \"");
      arg.append(winrt::to_string(winrt::unbox_value<hstring>(commandArgs.GetAt(0))));
      arg.append("\"");
      ::SampleLibraryCPP::DebugWriteLine(to_string(Name()), "CustomCommand", arg);
    }
  }
}

// IViewManagerWithExportedEventTypeConstants
IMapView<hstring, IInspectable> CustomUserControlViewManagerCPP::ExportedCustomBubblingEventTypeConstants() noexcept {
  auto constants = winrt::single_threaded_map<hstring, IInspectable>();
  return constants.GetView();
}

IMapView<hstring, IInspectable> CustomUserControlViewManagerCPP::ExportedCustomDirectEventTypeConstants() noexcept {
  auto constants = winrt::single_threaded_map<hstring, IInspectable>();

  auto registration = winrt::single_threaded_map<hstring, IInspectable>();
  registration.Insert(L"registrationName", box_value(L"onLabelChanged"));

  constants.Insert(L"topLabelChanged", registration.GetView());

  return constants.GetView();
}

} // namespace winrt::SampleLibraryCPP::implementation
