// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include <Views/ViewManagerBase.h>

#include <folly/dynamic.h>

namespace react::uwp {

class RawTextViewManager : public ViewManagerBase {
  using Super = ViewManagerBase;

 public:
  RawTextViewManager(const std::shared_ptr<IReactInstance> &reactInstance);

  const char *GetName() const override;

  void SetLayoutProps(
      ShadowNodeBase &nodeToUpdate,
      const XamlView &viewToUpdate,
      float left,
      float top,
      float width,
      float height) override;
  bool RequiresYogaNode() const override;

 protected:
  bool UpdateProperty(
      ShadowNodeBase *nodeToUpdate,
      const std::string &propertyName,
      const folly::dynamic &propertyValue) override;

  XamlView CreateViewCore(int64_t tag) override;

 private:
  void NotifyAncestorsTextChanged(IReactInstance *instance, ShadowNodeBase *nodeToUpdate);
};

} // namespace react::uwp
