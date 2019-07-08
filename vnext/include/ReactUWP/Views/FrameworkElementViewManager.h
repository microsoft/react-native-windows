// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include <winrt/Windows.UI.Composition.h>
#include "ViewManagerBase.h"

namespace react {
namespace uwp {

class REACTWINDOWS_EXPORT FrameworkElementViewManager : public ViewManagerBase {
  using Super = ViewManagerBase;

 public:
  FrameworkElementViewManager(
      const std::shared_ptr<IReactInstance> &reactInstance);

  folly::dynamic GetNativeProps() const override;
  void UpdateProperties(
      ShadowNodeBase *nodeToUpdate,
      const folly::dynamic &reactDiffMap) override;

  // Helper functions related to setting/updating TransformMatrix
  void RefreshTransformMatrix(ShadowNodeBase *shadowNode);
  void StartTransformAnimation(
      winrt::UIElement uielement,
      winrt::Windows::UI::Composition::CompositionPropertySet transformPS);

 protected:
  virtual void TransferProperties(XamlView oldView, XamlView newView) override;
  void TransferProperty(
      XamlView oldView,
      XamlView newView,
      winrt::Windows::UI::Xaml::DependencyProperty dp);

 private:
  void ApplyTransformMatrix(
      winrt::UIElement uielement,
      ShadowNodeBase *shadowNode,
      winrt::Windows::Foundation::Numerics::float4x4 transformMatrix);
};

} // namespace uwp
} // namespace react
