// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"
#include "ViewPanelViewManager.h"
#include "ViewPanel.h"

#include "NativeModules.h"
#include "JSValueXaml.h"
#include <winrt/Microsoft.ReactNative.h>

#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.UI.Xaml.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Input.h>
#include <winrt/Windows.UI.Xaml.Media.h>

namespace winrt
{
using namespace winrt::Windows::Foundation::Collections;
using namespace winrt::Windows::UI::Xaml;
using namespace winrt::Windows::UI::Xaml::Controls;
using namespace winrt::Windows::UI::Xaml::Input;
using namespace winrt::Windows::UI::Xaml::Media;
} // namespace winrt
using namespace winrt::Microsoft::ReactNative;

namespace Microsoft::ReactNative {
winrt::hstring ViewPanelViewManager::Name() noexcept
{
    return L"GDSViewPanel"; //TODO: remove or rename
}

winrt::FrameworkElement ViewPanelViewManager::CreateView() noexcept
{
    return winrt::Gaming::React::Primitives::ViewPanel();
}

winrt::IMapView<hstring, ViewManagerPropertyType> ViewPanelViewManager::NativeProps() noexcept
{
    auto nativeProps = winrt::single_threaded_map<winrt::hstring, ViewManagerPropertyType>();

    nativeProps.Insert(L"importantForAccessibility", ViewManagerPropertyType::Boolean);

    return nativeProps.GetView();
}

void ViewPanelViewManager::UpdateProperties(winrt::FrameworkElement const& view, IJSValueReader const& propertyMapReader) noexcept
{
    auto element = view.as<winrt::Microsoft::ReactNative::ViewPanel>();
    if (element) {
        const JSValueObject& propertyMap = JSValue::ReadObjectFrom(propertyMapReader);

        const JSValue *backgroundColor = nullptr;
        const JSValue *backgroundTint = nullptr;
        const JSValue *backgroundFallbackColor = nullptr;
        const JSValue *backgroundOpacity = nullptr;

        for (auto const& pair : propertyMap) {
            auto const& propertyName = pair.first;
            auto const& propertyValue = pair.second;

            if (propertyName == "importantForAccessibility") {
              if (propertyValue.Type() == winrt::Microsoft::ReactNative::JSValueType::String) {
                auto value = propertyValue.AsString();
                if (value == "no-hide-descendants") {
                  element.IsNarratorHidden(propertyValue.To<bool>());
                  element.IsTabHidden(propertyValue.To<bool>());
                }
              } else if (propertyValue.IsNull()) {
                element.ClearValue(winrt::Microsoft::ReactNative::ViewPanel::IsNarratorHiddenProperty()); 
                element.ClearValue(winrt::Microsoft::ReactNative::ViewPanel::IsTabHiddenProperty());
              }
            }
        }
    }
}

void ViewPanelViewManager::AddView(winrt::FrameworkElement const& parent, winrt::UIElement const& child, int64_t index) noexcept
{
    if (auto container = parent.try_as<winrt::Panel>()) {
        container.Children().InsertAt(static_cast<uint32_t>(index), child.as<winrt::UIElement>());
    }
}

void ViewPanelViewManager::RemoveChildAt(winrt::FrameworkElement const& parent, int64_t index) noexcept
{
    if (auto container = parent.try_as<winrt::Panel>()) {
        container.Children().RemoveAt(static_cast<uint32_t>(index));
    }
}

void ViewPanelViewManager::RemoveAllChildren(winrt::FrameworkElement const& parent) noexcept
{
    if (auto container = parent.try_as<winrt::Panel>()) {
        container.Children().Clear();
    }
}

void ViewPanelViewManager::ReplaceChild(winrt::FrameworkElement const& parent, winrt::UIElement const& oldChild, winrt::UIElement const& newChild) noexcept
{
    if (auto container = parent.try_as<winrt::Panel>()) {
        uint32_t index;
        if (container.Children().IndexOf(oldChild.as<winrt::UIElement>(), index)) {
            container.Children().SetAt(index, newChild.as<winrt::UIElement>());
        } else {
            container.Children().Append(newChild.as<winrt::UIElement>());
        }
    }
}

} // namespace Microsoft::ReactNative