// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include "ViewViewManager.h"
#include "ViewControl.h"
#include <cdebug.h>
#include <Views/FrameworkElementViewManager.h>
#include <Modules/NativeUIManager.h>
#include <Modules/PaperUIManagerModule.h>
#include <Views/ViewPanel.h>
#include "DynamicAutomationProperties.h"
#include <inspectable.h>
#include <unicode.h>
#include <winrt/Windows.System.h>
#include <winrt/Windows.UI.Xaml.Interop.h>
#include <Utils/PropertyUtils.h>
#include <Views/ShadowNodeBase.h>
#include <winstring.h>
namespace Microsoft::ReactNative {

class ViewShadowNode;
// ViewShadowNode

class ViewShadowNode : public ShadowNodeBase {
  using Super = ShadowNodeBase;

 public:
  ViewShadowNode() = default;

  void createView() override {
    Super::createView();

    auto panel = GetViewPanel();

    react::uwp::DynamicAutomationProperties::SetAccessibilityInvokeEventHandler(panel, [=]() {
      if (OnClick())
        DispatchEvent("topClick", std::move(folly::dynamic::object("target", m_tag)));
      else
        DispatchEvent("topAccessibilityTap", std::move(folly::dynamic::object("target", m_tag)));
    });

    react::uwp::DynamicAutomationProperties::SetAccessibilityActionEventHandler(
        panel, [=](winrt::react::uwp::AccessibilityAction const &action) {
          folly::dynamic eventData = folly::dynamic::object("target", m_tag);

          eventData.insert(
              "actionName",
              action.Label.empty() ? react::uwp::HstringToDynamic(action.Name)
                                   : react::uwp::HstringToDynamic(action.Label));

          DispatchEvent("topAccessibilityAction", std::move(eventData));
        });
  }

  void dispatchCommand(const std::string &commandId, winrt::Microsoft::ReactNative::JSValueArray &&commandArgs) {
    if (commandId == "focus") {
      if (auto uiManager = GetNativeUIManager(GetViewManager()->GetReactContext()).lock()) {
        uiManager->focus(m_tag);
      }
    } else if (commandId == "blur") {
      if (auto uiManager = GetNativeUIManager(GetViewManager()->GetReactContext()).lock()) {
        uiManager->blur(m_tag);
      }
    } else {
      Super::dispatchCommand(commandId, std::move(commandArgs));
    }
  }

  bool IsControl() {
    return m_isControl;
  }
  void IsControl(bool isControl) {
    m_isControl = isControl;
  }

  bool HasOuterBorder() {
    return m_hasOuterBorder;
  }
  void HasOuterBorder(bool hasOuterBorder) {
    m_hasOuterBorder = hasOuterBorder;
  }

  bool EnableFocusRing() {
    return m_enableFocusRing;
  }
  void EnableFocusRing(bool enable) {
    m_enableFocusRing = enable;

    if (IsControl())
      GetControl().UseSystemFocusVisuals(m_enableFocusRing);
  }

  int32_t TabIndex() {
    return m_tabIndex;
  }
  void TabIndex(int32_t tabIndex) {
    m_tabIndex = tabIndex;

    if (IsControl())
      GetControl().TabIndex(m_tabIndex);
  }

  bool OnClick() const {
    return m_onClick;
  }
  void OnClick(bool isSet) {
    m_onClick = isSet;
  }

  bool IsFocusable() const {
    return m_isFocusable;
  }
  void IsFocusable(bool isFocusable) {
    m_isFocusable = isFocusable;
  }

  bool IsHitTestBrushRequired() const {
    return IsRegisteredForMouseEvents();
  }

  void AddView(ShadowNode &child, int64_t index) override {
    const auto &view = static_cast<ShadowNodeBase &>(child).GetView();
    if (view.try_as<xaml::UIElement>() == nullptr) {
      const auto &ii = view.as<winrt::IInspectable>();
      auto name = winrt::get_class_name(ii);
      YellowBox(
          std::string("ViewViewManager::AddView expected a UIElement but got a ") +
          Microsoft::Common::Unicode::Utf16ToUtf8(name.c_str()));
    }

    GetViewPanel().InsertAt(static_cast<uint32_t>(index), view.as<xaml::UIElement>());
  }

