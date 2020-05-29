// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include "XamlUIService.g.h"
#include "IUIManager.h"

#include "ReactHost/React.h"
#include "ReactPropertyBag.h"
#include "winrt/Microsoft.ReactNative.h"

namespace winrt::Microsoft::ReactNative::implementation {

struct XamlUIService : XamlUIServiceT<XamlUIService> {
 public:
  XamlUIService(
      std::weak_ptr<facebook::react::IUIManager> &&uimanager,
      Mso::CntPtr<Mso::React::IReactContext> &&context) noexcept;
  static ReactPropertyId<XamlUIService> XamlUIServiceProperty() noexcept;

  winrt::Windows::UI::Xaml::DependencyObject ElementFromReactTag(int64_t reactTag) noexcept;
  static winrt::Microsoft::ReactNative::XamlUIService FromContext(IReactContext context);
  void DispatchEvent(
      winrt::Windows::UI::Xaml::FrameworkElement const &view,
      hstring const &eventName,
      JSValueArgWriter const &eventDataArgWriter) noexcept;

 private:
  std::weak_ptr<facebook::react::IUIManager> m_wkUIManager;
  Mso::CntPtr<Mso::React::IReactContext> m_context;
};

} // namespace winrt::Microsoft::ReactNative::implementation

namespace winrt::Microsoft::ReactNative::factory_implementation {
struct XamlUIService : XamlUIServiceT<XamlUIService, implementation::XamlUIService> {};
} // namespace winrt::Microsoft::ReactNative::factory_implementation
