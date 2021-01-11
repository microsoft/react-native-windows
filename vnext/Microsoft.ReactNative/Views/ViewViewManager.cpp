// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"
#include "ViewViewManager.h"
#include <cdebug.h>

#include "ViewControl.h"

#include "DynamicAutomationProperties.h"

#include <JSValueWriter.h>
#include <Modules/NativeUIManager.h>
#include <Modules/PaperUIManagerModule.h>
#include <Utils/AccessibilityUtils.h>
#include <Utils/PropertyUtils.h>

#include <INativeUIManager.h>
#include <IReactInstance.h>

#include <inspectable.h>
#include <unicode.h>
#include <winrt/Windows.System.h>
#include <winrt/Windows.UI.Xaml.Interop.h>
#include <winstring.h>

#if defined(_DEBUG)
// Currently only used for tagging controls in debug
#include <winrt/Windows.Foundation.h>
#endif

namespace Microsoft::ReactNative {

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

    if (IsControl())
      GetControl().IsTabStop(m_isFocusable);
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
    IsFocusable(IsFocusable());
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

// ViewPanel uses a ViewBackground property, not Background, so need to
// specialize
// PropertyUtils' TryUpdateBackgroundBrush to use ViewBackground.
// Issue #2172: Additionally, we need to use
// winrt::react::uwp::ViewPanel::implementation::ViewBackgroundProperty
// rather than the proper projected type, because of how we're using cppwinrt.

template <>
bool TryUpdateBackgroundBrush(
    const winrt::react::uwp::ViewPanel &element,
    const std::string &propertyName,
    const winrt::Microsoft::ReactNative::JSValue &propertyValue) {
  if (propertyName == "backgroundColor") {
    if (react::uwp::IsValidColorValue(propertyValue))
      element.ViewBackground(react::uwp::BrushFrom(propertyValue));
    else if (propertyValue.IsNull())
      element.ClearValue(react::uwp::ViewPanel::ViewBackgroundProperty());

    return true;
  }

  return false;
}

// Issue #2172: Calling winrt::react::uwp::ViewPanel::BorderBrushProperty fails
// to call
// down into winrt::react::uwp::implementation::ViewPanel::BorderBrushProperty
// because of how we're using cppwinrt. So we specialize PropertyUtils'
// TryUpdateBorderProperties
// to use winrt::react::uwp::ViewPanel::implementation::BorderBrushProperty

template <>
bool TryUpdateBorderProperties(
    ShadowNodeBase *node,
    const winrt::react::uwp::ViewPanel &element,
    const std::string &propertyName,
    const winrt::Microsoft::ReactNative::JSValue &propertyValue) {
  bool isBorderProperty = true;

  if (propertyName == "borderColor") {
    if (react::uwp::IsValidColorValue(propertyValue))
      element.BorderBrush(react::uwp::BrushFrom(propertyValue));
    else if (propertyValue.IsNull())
      element.ClearValue(react::uwp::ViewPanel::BorderBrushProperty());
  } else if (propertyName == "borderLeftWidth") {
    if (propertyValue.Type() == winrt::Microsoft::ReactNative::JSValueType::Double ||
        propertyValue.Type() == winrt::Microsoft::ReactNative::JSValueType::Int64)
      SetBorderThickness(node, element, ShadowEdges::Left, propertyValue.AsDouble());
  } else if (propertyName == "borderTopWidth") {
    if (propertyValue.Type() == winrt::Microsoft::ReactNative::JSValueType::Double ||
        propertyValue.Type() == winrt::Microsoft::ReactNative::JSValueType::Int64)
      SetBorderThickness(node, element, ShadowEdges::Top, propertyValue.AsDouble());
  } else if (propertyName == "borderRightWidth") {
    if (propertyValue.Type() == winrt::Microsoft::ReactNative::JSValueType::Double ||
        propertyValue.Type() == winrt::Microsoft::ReactNative::JSValueType::Int64)
      SetBorderThickness(node, element, ShadowEdges::Right, propertyValue.AsDouble());
  } else if (propertyName == "borderBottomWidth") {
    if (propertyValue.Type() == winrt::Microsoft::ReactNative::JSValueType::Double ||
        propertyValue.Type() == winrt::Microsoft::ReactNative::JSValueType::Int64)
      SetBorderThickness(node, element, ShadowEdges::Bottom, propertyValue.AsDouble());
  } else if (propertyName == "borderStartWidth") {
    if (propertyValue.Type() == winrt::Microsoft::ReactNative::JSValueType::Double ||
        propertyValue.Type() == winrt::Microsoft::ReactNative::JSValueType::Int64)
      SetBorderThickness(node, element, ShadowEdges::Start, propertyValue.AsDouble());
  } else if (propertyName == "borderEndWidth") {
    if (propertyValue.Type() == winrt::Microsoft::ReactNative::JSValueType::Double ||
        propertyValue.Type() == winrt::Microsoft::ReactNative::JSValueType::Int64)
      SetBorderThickness(node, element, ShadowEdges::End, propertyValue.AsDouble());
  } else if (propertyName == "borderWidth") {
    if (propertyValue.Type() == winrt::Microsoft::ReactNative::JSValueType::Double ||
        propertyValue.Type() == winrt::Microsoft::ReactNative::JSValueType::Int64)
      SetBorderThickness(node, element, ShadowEdges::AllEdges, propertyValue.AsDouble());
    else if (propertyValue.IsNull())
      element.ClearValue(react::uwp::ViewPanel::BorderThicknessProperty());
  } else {
    isBorderProperty = false;
  }

  return isBorderProperty;
}

// ViewViewManager

ViewViewManager::ViewViewManager(const Mso::React::IReactContext &context) : Super(context) {}

const wchar_t *ViewViewManager::GetName() const {
  return L"RCTView";
}

void ViewViewManager::GetExportedCustomDirectEventTypeConstants(
    const winrt::Microsoft::ReactNative::IJSValueWriter &writer) const {
  Super::GetExportedCustomDirectEventTypeConstants(writer);

  writer.WritePropertyName(L"topClick");
  writer.WriteObjectBegin();
  winrt::Microsoft::ReactNative::WriteProperty(writer, L"registrationName", L"onClick");
  writer.WriteObjectEnd();

  writer.WritePropertyName(L"topAccessibilityTap");
  writer.WriteObjectBegin();
  winrt::Microsoft::ReactNative::WriteProperty(writer, L"registrationName", L"onAccessibilityTap");
  writer.WriteObjectEnd();
}

ShadowNode *ViewViewManager::createShadow() const {
  return new ViewShadowNode();
}

XamlView ViewViewManager::CreateViewCore(int64_t /*tag*/) {
  auto panel = winrt::make<winrt::react::uwp::implementation::ViewPanel>();
  panel.VerticalAlignment(xaml::VerticalAlignment::Stretch);
  panel.HorizontalAlignment(xaml::HorizontalAlignment::Stretch);

  return panel.as<XamlView>();
}

void ViewViewManager::GetNativeProps(const winrt::Microsoft::ReactNative::IJSValueWriter &writer) const {
  Super::GetNativeProps(writer);

  winrt::Microsoft::ReactNative::WriteProperty(writer, L"pointerEvents", L"string");
  winrt::Microsoft::ReactNative::WriteProperty(writer, L"onClick", L"function");
  winrt::Microsoft::ReactNative::WriteProperty(writer, L"onMouseEnter", L"function");
  winrt::Microsoft::ReactNative::WriteProperty(writer, L"onMouseLeave", L"function");
  winrt::Microsoft::ReactNative::WriteProperty(writer, L"focusable", L"boolean");
  winrt::Microsoft::ReactNative::WriteProperty(writer, L"enableFocusRing", L"boolean");
  winrt::Microsoft::ReactNative::WriteProperty(writer, L"tabIndex", L"number");
}

bool ViewViewManager::UpdateProperty(
    ShadowNodeBase *nodeToUpdate,
    const std::string &propertyName,
    const winrt::Microsoft::ReactNative::JSValue &propertyValue) {
  auto *pViewShadowNode = static_cast<ViewShadowNode *>(nodeToUpdate);

  auto pPanel = pViewShadowNode->GetViewPanel();
  bool ret = true;
  if (pPanel != nullptr) {
    if (TryUpdateBackgroundBrush(pPanel, propertyName, propertyValue)) {
    } else if (TryUpdateBorderProperties(nodeToUpdate, pPanel, propertyName, propertyValue)) {
    } else if (TryUpdateCornerRadiusOnNode(nodeToUpdate, pPanel, propertyName, propertyValue)) {
      UpdateCornerRadiusOnElement(nodeToUpdate, pPanel);
    } else if (TryUpdateMouseEvents(nodeToUpdate, propertyName, propertyValue)) {
    } else if (propertyName == "onClick") {
      pViewShadowNode->OnClick(!propertyValue.IsNull() && propertyValue.AsBoolean());
    } else if (propertyName == "overflow") {
      if (propertyValue.Type() == winrt::Microsoft::ReactNative::JSValueType::String) {
        bool clipChildren = propertyValue.AsString() == "hidden";
        pPanel.ClipChildren(clipChildren);
      }
    } else if (propertyName == "pointerEvents") {
      if (propertyValue.Type() == winrt::Microsoft::ReactNative::JSValueType::String) {
        bool hitTestable = propertyValue.AsString() != "none";
        pPanel.IsHitTestVisible(hitTestable);
      }
    } else if (propertyName == "focusable") {
      if (propertyValue.Type() == winrt::Microsoft::ReactNative::JSValueType::Boolean)
        pViewShadowNode->IsFocusable(propertyValue.AsBoolean());
    } else if (propertyName == "enableFocusRing") {
      if (propertyValue.Type() == winrt::Microsoft::ReactNative::JSValueType::Boolean)
        pViewShadowNode->EnableFocusRing(propertyValue.AsBoolean());
      else if (propertyValue.IsNull())
        pViewShadowNode->EnableFocusRing(false);
    } else if (propertyName == "tabIndex") {
      auto tabIndex = propertyValue.AsInt64();
      if (tabIndex == static_cast<int32_t>(tabIndex)) {
        pViewShadowNode->TabIndex(static_cast<int32_t>(tabIndex));
      } else if (propertyValue.IsNull()) {
        pViewShadowNode->TabIndex(std::numeric_limits<std::int32_t>::max());
      }
    } else {
      ret = Super::UpdateProperty(nodeToUpdate, propertyName, propertyValue);
    }
  }

  return ret;
}

void ViewViewManager::OnPropertiesUpdated(ShadowNodeBase *node) {
  auto *viewShadowNode = static_cast<ViewShadowNode *>(node);
  auto panel = viewShadowNode->GetViewPanel();

  if (panel.Background() == nullptr) {
    // In XAML, a null background means no hit-test will happen.
    // We actually want hit-testing to happen if the app has registered
    // for mouse events, so detect that case and add a transparent background.
    if (viewShadowNode->IsHitTestBrushRequired()) {
      panel.Background(EnsureTransparentBrush());
    }
    // Note:  Technically we could detect when the transparent brush is
    // no longer needed, but this adds complexity and it can't hurt to
    // keep it around, so not adding that code (yet).
  }

  bool shouldBeControl = viewShadowNode->IsFocusable();
  if (auto view = viewShadowNode->GetView().try_as<xaml::UIElement>()) {
    // If we have DynamicAutomationProperties, we need a ViewControl with a
    // DynamicAutomationPeer
    shouldBeControl = shouldBeControl || react::uwp::HasDynamicAutomationProperties(view);
  }

  panel.FinalizeProperties();

  TryUpdateView(viewShadowNode, panel, shouldBeControl);
}

void ViewViewManager::TryUpdateView(
    ViewShadowNode *pViewShadowNode,
    winrt::react::uwp::ViewPanel &pPanel,
    bool useControl) {
  bool isControl = pViewShadowNode->IsControl();
  bool hadOuterBorder = pViewShadowNode->HasOuterBorder();
  bool hasOuterBorder = pPanel.GetOuterBorder() != nullptr;

  // This short-circuits all of the update code when we have the same hierarchy
  if (isControl == useControl && hadOuterBorder == hasOuterBorder)
    return;

  //
  // 1. Ensure we have the new 'root' and do the child replacement
  //      This is first to ensure that we can re-parent the Border or ViewPanel
  //      we already have
  // 2. Transfer properties
  //      There are likely some complexities to handle here
  // 3. Do any sub=parenting
  //      This means Panel under Border and/or Border under Control
  //

  XamlView oldXamlView(pViewShadowNode->GetView());
  XamlView newXamlView(nullptr);

  //
  // 1. Determine new view & clean up any parent-child relationships
  //

  // If we need a Control then get existing reference or create it
  if (useControl) {
    newXamlView = pViewShadowNode->GetControl().try_as<XamlView>();
    if (newXamlView == nullptr) {
      newXamlView = pViewShadowNode->CreateViewControl();
    }
  }

  // Clean up child of Control if needed
  if (isControl && (!useControl || (hasOuterBorder != hadOuterBorder))) {
    pViewShadowNode->GetControl().Content(nullptr);
  }

  // If don't need a control, then set Outer Border or the Panel as the view
  // root
  if (!useControl) {
    newXamlView = hasOuterBorder ? pPanel.GetOuterBorder().try_as<XamlView>() : pPanel.try_as<XamlView>();
  }

  // Clean up child of Border if needed
  if (hasOuterBorder && !hadOuterBorder)
    pPanel.GetOuterBorder().Child(nullptr);

  // ASSERT: One of the scenarios should be true, so we should have a root view
  assert(newXamlView != nullptr);

  //
  // 2. Transfer needed properties from old to new view
  //

  // Transfer properties from old XamlView to the new one
  TransferProperties(oldXamlView, newXamlView);

  // Since we transferred properties to the new view we need to make the call to
  // finalize
  pPanel.FinalizeProperties();

  // Update the meta-data in the shadow node
  pViewShadowNode->IsControl(useControl);
  pViewShadowNode->HasOuterBorder(hasOuterBorder);

  //
  // 3. Setup any new parent-child relationships
  //

  // If we need to change the root of our view, do it now
  if (oldXamlView != newXamlView) {
    if (auto pNativeUiManager = GetNativeUIManager(*m_context).lock()) {
      // Inform the parent ShadowNode of this change so the hierarchy can be
      // updated
      int64_t parentTag = pViewShadowNode->GetParent();
      auto host = pNativeUiManager->getHost();
      auto *pParentNode = static_cast<ShadowNodeBase *>(host->FindShadowNodeForTag(parentTag));
      if (pParentNode != nullptr)
        pParentNode->ReplaceChild(oldXamlView, newXamlView);

      // Update the ShadowNode with the new XamlView
      pViewShadowNode->ReplaceView(newXamlView);
      pViewShadowNode->RefreshProperties();

      // Inform the NativeUIManager of this change so the yoga layout can be
      // updated
      pNativeUiManager->ReplaceView(*pViewShadowNode);
    }
  }

  // Ensure parenting is setup properly
  auto visualRoot = pPanel.try_as<xaml::UIElement>();

  if (hasOuterBorder) {
    xaml::Controls::Border outerBorder = pPanel.GetOuterBorder();
    if (outerBorder.Child() == nullptr)
      outerBorder.Child(pPanel.try_as<xaml::UIElement>());

    visualRoot = outerBorder;
  }

  if (useControl)
    pViewShadowNode->GetControl().Content(visualRoot);
}

void ViewViewManager::SetLayoutProps(
    ShadowNodeBase &nodeToUpdate,
    const XamlView &viewToUpdate,
    float left,
    float top,
    float width,
    float height) {
  // When the View has a ContentControl the ViewPanel must also have the Width &
  // Height set
  // Do this first so that it is setup properly before any events are fired by
  // the Super implementation
  auto *pViewShadowNode = static_cast<ViewShadowNode *>(&nodeToUpdate);
  if (pViewShadowNode->IsControl()) {
    auto pPanel = pViewShadowNode->GetViewPanel();
    pPanel.Width(width);
    pPanel.Height(height);
  }

  Super::SetLayoutProps(nodeToUpdate, viewToUpdate, left, top, width, height);
}

xaml::Media::SolidColorBrush ViewViewManager::EnsureTransparentBrush() {
  if (!m_transparentBrush) {
    m_transparentBrush = xaml::Media::SolidColorBrush(winrt::Colors::Transparent());
  }
  return m_transparentBrush;
}

} // namespace Microsoft::ReactNative
