// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include <winrt/Windows.Foundation.h>

#include "DynamicAutomationProperties.h"

#include "DynamicAutomationPeer.g.h"

namespace winrt::Microsoft::ReactNative::implementation {

//
// DynamicAutomationPeer translates the values of the attached properties in
// DynamicAutomationProperties into the equivalent UIA AutomationPeer
//
struct DynamicAutomationPeer : DynamicAutomationPeerT<DynamicAutomationPeer> {
  using Super = DynamicAutomationPeerT<DynamicAutomationPeer>;

  DynamicAutomationPeer() = delete;
  DynamicAutomationPeer(xaml::FrameworkElement const &owner);

  winrt::hstring GetClassNameCore() const;
  winrt::hstring GetNameCore() const;

  xaml::Automation::Peers::AutomationControlType GetAutomationControlTypeCore() const;
  winrt::Windows::Foundation::IInspectable GetPatternCore(
      xaml::Automation::Peers::PatternInterface const &patternInterface) const;

  bool IsEnabledCore() const;

  winrt::hstring GetItemStatusCore() const;

  // IInvokeProvider
  void Invoke() const;

  // ISelectionProvider
  bool CanSelectMultiple() const {
    return true;
  }
  bool IsSelectionRequired() const {
    return false;
  }
  winrt::com_array<xaml::Automation::Provider::IRawElementProviderSimple> GetSelection() const;

  // ISelectionItemProvider
  bool IsSelected() const;
  xaml::Automation::Provider::IRawElementProviderSimple SelectionContainer() const;
  void AddToSelection() const;
  void RemoveFromSelection() const;
  void Select() const;

  // IToggleProvider
  xaml::Automation::ToggleState ToggleState() const;
  void Toggle() const;

  // IExpandCollapseProvider
  xaml::Automation::ExpandCollapseState ExpandCollapseState() const;
  void Expand() const;
  void Collapse() const;

  // IRangeValueProvider
  double Minimum();
  double Maximum();
  double Value();
  double SmallChange();
  double LargeChange();
  bool IsReadOnly();
  void SetValue(double value);

 private:
  winrt::hstring GetContentName() const;
  winrt::Microsoft::ReactNative::AccessibilityRoles GetAccessibilityRole() const;
  bool HasAccessibilityState(winrt::Microsoft::ReactNative::AccessibilityStates state) const;
  bool GetAccessibilityState(winrt::Microsoft::ReactNative::AccessibilityStates state) const;
  bool HasAccessibilityValue(winrt::Microsoft::ReactNative::AccessibilityValue value) const;
  double GetAccessibilityValueRange(winrt::Microsoft::ReactNative::AccessibilityValue value) const;

  winrt::Microsoft::ReactNative::AccessibilityInvokeEventHandler GetAccessibilityInvokeEventHandler() const;

  static xaml::DependencyProperty AccessibilityActionsProperty();
  static void SetAccessibilityActions(
      xaml::UIElement const &element,
      Windows::Foundation::Collections::IVector<Microsoft::ReactNative::AccessibilityAction> const &value);
  static Windows::Foundation::Collections::IVector<Microsoft::ReactNative::AccessibilityAction> GetAccessibilityActions(
      xaml::UIElement const &element);
  static void DispatchAccessibilityAction(xaml::UIElement const &element, std::wstring_view const &actionName);
  static xaml::DependencyProperty AccessibilityActionEventHandlerProperty();
  static void SetAccessibilityActionEventHandler(
      xaml::UIElement const &element,
      winrt::Microsoft::ReactNative::AccessibilityActionEventHandler const &value);
  static winrt::Microsoft::ReactNative::AccessibilityActionEventHandler GetAccessibilityActionEventHandler(
      xaml::UIElement const &element);
};
} // namespace winrt::Microsoft::ReactNative::implementation

namespace winrt::Microsoft::ReactNative::factory_implementation {
struct DynamicAutomationPeer : DynamicAutomationPeerT<DynamicAutomationPeer, implementation::DynamicAutomationPeer> {};
} // namespace winrt::Microsoft::ReactNative::factory_implementation
