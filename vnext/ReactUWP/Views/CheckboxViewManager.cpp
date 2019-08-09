// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"

#include <Views/ShadowNodeBase.h>
#include "CheckboxViewManager.h"

#include <Utils/ValueUtils.h>

#include <IReactInstance.h>

#include <winrt/Windows.UI.Xaml.Controls.Primitives.h>

namespace winrt {
using ToggleButton = Windows::UI::Xaml::Controls::Primitives::ToggleButton;
}

namespace react {
namespace uwp {

class CheckBoxShadowNode : public ShadowNodeBase {
  using Super = ShadowNodeBase;

 public:
  CheckBoxShadowNode() = default;
  void createView() override;
  void updateProperties(const folly::dynamic &&props) override;

 private:
  static void
  OnCheckedChanged(IReactInstance &instance, int64_t tag, bool newValue);

  winrt::CheckBox::Checked_revoker m_checkBoxCheckedRevoker{};
  winrt::CheckBox::Unchecked_revoker m_checkBoxUncheckedRevoker{};
};

void CheckBoxShadowNode::createView() {
  Super::createView();

  auto checkbox = GetView().as<winrt::CheckBox>();
  auto wkinstance = GetViewManager()->GetReactInstance();
  m_checkBoxCheckedRevoker =
      checkbox.Checked(winrt::auto_revoke, [=](auto &&, auto &&) {
        auto instance = wkinstance.lock();
        if (!m_updating && instance != nullptr)
          OnCheckedChanged(*instance, m_tag, true);
      });
  m_checkBoxUncheckedRevoker =
      checkbox.Unchecked(winrt::auto_revoke, [=](auto &&, auto &&) {
        auto instance = wkinstance.lock();
        if (!m_updating && instance != nullptr)
          OnCheckedChanged(*instance, m_tag, false);
      });
}

void CheckBoxShadowNode::updateProperties(const folly::dynamic &&props) {
  m_updating = true;
  Super::updateProperties(std::move(props));
  m_updating = false;
}

/*static*/ void CheckBoxShadowNode::OnCheckedChanged(
    IReactInstance &instance,
    int64_t tag,
    bool newValue) {
  folly::dynamic eventData =
      folly::dynamic::object("target", tag)("value", newValue);
  instance.DispatchEvent(tag, "topChange", std::move(eventData));
}

CheckBoxViewManager::CheckBoxViewManager(
    const std::shared_ptr<IReactInstance> &reactInstance)
    : Super(reactInstance) {}

const char *CheckBoxViewManager::GetName() const {
  return "RCTCheckBox";
}

folly::dynamic CheckBoxViewManager::GetNativeProps() const {
  auto props = Super::GetNativeProps();

  props.update(
      folly::dynamic::object("checked", "boolean")("disabled", "boolean"));

  return props;
}

facebook::react::ShadowNode *CheckBoxViewManager::createShadow() const {
  return new CheckBoxShadowNode();
}

XamlView CheckBoxViewManager::CreateViewCore(int64_t tag) {
  auto checkbox = winrt::CheckBox();
  return checkbox;
}

void CheckBoxViewManager::UpdateProperties(
    ShadowNodeBase *nodeToUpdate,
    const folly::dynamic &reactDiffMap) {
  auto checkbox = nodeToUpdate->GetView().as<winrt::CheckBox>();
  if (checkbox == nullptr)
    return;

  for (const auto &pair : reactDiffMap.items()) {
    const std::string &propertyName = pair.first.getString();
    const folly::dynamic &propertyValue = pair.second;

    if (propertyName == "disabled") {
      if (propertyValue.isBool())
        checkbox.IsEnabled(!propertyValue.asBool());
      else if (pair.second.isNull())
        checkbox.ClearValue(winrt::Control::IsEnabledProperty());
    } else if (propertyName == "checked") {
      if (propertyValue.isBool())
        checkbox.IsChecked(propertyValue.asBool());
      else if (pair.second.isNull())
        checkbox.ClearValue(winrt::ToggleButton::IsCheckedProperty());
    }
  }

  Super::UpdateProperties(nodeToUpdate, reactDiffMap);
}

} // namespace uwp
} // namespace react
