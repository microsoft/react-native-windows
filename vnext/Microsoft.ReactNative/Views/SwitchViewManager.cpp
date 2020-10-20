// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"

#include <IReactInstance.h>
#include <JSValueWriter.h>
#include <UI.Xaml.Controls.h>
#include <UI.Xaml.Shapes.h>
#include <Utils/ResourceBrushUtils.h>
#include <Utils/ValueUtils.h>
#include <Views/ShadowNodeBase.h>
#include "SwitchViewManager.h"

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
  void createView() override;
  void updateProperties(winrt::Microsoft::ReactNative::JSValueObject &props) override;
  void UpdateThumbColor();
  void UpdateTrackColor();
  void dispatchCommand(const std::string &commandId, winrt::Microsoft::ReactNative::JSValueArray &&commandArgs);

 private:
  static void OnToggled(const Mso::React::IReactContext &context, int64_t tag, bool newValue);

  winrt::ToggleSwitch::Toggled_revoker m_toggleSwitchToggledRevoker{};
  winrt::ToggleSwitch::Loading_revoker m_toggleSwitchLoadingRevoker{};

  winrt::Microsoft::ReactNative::JSValue m_thumbColor;
  winrt::Microsoft::ReactNative::JSValue m_offTrackColor;
  winrt::Microsoft::ReactNative::JSValue m_onTrackColor;
};

void SwitchShadowNode::createView() {
  Super::createView();

  auto toggleSwitch = GetView().as<winrt::ToggleSwitch>();
  m_toggleSwitchToggledRevoker = toggleSwitch.Toggled(winrt::auto_revoke, [=](auto &&, auto &&) {
    UpdateTrackColor();
    if (!m_updating)
      OnToggled(GetViewManager()->GetReactContext(), m_tag, toggleSwitch.IsOn());
  });

  // properties can come down early before native XAML element added into tree
  // hook up loading event which is called right at beginning of Measure
  m_toggleSwitchLoadingRevoker = toggleSwitch.Loading(winrt::auto_revoke, [=](auto &&, auto &&) {
    UpdateThumbColor();
    UpdateTrackColor();
  });
}

void SwitchShadowNode::UpdateThumbColor() {
  auto toggleSwitch = GetView().as<winrt::ToggleSwitch>();
  if (toggleSwitch == nullptr)
    return;

  const auto thumbBrush = react::uwp::IsValidColorValue(m_thumbColor) ? react::uwp::BrushFrom(m_thumbColor) : nullptr;
  react::uwp::UpdateToggleSwitchThumbResourceBrushes(toggleSwitch, thumbBrush);
}

void SwitchShadowNode::UpdateTrackColor() {
  auto toggleSwitch = GetView().as<winrt::ToggleSwitch>();
  if (toggleSwitch == nullptr)
    return;

  const auto onTrackBrush =
      react::uwp::IsValidColorValue(m_onTrackColor) ? react::uwp::BrushFrom(m_onTrackColor) : nullptr;
  const auto offTrackBrush =
      react::uwp::IsValidColorValue(m_offTrackColor) ? react::uwp::BrushFrom(m_offTrackColor) : nullptr;
  react::uwp::UpdateToggleSwitchTrackResourceBrushes(toggleSwitch, onTrackBrush, offTrackBrush);
}

void SwitchShadowNode::updateProperties(winrt::Microsoft::ReactNative::JSValueObject &props) {
  m_updating = true;

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
      UpdateTrackColor();
    } else if (propertyName == "onTintColor") {
      m_onTrackColor = propertyValue.Copy();
      UpdateTrackColor();
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

XamlView SwitchViewManager::CreateViewCore(int64_t /*tag*/) {
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
