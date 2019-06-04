// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include "pch.h"

#include <Views/ControlViewManager.h>
#include "Impl/ScrollViewUWPImplementation.h"

namespace react { namespace uwp {

class ScrollViewManager : public ControlViewManager
{
  using Super = ControlViewManager;
public:
  ScrollViewManager(const std::shared_ptr<IReactInstance>& reactInstance);

  const char* GetName() const override;
  folly::dynamic GetCommands() const override;
  folly::dynamic GetNativeProps() const override;
  folly::dynamic GetExportedCustomDirectEventTypeConstants() const override;

  facebook::react::ShadowNode* createShadow() const override;

  void AddView(XamlView parent, XamlView child, int64_t index) override;
  void RemoveAllChildren(XamlView parent) override;
  void RemoveChildAt(XamlView parent, int64_t index) override;

  void SnapToInterval(XamlView parent, float interval);
  void SnapToOffsets(XamlView parent, const winrt::IVectorView<float>& offsets);

protected:
  XamlView CreateViewCore(int64_t tag) override;

private:
  friend class ScrollViewShadowNode;
};

} }
