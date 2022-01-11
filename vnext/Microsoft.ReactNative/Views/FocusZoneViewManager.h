// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include <NativeModules.h>
#include <winrt/Microsoft.ReactNative.h>

namespace Microsoft::ReactNative {

namespace rn {
using namespace winrt::Microsoft::ReactNative;
}

struct FocusZoneViewManager : winrt::implements<FocusZoneViewManager,
                                                rn::IViewManager,
                                                rn::IViewManagerWithReactContext,
                                                rn::IViewManagerWithNativeProperties,
                                                rn::IViewManagerWithChildren> {
 public:
  FocusZoneViewManager() = default;

  // IViewManager
  winrt::hstring Name() noexcept;
  winrt::Windows::UI::Xaml::FrameworkElement CreateView() noexcept;

  // IViewManagerWithReactContext
  rn::IReactContext ReactContext() noexcept;
  void ReactContext(rn::IReactContext reactContext) noexcept;

  // IViewManagerWithNativeProperties
  winrt::Windows::Foundation::Collections::IMapView<winrt::hstring, rn::ViewManagerPropertyType>
  NativeProps() noexcept;
  void UpdateProperties(winrt::Windows::UI::Xaml::FrameworkElement const& view,
                        rn::IJSValueReader const& propertyMapReader) noexcept;

  // IViewManagerWithChildren
  void AddView(winrt::Windows::UI::Xaml::FrameworkElement const& parent,
               winrt::Windows::UI::Xaml::UIElement const& child,
               int64_t index) noexcept;
  void RemoveAllChildren(winrt::Windows::UI::Xaml::FrameworkElement const& parent) noexcept;
  void RemoveChildAt(winrt::Windows::UI::Xaml::FrameworkElement const& parent,
                     int64_t index) noexcept;
  void ReplaceChild(winrt::Windows::UI::Xaml::FrameworkElement const& parent,
                    winrt::Windows::UI::Xaml::UIElement const& oldChild,
                    winrt::Windows::UI::Xaml::UIElement const& newChild) noexcept;

 private:
  rn::IReactContext m_reactContext{nullptr};
};

} // namespace Microsoft::ReactNative
