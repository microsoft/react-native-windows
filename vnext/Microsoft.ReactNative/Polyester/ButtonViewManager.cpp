// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"

#include "ButtonViewManager.h"

#include <UI.Xaml.Controls.Primitives.h>
#include <UI.Xaml.Controls.h>
#include <Utils/ValueUtils.h>
#include <Views/ShadowNodeBase.h>

#include <IReactInstance.h>

namespace react::uwp {
namespace polyester {

class ButtonShadowNode : public ContentControlShadowNode {
  using Super = ContentControlShadowNode;

 public:
  ButtonShadowNode() = default;
  void createView() override;

 private:
  winrt::Button::Click_revoker m_buttonClickRevoker{};
};

void ButtonShadowNode::createView() {
  Super::createView();

  auto button = GetView().as<winrt::Button>();

  m_buttonClickRevoker = button.Click(winrt::auto_revoke, [=](auto &&, auto &&) {
    auto instance = GetViewManager()->GetReactInstance().lock();
    folly::dynamic eventData = folly::dynamic::object("target", m_tag);
    if (instance != nullptr)
      instance->DispatchEvent(m_tag, "topClick", std::move(eventData));
  });
}

ButtonViewManager::ButtonViewManager(const std::shared_ptr<IReactInstance> &reactInstance)
    : ContentControlViewManager(reactInstance) {}

const char *ButtonViewManager::GetName() const {
  // TODO: Is this right? Or should it be RCTButton?
  return "PLYButton";
}

folly::dynamic ButtonViewManager::GetNativeProps() const {
  auto props = Super::GetNativeProps();

  props.update(folly::dynamic::object("accessibilityLabel", "string")("disabled", "boolean")("buttonType", "string"));

  return props;
}

folly::dynamic ButtonViewManager::GetExportedCustomDirectEventTypeConstants() const {
  auto directEvents = Super::GetExportedCustomDirectEventTypeConstants();
  directEvents["topClick"] = folly::dynamic::object("registrationName", "onClick");

  return directEvents;
}

facebook::react::ShadowNode *ButtonViewManager::createShadow() const {
  return new ButtonShadowNode();
}

XamlView ButtonViewManager::CreateViewCore(int64_t /*tag*/) {
  winrt::Button button = winrt::Button();
  return button;
}

bool ButtonViewManager::UpdateProperty(
    ShadowNodeBase *nodeToUpdate,
    const std::string &propertyName,
    const folly::dynamic &propertyValue) {
  auto button = nodeToUpdate->GetView().as<winrt::Button>();
  if (button == nullptr)
    return true;

  if (propertyName == "disabled") {
    if (propertyValue.isBool())
      button.IsEnabled(!propertyValue.asBool());
  } else {
    return Super::UpdateProperty(nodeToUpdate, propertyName, propertyValue);
  }
  return true;
}

} // namespace polyester
} // namespace react::uwp
