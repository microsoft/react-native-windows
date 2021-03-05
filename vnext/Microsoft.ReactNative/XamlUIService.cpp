// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"
#include "XamlUIService.h"
#include "XamlUIService.g.cpp"
#include <Modules/NativeUIManager.h>
#include <Modules/PaperUIManagerModule.h>
#include "DynamicWriter.h"
#include "ShadowNodeBase.h"
#include "Views/ShadowNodeBase.h"

namespace winrt::Microsoft::ReactNative::implementation {

XamlUIService::XamlUIService(Mso::CntPtr<Mso::React::IReactContext> &&context) noexcept : m_context(context) {}

xaml::DependencyObject XamlUIService::ElementFromReactTag(int64_t reactTag) noexcept {
  if (auto uiManager = ::Microsoft::ReactNative::GetNativeUIManager(*m_context).lock()) {
    auto shadowNode = uiManager->getHost()->FindShadowNodeForTag(reactTag);
    if (!shadowNode)
      return nullptr;

    return static_cast<::Microsoft::ReactNative::ShadowNodeBase *>(shadowNode)->GetView();
  }
  return nullptr;
}

/*static*/ winrt::Microsoft::ReactNative::XamlUIService XamlUIService::FromContext(IReactContext context) {
  return context.Properties()
      .Get(XamlUIService::XamlUIServiceProperty().Handle())
      .try_as<winrt::Microsoft::ReactNative::XamlUIService>();
}

void XamlUIService::DispatchEvent(
    xaml::FrameworkElement const &view,
    hstring const &eventName,
    JSValueArgWriter const &eventDataArgWriter) noexcept {
  folly::dynamic eventData; // default to NULLT
  if (eventDataArgWriter != nullptr) {
    auto eventDataWriter = winrt::make_self<DynamicWriter>();
    eventDataArgWriter(*eventDataWriter);
    eventData = eventDataWriter->TakeValue();
  }

  auto tag = view ? unbox_value<int64_t>(view.Tag()) : 0;
  m_context->DispatchEvent(tag, to_string(eventName), std::move(eventData));
}

/*static*/ ReactPropertyId<XamlUIService> XamlUIService::XamlUIServiceProperty() noexcept {
  static ReactPropertyId<XamlUIService> uiManagerProperty{L"ReactNative.UIManager", L"XamlUIManager"};
  return uiManagerProperty;
}

ReactPropertyId<xaml::XamlRoot> XamlRootProperty() noexcept {
  static ReactPropertyId<xaml::XamlRoot> propId{L"ReactNative.UIManager", L"XamlRoot"};
  return propId;
}

ReactPropertyId<xaml::XamlRoot> AccessibleXamlRootProperty() noexcept {
  static ReactPropertyId<xaml::XamlRoot> propId{L"ReactNative.UIManager", L"AccessibleXamlRoot"};
  return propId;
}

/*static*/ void XamlUIService::SetXamlRoot(
    IReactPropertyBag const &properties,
    xaml::XamlRoot const &xamlRoot) noexcept {
  winrt::Microsoft::ReactNative::ReactPropertyBag(properties).Set(XamlRootProperty(), xamlRoot);
}

/*static*/ void XamlUIService::SetAccessibleXamlRoot(
    IReactPropertyBag const &properties,
    xaml::XamlRoot const &xamlRoot) noexcept {
  winrt::Microsoft::ReactNative::ReactPropertyBag(properties).Set(AccessibleXamlRootProperty(), xamlRoot);
}

/*static*/ xaml::XamlRoot XamlUIService::GetXamlRoot(IReactPropertyBag const &properties) noexcept {
  return winrt::Microsoft::ReactNative::ReactPropertyBag(properties).Get(XamlRootProperty());
}

/*static*/ xaml::XamlRoot XamlUIService::GetAccessibleXamlRoot(IReactPropertyBag const &properties) noexcept {
  return winrt::Microsoft::ReactNative::ReactPropertyBag(properties).Get(AccessibleXamlRootProperty());
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
