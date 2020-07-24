// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include "ContentControlViewManager.h"

namespace winrt {
using Button = xaml::Controls::Button;
}

namespace react::uwp {
namespace polyester {

class ButtonViewManager : public ContentControlViewManager {
  using Super = ContentControlViewManager;

 public:
  ButtonViewManager(const std::shared_ptr<IReactInstance> &reactInstance);

  const char *GetName() const override;
  folly::dynamic GetNativeProps() const override;
  folly::dynamic GetExportedCustomDirectEventTypeConstants() const override;
  facebook::react::ShadowNode *createShadow() const override;

 protected:
  bool UpdateProperty(
      ShadowNodeBase *nodeToUpdate,
      const std::string &propertyName,
      const folly::dynamic &propertyValue) override;

  XamlView CreateViewCore(int64_t tag) override;
};

} // namespace polyester
} // namespace react::uwp
