// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include <Views/ControlViewManager.h>
#include "Impl/ScrollViewUWPImplementation.h"

namespace react::uwp {

class ScrollViewManager : public ControlViewManager {
  using Super = ControlViewManager;

 public:
  ScrollViewManager(const std::shared_ptr<IReactInstance> &reactInstance);

  const char *GetName() const override;
  folly::dynamic GetCommands() const override;
  folly::dynamic GetNativeProps() const override;
  folly::dynamic GetExportedCustomDirectEventTypeConstants() const override;

  facebook::react::ShadowNode *createShadow() const override;

  void AddView(const XamlView &parent, const XamlView &child, int64_t index) override;
  void RemoveAllChildren(const XamlView &parent) override;
  void RemoveChildAt(const XamlView &parent, int64_t index) override;

  void SnapToInterval(const XamlView &parent, float interval);
  void SnapToOffsets(const XamlView &parent, const winrt::IVectorView<float> &offsets);

 protected:
  XamlView CreateViewCore(int64_t tag) override;

 private:
  friend class ScrollViewShadowNode;
};

} // namespace react::uwp
