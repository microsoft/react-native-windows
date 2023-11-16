// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"

#include "ModalViewManager.h"

#include <JSValueWriter.h>
#include <UI.Xaml.Controls.h>
#include <Utils/PropertyUtils.h>
#include <Utils/ValueUtils.h>
#include <ViewControl.h>

namespace Microsoft::ReactNative {

ModalViewManager::ModalViewManager(const Mso::React::IReactContext &context) : Super(context) {}

const wchar_t *ModalViewManager::GetName() const {
  return L"RCTModalHostView";
}

void ModalViewManager::GetNativeProps(const winrt::Microsoft::ReactNative::IJSValueWriter &props) const {
  Super::GetNativeProps(props);

  winrt::Microsoft::ReactNative::WriteProperty(props, L"animationType", L"string");
  winrt::Microsoft::ReactNative::WriteProperty(props, L"transparent", L"boolean");
  winrt::Microsoft::ReactNative::WriteProperty(props, L"visible", L"boolean");
}

ShadowNode *Microsoft::ReactNative::ModalViewManager::createShadow() const {
  ShadowNode *shadowNode = Super::createShadow();
  ShadowNodeBase *shadowNodeBase = static_cast<ShadowNodeBase *>(shadowNode);
  shadowNodeBase->IsFocusable(false);
  return shadowNode;
}

XamlView ModalViewManager::CreateViewCore(int64_t /*tag*/, const winrt::Microsoft::ReactNative::JSValueObject &) {
  auto contentControl = winrt::make<winrt::Microsoft::ReactNative::implementation::ViewControl>();
  return contentControl;
}

bool ModalViewManager::UpdateProperty(
    ShadowNodeBase *nodeToUpdate,
    const std::string &propertyName,
    const winrt::Microsoft::ReactNative::JSValue &propertyValue) {
  return Super::UpdateProperty(nodeToUpdate, propertyName, propertyValue);
}

} // namespace Microsoft::ReactNative
