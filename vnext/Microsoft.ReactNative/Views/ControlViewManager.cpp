// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"

#include <UI.Xaml.Controls.h>

#include <Views/ControlViewManager.h>
#include <Views/FrameworkElementTransferProperties.h>
#include <Views/ShadowNodeBase.h>

#include <JSValueWriter.h>
#include <Utils/PropertyUtils.h>

#include <UI.Xaml.Automation.Peers.h>

#ifdef USE_WINUI3
#define TAB_INDEX_PROPERTY xaml::UIElement::TabIndexProperty
#define TAB_STOP_PROPERTY xaml::UIElement::IsTabStopProperty
#else
#define TAB_INDEX_PROPERTY xaml::Controls::Control::TabIndexProperty
#define TAB_STOP_PROPERTY xaml::Controls::Control::IsTabStopProperty
#endif

namespace Microsoft::ReactNative {

ControlViewManager::ControlViewManager(const Mso::React::IReactContext &context) : Super(context) {}

void ControlViewManager::GetNativeProps(const winrt::Microsoft::ReactNative::IJSValueWriter &writer) const {
  Super::GetNativeProps(writer);
  winrt::Microsoft::ReactNative::WriteProperty(writer, L"tabIndex", L"number");
  winrt::Microsoft::ReactNative::WriteProperty(writer, L"focusable", L"boolean");
}
void ControlViewManager::TransferProperties(const XamlView &oldView, const XamlView &newView) {
  TransferProperty(oldView, newView, xaml::Controls::Control::FontSizeProperty());
  TransferProperty(oldView, newView, xaml::Controls::Control::FontFamilyProperty());
  TransferProperty(oldView, newView, xaml::Controls::Control::FontWeightProperty());
  TransferProperty(oldView, newView, xaml::Controls::Control::FontStyleProperty());
  TransferProperty(oldView, newView, xaml::Controls::Control::CharacterSpacingProperty());
  TransferProperty(oldView, newView, xaml::Controls::Control::IsTextScaleFactorEnabledProperty());
  TransferProperty(oldView, newView, xaml::Controls::Control::BackgroundProperty());
  TransferProperty(oldView, newView, xaml::Controls::Control::BorderBrushProperty());
  TransferProperty(oldView, newView, xaml::Controls::Control::BorderThicknessProperty());
  TransferProperty(oldView, newView, xaml::Controls::Control::PaddingProperty());
  TransferProperty(oldView, newView, xaml::Controls::Control::ForegroundProperty());
  TransferProperty(oldView, newView, TAB_INDEX_PROPERTY());
  TransferProperty(oldView, newView, TAB_STOP_PROPERTY());

  // Control.CornerRadius is only supported on >= RS5
  if (oldView.try_as<xaml::Controls::IControl7>() && newView.try_as<xaml::Controls::IControl7>()) {
    TransferProperty(oldView, newView, xaml::Controls::Control::CornerRadiusProperty());
  }
  Super::TransferProperties(oldView, newView);
}

bool ControlViewManager::UpdateProperty(
    ShadowNodeBase *nodeToUpdate,
    const std::string &propertyName,
    const winrt::Microsoft::ReactNative::JSValue &propertyValue) {
  auto control(nodeToUpdate->GetView().as<xaml::Controls::Control>());

  bool implementsPadding = nodeToUpdate->ImplementsPadding();
  bool finalizeBorderRadius{false};
  bool ret = true;

  if (control != nullptr) {
    if (TryUpdateBackgroundBrush(control, propertyName, propertyValue)) {
    } else if (TryUpdateBorderProperties(nodeToUpdate, control, propertyName, propertyValue)) {
    } else if (TryUpdateForeground(control, propertyName, propertyValue)) {
    } else if (TryUpdateCornerRadiusOnNode(nodeToUpdate, control, propertyName, propertyValue)) {
      finalizeBorderRadius = true;
    } else if (implementsPadding && TryUpdatePadding(nodeToUpdate, control, propertyName, propertyValue)) {
    } else if (propertyName == "tabIndex") {
      if (propertyValue.Type() == winrt::Microsoft::ReactNative::JSValueType::Double ||
          propertyValue.Type() == winrt::Microsoft::ReactNative::JSValueType::Int64) {
        auto tabIndex = propertyValue.AsDouble();
        if (tabIndex == static_cast<int32_t>(tabIndex))
          control.ClearValue(TAB_INDEX_PROPERTY());
        control.TabIndex(static_cast<int32_t>(tabIndex));
      } else if (propertyValue.IsNull()) {
        control.ClearValue(TAB_INDEX_PROPERTY());
      }
    } else if (propertyName == "focusable") {
      if (propertyValue.Type() == winrt::Microsoft::ReactNative::JSValueType::Boolean) {
        IsFocusable(propertyValue.AsBoolean());
        control.IsTabStop(propertyValue.AsBoolean());
      } else if (propertyValue.IsNull()) {
        control.ClearValue(TAB_STOP_PROPERTY());
        IsFocusable(false);
      }
    } else {
      if (propertyName == "accessible") {
        if (propertyValue.Type() == winrt::Microsoft::ReactNative::JSValueType::Boolean) {
          IsAccessible(propertyValue.AsBoolean());
        }
      }
      ret = Super::UpdateProperty(nodeToUpdate, propertyName, propertyValue);
    }
  }

  if (finalizeBorderRadius && control.try_as<xaml::Controls::IControl7>()) {
    // Control.CornerRadius is only supported on >= RS5, setting borderRadius on Controls have no effect < RS5
    UpdateCornerRadiusOnElement(nodeToUpdate, control);
  }
  return ret;
}

void ControlViewManager::OnPropertiesUpdated(ShadowNodeBase *node) {
  auto control(node->GetView().as<xaml::Controls::Control>());

  if (IsAccessible() != IsFocusable()) {
    control.IsTabStop(false);
    xaml::Automation::AutomationProperties::SetAccessibilityView(
        control, xaml::Automation::Peers::AccessibilityView::Raw);
  }
}

void ControlViewManager::OnViewCreated(XamlView view) {
  // Set the default cornerRadius to 0 for Control: WinUI usually default cornerRadius to 2
  // Only works on >= RS5 because Control.CornerRadius is only supported >= RS5
  if (auto control = view.try_as<xaml::Controls::IControl7>()) {
    control.CornerRadius({0, 0, 0, 0});
  }
}

void ControlViewManager::IsAccessible(bool accessible) {
  m_isAccessible = accessible;
}
bool ControlViewManager::IsAccessible() {
  return m_isAccessible;
}
void ControlViewManager::IsFocusable(bool focusable) {
  m_isFocusable = focusable;
}
bool ControlViewManager::IsFocusable() {
  return m_isFocusable;
}

} // namespace Microsoft::ReactNative
