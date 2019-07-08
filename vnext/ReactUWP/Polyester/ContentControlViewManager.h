// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include <Views/ControlViewManager.h>

namespace react {
namespace uwp {
namespace polyester {

class ContentControlViewManager : public ControlViewManager {
  using Super = ControlViewManager;

 public:
  ContentControlViewManager(
      const std::shared_ptr<IReactInstance> &reactInstance);

  facebook::react::ShadowNode *createShadow() const override;

  void AddView(XamlView parent, XamlView child, int64_t index) override;
  void RemoveAllChildren(XamlView parent) override;
  void RemoveChildAt(XamlView parent, int64_t index) override;
};

} // namespace polyester
} // namespace uwp
} // namespace react
