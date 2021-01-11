// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include <Views/FrameworkElementViewManager.h>

namespace Microsoft::ReactNative {

class RootViewManager : public FrameworkElementViewManager {
  using Super = FrameworkElementViewManager;

 public:
  RootViewManager(const Mso::React::IReactContext &context);

  const wchar_t *GetName() const override;

  void AddView(const XamlView &parent, const XamlView &child, int64_t index) override;
  void RemoveAllChildren(const XamlView &parent) override;
  void RemoveChildAt(const XamlView &parent, int64_t index) override;

  void SetLayoutProps(
      ShadowNodeBase &nodeToUpdate,
      const XamlView &viewToUpdate,
      float left,
      float top,
      float width,
      float height) override;

  void destroyShadow(ShadowNode *node) const override;

 protected:
  XamlView CreateViewCore(int64_t tag) override;
};

} // namespace Microsoft::ReactNative
