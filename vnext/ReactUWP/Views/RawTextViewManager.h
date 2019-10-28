// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include <Views/ViewManagerBase.h>

#include <folly/dynamic.h>

namespace react {
namespace uwp {

class RawTextViewManager : public ViewManagerBase {
  using Super = ViewManagerBase;

 public:
  RawTextViewManager(const std::shared_ptr<IReactInstance> &reactInstance);

  const char *GetName() const override;
  void UpdateProperties(ShadowNodeBase *nodeToUpdate, const folly::dynamic &reactDiffMap) override;

  void SetLayoutProps(
      ShadowNodeBase &nodeToUpdate,
      XamlView viewToUpdate,
      float left,
      float top,
      float width,
      float height) override;
  bool RequiresYogaNode() const override;

 protected:
  XamlView CreateViewCore(int64_t tag) override;

 private:
  void NotifyAncestorsTextChanged(IReactInstance *instance, ShadowNodeBase *nodeToUpdate);
};

} // namespace uwp
} // namespace react
