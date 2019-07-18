// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include <Views/ViewManagerBase.h>

#include <folly/dynamic.h>

namespace react {
namespace uwp {

class RawTextViewManager : public ViewManagerBase {
  using Super = ViewManagerBase;

 private:
  std::map<int64_t, facebook::react::ShadowNode*> m_parentNodes;

 public:
  RawTextViewManager(const std::shared_ptr<IReactInstance> &reactInstance);
  const char *GetName() const override;
  void UpdateProperties(
      ShadowNodeBase *nodeToUpdate,
      const folly::dynamic &reactDiffMap) override;

  void SetLayoutProps(
      ShadowNodeBase &nodeToUpdate,
      XamlView viewToUpdate,
      float left,
      float top,
      float width,
      float height) override;
  bool RequiresYogaNode() const override;
  void AddParent(facebook::react::ShadowNode *parent, int64_t tag);
  void RemoveParent(int64_t tag);

 protected:
  XamlView CreateViewCore(int64_t tag) override;
};

} // namespace uwp
} // namespace react
