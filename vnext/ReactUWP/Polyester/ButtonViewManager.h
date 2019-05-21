// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include "ContentControlViewManager.h"

namespace react { namespace uwp { namespace polyester {

class ButtonViewManager : public ContentControlViewManager
{
  using Super = ContentControlViewManager;
public:
  ButtonViewManager(const std::shared_ptr<IReactInstance>& reactInstance);

  const char* GetName() const override;
  folly::dynamic GetNativeProps() const override;
  folly::dynamic GetExportedCustomDirectEventTypeConstants() const override;

  void UpdateProperties(ShadowNodeBase* nodeToUpdate, const folly::dynamic& reactDiffMap) override;

protected:
  XamlView CreateViewCore(int64_t tag) override;
};

} } }
