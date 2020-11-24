// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include <Views/FrameworkElementViewManager.h>
#include <Views/ShadowNodeBase.h>
#include <Views/ViewPanel.h>

namespace Microsoft::ReactNative {

class ViewShadowNode : public ShadowNodeBase {
  using Super = ShadowNodeBase;

 public:
  ViewShadowNode() = default;

  void createView() override;
  void dispatchCommand(const std::string &commandId, winrt::Microsoft::ReactNative::JSValueArray &&commandArgs);
  bool IsControl();
  void IsControl(bool isControl);
  bool HasOuterBorder();
  void HasOuterBorder(bool hasOuterBorder);
  bool EnableFocusRing();
  void EnableFocusRing(bool enable);
  int32_t TabIndex();
  void TabIndex(int32_t tabIndex);
  bool OnClick() const;
  void OnClick(bool isSet);
  bool IsFocusable() const;
  void IsFocusable(bool isFocusable);
  bool IsHitTestBrushRequired() const;
  void AddView(ShadowNode &child, int64_t index) override;
  void RemoveChildAt(int64_t indexToRemove) override;
  void removeAllChildren() override;
  void ReplaceChild(const XamlView &oldChildView, const XamlView &newChildView) override;
  void RefreshProperties();
  winrt::react::uwp::ViewPanel GetViewPanel();
  winrt::react::uwp::ViewControl GetControl();
  XamlView CreateViewControl();
  void DispatchEvent(std::string &&eventName, folly::dynamic &&eventData);

 private:
  bool m_isControl = false;
  bool m_hasOuterBorder = false;

  bool m_enableFocusRing = true;
  bool m_onClick = false;
  bool m_isFocusable = false;
  int32_t m_tabIndex = std::numeric_limits<std::int32_t>::max();

  xaml::Controls::ContentControl::GotFocus_revoker m_contentControlGotFocusRevoker{};
  xaml::Controls::ContentControl::LostFocus_revoker m_contentControlLostFocusRevoker{};
};

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
