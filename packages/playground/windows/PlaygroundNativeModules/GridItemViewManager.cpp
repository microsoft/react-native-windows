// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include "GridItemViewManager.h"
#include "GridItemView.h"
#include "JSValue.h"
#include "JSValueXaml.h"

namespace winrt {
using namespace Windows::Foundation::Collections;
} // namespace winrt

namespace winrt::PlaygroundNativeModules {

winrt::hstring GridItemViewManager::Name() noexcept {
  return L"PlaygroundGridItemView";
}

xaml::FrameworkElement GridItemViewManager::CreateView() noexcept {
  return winrt::PlaygroundNativeModules::GridItemView(m_reactContext.Handle());
}

React::IReactContext GridItemViewManager::ReactContext() noexcept {
  return m_reactContext.Handle();
}

void GridItemViewManager::ReactContext(React::IReactContext reactContext) noexcept {
  m_reactContext = reactContext;
}

void GridItemViewManager::AddView(
    xaml::FrameworkElement const &parent,
    xaml::UIElement const &child,
    int64_t index) noexcept {
  if (auto const &grid = parent.try_as<xaml::Controls::Grid>()) {
    if (grid.Children().Size() > 0 || index != 0) {
      m_reactContext.CallJSFunction(L"RCTLog", L"logToConsole", "warn", "GridItem only supports one child.");
    } else {
      grid.Children().InsertAt(static_cast<uint32_t>(index), child);
    }
  }
}

void GridItemViewManager::RemoveAllChildren(xaml::FrameworkElement const &parent) noexcept {
  if (auto const &grid = parent.try_as<xaml::Controls::Grid>()) {
    grid.Children().Clear();
  }
}

void GridItemViewManager::RemoveChildAt(xaml::FrameworkElement const &parent, int64_t index) noexcept {
  if (auto const &grid = parent.try_as<xaml::Controls::Grid>()) {
    if (index == 0) {
      grid.Children().RemoveAt(static_cast<uint32_t>(index));
    }
  }
}

void GridItemViewManager::ReplaceChild(
    xaml::FrameworkElement const &parent,
    xaml::UIElement const &oldChild,
    xaml::UIElement const &newChild) noexcept {
  if (auto const &grid = parent.try_as<xaml::Controls::Grid>()) {
    uint32_t index;
    if (grid.Children().IndexOf(oldChild, index)) {
      grid.Children().RemoveAt(index);
      grid.Children().InsertAt(index, newChild);
    }
  }
}

winrt::IMapView<winrt::hstring, React::ViewManagerPropertyType> GridItemViewManager::NativeProps() noexcept {
  auto nativeProps = winrt::single_threaded_map<winrt::hstring, React::ViewManagerPropertyType>();
  nativeProps.Insert(L"gridRow", React::ViewManagerPropertyType::Number);
  nativeProps.Insert(L"gridColumn", React::ViewManagerPropertyType::Number);
  return nativeProps.GetView();
}

void GridItemViewManager::UpdateProperties(
    xaml::FrameworkElement const &view,
    winrt::Microsoft::ReactNative::IJSValueReader const &propertyMapReader) noexcept {
  React::JSValueObject propertyMap = React::JSValueObject::ReadFrom(propertyMapReader);
  for (const auto &pair : propertyMap) {
    const auto &propertyName = winrt::to_hstring(pair.first);
    const auto &propertyValue = pair.second;

    if (propertyName == L"gridRow") {
      xaml::Controls::Grid::SetRow(view, propertyValue.AsInt32());
    } else if (propertyName == L"gridColumn") {
      xaml::Controls::Grid::SetColumn(view, propertyValue.AsInt32());
    } else if (propertyName == L"backgroundColor") {
      view.as<xaml::Controls::Grid>().Background(propertyValue.To<xaml::Media::Brush>());
    }
  }
}

} // namespace winrt::PlaygroundNativeModules