  void RemoveChildAt(int64_t indexToRemove) override {
    if (indexToRemove == static_cast<uint32_t>(indexToRemove))
      GetViewPanel().RemoveAt(static_cast<uint32_t>(indexToRemove));
  }

  void removeAllChildren() override {
    GetViewPanel().Clear();

    XamlView current = m_view;

    // TODO NOW: Why do we do this? Removal of children doesn't seem to imply we
    // tear down the infrastr
    if (IsControl()) {
      if (auto control = m_view.try_as<xaml::Controls::ContentControl>()) {
        current = control.Content().as<XamlView>();
        control.Content(nullptr);
      } else {
        std::string name = Microsoft::Common::Unicode::Utf16ToUtf8(winrt::get_class_name(current).c_str());
        cdebug << "Tearing down, IsControl=true but the control is not a ContentControl, it's a " << name << std::endl;
      }
    }

    if (HasOuterBorder()) {
      if (auto border = current.try_as<xaml::Controls::Border>()) {
        border.Child(nullptr);
      }
    }
  }

  void ReplaceChild(const XamlView &oldChildView, const XamlView &newChildView) override {
    auto pPanel = GetViewPanel();
    if (pPanel != nullptr) {
      uint32_t index;
      if (pPanel.Children().IndexOf(oldChildView.as<xaml::UIElement>(), index)) {
        pPanel.RemoveAt(index);
        pPanel.InsertAt(index, newChildView.as<xaml::UIElement>());
      } else {
        assert(false);
      }
    }
  }

  void RefreshProperties() {
    // The view may have been replaced, so transfer properties stored on the
    // shadow node to the view
    EnableFocusRing(EnableFocusRing());
    TabIndex(TabIndex());
    static_cast<FrameworkElementViewManager *>(GetViewManager())->RefreshTransformMatrix(this);
  }

  winrt::react::uwp::ViewPanel GetViewPanel() {
    XamlView current = m_view;

    if (IsControl()) {
      if (auto control = m_view.try_as<xaml::Controls::ContentControl>()) {
        current = control.Content().as<XamlView>();
      }
    }

    if (HasOuterBorder()) {
      if (auto border = current.try_as<xaml::Controls::Border>()) {
        current = border.Child().try_as<XamlView>();
      }
    }

    auto panel = current.try_as<winrt::react::uwp::ViewPanel>();
    assert(panel != nullptr);

    return panel;
  }

  winrt::react::uwp::ViewControl GetControl() {
    return IsControl() ? m_view.as<winrt::react::uwp::ViewControl>() : nullptr;
  }

  XamlView CreateViewControl() {
    auto contentControl = winrt::make<winrt::react::uwp::implementation::ViewControl>();

    m_contentControlGotFocusRevoker = contentControl.GotFocus(winrt::auto_revoke, [=](auto &&, auto &&args) {
      if (args.OriginalSource().try_as<xaml::UIElement>() == contentControl.as<xaml::UIElement>()) {
        auto tag = m_tag;
        DispatchEvent("topFocus", std::move(folly::dynamic::object("target", tag)));
      }
    });

    m_contentControlLostFocusRevoker = contentControl.LostFocus(winrt::auto_revoke, [=](auto &&, auto &&args) {
      if (args.OriginalSource().try_as<xaml::UIElement>() == contentControl.as<xaml::UIElement>()) {
        auto tag = m_tag;
        DispatchEvent("topBlur", std::move(folly::dynamic::object("target", tag)));
      }
    });

    return contentControl.try_as<XamlView>();
  }

  void DispatchEvent(std::string &&eventName, folly::dynamic &&eventData) {
    GetViewManager()->GetReactContext().DispatchEvent(m_tag, std::move(eventName), std::move(eventData));
  }

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
