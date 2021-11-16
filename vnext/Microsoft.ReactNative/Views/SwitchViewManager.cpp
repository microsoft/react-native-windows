// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"
#include "SwitchViewManager.h"
#include <IReactInstance.h>
#include <JSValueWriter.h>
#include <UI.Xaml.Controls.h>
#include <UI.Xaml.Shapes.h>
#include <Utils/ResourceBrushUtils.h>
#include <Utils/ValueUtils.h>
#include <Views/ShadowNodeBase.h>
using namespace Microsoft::ReactNative;

namespace winrt {
using namespace xaml;
using namespace xaml::Controls;
using namespace xaml::Shapes;
} // namespace winrt

namespace Microsoft::ReactNative {

class SwitchShadowNode : public ShadowNodeBase {
  using Super = ShadowNodeBase;

 public:
  SwitchShadowNode() = default;
  void createView(const winrt::Microsoft::ReactNative::JSValueObject &) override;
  void updateProperties(winrt::Microsoft::ReactNative::JSValueObject &props) override;
  void UpdateThumbColor();
  void UpdateTrackColorOn();
  void UpdateTrackColorOff();
  void dispatchCommand(const std::string &commandId, winrt::Microsoft::ReactNative::JSValueArray &&commandArgs);

 private:
  static void OnToggled(const Mso::React::IReactContext &context, int64_t tag, bool newValue);

  winrt::ToggleSwitch::Toggled_revoker m_toggleSwitchToggledRevoker{};
  winrt::ToggleSwitch::Loading_revoker m_toggleSwitchLoadingRevoker{};

