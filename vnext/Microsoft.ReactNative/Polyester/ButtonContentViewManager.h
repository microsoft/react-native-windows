// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include <Views/FrameworkElementViewManager.h>

namespace react {
namespace uwp {
namespace polyester {

class ButtonContentViewManager : public FrameworkElementViewManager {
  using Super = FrameworkElementViewManager;

 public:
  ButtonContentViewManager(
      const std::shared_ptr<IReactInstance> &reactInstance);

  const char *GetName() const override;

  void AddView(XamlView parent, XamlView child, int64_t index) override;
  void RemoveAllChildren(XamlView parent) override;
  void RemoveChildAt(XamlView parent, int64_t index) override;

 protected:
  XamlView CreateViewCore(int64_t tag) override;
};

} // namespace polyester
} // namespace uwp
} // namespace react
