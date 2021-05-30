// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"

#include <UI.Xaml.Automation.h>
#include <UI.Xaml.Controls.h>
#include "DynamicAutomationProperties.h"
#include "FrameworkElementTransferProperties.h"

namespace Microsoft::ReactNative {

void TransferProperty(
    const xaml::DependencyObject &oldView,
    const xaml::DependencyObject &newView,
    xaml::DependencyProperty oldViewDP,
    xaml::DependencyProperty newViewDP) {
  auto oldValue = oldView.ReadLocalValue(oldViewDP);
  if (oldValue != nullptr) {
    oldView.ClearValue(oldViewDP);
    newView.SetValue(newViewDP, oldValue);
  }
}

void TransferProperty(
    const xaml::DependencyObject &oldView,
    const xaml::DependencyObject &newView,
    xaml::DependencyProperty dp) {
  TransferProperty(oldView, newView, dp, dp);
}

void TransferFrameworkElementProperties(const xaml::DependencyObject &oldView, const xaml::DependencyObject &newView) {
  // Render Properties
  TransferProperty(oldView, newView, xaml::UIElement::OpacityProperty());

  // Layout Properties
  TransferProperty(oldView, newView, xaml::FrameworkElement::WidthProperty());
  TransferProperty(oldView, newView, xaml::FrameworkElement::HeightProperty());
  TransferProperty(oldView, newView, xaml::FrameworkElement::MinWidthProperty());
  TransferProperty(oldView, newView, xaml::FrameworkElement::MinHeightProperty());
  TransferProperty(oldView, newView, xaml::FrameworkElement::MaxWidthProperty());
  TransferProperty(oldView, newView, xaml::FrameworkElement::MaxHeightProperty());
  TransferProperty(oldView, newView, xaml::FrameworkElement::FlowDirectionProperty());
  TransferProperty(oldView, newView, xaml::Controls::Canvas::ZIndexProperty());
  TransferProperty(oldView, newView, winrt::Microsoft::ReactNative::ViewPanel::LeftProperty());
  TransferProperty(oldView, newView, winrt::Microsoft::ReactNative::ViewPanel::TopProperty());

  // Accessibility Properties
  TransferProperty(oldView, newView, xaml::Automation::AutomationProperties::AutomationIdProperty());
  TransferProperty(oldView, newView, xaml::Automation::AutomationProperties::NameProperty());
  TransferProperty(oldView, newView, xaml::Automation::AutomationProperties::HelpTextProperty());
  TransferProperty(oldView, newView, xaml::Automation::AutomationProperties::LiveSettingProperty());
  TransferProperty(oldView, newView, xaml::Automation::AutomationProperties::PositionInSetProperty());
  TransferProperty(oldView, newView, xaml::Automation::AutomationProperties::SizeOfSetProperty());
  auto accessibilityView = xaml::Automation::AutomationProperties::GetAccessibilityView(oldView);
  xaml::Automation::AutomationProperties::SetAccessibilityView(newView, accessibilityView);
  xaml::Automation::AutomationProperties::SetAccessibilityView(
      oldView, xaml::Automation::Peers::AccessibilityView::Raw);
  TransferProperty(
      oldView, newView, winrt::Microsoft::ReactNative::DynamicAutomationProperties::AccessibilityRoleProperty());
  TransferProperty(
      oldView,
      newView,
      winrt::Microsoft::ReactNative::DynamicAutomationProperties::AccessibilityStateSelectedProperty());
  TransferProperty(
      oldView,
      newView,
      winrt::Microsoft::ReactNative::DynamicAutomationProperties::AccessibilityStateDisabledProperty());
  TransferProperty(
      oldView,
      newView,
      winrt::Microsoft::ReactNative::DynamicAutomationProperties::AccessibilityStateCheckedProperty());
  TransferProperty(
      oldView,
      newView,
      winrt::Microsoft::ReactNative::DynamicAutomationProperties::AccessibilityStateUncheckedProperty());
  TransferProperty(
      oldView, newView, winrt::Microsoft::ReactNative::DynamicAutomationProperties::AccessibilityStateBusyProperty());
  TransferProperty(
      oldView,
      newView,
      winrt::Microsoft::ReactNative::DynamicAutomationProperties::AccessibilityStateExpandedProperty());
  TransferProperty(
      oldView,
      newView,
      winrt::Microsoft::ReactNative::DynamicAutomationProperties::AccessibilityStateCollapsedProperty());
  TransferProperty(
      oldView, newView, winrt::Microsoft::ReactNative::DynamicAutomationProperties::AccessibilityValueMinProperty());
  TransferProperty(
      oldView, newView, winrt::Microsoft::ReactNative::DynamicAutomationProperties::AccessibilityValueMaxProperty());
  TransferProperty(
      oldView, newView, winrt::Microsoft::ReactNative::DynamicAutomationProperties::AccessibilityValueNowProperty());
  TransferProperty(
      oldView, newView, winrt::Microsoft::ReactNative::DynamicAutomationProperties::AccessibilityValueTextProperty());
  TransferProperty(
      oldView,
      newView,
      winrt::Microsoft::ReactNative::DynamicAutomationProperties::AccessibilityInvokeEventHandlerProperty());
  TransferProperty(
      oldView,
      newView,
      winrt::Microsoft::ReactNative::DynamicAutomationProperties::AccessibilityActionEventHandlerProperty());
  TransferProperty(
      oldView, newView, winrt::Microsoft::ReactNative::DynamicAutomationProperties::AccessibilityActionsProperty());

  auto tooltip = xaml::Controls::ToolTipService::GetToolTip(oldView);
  oldView.ClearValue(xaml::Controls::ToolTipService::ToolTipProperty());
  xaml::Controls::ToolTipService::SetToolTip(newView, tooltip);

  // Clear the TransformMatrix from the old View.  The TransformMatrix will be
  // set on the new View a bit later in RefreshProperties() (as we need data
  // from the ShadowNode not available here).
  auto oldElement = oldView.try_as<xaml::UIElement>();
  if (oldElement && oldElement.try_as<xaml::IUIElement10>()) {
    oldElement.TransformMatrix(winrt::Windows::Foundation::Numerics::float4x4::identity());
  }
}
} // namespace Microsoft::ReactNative
