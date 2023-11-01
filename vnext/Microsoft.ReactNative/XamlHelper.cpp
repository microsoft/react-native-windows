// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"
#include "XamlHelper.h"
#include "XamlHelper.g.cpp"

#include <Modules/NativeUIManager.h>
#include <Modules/PaperUIManagerModule.h>
#include <UI.Xaml.Media.h>
#include <Utils/ValueUtils.h>
#include "DynamicWriter.h"
#include "XamlView.h"

namespace winrt::Microsoft::ReactNative::implementation {

xaml::Media::Brush XamlHelper::BrushFrom(JSValueArgWriter const &valueProvider) noexcept {
  auto value = GetFollyDynamicFromValueProvider(valueProvider);
  return ::Microsoft::ReactNative::IsValidColorValue(value) ? ::Microsoft::ReactNative::BrushFrom(value) : nullptr;
}

winrt::Windows::UI::Color XamlHelper::ColorFrom(JSValueArgWriter const &valueProvider) noexcept {
  auto value = GetFollyDynamicFromValueProvider(valueProvider);
  return ::Microsoft::ReactNative::ColorFrom(value);
}

xaml::DependencyProperty XamlHelper::ReactTagProperty() noexcept {
  return ::Microsoft::ReactNative::ReactTagProperty();
}

int64_t XamlHelper::GetReactTag(xaml::DependencyObject const &dependencyObject) noexcept {
  return ::Microsoft::ReactNative::GetTag(dependencyObject);
}

void XamlHelper::SetReactTag(xaml::DependencyObject const &dependencyObject, int64_t tag) noexcept {
  return ::Microsoft::ReactNative::SetTag(dependencyObject, tag);
}

folly::dynamic XamlHelper::GetFollyDynamicFromValueProvider(JSValueArgWriter const &valueProvider) noexcept {
  auto writer = winrt::make_self<DynamicWriter>();
  valueProvider(*writer);
  return writer->TakeValue();
}

} // namespace winrt::Microsoft::ReactNative::implementation

namespace Microsoft::ReactNative {

// Not only react-native, native modules could set tag too for controls.
// For example, to identify an clicked item, customer may add tag in
// NavigationView since content for the two NavigationViewItem are empty.
//
// <NavigationView>
//  <NavigationViewItem Icon="Accept" Tag="1" />
//  <NavigationViewItem Icon="Accept" Tag="2" />
// </NavigationView>
// Instead of deduce view id directly from FrameworkElement.Tag, this do
// additional check by uimanager.
ReactId getViewId(const Mso::React::IReactContext &context, xaml::FrameworkElement const &fe) {
  ReactId reactId{};
  if (auto uiManager = Microsoft::ReactNative::GetNativeUIManager(context).lock()) {
    if (auto peer = uiManager->reactPeerOrContainerFrom(fe)) {
      reactId.isValid = true;
      reactId.tag = Microsoft::ReactNative::GetTag(peer);
    }
  }
  return reactId;
};

std::int32_t CountOpenPopups() {
  // TODO: Use VisualTreeHelper::GetOpenPopupsFromXamlRoot when running against
  // RS6
  winrt::Windows::Foundation::Collections::IVectorView<xaml::Controls::Primitives::Popup> popups =
      xaml::Media::VisualTreeHelper::GetOpenPopups(xaml::Window::Current());
  return (int32_t)popups.Size();
}

} // namespace Microsoft::ReactNative
