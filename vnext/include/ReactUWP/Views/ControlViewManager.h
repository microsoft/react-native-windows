// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include "FrameworkElementViewManager.h"

namespace react {
namespace uwp {

struct ShadowNodeBase;

class REACTWINDOWS_EXPORT ControlViewManager
    : public FrameworkElementViewManager {
  using Super = FrameworkElementViewManager;

 public:
  ControlViewManager(const std::shared_ptr<IReactInstance> &reactInstance);

  folly::dynamic GetNativeProps() const override;
  void UpdateProperties(
      ShadowNodeBase *nodeToUpdate,
      const folly::dynamic &reactDiffMap) override;
};

} // namespace uwp
} // namespace react
