// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include "FrameworkElementViewManager.h"

namespace react { namespace uwp {

class ControlViewManager : public FrameworkElementViewManager
{
  using Super = FrameworkElementViewManager;
public:
  ControlViewManager(const std::shared_ptr<IReactInstance>& reactInstance);

  void UpdateProperties(ShadowNodeBase* nodeToUpdate, folly::dynamic reactDiffMap) override;
};

} }
