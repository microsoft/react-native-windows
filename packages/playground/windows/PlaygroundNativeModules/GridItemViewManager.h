// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once
#include "winrt/Microsoft.ReactNative.h"
#include "NativeModules.h"

namespace winrt::PlaygroundNativeModules {

class GridItemViewManager : public winrt::implements<GridItemViewManager,
                                                     React::IViewManager,
                                                     React::IViewManagerWithChildren,
                                                     React::IViewManagerWithNativeProperties,
                                                     React::IViewManagerRequiresNativeLayout> {
 public:
  // IViewManager
  winrt::hstring Name() noexcept;

  xaml::FrameworkElement CreateView() noexcept;

  // IViewManagerWithChildren
  void AddView(
      xaml::FrameworkElement const &parent,
      xaml::UIElement const &child,
      int64_t index) noexcept;
  void RemoveAllChildren(xaml::FrameworkElement const &parent) noexcept;
  void RemoveChildAt(xaml::FrameworkElement const &parent, int64_t index) noexcept;
  void ReplaceChild(
      xaml::FrameworkElement const &parent,
      xaml::UIElement const &oldChild,
      xaml::UIElement const &newChild) noexcept;

  // IViewManagerWithNativeProperties
  winrt::Windows::Foundation::Collections::
      IMapView<winrt::hstring, React::ViewManagerPropertyType>
      NativeProps() noexcept;

  void UpdateProperties(
      xaml::FrameworkElement const &view,
      React::IJSValueReader const &propertyMapReader) noexcept;

  // IViewManagerRequiresNativeLayout
  bool RequiresNativeLayout() const noexcept {
    return true;
  }
};

} // namespace facebook::archon
