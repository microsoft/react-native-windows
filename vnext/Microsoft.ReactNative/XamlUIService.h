// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include "XamlUIService.g.h"
#include "INativeUIManager.h"
#include "ReactHost/React.h"
#include "ReactPropertyBag.h"
#include "winrt/Microsoft.ReactNative.h"

namespace winrt::Microsoft::ReactNative::implementation {
struct XamlUIService : XamlUIServiceT<XamlUIService> {
 public:
  XamlUIService(Mso::CntPtr<Mso::React::IReactContext> &&context) noexcept;
  static ReactPropertyId<XamlUIService> XamlUIServiceProperty() noexcept;

  xaml::DependencyObject ElementFromReactTag(int64_t reactTag) noexcept;
  static winrt::Microsoft::ReactNative::XamlUIService FromContext(IReactContext context);
  void DispatchEvent(
      xaml::FrameworkElement const &view,
      hstring const &eventName,
      JSValueArgWriter const &eventDataArgWriter) noexcept;

  static void SetXamlRoot(IReactPropertyBag const &properties, xaml::XamlRoot const &xamlRoot) noexcept;
  static void SetAccessibleRoot(
      IReactPropertyBag const &properties,
      xaml::FrameworkElement const &accessibleRoot) noexcept;
  static xaml::XamlRoot GetXamlRoot(IReactPropertyBag const &properties) noexcept;
  static xaml::FrameworkElement GetAccessibleRoot(IReactPropertyBag const &properties) noexcept;

  static void SetIslandWindowHandle(IReactPropertyBag const &properties, uint64_t hwnd) noexcept;
  static uint64_t GetIslandWindowHandle(IReactPropertyBag const &properties) noexcept;

 private:
  std::weak_ptr<::Microsoft::ReactNative::INativeUIManagerHost> m_wkUIManager;
  Mso::CntPtr<Mso::React::IReactContext> m_context;
};

} // namespace winrt::Microsoft::ReactNative::implementation

namespace winrt::Microsoft::ReactNative::factory_implementation {
struct XamlUIService : XamlUIServiceT<XamlUIService, implementation::XamlUIService> {};
} // namespace winrt::Microsoft::ReactNative::factory_implementation
