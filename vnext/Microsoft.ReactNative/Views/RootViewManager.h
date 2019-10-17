// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include <Views/FrameworkElementViewManager.h>

namespace react {
namespace uwp {

class RootViewManager : public FrameworkElementViewManager {
  using Super = FrameworkElementViewManager;

 public:
  RootViewManager(const std::shared_ptr<IReactInstance> &reactInstance);

  const char *GetName() const override;

  void AddView(XamlView parent, XamlView child, int64_t index) override;
  void RemoveAllChildren(XamlView parent) override;
  void RemoveChildAt(XamlView parent, int64_t index) override;

  void SetLayoutProps(
      ShadowNodeBase &nodeToUpdate,
      XamlView viewToUpdate,
      float left,
      float top,
      float width,
      float height) override;

  void destroyShadow(facebook::react::ShadowNode *node) const override;

 protected:
  XamlView CreateViewCore(int64_t tag) override;
};

} // namespace uwp
} // namespace react
