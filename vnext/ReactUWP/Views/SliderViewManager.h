// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include <Views/ControlViewManager.h>

namespace react {
namespace uwp {

class SliderViewManager : public ControlViewManager {
  using Super = ControlViewManager;

 public:
  SliderViewManager(const std::shared_ptr<IReactInstance> &reactInstance);

  const char *GetName() const override;
  folly::dynamic GetNativeProps() const override;

  facebook::react::ShadowNode *createShadow() const override;

 protected:
  bool UpdateProperty(
      ShadowNodeBase *nodeToUpdate,
      const std::string &propertyName,
      const folly::dynamic &propertyValue) override;

  XamlView CreateViewCore(int64_t tag) override;

  friend class SliderShadowNode;
};

} // namespace uwp
} // namespace react
