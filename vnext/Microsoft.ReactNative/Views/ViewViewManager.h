// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include <Views/FrameworkElementViewManager.h>
#include <Views/ViewPanel.h>

namespace Microsoft::ReactNative {

class ViewShadowNode;

class ViewViewManager : public FrameworkElementViewManager {
  using Super = FrameworkElementViewManager;

 public:
  ViewViewManager(const Mso::React::IReactContext &context);

  const wchar_t *GetName() const override;

  void GetNativeProps(const winrt::Microsoft::ReactNative::IJSValueWriter &writer) const override;
  void GetExportedCustomDirectEventTypeConstants(
      const winrt::Microsoft::ReactNative::IJSValueWriter &writer) const override;
  ShadowNode *createShadow() const override;

  // Yoga Layout
  void SetLayoutProps(
      ShadowNodeBase &nodeToUpdate,
      const XamlView &viewToUpdate,
      float left,
      float top,
      float width,
      float height) override;

 protected:
  bool UpdateProperty(
      ShadowNodeBase *nodeToUpdate,
      const std::string &propertyName,
      const winrt::Microsoft::ReactNative::JSValue &propertyValue) override;
  void OnPropertiesUpdated(ShadowNodeBase *node) override;

  XamlView CreateViewCore(int64_t tag) override;
  void TryUpdateView(ViewShadowNode *viewShadowNode, winrt::react::uwp::ViewPanel &pPanel, bool useControl);

  xaml::Media::SolidColorBrush EnsureTransparentBrush();

 private:
  xaml::Media::SolidColorBrush m_transparentBrush{nullptr};
};

} // namespace Microsoft::ReactNative
