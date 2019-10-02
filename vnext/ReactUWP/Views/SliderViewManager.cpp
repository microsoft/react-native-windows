// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"

#include <Views/ShadowNodeBase.h>
#include "SliderViewManager.h"

#include <Utils/ValueUtils.h>

#include <IReactInstance.h>

#include <winrt/Windows.UI.Xaml.Controls.Primitives.h>

namespace winrt {
using ToggleButton = Windows::UI::Xaml::Controls::Primitives::ToggleButton;
}

namespace react {
namespace uwp {

class SliderShadowNode : public ShadowNodeBase {
  using Super = ShadowNodeBase;

 public:
  SliderShadowNode() = default;
  void createView() override;
  void updateProperties(const folly::dynamic &&props) override;

 private:
  //static void
  //OnCheckedChanged(IReactInstance &instance, int64_t tag, bool newValue);

  //winrt::CheckBox::Checked_revoker m_checkBoxCheckedRevoker{};
  //winrt::CheckBox::Unchecked_revoker m_checkBoxUncheckedRevoker{};
};

void SliderShadowNode::createView() {
  Super::createView();

  auto slider = GetView().as<winrt::Slider>();
  auto wkinstance = GetViewManager()->GetReactInstance();
  //m_checkBoxCheckedRevoker =
  //    checkbox.Checked(winrt::auto_revoke, [=](auto &&, auto &&) {
  //      auto instance = wkinstance.lock();
  //      if (!m_updating && instance != nullptr)
  //        OnCheckedChanged(*instance, m_tag, true);
  //    });
  //m_checkBoxUncheckedRevoker =
  //    checkbox.Unchecked(winrt::auto_revoke, [=](auto &&, auto &&) {
  //      auto instance = wkinstance.lock();
  //      if (!m_updating && instance != nullptr)
  //        OnCheckedChanged(*instance, m_tag, false);
  //    });
}

void SliderShadowNode::updateProperties(const folly::dynamic &&props) {
  m_updating = true;
  Super::updateProperties(std::move(props));
  m_updating = false;
}

///*static*/ void SliderShadowNode::OnCheckedChanged(
//    IReactInstance &instance,
//    int64_t tag,
//    bool newValue) {
//  folly::dynamic eventData =
//      folly::dynamic::object("target", tag)("value", newValue);
//  instance.DispatchEvent(tag, "topChange", std::move(eventData));
//}

SliderViewManager::SliderViewManager(
    const std::shared_ptr<IReactInstance> &reactInstance)
    : Super(reactInstance) {}

const char *SliderViewManager::GetName() const {
  return "RCTSlider";
}

folly::dynamic SliderViewManager::GetNativeProps() const {
  auto props = Super::GetNativeProps();

  //props.update(
  //    folly::dynamic::object("value", "boolean")("disabled", "boolean"));

  return props;
}

facebook::react::ShadowNode *SliderViewManager::createShadow() const {
  return new SliderShadowNode();
}

XamlView SliderViewManager::CreateViewCore(int64_t tag) {
  auto slider = winrt::Slider();
  return slider;
}

void SliderViewManager::UpdateProperties(
    ShadowNodeBase *nodeToUpdate,
    const folly::dynamic &reactDiffMap) {
  auto slider = nodeToUpdate->GetView().as<winrt::Slider>();
  if (slider == nullptr)
    return;

  for (const auto &pair : reactDiffMap.items()) {
    const std::string &propertyName = pair.first.getString();
    const folly::dynamic &propertyValue = pair.second;

    if (propertyName == "disabled") {
      if (propertyValue.isBool())
        slider.IsEnabled(!propertyValue.asBool());
      else if (pair.second.isNull())
        slider.ClearValue(winrt::Control::IsEnabledProperty());
    } 
  }

  Super::UpdateProperties(nodeToUpdate, reactDiffMap);
}

} // namespace uwp
} // namespace react
