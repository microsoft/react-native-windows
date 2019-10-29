// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"

#include "HyperlinkViewManager.h"

#include <Utils/ValueUtils.h>
#include <Views/ShadowNodeBase.h>

#include <IReactInstance.h>

#include <winrt/Windows.UI.Xaml.Controls.Primitives.h>
#include <winrt/Windows.UI.Xaml.Controls.h>

namespace winrt {
using namespace Windows::Foundation;
using namespace Windows::UI;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Xaml::Media;
} // namespace winrt

namespace react {
namespace uwp {
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

void HyperlinkViewManager::UpdateProperties(ShadowNodeBase *nodeToUpdate, const folly::dynamic &reactDiffMap) {
  auto button = nodeToUpdate->GetView().as<winrt::HyperlinkButton>();
  if (button == nullptr)
    return;

  for (const auto &pair : reactDiffMap.items()) {
    const std::string &propertyName = pair.first.getString();
    const folly::dynamic &propertyValue = pair.second;

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
    }
  }

  Super::UpdateProperties(nodeToUpdate, reactDiffMap);
}

} // namespace polyester
} // namespace uwp
} // namespace react
