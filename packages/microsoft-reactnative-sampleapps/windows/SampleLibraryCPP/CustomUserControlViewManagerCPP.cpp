// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"
#include <winrt/Windows.UI.Xaml.Media.h>

#include "CustomUserControlViewManagerCpp.h"
#include "JSValueReader.h"
#include "JSValueXaml.h"
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
              [this, c](winrt::Microsoft::ReactNative::IJSValueWriter const &eventDataWriter) noexcept {
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
    IJSValueReader const &propertyMapReader) noexcept {
  if (auto control = view.try_as<winrt::SampleLibraryCpp::CustomUserControlCpp>()) {
    JSValueObject propertyMap = JSValueObject::ReadFrom(propertyMapReader);

    for (auto const &pair : propertyMap) {
      auto const &propertyName = pair.first;
      auto const &propertyValue = pair.second;

      if (propertyName == "label") {
        if (!propertyValue.IsNull()) {
          auto value = winrt::box_value(winrt::to_hstring(propertyValue.AsString()));
          control.SetValue(winrt::SampleLibraryCpp::CustomUserControlCpp::LabelProperty(), value);
        } else {
          control.ClearValue(winrt::SampleLibraryCpp::CustomUserControlCpp::LabelProperty());
        }
      } else if (propertyName == "color") {
        if (auto value = propertyValue.To<Brush>()) {
          control.SetValue(Control::ForegroundProperty(), value);
        } else {
          control.ClearValue(Control::ForegroundProperty());
        }
      } else if (propertyName == "backgroundColor") {
        if (auto value = propertyValue.To<Brush>()) {
          control.SetValue(Control::BackgroundProperty(), value);
        } else {
          control.ClearValue(Control::BackgroundProperty());
        }
      }
    }
  }
}

// IViewManagerWithCommands
IVectorView<hstring> CustomUserControlViewManagerCpp::Commands() noexcept {
  auto commands = winrt::single_threaded_vector<hstring>();
  commands.Append(L"CustomCommand");
  return commands.GetView();
}

void CustomUserControlViewManagerCpp::DispatchCommand(
    FrameworkElement const &view,
    winrt::hstring const &commandId,
    IJSValueReader const &commandArgsReader) noexcept {
  if (auto control = view.try_as<winrt::SampleLibraryCpp::CustomUserControlCpp>()) {
    if (commandId == L"CustomCommand") {
      std::string arg = std::to_string(winrt::unbox_value<int64_t>(view.Tag()));
      arg.append(", \"");
      arg.append(winrt::to_string(commandArgsReader.GetString()));
      arg.append("\"");
      ::SampleLibraryCpp::DebugWriteLine(to_string(Name()), "CustomCommand", arg);
    }
  }
}

// IViewManagerWithExportedEventTypeConstants
ConstantProviderDelegate CustomUserControlViewManagerCpp::ExportedCustomBubblingEventTypeConstants() noexcept {
  return nullptr;
}

ConstantProviderDelegate CustomUserControlViewManagerCpp::ExportedCustomDirectEventTypeConstants() noexcept {
  return [](winrt::Microsoft::ReactNative::IJSValueWriter const &constantWriter) {
    constantWriter.WritePropertyName(L"topLabelChanged");
    constantWriter.WriteObjectBegin();
    WriteProperty(constantWriter, L"registrationName", L"onLabelChanged");
    constantWriter.WriteObjectEnd();
  };
}

} // namespace winrt::SampleLibraryCpp::implementation
