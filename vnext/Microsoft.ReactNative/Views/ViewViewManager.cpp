// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"
#include "ViewViewManager.h"
#include <cdebug.h>

#include "ViewControl.h"
#include "ViewGrid.h"

#include <UI.Xaml.Automation.Peers.h>
#include "DynamicAutomationProperties.h"

#include <JSValueWriter.h>
#include <Modules/NativeUIManager.h>
#include <Modules/PaperUIManagerModule.h>
#include <Utils/AccessibilityUtils.h>
#include <Utils/PropertyUtils.h>
#include <Views/FrameworkElementTransferProperties.h>

#include <INativeUIManager.h>
#include <IReactInstance.h>

#include <cxxreact/SystraceSection.h>
#include <inspectable.h>
#include <unicode.h>
#include <winrt/Windows.System.h>
#include <winrt/Windows.UI.Xaml.Interop.h>
#include <winstring.h>

#if defined(_DEBUG)
// Currently only used for tagging controls in debug
#include <winrt/Windows.Foundation.h>
#endif

using namespace facebook::react;

namespace Microsoft::ReactNative {

// ViewTraits

enum class ViewTraits : std::uint_fast8_t {
  None = 0,
  HasOuterBorder = 1 << 1,
  IsControl = 1 << 2,
  EnableFocusRing = 1 << 3,
  IsFocusable = 1 << 4,
  IsAccessible = 1 << 5,
  IsPressable = 1 << 6,
  IsGrid = 1 << 7,
};

DEFINE_ENUM_FLAG_OPERATORS(ViewTraits);

// ViewShadowNode

class ViewShadowNode : public ShadowNodeBase {
  using Super = ShadowNodeBase;

 public:
  ViewShadowNode() = default;

