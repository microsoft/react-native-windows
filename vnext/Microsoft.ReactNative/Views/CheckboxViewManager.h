// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include <Views/ControlViewManager.h>

namespace react::uwp {

class CheckBoxViewManager : public ControlViewManager {
  using Super = ControlViewManager;

 public:
  CheckBoxViewManager(const std::shared_ptr<IReactInstance> &reactInstance);

  const char *GetName() const override;
  folly::dynamic GetNativeProps() const override;

  facebook::react::ShadowNode *createShadow() const override;

 protected:
  bool UpdateProperty(
      ShadowNodeBase *nodeToUpdate,
      const std::string &propertyName,
      const folly::dynamic &propertyValue) override;

  void DispatchCommand(const XamlView &viewToUpdate, const std::string &commandId, const folly::dynamic &commandArgs)
      override;

  XamlView CreateViewCore(int64_t tag) override;

  friend class CheckBoxShadowNode;
};

} // namespace react::uwp
