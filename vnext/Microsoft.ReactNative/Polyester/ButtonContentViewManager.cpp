// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"

#include "ButtonContentViewManager.h"

#include <UI.Xaml.Controls.h>
#include <Utils/ValueUtils.h>

namespace react::uwp {
namespace polyester {

ButtonContentViewManager::ButtonContentViewManager(const std::shared_ptr<IReactInstance> &reactInstance)
    : Super(reactInstance) {}

void ButtonContentViewManager::AddView(const XamlView &parent, const XamlView &child, int64_t index) {
  auto stackPanel(parent.as<xaml::Controls::StackPanel>());
  if (stackPanel != nullptr) {
    stackPanel.Children().InsertAt(static_cast<uint32_t>(index), child.as<xaml::UIElement>());
  }
}

void ButtonContentViewManager::RemoveAllChildren(const XamlView &parent) {
  auto stackPanel(parent.as<xaml::Controls::StackPanel>());
  if (stackPanel != nullptr) {
    stackPanel.Children().Clear();
    stackPanel.InvalidateMeasure();
    stackPanel.InvalidateArrange();
  }
}

void ButtonContentViewManager::RemoveChildAt(const XamlView &parent, int64_t index) {
  auto stackPanel(parent.as<xaml::Controls::StackPanel>());
  if (stackPanel != nullptr) {
    stackPanel.Children().RemoveAt(static_cast<uint32_t>(index));
    stackPanel.InvalidateMeasure();
    stackPanel.InvalidateArrange();
  }
}

const char *ButtonContentViewManager::GetName() const {
  return "PLYButtonContent";
}

XamlView ButtonContentViewManager::CreateViewCore(int64_t /*tag*/) {
  auto stackPanel = xaml::Controls::StackPanel();
  stackPanel.Orientation(xaml::Controls::Orientation::Horizontal);
  return stackPanel;
}

} // namespace polyester
} // namespace react::uwp
