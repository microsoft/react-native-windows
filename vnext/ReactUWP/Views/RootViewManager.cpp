// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"

#include "RootViewManager.h"

#include <IXamlRootView.h>

#include <winrt/Windows.UI.Xaml.Controls.h>

namespace winrt {
using namespace Windows::UI;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
} // namespace winrt

namespace react {
namespace uwp {

RootViewManager::RootViewManager(
    const std::shared_ptr<IReactInstance> &reactInstance)
    : Super(reactInstance) {}

const char *RootViewManager::GetName() const {
  return "ROOT";
}

XamlView RootViewManager::CreateViewCore(int64_t tag) {
  // ASSERT: Can't create a root view directly. Comes from host.
  assert(false);
  return nullptr;
}

void RootViewManager::AddView(XamlView parent, XamlView child, int64_t index) {
  auto panel(parent.as<winrt::Panel>());
  if (panel != nullptr)
    panel.Children().InsertAt(
        static_cast<uint32_t>(index), child.as<winrt::UIElement>());
}

void RootViewManager::RemoveAllChildren(XamlView parent) {
  auto panel(parent.as<winrt::Panel>());
  if (panel != nullptr)
    panel.Children().Clear();
}

void RootViewManager::RemoveChildAt(XamlView parent, int64_t index) {
  auto panel(parent.as<winrt::Panel>());
  if (panel != nullptr)
    panel.Children().RemoveAt(static_cast<uint32_t>(index));
}

void RootViewManager::SetLayoutProps(
    ShadowNodeBase & /*nodeToUpdate*/,
    XamlView /*nodeToUpdate*/,
    float /*left*/,
    float /*top*/,
    float /*width*/,
    float /*height*/) {
  // Don't set any of these props for the root view, otherwise Xaml will stop
  // resizing it when the upper layout (i.e. the non-react UI in the app)
  // changes.
}

void RootViewManager::destroyShadow(facebook::react::ShadowNode *node) const {
  // Don't delete the node here, allow the NativeUIManager to handle that since
  // it creates it.
}

} // namespace uwp
} // namespace react
