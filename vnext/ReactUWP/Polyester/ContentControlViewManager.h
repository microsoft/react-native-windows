// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include <Views/ControlViewManager.h>
#include <Views/ShadowNodeBase.h>

namespace react {
namespace uwp {
namespace polyester {

//
// ContentControlShadowNode
// - required for subclasses of ContentControlViewManager with a custom
// shadownode
//
class ContentControlShadowNode : public ShadowNodeBase {
  using Super = ShadowNodeBase;

 public:
  ContentControlShadowNode() = default;
  void createView() override;
  bool IsExternalLayoutDirty() const override {
    return m_paddingDirty;
  }
  void DoExtraLayoutPrep(YGNodeRef yogaNode) override;

 private:
  bool m_paddingDirty = false;
};

//
// ContentControlViewManager
//
class ContentControlViewManager : public ControlViewManager {
  using Super = ControlViewManager;

 public:
  ContentControlViewManager(const std::shared_ptr<IReactInstance> &reactInstance);

  facebook::react::ShadowNode *createShadow() const override;

  void AddView(const XamlView &parent, const XamlView &child, int64_t index) override;
  void RemoveAllChildren(const XamlView &parent) override;
  void RemoveChildAt(const XamlView &parent, int64_t index) override;
};

} // namespace polyester
} // namespace uwp
} // namespace react
