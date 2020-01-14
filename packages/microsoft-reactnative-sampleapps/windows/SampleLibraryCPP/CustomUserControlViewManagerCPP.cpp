// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include "CustomUserControlViewManagerCpp.h"
#include "NativeModules.h"

#include "CustomUserControlCpp.h"
#include "DebugHelpers.h"

using namespace winrt;
using namespace Microsoft::ReactNative;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;

using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Controls;

namespace winrt::SampleLibraryCpp::implementation {

CustomUserControlViewManagerCpp::CustomUserControlViewManagerCpp() {}

// IViewManager
hstring CustomUserControlViewManagerCpp::Name() noexcept {
  return L"CustomUserControlCpp";
}

FrameworkElement CustomUserControlViewManagerCpp::CreateView() noexcept {
  auto const &view = winrt::SampleLibraryCpp::CustomUserControlCpp();

  view.RegisterPropertyChangedCallback(
      winrt::SampleLibraryCpp::CustomUserControlCpp::LabelProperty(),
      [this](
          winrt::Windows::UI::Xaml::DependencyObject obj, winrt::Windows::UI::Xaml::DependencyProperty prop) noexcept {
        if (auto c = obj.try_as<winrt::SampleLibraryCpp::CustomUserControlCpp>()) {
          ReactContext().DispatchEvent(
              c,
              L"topLabelChanged",
              [ this, c ](winrt::Microsoft::ReactNative::IJSValueWriter const &eventDataWriter) noexcept {
                eventDataWriter.WriteString(c.Label());
              });
        }
      });

  return view;
}

// IViewManagerWithReactContext
IReactContext CustomUserControlViewManagerCpp::ReactContext() noexcept {
  return m_reactContext;
}

void CustomUserControlViewManagerCpp::ReactContext(IReactContext reactContext) noexcept {
  m_reactContext = reactContext;
}

// IViewManagerWithNativeProperties
IMapView<hstring, ViewManagerPropertyType> CustomUserControlViewManagerCpp::NativeProps() noexcept {
  auto nativeProps = winrt::single_threaded_map<hstring, ViewManagerPropertyType>();

  nativeProps.Insert(L"label", ViewManagerPropertyType::String);
  nativeProps.Insert(L"color", ViewManagerPropertyType::Color);
  nativeProps.Insert(L"backgroundColor", ViewManagerPropertyType::Color);

  return nativeProps.GetView();
}

void CustomUserControlViewManagerCpp::UpdateProperties(
    FrameworkElement const &view,
    IMapView<hstring, IInspectable> const &propertyMap) {
  if (auto control = view.try_as<winrt::SampleLibraryCpp::CustomUserControlCpp>()) {
    for (auto const &pair : propertyMap) {
      auto const &propertyName = pair.Key();
      auto const &propertyValue = pair.Value();

      if (propertyName == L"label") {
        if (propertyValue != nullptr) {
          auto value = winrt::unbox_value<hstring>(propertyValue);
          control.SetValue(winrt::SampleLibraryCpp::CustomUserControlCpp::LabelProperty(), propertyValue);
        } else {
          control.ClearValue(winrt::SampleLibraryCpp::CustomUserControlCpp::LabelProperty());
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
IMapView<hstring, int64_t> CustomUserControlViewManagerCpp::Commands() noexcept {
  auto commands = winrt::single_threaded_map<hstring, int64_t>();
  commands.Insert(L"CustomCommand", 0);
  return commands.GetView();
}

void CustomUserControlViewManagerCpp::DispatchCommand(
    FrameworkElement const &view,
    int64_t commandId,
    IVectorView<IInspectable> commandArgs) noexcept {
  if (auto control = view.try_as<winrt::SampleLibraryCpp::CustomUserControlCpp>()) {
    if (commandId == 0) {
      std::string arg = std::to_string(winrt::unbox_value<int64_t>(view.Tag()));
      arg.append(", \"");
      arg.append(winrt::to_string(winrt::unbox_value<hstring>(commandArgs.GetAt(0))));
      arg.append("\"");
      ::SampleLibraryCpp::DebugWriteLine(to_string(Name()), "CustomCommand", arg);
    }
  }
}

// IViewManagerWithExportedEventTypeConstants
ConstantProvider CustomUserControlViewManagerCpp::ExportedCustomBubblingEventTypeConstants() noexcept {
  return nullptr;
}

ConstantProvider CustomUserControlViewManagerCpp::ExportedCustomDirectEventTypeConstants() noexcept {
  return [](winrt::Microsoft::ReactNative::IJSValueWriter const &constantWriter) {
    constantWriter.WritePropertyName(L"topLabelChanged");
    constantWriter.WriteObjectBegin();
    WriteProperty(constantWriter, L"registrationName", L"onLabelChanged");
    constantWriter.WriteObjectEnd();
  };
}

} // namespace winrt::SampleLibraryCpp::implementation
