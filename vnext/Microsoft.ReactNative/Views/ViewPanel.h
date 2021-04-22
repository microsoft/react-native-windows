// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include "ViewPanel.g.h"

#ifndef PROJECT_ROOT_NAMESPACE
#define PROJECT_ROOT_NAMESPACE react::uwp
#else
namespace winrt::Microsoft::ReactNative {}
namespace winrt::react::uwp {
using namespace winrt::Microsoft::ReactNative;
}
#endif

namespace winrt::PROJECT_ROOT_NAMESPACE::implementation {

//
// ViewPanel is our custom Panel used by ViewViewManager
//
struct ViewPanel : ViewPanelT<ViewPanel> {
  using Super = ViewPanelT<ViewPanel>;

 public:
  // Constructors
  ViewPanel();

  xaml::Automation::Peers::AutomationPeer OnCreateAutomationPeer();

  // Overrides
  virtual winrt::Windows::Foundation::Size MeasureOverride(winrt::Windows::Foundation::Size availableSize);
  virtual winrt::Windows::Foundation::Size ArrangeOverride(winrt::Windows::Foundation::Size finalSize);

  // Public Methods
  void InsertAt(uint32_t const index, xaml::UIElement const &value) const;
  void RemoveAt(uint32_t const index) const;
  void Clear() const;

  void FinalizeProperties();
  xaml::Controls::Border GetOuterBorder();

  // Public Properties
  xaml::Media::Brush ViewBackground() {
    return GetValue(ViewBackgroundProperty()).as<xaml::Media::Brush>();
  }
  void ViewBackground(xaml::Media::Brush const &value);

  xaml::Thickness BorderThickness() {
    return winrt::unbox_value<xaml::Thickness>(GetValue(BorderThicknessProperty()));
  }
  void BorderThickness(xaml::Thickness const &value);

  xaml::Media::Brush BorderBrush() {
    return GetValue(BorderBrushProperty()).as<xaml::Media::Brush>();
  }
  void BorderBrush(xaml::Media::Brush const &value);

  xaml::CornerRadius CornerRadius() {
    return winrt::unbox_value<xaml::CornerRadius>(GetValue(CornerRadiusProperty()));
  }
  void CornerRadius(xaml::CornerRadius const &value);

  bool ClipChildren() {
    return winrt::unbox_value<bool>(GetValue(ClipChildrenProperty()));
  }
  void ClipChildren(bool value);

  // ViewPanel Properties
  static xaml::DependencyProperty ViewBackgroundProperty();
  static xaml::DependencyProperty BorderThicknessProperty();
  static xaml::DependencyProperty BorderBrushProperty();
  static xaml::DependencyProperty CornerRadiusProperty();
  static xaml::DependencyProperty ClipChildrenProperty();

  // Attached Properties
  static xaml::DependencyProperty TopProperty();
  static void SetTop(xaml::UIElement const &element, double value);
  static double GetTop(xaml::UIElement const &element) {
    return winrt::unbox_value<double>(element.GetValue(TopProperty()));
  }

  static xaml::DependencyProperty LeftProperty();
  static void SetLeft(xaml::UIElement const &element, double value);
  static double GetLeft(xaml::UIElement const &element) {
    return winrt::unbox_value<double>(element.GetValue(LeftProperty()));
  }

 private:
  void Remove(xaml::UIElement element) const;

  void UpdateClip(winrt::Windows::Foundation::Size &finalSize);

  static void InvalidateForArrange(xaml::UIElement element);

 private:
  bool m_propertiesChanged{false};

  // Child Elements
  xaml::Controls::Border m_border{nullptr};
  bool m_hasOuterBorder{false};

 private:
  static void VisualPropertyChanged(xaml::DependencyObject sender, xaml::DependencyPropertyChangedEventArgs e);
  static void PositionPropertyChanged(xaml::DependencyObject sender, xaml::DependencyPropertyChangedEventArgs e);
};

} // namespace winrt::PROJECT_ROOT_NAMESPACE::implementation

namespace winrt::PROJECT_ROOT_NAMESPACE::factory_implementation {
struct ViewPanel : ViewPanelT<ViewPanel, implementation::ViewPanel> {};
} // namespace winrt::PROJECT_ROOT_NAMESPACE::factory_implementation

namespace react::uwp {
// Issue #2172: Calling static members on winrt::react::uwp::ViewPanel fails to
// call
// down into winrt::react::uwp::implementation::ViewPanel because of how we're
// using cppwinrt. This workaround is so that consumers in react::uwp can just
// call ViewPanel

using ViewPanel = winrt::PROJECT_ROOT_NAMESPACE::implementation::ViewPanel;
} // namespace react::uwp
