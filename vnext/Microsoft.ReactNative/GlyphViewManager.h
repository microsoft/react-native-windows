// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include <Views/FrameworkElementViewManager.h>

namespace react::uwp {
namespace polyester {

class GlyphViewManager : public FrameworkElementViewManager {
  using Super = FrameworkElementViewManager;

 public:
  GlyphViewManager(const Mso::React::IReactContext &context);

  const char *GetName() const override;
  facebook::react::ShadowNode *createShadow() const override;
  folly::dynamic GetNativeProps() const override;

 protected:
  XamlView CreateViewCore(int64_t tag) override;
};

} // namespace polyester
} // namespace react::uwp
