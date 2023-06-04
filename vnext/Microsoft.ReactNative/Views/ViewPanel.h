// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include "ViewPanel.g.h"

namespace winrt::Microsoft::ReactNative::implementation {

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

  // Public Properties
  xaml::Media::Brush ViewBackground() {
    return GetValue(ViewBackgroundProperty()).as<xaml::Media::Brush>();
  }
  void ViewBackground(xaml::Media::Brush const &value);

  // ViewPanel Properties
  static xaml::DependencyProperty ViewBackgroundProperty();
  static xaml::DependencyProperty BorderThicknessProperty();
  static xaml::DependencyProperty BorderBrushProperty();
  static xaml::DependencyProperty CornerRadiusProperty();

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

  static void InvalidateForArrange(const xaml::DependencyObject &element);

 private:
  void Remove(xaml::UIElement element) const;

 private:
  bool m_propertiesChanged{false};

 private:
  static void VisualPropertyChanged(xaml::DependencyObject sender, xaml::DependencyPropertyChangedEventArgs e);
};

} // namespace winrt::Microsoft::ReactNative::implementation

namespace winrt::Microsoft::ReactNative::factory_implementation {
struct ViewPanel : ViewPanelT<ViewPanel, implementation::ViewPanel> {};
} // namespace winrt::Microsoft::ReactNative::factory_implementation

namespace Microsoft::ReactNative {
// Issue #2172: Calling static members on winrt::Microsoft::ReactNative::ViewPanel fails to
// call
// down into winrt::Microsoft::ReactNative::implementation::ViewPanel because of how we're
// using cppwinrt. This workaround is so that consumers in Microsoft::ReactNative can just
// call ViewPanel

using ViewPanel = winrt::Microsoft::ReactNative::implementation::ViewPanel;
} // namespace Microsoft::ReactNative
