// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"

#include <Views/ShadowNodeBase.h>
#include "CheckboxViewManager.h"

#include <Utils/ValueUtils.h>

#include <IReactInstance.h>

namespace winrt {
using ToggleButton = xaml::Controls::Primitives::ToggleButton;
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
  static void OnCheckedChanged(IReactInstance &instance, int64_t tag, bool newValue);

  xaml::Controls::CheckBox::Checked_revoker m_checkBoxCheckedRevoker{};
  xaml::Controls::CheckBox::Unchecked_revoker m_checkBoxUncheckedRevoker{};
};

void CheckBoxShadowNode::createView() {
  Super::createView();

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
  return "RCTCheckBox";
}

folly::dynamic CheckBoxViewManager::GetNativeProps() const {
  auto props = Super::GetNativeProps();

  props.update(folly::dynamic::object("value", "boolean")("disabled", "boolean"));

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

  if (propertyName == "disabled") {
    if (propertyValue.isBool())
      checkbox.IsEnabled(!propertyValue.asBool());
    else if (propertyValue.isNull())
      checkbox.ClearValue(xaml::Controls::Control::IsEnabledProperty());
  } else if (propertyName == "value") {
    if (propertyValue.isBool())
      checkbox.IsChecked(propertyValue.asBool());
    else if (propertyValue.isNull())
      checkbox.ClearValue(winrt::ToggleButton::IsCheckedProperty());
  } else {
    return Super::UpdateProperty(nodeToUpdate, propertyName, propertyValue);
  }
  return true;
}

} // namespace uwp
} // namespace react
