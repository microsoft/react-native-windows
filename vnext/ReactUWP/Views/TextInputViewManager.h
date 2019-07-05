// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include <Views/ControlViewManager.h>

namespace react {
namespace uwp {

class TextInputViewManager : public ControlViewManager {
  using Super = ControlViewManager;

 public:
  TextInputViewManager(const std::shared_ptr<IReactInstance> &reactInstance);

  const char *GetName() const override;
  folly::dynamic GetNativeProps() const override;
  folly::dynamic GetExportedCustomDirectEventTypeConstants() const override;
  facebook::react::ShadowNode *createShadow() const override;

  YGMeasureFunc GetYogaCustomMeasureFunc() const override;

 protected:
  XamlView CreateViewCore(int64_t tag) override;
  friend class TextInputShadowNode;
};

} // namespace uwp
} // namespace react
