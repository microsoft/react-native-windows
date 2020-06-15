// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include "XamlUIService.h"
#include "XamlUIService.g.cpp"
#include "DynamicWriter.h"
#include "Views/ShadowNodeBase.h"

namespace winrt::Microsoft::ReactNative::implementation {

XamlUIService::XamlUIService(
    std::weak_ptr<facebook::react::IUIManager> &&uimanager,
    Mso::CntPtr<Mso::React::IReactContext> &&context) noexcept
    : m_wkUIManager(uimanager), m_context(context) {}

winrt::Windows::UI::Xaml::DependencyObject XamlUIService::ElementFromReactTag(int64_t reactTag) noexcept {
  if (auto strongUIManager = m_wkUIManager.lock()) {
    auto shadowNode = strongUIManager->FindShadowNodeForTag(reactTag);
    if (!shadowNode)
      return nullptr;

    return static_cast<react::uwp::ShadowNodeBase *>(shadowNode)->GetView();
  }
  return nullptr;
}

/*static*/ winrt::Microsoft::ReactNative::XamlUIService XamlUIService::FromContext(IReactContext context) {
  return context.Properties()
      .Get(XamlUIService::XamlUIServiceProperty().Handle())
      .try_as<winrt::Microsoft::ReactNative::XamlUIService>();
}

void XamlUIService::DispatchEvent(
    winrt::Windows::UI::Xaml::FrameworkElement const &view,
    hstring const &eventName,
    JSValueArgWriter const &eventDataArgWriter) noexcept {
  folly::dynamic eventData; // default to NULLT
  if (eventDataArgWriter != nullptr) {
    auto eventDataWriter = winrt::make_self<DynamicWriter>();
    eventDataArgWriter(*eventDataWriter);
    eventData = eventDataWriter->TakeValue();
  }

  m_context->DispatchEvent(unbox_value<int64_t>(view.Tag()), to_string(eventName), std::move(eventData));
}

/*static*/ ReactPropertyId<XamlUIService> XamlUIService::XamlUIServiceProperty() noexcept {
  static ReactPropertyId<XamlUIService> uiManagerProperty{L"ReactNative.UIManager", L"XamlUIManager"};
  return uiManagerProperty;
}

} // namespace winrt::Microsoft::ReactNative::implementation
