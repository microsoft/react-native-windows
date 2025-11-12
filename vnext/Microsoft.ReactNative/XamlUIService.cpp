// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"
#include "XamlUIService.h"
#include "XamlUIService.g.cpp"

#include "DynamicWriter.h"

namespace winrt::Microsoft::ReactNative::implementation {

XamlUIService::XamlUIService(Mso::CntPtr<Mso::React::IReactContext> &&context) noexcept : m_context(context) {}

/*static*/ winrt::Microsoft::ReactNative::XamlUIService XamlUIService::FromContext(IReactContext context) {
  return context.Properties()
      .Get(XamlUIService::XamlUIServiceProperty().Handle())
      .try_as<winrt::Microsoft::ReactNative::XamlUIService>();
}

/*static*/ ReactPropertyId<XamlUIService> XamlUIService::XamlUIServiceProperty() noexcept {
  static ReactPropertyId<XamlUIService> uiManagerProperty{L"ReactNative.UIManager", L"XamlUIManager"};
  return uiManagerProperty;
}

ReactPropertyId<xaml::XamlRoot> XamlRootProperty() noexcept {
  static ReactPropertyId<xaml::XamlRoot> propId{L"ReactNative.UIManager", L"XamlRoot"};
  return propId;
}

ReactPropertyId<xaml::FrameworkElement> AccessibleRootProperty() noexcept {
  static ReactPropertyId<xaml::FrameworkElement> propId{L"ReactNative.UIManager", L"AccessibleRoot"};
  return propId;
}

/*static*/ void XamlUIService::SetXamlRoot(
    IReactPropertyBag const &properties,
    xaml::XamlRoot const &xamlRoot) noexcept {
  winrt::Microsoft::ReactNative::ReactPropertyBag(properties).Set(XamlRootProperty(), xamlRoot);
}

/*static*/ void XamlUIService::SetAccessibleRoot(
    IReactPropertyBag const &properties,
    xaml::FrameworkElement const &accessibleRoot) noexcept {
  winrt::Microsoft::ReactNative::ReactPropertyBag(properties).Set(AccessibleRootProperty(), accessibleRoot);
}

/*static*/ xaml::XamlRoot XamlUIService::GetXamlRoot(IReactPropertyBag const &properties) noexcept {
  return winrt::Microsoft::ReactNative::ReactPropertyBag(properties).Get(XamlRootProperty());
}

/*static*/ xaml::FrameworkElement XamlUIService::GetAccessibleRoot(IReactPropertyBag const &properties) noexcept {
  return winrt::Microsoft::ReactNative::ReactPropertyBag(properties).Get(AccessibleRootProperty());
}

ReactPropertyId<uint64_t> XamlIslandProperty() noexcept {
  static ReactPropertyId<uint64_t> propId{L"ReactNative.UIManager", L"XamlIsland"};
  return propId;
}

/*static*/ void XamlUIService::SetIslandWindowHandle(IReactPropertyBag const &properties, uint64_t hwnd) noexcept {
  winrt::Microsoft::ReactNative::ReactPropertyBag(properties).Set(XamlIslandProperty(), hwnd);
}
/*static*/ uint64_t XamlUIService::GetIslandWindowHandle(IReactPropertyBag const &properties) noexcept {
  auto hwnd = winrt::Microsoft::ReactNative::ReactPropertyBag(properties).Get(XamlIslandProperty());
  return hwnd.value_or(0);
}

} // namespace winrt::Microsoft::ReactNative::implementation
