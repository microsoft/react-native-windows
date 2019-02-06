// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include "ViewManagerBase.h"

namespace react { namespace uwp {

class FrameworkElementViewManager : public ViewManagerBase
{
  using Super = ViewManagerBase;
public:
  FrameworkElementViewManager(const std::shared_ptr<IReactInstance>& reactInstance);

  folly::dynamic GetNativeProps() const override;
  void UpdateProperties(ShadowNodeBase* nodeToUpdate, XamlView viewToUpdate, folly::dynamic reactDiffMap) override;
};

} }
