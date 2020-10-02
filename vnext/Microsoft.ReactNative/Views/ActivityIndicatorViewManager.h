// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include <Views/ControlViewManager.h>

namespace react::uwp {

class ActivityIndicatorViewManager : public ControlViewManager {
  using Super = ControlViewManager;

 public:
  ActivityIndicatorViewManager(const Mso::React::IReactContext &context);

  const char *GetName() const override;
  folly::dynamic GetNativeProps() const override;

 protected:
  bool UpdateProperty(
      ShadowNodeBase *nodeToUpdate,
      const std::string &propertyName,
      const folly::dynamic &propertyValue) override;

  XamlView CreateViewCore(int64_t tag) override;
};

} // namespace react::uwp
