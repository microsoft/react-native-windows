// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"

#include "RootViewManager.h"

#include <IXamlRootView.h>
#include <UI.Xaml.Controls.h>

namespace winrt {
using namespace Windows::UI;
using namespace xaml;
using namespace xaml::Controls;
} // namespace winrt

namespace Microsoft::ReactNative {

RootViewManager::RootViewManager(const Mso::React::IReactContext &context) : Super(context) {}

const wchar_t *RootViewManager::GetName() const {
  return L"ROOT";
}

XamlView RootViewManager::CreateViewCore(int64_t /*tag*/) {
  // ASSERT: Can't create a root view directly. Comes from host.
  assert(false);
  return nullptr;
}

void RootViewManager::AddView(const XamlView &parent, const XamlView &child, int64_t index) {
  auto panel(parent.as<winrt::Panel>());
  if (panel != nullptr)
    panel.Children().InsertAt(static_cast<uint32_t>(index), child.as<xaml::UIElement>());
}

void RootViewManager::RemoveAllChildren(const XamlView &parent) {
  auto panel(parent.as<winrt::Panel>());
  if (panel != nullptr)
    panel.Children().Clear();
}

void RootViewManager::RemoveChildAt(const XamlView &parent, int64_t index) {
  auto panel(parent.as<winrt::Panel>());
  if (panel != nullptr)
    panel.Children().RemoveAt(static_cast<uint32_t>(index));
}

void RootViewManager::SetLayoutProps(
    ShadowNodeBase & /*nodeToUpdate*/,
    const XamlView & /*nodeToUpdate*/,
    float /*left*/,
    float /*top*/,
    float /*width*/,
    float /*height*/) {
  // Don't set any of these props for the root view, otherwise Xaml will stop
  // resizing it when the upper layout (i.e. the non-react UI in the app)
  // changes.
}

void RootViewManager::destroyShadow(ShadowNode * /*node*/) const {
  // Don't delete the node here, allow the NativeUIManager to handle that since
  // it creates it.
}

} // namespace Microsoft::ReactNative
