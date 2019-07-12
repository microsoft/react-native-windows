// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include <winrt/Windows.UI.Xaml.Automation.Peers.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Media.h>
#include <winrt/Windows.UI.Xaml.h>

#include "Utils/XamlDirectInstance.h"

#include "cppwinrt/ViewPanel.g.h"

namespace winrt::react::uwp::implementation {

//
// ViewPanel is our custom Panel used by ViewViewManager
//
struct ViewPanel : ViewPanelT<ViewPanel> {
  using Super = ViewPanelT<ViewPanel>;

 public:
  // Constructors
  ViewPanel();

  winrt::Windows::UI::Xaml::Automation::Peers::AutomationPeer
  OnCreateAutomationPeer();

  // Overrides
  virtual winrt::Windows::Foundation::Size MeasureOverride(
      winrt::Windows::Foundation::Size availableSize);
  virtual winrt::Windows::Foundation::Size ArrangeOverride(
      winrt::Windows::Foundation::Size finalSize);

  // Public Methods
  void InsertAt(
      uint32_t const index,
      winrt::Windows::UI::Xaml::UIElement const &value) const;
  void RemoveAt(uint32_t const index) const;
  void Clear() const;

  void FinalizeProperties();
  winrt::Windows::UI::Xaml::Controls::Border GetOuterBorder();

  // Public Properties
  winrt::Windows::UI::Xaml::Media::Brush ViewBackground() {
    return GetValue(ViewBackgroundProperty())
        .as<winrt::Windows::UI::Xaml::Media::Brush>();
  }
  void ViewBackground(winrt::Windows::UI::Xaml::Media::Brush const &value);

  winrt::Windows::UI::Xaml::Thickness BorderThickness() {
    return winrt::unbox_value<winrt::Windows::UI::Xaml::Thickness>(
        GetValue(BorderThicknessProperty()));
  }
  void BorderThickness(winrt::Windows::UI::Xaml::Thickness const &value);

  winrt::Windows::UI::Xaml::Media::Brush BorderBrush() {
    return GetValue(BorderBrushProperty())
        .as<winrt::Windows::UI::Xaml::Media::Brush>();
  }
  void BorderBrush(winrt::Windows::UI::Xaml::Media::Brush const &value);

  winrt::Windows::UI::Xaml::CornerRadius CornerRadius() {
    return winrt::unbox_value<winrt::Windows::UI::Xaml::CornerRadius>(
        GetValue(CornerRadiusProperty()));
  }
  void CornerRadius(winrt::Windows::UI::Xaml::CornerRadius const &value);

  bool ClipChildren() {
    return winrt::unbox_value<bool>(GetValue(ClipChildrenProperty()));
  }
  void ClipChildren(bool value);

  // ViewPanel Properties
  static winrt::Windows::UI::Xaml::DependencyProperty ViewBackgroundProperty();
  static winrt::Windows::UI::Xaml::DependencyProperty BorderThicknessProperty();
  static winrt::Windows::UI::Xaml::DependencyProperty BorderBrushProperty();
  static winrt::Windows::UI::Xaml::DependencyProperty CornerRadiusProperty();
  static winrt::Windows::UI::Xaml::DependencyProperty ClipChildrenProperty();

  // Attached Properties
  static winrt::Windows::UI::Xaml::DependencyProperty TopProperty();
  static void SetTop(
      winrt::Windows::UI::Xaml::UIElement const &element,
      double value);
  static double GetTop(winrt::Windows::UI::Xaml::UIElement const &element) {
    return winrt::unbox_value<double>(element.GetValue(TopProperty()));
  }

  static winrt::Windows::UI::Xaml::DependencyProperty LeftProperty();
  static void SetLeft(
      winrt::Windows::UI::Xaml::UIElement const &element,
      double value);
  static double GetLeft(winrt::Windows::UI::Xaml::UIElement const &element) {
    return winrt::unbox_value<double>(element.GetValue(LeftProperty()));
  }

 private:
  void Remove(winrt::Windows::UI::Xaml::UIElement element) const;

  void UpdateClip(winrt::Windows::Foundation::Size &finalSize);

 private:
  bool m_propertiesChanged{false};

  // Child Elements
  winrt::Windows::UI::Xaml::Controls::Border m_border{nullptr};
  winrt::Windows::Foundation::IInspectable m_border_xd;
  bool m_hasOuterBorder;

 private:
  static void VisualPropertyChanged(
      winrt::Windows::UI::Xaml::DependencyObject sender,
      winrt::Windows::UI::Xaml::DependencyPropertyChangedEventArgs e);
  static void PositionPropertyChanged(
      winrt::Windows::UI::Xaml::DependencyObject sender,
      winrt::Windows::UI::Xaml::DependencyPropertyChangedEventArgs e);
};

} // namespace winrt::react::uwp::implementation

namespace winrt::react::uwp::factory_implementation {
struct ViewPanel : ViewPanelT<ViewPanel, implementation::ViewPanel> {};
} // namespace winrt::react::uwp::factory_implementation

namespace react::uwp {
// Issue #2172: Calling static members on winrt::react::uwp::ViewPanel fails to
// call
// down into winrt::react::uwp::implementation::ViewPanel because of how we're
// using cppwinrt. This workaround is so that consumers in react::uwp can just
// call ViewPanel

using ViewPanel = winrt::react::uwp::implementation::ViewPanel;
} // namespace react::uwp
