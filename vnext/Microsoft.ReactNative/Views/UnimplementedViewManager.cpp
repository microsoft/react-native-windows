// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"

#include "UnimplementedViewManager.h"

#include <JSValueWriter.h>
#include <UI.Xaml.Controls.h>
#include <Utils/PropertyUtils.h>
#include <Utils/ValueUtils.h>
#include <ViewControl.h>

namespace Microsoft::ReactNative {

class UnimplementedShadowNode : public ShadowNodeBase {
  using Super = ShadowNodeBase;
  std::string m_name;

 public:
  UnimplementedShadowNode(std::wstring name) {
    std::string str;
    std::transform(name.begin(), name.end(), std::back_inserter(str), [](wchar_t c) { return (char)c; });
    m_name = str;
  };

  void createView(const winrt::Microsoft::ReactNative::JSValueObject &) override;

  void AddView(ShadowNode &child, int64_t /*index*/) override {
    // Not implemented
    RedBox(m_name + " is not implemented on RNW Paper");
  }

  void RemoveChildAt(int64_t /*indexToRemove*/) override {
    // Not implemented
    RedBox(m_name + " is not implemented on RNW Paper");
  }

  void ReplaceChild(const XamlView & /*oldChildView*/, const XamlView & /*newChildView*/) override {
    // Not implemented
    RedBox(m_name + " is not implemented on RNW Paper");
  }
};

void UnimplementedShadowNode::createView(const winrt::Microsoft::ReactNative::JSValueObject &props) {
  Super::createView(props);
  RedBox(m_name + " is not implemented on RNW Paper");
}

UnimplementedViewManager::UnimplementedViewManager(const Mso::React::IReactContext &context, std::wstring name)
    : Super(context) {
  m_name = name;
}

const wchar_t *UnimplementedViewManager::GetName() const {
  return m_name.c_str();
}

void UnimplementedViewManager::GetNativeProps(const winrt::Microsoft::ReactNative::IJSValueWriter &writer) const {
  Super::GetNativeProps(writer);

  // Modal View Props (Todo: fix so these aren't in UnimplementedView) #12753
  winrt::Microsoft::ReactNative::WriteProperty(writer, L"animationType", L"string");
  winrt::Microsoft::ReactNative::WriteProperty(writer, L"presentationStyle", L"string");
  winrt::Microsoft::ReactNative::WriteProperty(writer, L"focusable", L"boolean");
  winrt::Microsoft::ReactNative::WriteProperty(writer, L"visible", L"boolean");
}

ShadowNode *UnimplementedViewManager::createShadow() const {
  return new UnimplementedShadowNode(m_name);
}

XamlView UnimplementedViewManager::CreateViewCore(
    int64_t /*tag*/,
    const winrt::Microsoft::ReactNative::JSValueObject &) {
  return winrt::make<winrt::Microsoft::ReactNative::implementation::ViewControl>();
}

} // namespace Microsoft::ReactNative
