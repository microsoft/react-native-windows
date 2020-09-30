// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"

#include <IReactInstance.h>
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

namespace react::uwp {

class SwitchShadowNode : public ShadowNodeBase {
  using Super = ShadowNodeBase;

 public:
  SwitchShadowNode() = default;
  void createView() override;
  void updateProperties(const folly::dynamic &&props) override;
  void UpdateThumbColor();
  void UpdateTrackColor();

 private:
  static void OnToggled(IReactInstance &instance, int64_t tag, bool newValue);

  winrt::ToggleSwitch::Toggled_revoker m_toggleSwitchToggledRevoker{};
  winrt::ToggleSwitch::Loading_revoker m_toggleSwitchLoadingRevoker{};

  folly::dynamic m_thumbColor;
  folly::dynamic m_offTrackColor;
  folly::dynamic m_onTrackColor;
};

void SwitchShadowNode::createView() {
  Super::createView();

  auto toggleSwitch = GetView().as<winrt::ToggleSwitch>();
  auto wkinstance = GetViewManager()->GetReactInstance();
  m_toggleSwitchToggledRevoker = toggleSwitch.Toggled(winrt::auto_revoke, [=](auto &&, auto &&) {
    UpdateTrackColor();
    auto instance = wkinstance.lock();
    if (!m_updating && instance != nullptr)
      OnToggled(*instance, m_tag, toggleSwitch.IsOn());
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

  const auto thumbBrush = IsValidColorValue(m_thumbColor) ? BrushFrom(m_thumbColor) : nullptr;
  UpdateToggleSwitchThumbResourceBrushes(toggleSwitch, thumbBrush);
}

void SwitchShadowNode::UpdateTrackColor() {
  auto toggleSwitch = GetView().as<winrt::ToggleSwitch>();
  if (toggleSwitch == nullptr)
    return;

  const auto onTrackBrush = IsValidColorValue(m_onTrackColor) ? BrushFrom(m_onTrackColor) : nullptr;
  const auto offTrackBrush = IsValidColorValue(m_offTrackColor) ? BrushFrom(m_offTrackColor) : nullptr;
  UpdateToggleSwitchTrackResourceBrushes(toggleSwitch, onTrackBrush, offTrackBrush);
}

void SwitchShadowNode::updateProperties(const folly::dynamic &&props) {
  m_updating = true;

  Super::updateProperties(std::move(props));
  for (const auto &pair : props.items()) {
    const std::string &propertyName = pair.first.getString();
    const folly::dynamic &propertyValue = pair.second;
    // RN maps thumbColor to deprecated thumbTintColor,
    // and trackColor to tintColor(not toggled state) and onTintColor( toggled
    // state)
    if (propertyName == "thumbTintColor") {
      m_thumbColor = propertyValue;
      UpdateThumbColor();
    } else if (propertyName == "tintColor") {
      m_offTrackColor = propertyValue;
      UpdateTrackColor();
    } else if (propertyName == "onTintColor") {
      m_onTrackColor = propertyValue;
      UpdateTrackColor();
    }
  }

  m_updating = false;
}

/*static*/ void SwitchShadowNode::OnToggled(IReactInstance &instance, int64_t tag, bool newValue) {
  folly::dynamic eventData = folly::dynamic::object("target", tag)("value", newValue);
  instance.DispatchEvent(tag, "topChange", std::move(eventData));
}

SwitchViewManager::SwitchViewManager(const std::shared_ptr<IReactInstance> &reactInstance) : Super(reactInstance) {}

const char *SwitchViewManager::GetName() const {
  return "RCTSwitch";
}

folly::dynamic SwitchViewManager::GetNativeProps() const {
  auto props = Super::GetNativeProps();

  props.update(folly::dynamic::object("value", "boolean")("disabled", "boolean")("thumbTintColor", "Color")(
      "tintColor", "Color")("onTintColor", "Color"));

  return props;
}

facebook::react::ShadowNode *SwitchViewManager::createShadow() const {
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
    const folly::dynamic &propertyValue) {
  auto toggleSwitch = nodeToUpdate->GetView().as<winrt::ToggleSwitch>();
  if (toggleSwitch == nullptr)
    return true;

  if (propertyName == "disabled") {
    if (propertyValue.isBool())
      toggleSwitch.IsEnabled(!propertyValue.asBool());
    else if (propertyValue.isNull())
      toggleSwitch.ClearValue(xaml::Controls::Control::IsEnabledProperty());
  } else if (propertyName == "value") {
    if (propertyValue.isBool())
      toggleSwitch.IsOn(propertyValue.asBool());
    else if (propertyValue.isNull())
      toggleSwitch.ClearValue(winrt::ToggleSwitch::IsOnProperty());
  } else {
    return Super::UpdateProperty(nodeToUpdate, propertyName, propertyValue);
  }
  return true;
}

void SwitchViewManager::DispatchCommand(
    const XamlView &viewToUpdate,
    const std::string &commandId,
    const folly::dynamic &commandArgs) {
  if (commandId == "setValue") {
    auto value = commandArgs[0].asBool();
    viewToUpdate.as<winrt::ToggleSwitch>().IsOn(value);
  } else {
    Super::DispatchCommand(viewToUpdate, commandId, commandArgs);
  }
}

} // namespace react::uwp
