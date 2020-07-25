// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"

#include <Views/ShadowNodeBase.h>
#include "CheckboxViewManager.h"

#include <UI.Xaml.Controls.Primitives.h>
#include <UI.Xaml.Controls.h>
#include <Utils/ValueUtils.h>

#include <IReactInstance.h>
#include <Modules/NativeUIManager.h>

namespace winrt {
using ToggleButton = xaml::Controls::Primitives::ToggleButton;
}

namespace react::uwp {

class CheckBoxShadowNode : public ShadowNodeBase {
  using Super = ShadowNodeBase;

 public:
  CheckBoxShadowNode() = default;
  void createView() override;
  void updateProperties(const folly::dynamic &&props) override;

  void RegisterChangedEvents();
  void UnregisterChangedEvents();

 private:
  static void OnCheckedChanged(IReactInstance &instance, int64_t tag, bool newValue);

  xaml::Controls::CheckBox::Checked_revoker m_checkBoxCheckedRevoker{};
  xaml::Controls::CheckBox::Unchecked_revoker m_checkBoxUncheckedRevoker{};
};

void CheckBoxShadowNode::createView() {
  Super::createView();

  RegisterChangedEvents();
}

void CheckBoxShadowNode::RegisterChangedEvents() {
  auto checkbox = GetView().as<xaml::Controls::CheckBox>();
  auto wkinstance = GetViewManager()->GetReactInstance();

  m_checkBoxCheckedRevoker = checkbox.Checked(winrt::auto_revoke, [=](auto &&, auto &&) {
    auto instance = wkinstance.lock();
    if (!m_updating && instance != nullptr)
      OnCheckedChanged(*instance, m_tag, true);
  });
  m_checkBoxUncheckedRevoker = checkbox.Unchecked(winrt::auto_revoke, [=](auto &&, auto &&) {
    auto instance = wkinstance.lock();
    if (!m_updating && instance != nullptr)
      OnCheckedChanged(*instance, m_tag, false);
  });
}

void CheckBoxShadowNode::UnregisterChangedEvents() {
  m_checkBoxCheckedRevoker.revoke();
  m_checkBoxUncheckedRevoker.revoke();
}

void CheckBoxShadowNode::updateProperties(const folly::dynamic &&props) {
  m_updating = true;
  Super::updateProperties(std::move(props));
  m_updating = false;
}

/*static*/ void CheckBoxShadowNode::OnCheckedChanged(IReactInstance &instance, int64_t tag, bool newValue) {
  folly::dynamic eventData = folly::dynamic::object("target", tag)("value", newValue);
  instance.DispatchEvent(tag, "topChange", std::move(eventData));
}

CheckBoxViewManager::CheckBoxViewManager(const std::shared_ptr<IReactInstance> &reactInstance) : Super(reactInstance) {}

const char *CheckBoxViewManager::GetName() const {
  return "AndroidCheckBox";
}

folly::dynamic CheckBoxViewManager::GetNativeProps() const {
  auto props = Super::GetNativeProps();

  props.update(folly::dynamic::object("on", "boolean")("enabled", "boolean"));

  return props;
}

facebook::react::ShadowNode *CheckBoxViewManager::createShadow() const {
  return new CheckBoxShadowNode();
}

XamlView CheckBoxViewManager::CreateViewCore(int64_t /*tag*/) {
  auto checkbox = xaml::Controls::CheckBox();
  return checkbox;
}

bool CheckBoxViewManager::UpdateProperty(
    ShadowNodeBase *nodeToUpdate,
    const std::string &propertyName,
    const folly::dynamic &propertyValue) {
  auto checkbox = nodeToUpdate->GetView().as<xaml::Controls::CheckBox>();
  if (checkbox == nullptr)
    return true;

  if (propertyName == "enabled") {
    if (propertyValue.isBool())
      checkbox.IsEnabled(propertyValue.asBool());
    else if (propertyValue.isNull())
      checkbox.ClearValue(xaml::Controls::Control::IsEnabledProperty());
  } else if (propertyName == "on") {
    if (propertyValue.isBool())
      checkbox.IsChecked(propertyValue.asBool());
    else if (propertyValue.isNull())
      checkbox.ClearValue(winrt::ToggleButton::IsCheckedProperty());
  } else {
    return Super::UpdateProperty(nodeToUpdate, propertyName, propertyValue);
  }
  return true;
}

void CheckBoxViewManager::DispatchCommand(
    const XamlView &viewToUpdate,
    const std::string &commandId,
    const folly::dynamic &commandArgs) {
  if (commandId == "setNativeValue") {
    auto value = commandArgs[0].asBool();

    if (auto instance = GetReactInstance().lock()) {
      auto shadow = static_cast<CheckBoxShadowNode *>(
          instance->NativeUIManager()->getHost()->FindShadowNodeForTag(GetTag(viewToUpdate)));
      // we unregister the changed events and then restore them so that we don't fire an
      // onChange event to JS when JS itself is updating the value of the checkbox
      shadow->UnregisterChangedEvents();
      viewToUpdate.as<xaml::Controls::CheckBox>().IsChecked(value);
      shadow->RegisterChangedEvents();
    }

    viewToUpdate.as<xaml::Controls::CheckBox>().IsChecked(value);
  } else {
    Super::DispatchCommand(viewToUpdate, commandId, commandArgs);
  }
}

} // namespace react::uwp
