// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once
#include "winrt/Microsoft.ReactNative.h"

namespace Microsoft::ReactNative {
    struct ViewPanelViewManager : winrt::implements<
        ViewPanelViewManager,
        winrt::Microsoft::ReactNative::IViewManager,
        winrt::Microsoft::ReactNative::IViewManagerWithNativeProperties, 
        winrt::Microsoft::ReactNative::IViewManagerWithChildren> {
    public:
        ViewPanelViewManager() = default;

        // IViewManager
        winrt::hstring Name() noexcept;
        winrt::Windows::UI::Xaml::FrameworkElement CreateView() noexcept;

        // IViewManagerWithNativeProperties
        winrt::Windows::Foundation::Collections::IMapView<winrt::hstring, winrt::Microsoft::ReactNative::ViewManagerPropertyType>
            NativeProps() noexcept;

        void UpdateProperties(winrt::Windows::UI::Xaml::FrameworkElement const& view,
            winrt::Microsoft::ReactNative::IJSValueReader const& propertyMapReader) noexcept;

        // IViewManagerWithChildren
        void AddView(winrt::Windows::UI::Xaml::FrameworkElement const& parent,
            winrt::Windows::UI::Xaml::UIElement const& child,
            int64_t index) noexcept;

        void RemoveAllChildren(winrt::Windows::UI::Xaml::FrameworkElement const& parent) noexcept;

        void RemoveChildAt(winrt::Windows::UI::Xaml::FrameworkElement const& parent, int64_t index) noexcept;

        void ReplaceChild(winrt::Windows::UI::Xaml::FrameworkElement const& parent,
            winrt::Windows::UI::Xaml::UIElement const& oldChild,
            winrt::Windows::UI::Xaml::UIElement const& newChild) noexcept;
    private:
    };
} // namespace Microsoft::ReactNative
