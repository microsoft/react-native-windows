/* TODO: Update or remove this example to reflect the new ViewManager architecture provided by Microsoft.ReactNative. */

#include "pch.h"

#include <Views/ShadowNodeBase.h>
#include "CustomViewManager.h"

#include <winrt/Windows.UI.Xaml.Controls.Primitives.h>
#include <winrt/Windows.UI.Xaml.Controls.h>

namespace winrt {
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
} // namespace winrt

enum class CustomFrameworkElementCommands {
  ExampleCommand = 1,
};

class CustomFrameworkElementShadowNode : public react::uwp::ShadowNodeBase {
  using Super = react::uwp::ShadowNodeBase;

 public:
  CustomFrameworkElementShadowNode() = default;
  void createView() override;
  void updateProperties(const folly::dynamic &&props) override;

 private:
  static void OnCheckedChanged(react::uwp::IReactInstance &instance, int64_t tag, bool newValue);
};

void CustomFrameworkElementShadowNode::createView() {
  Super::createView();
  auto checkbox = GetView().as<winrt::CheckBox>();
  auto wkinstance = GetViewManager()->GetReactInstance();
  checkbox.Checked([=](auto &&, auto &&) {
    auto instance = wkinstance.lock();
    if (!m_updating && instance != nullptr)
      OnCheckedChanged(*instance, m_tag, true);
  });
  checkbox.Unchecked([=](auto &&, auto &&) {
    auto instance = wkinstance.lock();
    if (!m_updating && instance != nullptr)
      OnCheckedChanged(*instance, m_tag, false);
  });
}

void CustomFrameworkElementShadowNode::updateProperties(const folly::dynamic &&props) {
  m_updating = true;
  Super::updateProperties(std::move(props));
  m_updating = false;
}

/*static*/ void
CustomFrameworkElementShadowNode::OnCheckedChanged(react::uwp::IReactInstance &instance, int64_t tag, bool newValue) {
  folly::dynamic eventData = folly::dynamic::object("target", tag)("value", newValue);
  instance.DispatchEvent(tag, "topCustomEvent", std::move(eventData));
}

CustomFrameworkElementViewManager::CustomFrameworkElementViewManager(
    const std::shared_ptr<react::uwp::IReactInstance> &reactInstance)
    : Super(reactInstance) {}

const char *CustomFrameworkElementViewManager::GetName() const {
  return "RCTCustomFrameworkElement";
}

folly::dynamic CustomFrameworkElementViewManager::GetCommands() const {
  auto commands = Super::GetCommands();
  commands.update(folly::dynamic::object(
      "ExampleCommand",
      static_cast<std::underlying_type_t<CustomFrameworkElementCommands>>(
          CustomFrameworkElementCommands::ExampleCommand)));
  return commands;
}

folly::dynamic CustomFrameworkElementViewManager::GetNativeProps() const {
  auto props = Super::GetNativeProps();
  props.update(folly::dynamic::object("disabled", "boolean")("test", "boolean"));
  return props;
}

folly::dynamic CustomFrameworkElementViewManager::GetExportedCustomDirectEventTypeConstants() const {
  auto directEvents = Super::GetExportedCustomDirectEventTypeConstants();
  directEvents["topCustomEvent"] = folly::dynamic::object("registrationName", "onCustomEvent");
  return directEvents;
}

facebook::react::ShadowNode *CustomFrameworkElementViewManager::createShadow() const {
  return new CustomFrameworkElementShadowNode();
}

react::uwp::XamlView CustomFrameworkElementViewManager::CreateViewCore(int64_t tag) {
  auto checkbox = winrt::CheckBox();
  return checkbox;
}

void CustomFrameworkElementViewManager::UpdateProperties(
    react::uwp::ShadowNodeBase *nodeToUpdate,
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
      else if (propertyValue.isNull())
        checkbox.ClearValue(winrt::Control::IsEnabledProperty());
    } else if (propertyName == "test") {
      if (propertyValue.isBool()) {
        // Do something with propertyValue.asBool()
      } else if (propertyValue.isNull()) {
        // Always handle isNull, called when a property is unset, especially
        // common with style props
      }
    }
  }

  Super::UpdateProperties(nodeToUpdate, reactDiffMap);
}

void CustomFrameworkElementViewManager::DispatchCommand(
    react::uwp::XamlView viewToUpdate,
    int64_t commandId,
    const folly::dynamic &commandArgs) {
  auto checkbox = viewToUpdate.as<winrt::CheckBox>();
  if (checkbox == nullptr)
    return;

  switch (static_cast<CustomFrameworkElementCommands>(commandId)) {
    case CustomFrameworkElementCommands::ExampleCommand: {
      checkbox.IsChecked(checkbox.IsChecked().GetBoolean() ? false : true);
      break;
    }
  }
}
