// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include "FrameworkElementViewManager.h"

namespace react::uwp {

struct ShadowNodeBase;

class REACTWINDOWS_EXPORT ControlViewManager : public FrameworkElementViewManager {
  using Super = FrameworkElementViewManager;

 public:
  ControlViewManager(const std::shared_ptr<IReactInstance> &reactInstance);

  folly::dynamic GetNativeProps() const override;
  bool UpdateProperty(
      ShadowNodeBase *nodeToUpdate,
      const std::string &propertyName,
      const folly::dynamic &propertyValue) override;
  void TransferProperties(const XamlView &oldView, const XamlView &newView) override;

 protected:
  void OnViewCreated(XamlView view) override;
};

} // namespace react::uwp
