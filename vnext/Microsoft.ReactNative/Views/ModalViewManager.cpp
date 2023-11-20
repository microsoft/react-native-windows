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

class ModalShadowNode : public ShadowNodeBase {
  using Super = ShadowNodeBase;

 public:
  ModalShadowNode() = default;
  void createView(const winrt::Microsoft::ReactNative::JSValueObject &) override;

  void AddView(ShadowNode &child, int64_t /*index*/) override {
    // Not implemented
    RedBox("Modal is not implemented on RNW Paper");
  }

  void RemoveChildAt(int64_t /*indexToRemove*/) override {
    // Not implemented
    RedBox("Modal is not implemented on RNW Paper");
  }

  void ReplaceChild(const XamlView & /*oldChildView*/, const XamlView & /*newChildView*/) override {
    // Not implemented
    RedBox("Modal is not implemented on RNW Paper");
  }
};

void ModalShadowNode::createView(const winrt::Microsoft::ReactNative::JSValueObject &props) {
  Super::createView(props);
  RedBox("Modal is not implemented on RNW Paper");
}

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

ShadowNode *ModalViewManager::createShadow() const {
  return new ModalShadowNode();
}

XamlView ModalViewManager::CreateViewCore(int64_t /*tag*/, const winrt::Microsoft::ReactNative::JSValueObject &) {
  return winrt::make<winrt::Microsoft::ReactNative::implementation::ViewControl>();
}

} // namespace Microsoft::ReactNative
