// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"

#include <Views/ShadowNodeBase.h>
#include "SliderViewManager.h"

#include <JSValueWriter.h>
#include <Utils/ValueUtils.h>

#include <IReactInstance.h>
#include <UI.Composition.h>
#include <UI.Xaml.Controls.Primitives.h>
#include <UI.Xaml.Controls.h>

namespace Microsoft::ReactNative {

class SliderShadowNode : public ShadowNodeBase {
  using Super = ShadowNodeBase;

 public:
  SliderShadowNode() = default;
  void createView(const winrt::Microsoft::ReactNative::JSValueObject &) override;
  void updateProperties(winrt::Microsoft::ReactNative::JSValueObject &props) override;
};

void SliderShadowNode::createView(const winrt::Microsoft::ReactNative::JSValueObject &props) {
  Super::createView(props);
}

void SliderShadowNode::updateProperties(winrt::Microsoft::ReactNative::JSValueObject &props) {
  m_updating = true;
  Super::updateProperties(props);
  m_updating = false;
}

SliderViewManager::SliderViewManager(const Mso::React::IReactContext &context) : Super(context) {}

const wchar_t *SliderViewManager::GetName() const {
  return L"RCTSlider";
}

void SliderViewManager::GetNativeProps(const winrt::Microsoft::ReactNative::IJSValueWriter &writer) const {
  Super::GetNativeProps(writer);

  winrt::Microsoft::ReactNative::WriteProperty(writer, L"value", L"integer");
  winrt::Microsoft::ReactNative::WriteProperty(writer, L"disabled", L"boolean");
}

ShadowNode *SliderViewManager::createShadow() const {
  return new SliderShadowNode();
}

XamlView SliderViewManager::CreateViewCore(int64_t /*tag*/, const winrt::Microsoft::ReactNative::JSValueObject &) {
  auto slider = xaml::Controls::Slider();
  return slider;
}

bool SliderViewManager::UpdateProperty(
    ShadowNodeBase *nodeToUpdate,
    const std::string &propertyName,
    const winrt::Microsoft::ReactNative::JSValue &propertyValue) {
  auto slider = nodeToUpdate->GetView().as<xaml::Controls::Slider>();
  if (slider == nullptr)
    return true;

  if (propertyName == "disabled") {
    if (propertyValue.Type() == winrt::Microsoft::ReactNative::JSValueType::Boolean)
      slider.IsEnabled(!propertyValue.AsBoolean());
    else if (propertyValue.IsNull())
      slider.ClearValue(xaml::Controls::Control::IsEnabledProperty());
  } else if (propertyName == "value") {
    if (propertyValue.Type() == winrt::Microsoft::ReactNative::JSValueType::Double ||
        propertyValue.Type() == winrt::Microsoft::ReactNative::JSValueType::Int64)
      slider.Value(propertyValue.AsDouble());
    else if (propertyValue.IsNull())
      slider.Value(0);
  } else if (propertyName == "maximumValue") {
    if (propertyValue.Type() == winrt::Microsoft::ReactNative::JSValueType::Double ||
        propertyValue.Type() == winrt::Microsoft::ReactNative::JSValueType::Int64)
      slider.Maximum(propertyValue.AsDouble());
    else if (propertyValue.IsNull())
      slider.Maximum(100);
  } else if (propertyName == "minimumValue") {
    if (propertyValue.Type() == winrt::Microsoft::ReactNative::JSValueType::Double ||
        propertyValue.Type() == winrt::Microsoft::ReactNative::JSValueType::Int64)
      slider.Minimum(propertyValue.AsDouble());
    else if (propertyValue.IsNull())
      slider.Minimum(0);
  } else {
    return Super::UpdateProperty(nodeToUpdate, propertyName, propertyValue);
  }
  return true;
}

} // namespace Microsoft::ReactNative