  void createView(const winrt::Microsoft::ReactNative::JSValueObject &props) override {
    Super::createView(props);

    auto panel = GetPanel();
    if (panel.try_as<winrt::Microsoft::ReactNative::ViewGrid>()) {
      IsGrid(true);
    }

    DynamicAutomationProperties::SetAccessibilityInvokeEventHandler(panel, [=]() {
      if (OnClick())
        DispatchEvent("topClick", std::move(folly::dynamic::object("target", m_tag)));
      else
        DispatchEvent("topAccessibilityTap", std::move(folly::dynamic::object("target", m_tag)));
    });

    DynamicAutomationProperties::SetAccessibilityActionEventHandler(
        panel, [=](winrt::Microsoft::ReactNative::AccessibilityAction const &action) {
          folly::dynamic eventData = folly::dynamic::object("target", m_tag);

          eventData.insert(
              "actionName", action.Label.empty() ? HstringToDynamic(action.Name) : HstringToDynamic(action.Label));

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
    return HasViewTraits(ViewTraits::IsControl);
  }
  void IsControl(bool isControl) {
    SetViewTraits(isControl, ViewTraits::IsControl);
  }

  bool IsGrid() {
    return HasViewTraits(ViewTraits::IsGrid);
  }
  void IsGrid(bool isGrid) {
    SetViewTraits(isGrid, ViewTraits::IsGrid);
  }

  bool HasOuterBorder() {
    return HasViewTraits(ViewTraits::HasOuterBorder);
  }
  void HasOuterBorder(bool hasOuterBorder) {
    SetViewTraits(hasOuterBorder, ViewTraits::HasOuterBorder);
  }

  bool EnableFocusRing() {
    return HasViewTraits(ViewTraits::EnableFocusRing);
  }
  void EnableFocusRing(bool enable) {
    SetViewTraits(enable, ViewTraits::EnableFocusRing);

    if (IsControl())
      GetControl().UseSystemFocusVisuals(enable);
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
    return HasViewTraits(ViewTraits::IsPressable);
  }
  void OnClick(bool isSet) {
    SetViewTraits(isSet, ViewTraits::IsPressable);
  }

  bool IsFocusable() const {
    return HasViewTraits(ViewTraits::IsFocusable);
  }
  void IsFocusable(bool isFocusable) {
    SetViewTraits(isFocusable, ViewTraits::IsFocusable);
  }

  bool IsAccessible() const {
    return HasViewTraits(ViewTraits::IsAccessible);
  }

  void IsAccessible(bool isAccessible) {
    SetViewTraits(isAccessible, ViewTraits::IsAccessible);
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

    GetPanel().Children().InsertAt(static_cast<uint32_t>(index), view.as<xaml::UIElement>());
  }

  void RemoveChildAt(int64_t indexToRemove) override {
    if (indexToRemove == static_cast<uint32_t>(indexToRemove))
      GetPanel().Children().RemoveAt(static_cast<uint32_t>(indexToRemove));
  }

  void removeAllChildren() override {
    GetPanel().Children().Clear();

    XamlView current = m_view;

    // TODO NOW: Why do we do this? Removal of children doesn't seem to imply we
    // tear down the infrastructure
    if (IsControl()) {
      if (auto control = m_view.try_as<xaml::Controls::ContentControl>()) {
        current = control.Content().as<XamlView>();
        control.Content(nullptr);
      } else {
        std::string name = Microsoft::Common::Unicode::Utf16ToUtf8(winrt::get_class_name(current).c_str());
        cdebug << "Tearing down, IsControl=true but the control is not a ContentControl, it's a " << name << "\n";
      }
    }

    if (HasOuterBorder()) {
      if (auto border = current.try_as<xaml::Controls::Border>()) {
        border.Child(nullptr);
      }
    }
  }

  void ReplaceChild(const XamlView &oldChildView, const XamlView &newChildView) override {
    auto pPanel = GetPanel();
    if (pPanel != nullptr) {
      uint32_t index;
      if (pPanel.Children().IndexOf(oldChildView.as<xaml::UIElement>(), index)) {
        pPanel.Children().RemoveAt(index);
        pPanel.Children().InsertAt(index, newChildView.as<xaml::UIElement>());
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

  xaml::Controls::Panel GetPanel() {
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

    auto panel = current.try_as<xaml::Controls::Panel>();
    assert(panel != nullptr);

    return panel;
  }

  winrt::Microsoft::ReactNative::ViewControl GetControl() {
    return IsControl() ? m_view.as<winrt::Microsoft::ReactNative::ViewControl>() : nullptr;
  }

  XamlView CreateViewControl() {
    auto contentControl = winrt::make<winrt::Microsoft::ReactNative::implementation::ViewControl>();

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
  int32_t m_tabIndex = std::numeric_limits<std::int32_t>::max();
  ViewTraits m_viewTraits = ViewTraits::EnableFocusRing;

  bool HasViewTraits(ViewTraits viewTraits) const {
    return (m_viewTraits & viewTraits) == viewTraits;
  }

  void SetViewTraits(bool enabled, ViewTraits viewTraits) {
    if (enabled) {
      m_viewTraits |= viewTraits;
    } else {
      m_viewTraits &= ~viewTraits;
    }
  }

  xaml::Controls::ContentControl::GotFocus_revoker m_contentControlGotFocusRevoker{};
  xaml::Controls::ContentControl::LostFocus_revoker m_contentControlLostFocusRevoker{};
};

// ViewPanel uses a ViewBackground property, not Background, so need to
// specialize
// PropertyUtils' TryUpdateBackgroundBrush to use ViewBackground.
// Issue #2172: Additionally, we need to use
// winrt::Microsoft::ReactNative::ViewPanel::implementation::ViewBackgroundProperty
// rather than the proper projected type, because of how we're using cppwinrt.
// Remove if we default to ViewGrid implementation.

template <>
bool TryUpdateBackgroundBrush(
    const winrt::Microsoft::ReactNative::ViewPanel &element,
    const std::string &propertyName,
    const winrt::Microsoft::ReactNative::JSValue &propertyValue) {
  if (propertyName == "backgroundColor") {
    if (IsValidColorValue(propertyValue))
      element.ViewBackground(BrushFrom(propertyValue));
    else if (propertyValue.IsNull())
      element.ClearValue(ViewPanel::ViewBackgroundProperty());

    return true;
  }

  return false;
}

// Issue #2172: Calling winrt::Microsoft::ReactNative::ViewPanel::BorderBrushProperty fails
// to call
// down into winrt::Microsoft::ReactNative::implementation::ViewPanel::BorderBrushProperty
// because of how we're using cppwinrt. So we specialize PropertyUtils'
// TryUpdateBorderProperties
// to use winrt::Microsoft::ReactNative::ViewPanel::implementation::BorderBrushProperty
// Remove if we default to ViewGrid implementation.

template <>
bool TryUpdateBorderProperties(
    ShadowNodeBase *node,
    const winrt::Microsoft::ReactNative::ViewPanel &element,
    const std::string &propertyName,
    const winrt::Microsoft::ReactNative::JSValue &propertyValue) {
  bool isBorderProperty = true;

  if (propertyName == "borderColor") {
    if (IsValidColorValue(propertyValue))
      element.BorderBrush(BrushFrom(propertyValue));
    else if (propertyValue.IsNull())
      element.ClearValue(ViewPanel::BorderBrushProperty());
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
      element.ClearValue(ViewPanel::BorderThicknessProperty());
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

static inline bool IsBorderProperty(
    const std::string &propertyName,
    const winrt::Microsoft::ReactNative::JSValue &propertyValue) {
  return propertyName.compare(0, 6, "border") == 0 && propertyValue.AsDouble() > 0;
}

static bool HasBorderProperties(const winrt::Microsoft::ReactNative::JSValueObject &props) {
  for (auto &prop : props) {
    if (IsBorderProperty(prop.first, prop.second)) {
      return true;
    }
  }
  return false;
}

static bool AllowViewGrid() {
  return true;
}

static bool ForceViewGrid() {
  return false;
}

static bool UseViewGrid(const winrt::Microsoft::ReactNative::JSValueObject &props) {
  return ForceViewGrid() || AllowViewGrid() && HasBorderProperties(props);
}


XamlView ViewViewManager::CreateViewCore(int64_t /*tag*/, const winrt::Microsoft::ReactNative::JSValueObject &props) {
  xaml::Controls::Panel panel = nullptr;
  if (UseViewGrid(props)) {
    panel = winrt::make<winrt::Microsoft::ReactNative::implementation::ViewGrid>();
  } else {
    panel = winrt::make<winrt::Microsoft::ReactNative::implementation::ViewPanel>();
  }

  panel.VerticalAlignment(xaml::VerticalAlignment::Stretch);
  panel.HorizontalAlignment(xaml::HorizontalAlignment::Stretch);
  return panel.as<XamlView>();
}

void ViewViewManager::GetNativeProps(const winrt::Microsoft::ReactNative::IJSValueWriter &writer) const {
  Super::GetNativeProps(writer);

  winrt::Microsoft::ReactNative::WriteProperty(writer, L"pointerEvents", L"string");
  winrt::Microsoft::ReactNative::WriteProperty(writer, L"onClick", L"function");
  winrt::Microsoft::ReactNative::WriteProperty(writer, L"focusable", L"boolean");
  winrt::Microsoft::ReactNative::WriteProperty(writer, L"enableFocusRing", L"boolean");
  winrt::Microsoft::ReactNative::WriteProperty(writer, L"tabIndex", L"number");
}

bool ViewViewManager::UpdateProperty(
    ShadowNodeBase *nodeToUpdate,
    const std::string &propertyName,
    const winrt::Microsoft::ReactNative::JSValue &propertyValue) {
  auto *pViewShadowNode = static_cast<ViewShadowNode *>(nodeToUpdate);
  auto pPanel = pViewShadowNode->GetPanel();

  // Replace current View control with Grid if needed for border properties
  if (AllowViewGrid() && !pViewShadowNode->IsGrid() && IsBorderProperty(propertyName, propertyValue)) {
    if (auto pNativeUiManager = GetNativeUIManager(*m_context).lock()) {
      // Create grid
      auto grid = winrt::make<winrt::Microsoft::ReactNative::implementation::ViewGrid>();
      grid.VerticalAlignment(xaml::VerticalAlignment::Stretch);
      grid.HorizontalAlignment(xaml::HorizontalAlignment::Stretch);

      // Move children from current View Control to Grid
      while (pPanel.Children().Size() > 0) {
        const auto child = pPanel.Children().GetAt(0);
        pPanel.Children().RemoveAt(0);
        grid.Children().Append(child);
      }

      // Transfer properties
      TransferProperties(pPanel, grid);
      TransferProperty(
          pPanel,
          grid,
          winrt::Microsoft::ReactNative::ViewPanel::ViewBackgroundProperty(),
          xaml::Controls::Panel::BackgroundProperty());

      if (pViewShadowNode->IsControl()) {
        // Replace the content of the parent control
        pViewShadowNode->GetControl().Content(grid);
      } else {
        // Replace the child in the view hierarchy
        int64_t parentTag = pViewShadowNode->GetParent();
        auto host = pNativeUiManager->getHost();
        auto *pParentNode = static_cast<ShadowNodeBase *>(host->FindShadowNodeForTag(parentTag));
        if (pParentNode != nullptr)
          pParentNode->ReplaceChild(pPanel, grid);
      }

      // Replace the ViewPanel control with a Grid
      pViewShadowNode->ReplaceView(grid);
      pViewShadowNode->RefreshProperties();
      pNativeUiManager->ReplaceView(*nodeToUpdate);

      // Update view type on shadow node
      pViewShadowNode->IsGrid(true);

      // Now that the control has been replaced with a grid, attempt to set the property again
      return UpdateProperty(nodeToUpdate, propertyName, propertyValue);
    }
  }

  auto pViewPanel = pPanel.try_as<winrt::Microsoft::ReactNative::ViewPanel>();
  auto pViewGrid = pPanel.try_as<winrt::Microsoft::ReactNative::ViewGrid>();
  bool ret = true;
  if (pPanel != nullptr) {
    if (!pViewPanel && TryUpdateBackgroundBrush(pPanel, propertyName, propertyValue)) {
    } else if (pViewPanel && UpdateViewPanelProperty(pViewShadowNode, pViewPanel, propertyName, propertyValue)) {
    } else if (pViewGrid && UpdateViewGridProperty(pViewShadowNode, pViewGrid, propertyName, propertyValue)) {
    } else if (UpdatePropertyCore(pViewShadowNode, propertyName, propertyValue)) {
    } else {
      if (propertyName == "accessible") {
        pViewShadowNode->IsAccessible(propertyValue.AsBoolean());
      }
      ret = Super::UpdateProperty(nodeToUpdate, propertyName, propertyValue);
    }
  }

  return ret;
}

bool ViewViewManager::UpdateViewPanelProperty(
    ViewShadowNode *node,
    winrt::Microsoft::ReactNative::ViewPanel &panel,
    const std::string &propertyName,
    const winrt::Microsoft::ReactNative::JSValue &propertyValue) {
  if (TryUpdateBackgroundBrush(panel, propertyName, propertyValue)) {
  } else if (TryUpdateBorderProperties(node, panel, propertyName, propertyValue)) {
  } else if (TryUpdateCornerRadiusOnNode(node, panel, propertyName, propertyValue)) {
    // Do not clamp until a size has been set for the View
    auto maxCornerRadius = std::numeric_limits<double>::max();
    // The Width and Height properties are not always set on ViewPanel. In
    // cases where it is embedded in a Control or outer Border, the values
    // dimensions are set on those wrapper elements. We cannot depend on the
    // default behavior of `UpdateCornerRadiusOnElement` to check for the
    // clamp dimension from only the ViewPanel.
    const xaml::FrameworkElement sizingElement = node->IsControl() ? node->GetControl()
        : node->HasOuterBorder()                                   ? panel.GetOuterBorder().as<xaml::FrameworkElement>()
                                                                   : panel;
    if (sizingElement.ReadLocalValue(xaml::FrameworkElement::WidthProperty()) !=
            xaml::DependencyProperty::UnsetValue() &&
        sizingElement.ReadLocalValue(xaml::FrameworkElement::HeightProperty()) !=
            xaml::DependencyProperty::UnsetValue()) {
      maxCornerRadius = std::min(sizingElement.Width(), sizingElement.Height()) / 2;
    }
    UpdateCornerRadiusOnElement(node, panel, maxCornerRadius);
  } else {
    return false;
  }

  return true;
}

bool ViewViewManager::UpdateViewGridProperty(
    ViewShadowNode *node,
    winrt::Microsoft::ReactNative::ViewGrid &grid,
    const std::string &propertyName,
    const winrt::Microsoft::ReactNative::JSValue &propertyValue) {
  if (TryUpdateBorderProperties(node, grid.as<xaml::Controls::Grid>(), propertyName, propertyValue)) {
  } else if (TryUpdateCornerRadiusOnNode(node, grid, propertyName, propertyValue)) {
    // Do not clamp until a size has been set for the View
    auto maxCornerRadius = std::numeric_limits<double>::max();
    // The Width and Height properties are not always set on ViewPanel. In
    // cases where it is embedded in a Control, the values dimensions are set
    // on those wrapper elements. We cannot depend on the default behavior of
    // `UpdateCornerRadiusOnElement` to check for the clamp dimension from only
    // the ViewPanel.
    const xaml::FrameworkElement sizingElement =
        node->IsControl() ? node->GetControl().as<xaml::FrameworkElement>() : grid;
    if (sizingElement.ReadLocalValue(xaml::FrameworkElement::WidthProperty()) !=
            xaml::DependencyProperty::UnsetValue() &&
        sizingElement.ReadLocalValue(xaml::FrameworkElement::HeightProperty()) !=
            xaml::DependencyProperty::UnsetValue()) {
      maxCornerRadius = std::min(sizingElement.Width(), sizingElement.Height()) / 2;
    }
    UpdateCornerRadiusOnElement(node, grid, maxCornerRadius);
  } else {
    return false;
  }

  return true;
}

bool ViewViewManager::UpdatePropertyCore(
    ViewShadowNode *node,
    const std::string &propertyName,
    const winrt::Microsoft::ReactNative::JSValue &propertyValue) {
  if (TryUpdateMouseEvents(node, propertyName, propertyValue)) {
  } else if (propertyName == "onClick") {
    node->OnClick(propertyValue.AsBoolean());
  } else if (propertyName == "focusable") {
    node->IsFocusable(propertyValue.AsBoolean());
  } else if (propertyName == "enableFocusRing") {
    node->EnableFocusRing(propertyValue.AsBoolean());
  } else if (propertyName == "tabIndex") {
    auto resetTabIndex = true;
    if (propertyValue.Type() == winrt::Microsoft::ReactNative::JSValueType::Int64 ||
        propertyValue.Type() == winrt::Microsoft::ReactNative::JSValueType::Double) {
      const auto tabIndex = propertyValue.AsInt64();
      if (tabIndex == static_cast<int32_t>(tabIndex)) {
        node->TabIndex(static_cast<int32_t>(tabIndex));
        resetTabIndex = false;
      }
    }
    if (resetTabIndex) {
      node->TabIndex(std::numeric_limits<std::int32_t>::max());
    }
  } else {
    return false;
  }

  return true;
}

void ViewViewManager::OnPropertiesUpdated(ShadowNodeBase *node) {
  auto *viewShadowNode = static_cast<ViewShadowNode *>(node);
  auto panel = viewShadowNode->GetPanel();
  auto viewPanel = panel.try_as<winrt::Microsoft::ReactNative::ViewPanel>();
  if (viewPanel &&
      viewPanel.ReadLocalValue(ViewPanel::ViewBackgroundProperty()) == xaml::DependencyProperty::UnsetValue()) {
    // In XAML, a null background means no hit-test will happen.
    // We actually want hit-testing to happen if the app has registered
    // for mouse events, so detect that case and add a transparent background.
    if (viewShadowNode->IsHitTestBrushRequired()) {
      viewPanel.ViewBackground(EnsureTransparentBrush());
    }
    // Note:  Technically we could detect when the transparent brush is
    // no longer needed, but this adds complexity and it can't hurt to
    // keep it around, so not adding that code (yet).
  } else if (
      !viewPanel &&
      panel.ReadLocalValue(xaml::Controls::Panel::BackgroundProperty()) == xaml::DependencyProperty::UnsetValue()) {
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

  // If component is already ViewControl, it should be ViewControl.
  // If component is focusable, it should be a ViewControl.
  // If component is a View with accessible set to true, the component should be focusable, thus we need a ViewControl.
  bool shouldBeControl = viewShadowNode->IsControl() ||
      (viewShadowNode->IsFocusable() || (viewShadowNode->IsAccessible() && !viewShadowNode->OnClick()));
  if (auto view = viewShadowNode->GetView().try_as<xaml::UIElement>()) {
    // If we have DynamicAutomationProperties, we need a ViewControl with a
    // DynamicAutomationPeer
    shouldBeControl = shouldBeControl || HasDynamicAutomationProperties(view);
  }

  if (viewPanel) {
    viewPanel.FinalizeProperties();
  }

  TryUpdateView(viewShadowNode, panel, shouldBeControl);
  SyncFocusableAndAccessible(viewShadowNode, shouldBeControl);
}

void ViewViewManager::TryUpdateView(ViewShadowNode *pViewShadowNode, xaml::Controls::Panel &pPanel, bool useControl) {
  bool isControl = pViewShadowNode->IsControl();
  bool hadOuterBorder = pViewShadowNode->HasOuterBorder();
  auto pViewPanel = pPanel.try_as<winrt::Microsoft::ReactNative::ViewPanel>();
  bool hasOuterBorder = pViewPanel && pViewPanel.GetOuterBorder() != nullptr;

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
  if (isControl && hasOuterBorder != hadOuterBorder) {
    pViewShadowNode->GetControl().Content(nullptr);
  }

  // If don't need a control, then set Outer Border or the Panel as the view
  // root
  if (!useControl) {
    newXamlView = hasOuterBorder ? pViewPanel.GetOuterBorder().try_as<XamlView>() : pPanel.try_as<XamlView>();
  }

  // Clean up child of Border if needed
  if (hasOuterBorder && !hadOuterBorder)
    pViewPanel.GetOuterBorder().Child(nullptr);

  // ASSERT: One of the scenarios should be true, so we should have a root view
  assert(newXamlView != nullptr);

  //
  // 2. Transfer needed properties from old to new view
  //
  {
    SystraceSection s("ViewViewManager::TransferProperties");
    // Transfer properties from old XamlView to the new one
    TransferProperties(oldXamlView, newXamlView);
  }

  // Since we transferred properties to the new view we need to make the call to
  // finalize
  if (pViewPanel) {
    pViewPanel.FinalizeProperties();
  }

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
    xaml::Controls::Border outerBorder = pViewPanel.GetOuterBorder();
    if (outerBorder.Child() == nullptr)
      outerBorder.Child(pPanel.try_as<xaml::UIElement>());

    visualRoot = outerBorder;
  }

  if (useControl)
    pViewShadowNode->GetControl().Content(visualRoot);
}

void ViewViewManager::SyncFocusableAndAccessible(ViewShadowNode *pViewShadowNode, bool useControl) {
  // If developer specifies either the accessible and focusable prop to be false
  // remove accessibility and keyboard focus for component. Exception is made
  // for case where a View with undefined onPress is specified, where
  // component gains accessibility focus when either the accessible and focusable prop are true.
  if (useControl) {
    const auto isFocusable = pViewShadowNode->IsFocusable();
    const auto isAccessible = pViewShadowNode->IsAccessible();
    const auto isPressable = pViewShadowNode->OnClick();
    const auto isTabStop =
        (isPressable && isFocusable && isAccessible) || (!isPressable && (isFocusable || isAccessible));
    const auto accessibilityView = isTabStop ? xaml::Automation::Peers::AccessibilityView::Content
                                             : xaml::Automation::Peers::AccessibilityView::Raw;
    pViewShadowNode->GetControl().IsTabStop(isTabStop);
    xaml::Automation::AutomationProperties::SetAccessibilityView(pViewShadowNode->GetControl(), accessibilityView);
  }
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
  auto pPanel = pViewShadowNode->GetPanel();
  auto pViewPanel = pPanel.try_as<winrt::Microsoft::ReactNative::ViewPanel>();
  auto pViewGrid = pPanel.try_as<winrt::Microsoft::ReactNative::ViewGrid>();
  if (pViewShadowNode->IsControl()) {
    pPanel.Width(width);
    pPanel.Height(height);
  }

  Super::SetLayoutProps(nodeToUpdate, viewToUpdate, left, top, width, height);
  if (pViewPanel &&
      pViewPanel.ReadLocalValue(ViewPanel::CornerRadiusProperty()) != xaml::DependencyProperty::UnsetValue()) {
    // Rather than use ViewPanel::FinalizeProperties, only perform the explicit
    // logic required to propagate the CornerRadius value to the Border parent.
    auto border = pViewPanel.GetOuterBorder();
    if (border) {
      const auto maxCornerRadius = std::min(width, height) / 2;
      UpdateCornerRadiusOnElement(&nodeToUpdate, pViewPanel, maxCornerRadius);
      border.CornerRadius(pViewPanel.CornerRadius());
    }
  }
  if (pViewGrid &&
      pViewGrid.ReadLocalValue(xaml::Controls::Grid::CornerRadiusProperty()) !=
          xaml::DependencyProperty::UnsetValue()) {
    const auto maxCornerRadius = std::min(width, height) / 2;
    UpdateCornerRadiusOnElement(&nodeToUpdate, pViewGrid, maxCornerRadius);
  }
}

xaml::Media::SolidColorBrush ViewViewManager::EnsureTransparentBrush() {
  if (!m_transparentBrush) {
    m_transparentBrush = xaml::Media::SolidColorBrush(winrt::Colors::Transparent());
  }
  return m_transparentBrush;
}

} // namespace Microsoft::ReactNative
