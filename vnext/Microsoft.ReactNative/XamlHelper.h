// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once
#include "XamlHelper.g.h"
#include "Base/FollyIncludes.h"

namespace Mso::React {
struct IReactContext;
} // namespace Mso::React

namespace winrt::Microsoft::ReactNative::implementation {

struct XamlHelper : XamlHelperT<XamlHelper> {
  XamlHelper() = default;

  static xaml::Media::Brush BrushFrom(JSValueArgWriter const &valueProvider) noexcept;
  static winrt::Windows::UI::Color ColorFrom(JSValueArgWriter const &valueProvider) noexcept;

  static xaml::DependencyProperty ReactTagProperty() noexcept;
  static int64_t GetReactTag(xaml::DependencyObject const &dependencyObject) noexcept;
  static void SetReactTag(xaml::DependencyObject const &dependencyObject, int64_t tag) noexcept;

  static folly::dynamic GetFollyDynamicFromValueProvider(JSValueArgWriter const &valueProvider) noexcept;
};

} // namespace winrt::Microsoft::ReactNative::implementation

namespace winrt::Microsoft::ReactNative::factory_implementation {
struct XamlHelper : XamlHelperT<XamlHelper, implementation::XamlHelper> {};
} // namespace winrt::Microsoft::ReactNative::factory_implementation

namespace Microsoft::ReactNative {

struct ReactId {
  int64_t tag{0};
  bool isValid{false};
};

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
ReactId getViewId(const Mso::React::IReactContext &context, xaml::FrameworkElement const &fe);

std::int32_t CountOpenPopups();

} // namespace Microsoft::ReactNative
