// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"

#include "HyperlinkViewManager.h"

#include <UI.Xaml.Controls.Primitives.h>
#include <UI.Xaml.Controls.h>
#include <Utils/ValueUtils.h>
#include <Views/ShadowNodeBase.h>

#include <IReactInstance.h>

namespace winrt {
using namespace Windows::Foundation;
using namespace Windows::UI;
using namespace xaml;
using namespace xaml::Controls;
using namespace xaml::Controls::Primitives;
using namespace xaml::Media;
} // namespace winrt

namespace react::uwp {
namespace polyester {

HyperlinkViewManager::HyperlinkViewManager(const std::shared_ptr<IReactInstance> &reactInstance)
    : ContentControlViewManager(reactInstance) {}

const char *HyperlinkViewManager::GetName() const {
  return "PLYHyperlink";
}

folly::dynamic HyperlinkViewManager::GetNativeProps() const {
  auto props = Super::GetNativeProps();

  props.update(folly::dynamic::object("disabled", "boolean")("url", "string")("tooltip", "string"));

  return props;
}

XamlView HyperlinkViewManager::CreateViewCore(int64_t tag) {
  auto button = winrt::HyperlinkButton();
  button.Click([=](auto &&, auto &&) {
    auto instance = m_wkReactInstance.lock();
    folly::dynamic eventData = folly::dynamic::object("target", tag);
    if (instance != nullptr)
      instance->DispatchEvent(tag, "topClick", std::move(eventData));
  });

  return button;
}

folly::dynamic HyperlinkViewManager::GetExportedCustomDirectEventTypeConstants() const {
  auto directEvents = Super::GetExportedCustomDirectEventTypeConstants();
  directEvents["topClick"] = folly::dynamic::object("registrationName", "onClick");

  return directEvents;
}

bool HyperlinkViewManager::UpdateProperty(
    ShadowNodeBase *nodeToUpdate,
    const std::string &propertyName,
    const folly::dynamic &propertyValue) {
  auto button = nodeToUpdate->GetView().as<winrt::HyperlinkButton>();
  if (button == nullptr)
    return true;

  if (propertyName == "disabled") {
    if (propertyValue.isBool())
      button.IsEnabled(!propertyValue.asBool());
  } else if (propertyName == "tooltip") {
    if (propertyValue.isString()) {
      winrt::TextBlock tooltip = winrt::TextBlock();
      tooltip.Text(asHstring(propertyValue));
      winrt::ToolTipService::SetToolTip(button, tooltip);
    }
  } else if (propertyName == "url") {
    winrt::Uri myUri(asHstring(propertyValue));
    button.NavigateUri(myUri);
  } else {
    return Super::UpdateProperty(nodeToUpdate, propertyName, propertyValue);
  }

  return true;
}

} // namespace polyester
} // namespace react::uwp
