// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include "ViewManagerBase.h"

namespace Microsoft::ReactNative {

class REACTWINDOWS_EXPORT FrameworkElementViewManager : public ViewManagerBase {
  using Super = ViewManagerBase;

 public:
  FrameworkElementViewManager(const Mso::React::IReactContext &context);

  void GetNativeProps(const winrt::Microsoft::ReactNative::IJSValueWriter &writer) const override;

  // Helper functions related to setting/updating TransformMatrix
  void RefreshTransformMatrix(ShadowNodeBase *shadowNode);
  void StartTransformAnimation(xaml::UIElement uielement, comp::CompositionPropertySet transformPS);

  virtual void TransferProperties(const XamlView &oldView, const XamlView &newView) override;

 protected:
  bool UpdateProperty(
      ShadowNodeBase *nodeToUpdate,
      const std::string &propertyName,
      const winrt::Microsoft::ReactNative::JSValue &propertyValue) override;

  void TransferProperty(const XamlView &oldView, const XamlView &newView, xaml::DependencyProperty dp);

  void TransferProperty(
      const XamlView &oldView,
      const XamlView &newView,
      xaml::DependencyProperty oldViewDP,
      xaml::DependencyProperty newViewDP);

 private:
  void ApplyTransformMatrix(
      xaml::UIElement uielement,
      ShadowNodeBase *shadowNode,
      winrt::Windows::Foundation::Numerics::float4x4 transformMatrix);
};

} // namespace Microsoft::ReactNative
