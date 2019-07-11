// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"

#include <Views/ShadowNodeBase.h>
#include "SwitchViewManager.h"

#include <Utils/XamlDirectInstance.h>

#include <IReactInstance.h>

namespace react {
namespace uwp {

class SwitchShadowNode : public ShadowNodeBase {
  using Super = ShadowNodeBase;

 public:
  SwitchShadowNode() = default;
  void createView() override;
  void updateProperties(const folly::dynamic &&props) override;

 private:
  static void OnToggled(IReactInstance &instance, int64_t tag, bool newValue);

  winrt::ToggleSwitch::Toggled_revoker m_toggleSwitchToggledRevoker{};
};

void SwitchShadowNode::createView() {
  Super::createView();

  auto toggleSwitch = GetView().as<winrt::ToggleSwitch>();
  auto wkinstance = GetViewManager()->GetReactInstance();
  m_toggleSwitchToggledRevoker =
      toggleSwitch.Toggled(winrt::auto_revoke, [=](auto &&, auto &&) {
        auto instance = wkinstance.lock();
        if (!m_updating && instance != nullptr)
          OnToggled(*instance, m_tag, toggleSwitch.IsOn());
      });
}

void SwitchShadowNode::updateProperties(const folly::dynamic &&props) {
  m_updating = true;
  Super::updateProperties(std::move(props));
  m_updating = false;
}

/*static*/ void SwitchShadowNode::OnToggled(
    IReactInstance &instance,
    int64_t tag,
    bool newValue) {
  folly::dynamic eventData =
      folly::dynamic::object("target", tag)("value", newValue);
  instance.DispatchEvent(tag, "topChange", std::move(eventData));
}

SwitchViewManager::SwitchViewManager(
    const std::shared_ptr<IReactInstance> &reactInstance)
    : Super(reactInstance) {}

const char *SwitchViewManager::GetName() const {
  return "RCTSwitch";
}

folly::dynamic SwitchViewManager::GetNativeProps() const {
  auto props = Super::GetNativeProps();

  props.update(
      folly::dynamic::object("value", "boolean")("disabled", "boolean"));

  return props;
}

facebook::react::ShadowNode *SwitchViewManager::createShadow() const {
  return new SwitchShadowNode();
}

XamlView SwitchViewManager::CreateViewCore(int64_t tag) {
  auto toggleSwitch = winrt::ToggleSwitch();
  toggleSwitch.OnContent(nullptr);
  toggleSwitch.OffContent(nullptr);

  return toggleSwitch;
}

void SwitchViewManager::UpdateProperties(
    ShadowNodeBase *nodeToUpdate,
    const folly::dynamic &reactDiffMap) {
  const auto toggleSwitch =
      XamlDirectInstance::GetXamlDirect().GetXamlDirectObject(
          nodeToUpdate->GetView().as<winrt::ToggleSwitch>());

  if (toggleSwitch == nullptr)
    return;

  for (const auto &pair : reactDiffMap.items()) {
    const std::string &propertyName = pair.first.getString();
    const folly::dynamic &propertyValue = pair.second;

    if (propertyName == "disabled") {
      if (propertyValue.isBool())
        XamlDirectInstance::GetXamlDirect().SetBooleanProperty(
            toggleSwitch,
            XD::XamlPropertyIndex::Control_IsEnabled,
            !propertyValue.asBool());
      else if (pair.second.isNull())
        XamlDirectInstance::GetXamlDirect().ClearProperty(
            toggleSwitch, XD::XamlPropertyIndex::Control_IsEnabled);
    } else if (propertyName == "value") {
      if (propertyValue.isBool())
        XamlDirectInstance::GetXamlDirect().SetBooleanProperty(
            toggleSwitch,
            XD::XamlPropertyIndex::ToggleSwitch_IsOn,
            propertyValue.asBool());
      else if (pair.second.isNull())
        XamlDirectInstance::GetXamlDirect().ClearProperty(
            toggleSwitch, XD::XamlPropertyIndex::ToggleSwitch_IsOn);
    }
  }

  Super::UpdateProperties(nodeToUpdate, reactDiffMap);
}

} // namespace uwp
} // namespace react