  winrt::Microsoft::ReactNative::JSValue m_thumbColor;
  winrt::Microsoft::ReactNative::JSValue m_offTrackColor;
  winrt::Microsoft::ReactNative::JSValue m_onTrackColor;
};

// sets the resources to Empty SolidColorBrushes if the user sets the color before runtime, this lets the user change
// the color of the SolidColorBrush at runtime and keeps the toggleswitches who did not change colors on the default
// ThemeResource
void setEmptyResouceBrushes(
    const winrt::Microsoft::ReactNative::JSValueObject &props,
    const xaml::Controls::ToggleSwitch toggleSwitch) {
  auto brush = xaml::Media::SolidColorBrush();
  for (const auto &pair : props) {
    const std::string &propertyName = pair.first;
    const auto &propertyValue = pair.second;
    if (propertyName == "thumbTintColor") {
      UpdateResourceBrush(toggleSwitch, L"ToggleSwitchKnobFillOff", xaml::Media::SolidColorBrush());
      UpdateResourceBrush(toggleSwitch, L"ToggleSwitchKnobFillOffPointerOver", xaml::Media::SolidColorBrush());
      UpdateResourceBrush(toggleSwitch, L"ToggleSwitchKnobFillOffPressed", xaml::Media::SolidColorBrush());
      UpdateResourceBrush(toggleSwitch, L"ToggleSwitchKnobFillOffDisabled", xaml::Media::SolidColorBrush());

      UpdateResourceBrush(toggleSwitch, L"ToggleSwitchKnobFillOnPointerOver", xaml::Media::SolidColorBrush());
      UpdateResourceBrush(toggleSwitch, L"ToggleSwitchKnobFillOnDisabled", xaml::Media::SolidColorBrush());
      UpdateResourceBrush(toggleSwitch, L"ToggleSwitchKnobFillOn", xaml::Media::SolidColorBrush());
      UpdateResourceBrush(toggleSwitch, L"ToggleSwitchKnobFillOnPressed", xaml::Media::SolidColorBrush());

    } else if (propertyName == "tintColor") {
      UpdateResourceBrush(toggleSwitch, L"ToggleSwitchFillOff", xaml::Media::SolidColorBrush());
      UpdateResourceBrush(toggleSwitch, L"ToggleSwitchFillOffPressed", xaml::Media::SolidColorBrush());
      UpdateResourceBrush(toggleSwitch, L"ToggleSwitchFillOffDisabled", xaml::Media::SolidColorBrush());
      UpdateResourceBrush(toggleSwitch, L"ToggleSwitchFillOffPointerOver", xaml::Media::SolidColorBrush());

    } else if (propertyName == "onTintColor") {
      UpdateResourceBrush(toggleSwitch, L"ToggleSwitchFillOn", xaml::Media::SolidColorBrush());
      UpdateResourceBrush(toggleSwitch, L"ToggleSwitchFillOnPointerOver", xaml::Media::SolidColorBrush());
      UpdateResourceBrush(toggleSwitch, L"ToggleSwitchFillOnPressed", xaml::Media::SolidColorBrush());
      UpdateResourceBrush(toggleSwitch, L"ToggleSwitchFillOnDisabled", xaml::Media::SolidColorBrush());
    }
  }
}

void SwitchShadowNode::createView(const winrt::Microsoft::ReactNative::JSValueObject &props) {
  Super::createView(props);
  auto toggleSwitch = GetView().as<winrt::ToggleSwitch>();

  setEmptyResouceBrushes(props, toggleSwitch);

  m_toggleSwitchToggledRevoker = toggleSwitch.Toggled(winrt::auto_revoke, [=](auto &&, auto &&) {
    if (m_offTrackColor)
      UpdateTrackColorOff();
    if (m_onTrackColor)
      UpdateTrackColorOn();

    if (!m_updating)
      OnToggled(GetViewManager()->GetReactContext(), m_tag, toggleSwitch.IsOn());
  });

  // properties can come down early before native XAML element added into tree
  // hook up loading event which is called right at beginning of Measure
  m_toggleSwitchLoadingRevoker = toggleSwitch.Loading(winrt::auto_revoke, [=](auto &&, auto &&) {
    UpdateThumbColor();
    UpdateTrackColorOn();
    UpdateTrackColorOff();
  });
}

void SwitchShadowNode::UpdateThumbColor() {
  auto toggleSwitch = GetView().as<winrt::ToggleSwitch>();

  const auto thumbBrush = IsValidColorValue(m_thumbColor) ? BrushFrom(m_thumbColor) : nullptr;
  if (thumbBrush != nullptr) {
    UpdateToggleSwitchThumbResourceBrushes(toggleSwitch, thumbBrush);
  }
}

void SwitchShadowNode::UpdateTrackColorOn() {
  auto toggleSwitch = GetView().as<winrt::ToggleSwitch>();

  const auto onTrackBrush = IsValidColorValue(m_onTrackColor) ? BrushFrom(m_onTrackColor) : nullptr;
  UpdateToggleSwitchTrackResourceBrushesOn(toggleSwitch, onTrackBrush);
}

void SwitchShadowNode::UpdateTrackColorOff() {
  auto toggleSwitch = GetView().as<winrt::ToggleSwitch>();

  const auto offTrackBrush = IsValidColorValue(m_offTrackColor) ? BrushFrom(m_offTrackColor) : nullptr;
  UpdateToggleSwitchTrackResourceBrushesOff(toggleSwitch, offTrackBrush);
}

void SwitchShadowNode::updateProperties(winrt::Microsoft::ReactNative::JSValueObject &props) {
  m_updating = true;
  auto toggleSwitch = GetView().as<winrt::ToggleSwitch>();

  Super::updateProperties(props);
  for (const auto &pair : props) {
    const std::string &propertyName = pair.first;
    const auto &propertyValue = pair.second;
    // RN maps thumbColor to deprecated thumbTintColor,
    // and trackColor to tintColor(not toggled state) and onTintColor( toggled
    // state)
    if (propertyName == "thumbTintColor") {
      m_thumbColor = propertyValue.Copy();
      UpdateThumbColor();
    } else if (propertyName == "tintColor") {
      m_offTrackColor = propertyValue.Copy();
      UpdateTrackColorOff();
    } else if (propertyName == "onTintColor") {
      m_onTrackColor = propertyValue.Copy();
      UpdateTrackColorOn();
    }
  }

  m_updating = false;
}

void SwitchShadowNode::dispatchCommand(
    const std::string &commandId,
    winrt::Microsoft::ReactNative::JSValueArray &&commandArgs) {
  if (commandId == "setValue") {
    auto value = commandArgs[0].AsBoolean();
    auto toggleSwitch = GetView().as<winrt::ToggleSwitch>();
    m_updating = true;
    toggleSwitch.IsOn(value);
    m_updating = false;
  } else {
    Super::dispatchCommand(commandId, std::move(commandArgs));
  }
}

/*static*/ void SwitchShadowNode::OnToggled(const Mso::React::IReactContext &context, int64_t tag, bool newValue) {
  folly::dynamic eventData = folly::dynamic::object("target", tag)("value", newValue);
  context.DispatchEvent(tag, "topChange", std::move(eventData));
}

SwitchViewManager::SwitchViewManager(const Mso::React::IReactContext &context) : Super(context) {}

const wchar_t *SwitchViewManager::GetName() const {
  return L"RCTSwitch";
}

void SwitchViewManager::GetNativeProps(const winrt::Microsoft::ReactNative::IJSValueWriter &writer) const {
  Super::GetNativeProps(writer);

  winrt::Microsoft::ReactNative::WriteProperty(writer, L"value", L"boolean");
  winrt::Microsoft::ReactNative::WriteProperty(writer, L"disabled", L"boolean");
  winrt::Microsoft::ReactNative::WriteProperty(writer, L"thumbTintColor", L"Color");
  winrt::Microsoft::ReactNative::WriteProperty(writer, L"tintColor", L"Color");
  winrt::Microsoft::ReactNative::WriteProperty(writer, L"onTintColor", L"Color");
}

ShadowNode *SwitchViewManager::createShadow() const {
  return new SwitchShadowNode();
}

XamlView SwitchViewManager::CreateViewCore(int64_t /*tag*/, const winrt::Microsoft::ReactNative::JSValueObject &) {
  auto toggleSwitch = winrt::ToggleSwitch();
  toggleSwitch.OnContent(nullptr);
  toggleSwitch.OffContent(nullptr);

  return toggleSwitch;
}

bool SwitchViewManager::UpdateProperty(
    ShadowNodeBase *nodeToUpdate,
    const std::string &propertyName,
    const winrt::Microsoft::ReactNative::JSValue &propertyValue) {
  auto toggleSwitch = nodeToUpdate->GetView().as<winrt::ToggleSwitch>();
  if (toggleSwitch == nullptr)
    return true;

  if (propertyName == "disabled") {
    if (propertyValue.Type() == winrt::Microsoft::ReactNative::JSValueType::Boolean)
      toggleSwitch.IsEnabled(!propertyValue.AsBoolean());
    else if (propertyValue.IsNull())
      toggleSwitch.ClearValue(xaml::Controls::Control::IsEnabledProperty());
  } else if (propertyName == "value") {
    if (propertyValue.Type() == winrt::Microsoft::ReactNative::JSValueType::Boolean)
      toggleSwitch.IsOn(propertyValue.AsBoolean());
    else if (propertyValue.IsNull())
      toggleSwitch.ClearValue(winrt::ToggleSwitch::IsOnProperty());
  } else {
    return Super::UpdateProperty(nodeToUpdate, propertyName, propertyValue);
  }
  return true;
}

} // namespace Microsoft::ReactNative
