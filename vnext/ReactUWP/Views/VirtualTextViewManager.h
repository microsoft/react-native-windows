// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include <Views/FrameworkElementViewManager.h>

namespace react {
namespace uwp {

class VirtualTextViewManager : public ViewManagerBase {
  using Super = ViewManagerBase;

 public:
  VirtualTextViewManager(const std::shared_ptr<IReactInstance> &reactInstance);

  const char *GetName() const override;

  void AddView(const XamlView &parent, const XamlView &child, int64_t index) override;
  void RemoveAllChildren(const XamlView &parent) override;
  void RemoveChildAt(const XamlView &parent, int64_t index) override;

  bool RequiresYogaNode() const override;

 protected:
  bool UpdateProperty(
      ShadowNodeBase *nodeToUpdate,
      const std::string &propertyName,
      const folly::dynamic &propertyValue) override;

  XamlView CreateViewCore(int64_t tag) override;
};

} // namespace uwp
} // namespace react
