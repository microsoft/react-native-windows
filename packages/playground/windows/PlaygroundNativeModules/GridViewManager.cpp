// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include "GridViewManager.h"

namespace winrt::PlaygroundNativeModules {

winrt::hstring GridViewManager::Name() noexcept {
  return L"PlaygroundGridView";
}

xaml::FrameworkElement GridViewManager::CreateView() noexcept {
  return xaml::Controls::Grid();
}

void GridViewManager::AddView(
    xaml::FrameworkElement const &parent,
    xaml::UIElement const &child,
    int64_t index) noexcept {
  if (auto const &grid = parent.try_as<xaml::Controls::Grid>()) {
    grid.Children().InsertAt(static_cast<uint32_t>(index), child);
  }
}

void GridViewManager::RemoveAllChildren(xaml::FrameworkElement const &parent) noexcept {
  if (auto const &grid = parent.try_as<xaml::Controls::Grid>()) {
    grid.Children().Clear();
  }
}

void GridViewManager::RemoveChildAt(xaml::FrameworkElement const &parent, int64_t index) noexcept {
  if (auto const &grid = parent.try_as<xaml::Controls::Grid>()) {
    grid.Children().RemoveAt(static_cast<uint32_t>(index));
  }
}

void GridViewManager::ReplaceChild(
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

winrt::IMapView<winrt::hstring, React::ViewManagerPropertyType> GridViewManager::NativeProps() noexcept {
  auto nativeProps = winrt::single_threaded_map<winrt::hstring, React::ViewManagerPropertyType>();
  nativeProps.Insert(L"rows", React::ViewManagerPropertyType::Array);
  nativeProps.Insert(L"columns", React::ViewManagerPropertyType::Array);
  return nativeProps.GetView();
}

xaml::GridLength GetGridLength(const winrt::Microsoft::ReactNative::JSValue &v) {
  if (v.Type() == React::JSValueType::Double || v.Type() == React::JSValueType::Int64) {
    return xaml::GridLengthHelper::FromValueAndType(v.AsDouble(), xaml::GridUnitType::Pixel);
  } else if (v.Type() == React::JSValueType::String) {
    auto str = v.AsString();
    double units = 1;
    xaml::GridUnitType unitType = xaml::GridUnitType::Pixel;
    if (str.back() == '*') {
      unitType = xaml::GridUnitType::Star;
      str.pop_back();
      if (str.length() > 0) {
        units = std::stod(str);
      }
    } else if (str == "auto") {
      unitType = xaml::GridUnitType::Auto;
    } else {
      units = std::stod(str);
    }
    return xaml::GridLengthHelper::FromValueAndType(units, unitType);
  }
  return xaml::GridLengthHelper::FromValueAndType(1, xaml::GridUnitType::Auto);
}

void GridViewManager::UpdateProperties(
    xaml::FrameworkElement const &view,
    winrt::Microsoft::ReactNative::IJSValueReader const &propertyMapReader) noexcept {
  React::JSValueObject propertyMap = React::JSValueObject::ReadFrom(propertyMapReader);
  for (const auto &pair : propertyMap) {
    const auto &propertyName = winrt::to_hstring(pair.first);
    const auto &propertyValue = pair.second;

    if (propertyName == L"rows") {
      const auto grid = view.as<xaml::Controls::Grid>();
      for (const auto &row : propertyValue.AsArray()) {
        xaml::Controls::RowDefinition rowDefinition{};
        rowDefinition.Height(GetGridLength(row));
        grid.RowDefinitions().Append(rowDefinition);
      }
    } else if (propertyName == L"columns") {
      const auto grid = view.as<xaml::Controls::Grid>();
      for (const auto &column : propertyValue.AsArray()) {
        xaml::Controls::ColumnDefinition columnDefinition{};
        columnDefinition.Width(GetGridLength(column));
        grid.ColumnDefinitions().Append(columnDefinition);
      }
    }
  }
}

} // namespace winrt::PlaygroundNativeModules
