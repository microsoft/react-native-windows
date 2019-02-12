// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include <Views/FrameworkElementViewManager.h>

namespace react { namespace uwp { namespace polyester {

class ContentControlViewManager : public FrameworkElementViewManager
{
  using Super = FrameworkElementViewManager;
public:
  ContentControlViewManager(const std::shared_ptr<IReactInstance>& reactInstance);

  facebook::react::ShadowNode* createShadow() const override;

  void AddView(XamlView parent, XamlView child, int64_t index) override;
  XamlView GetChildAt(XamlView parent, int64_t index) override;
  int64_t GetChildCount(XamlView parent) override;
  void RemoveAllChildren(XamlView parent) override;
  void RemoveChildAt(XamlView parent, int64_t index) override;
};

} } }
