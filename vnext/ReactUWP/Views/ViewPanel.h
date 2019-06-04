// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include <winrt/Windows.UI.Xaml.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Media.h>

namespace react { namespace uwp {

struct ViewPanel : winrt::Windows::UI::Xaml::Controls::PanelT<ViewPanel>
{
  using Super = winrt::Windows::UI::Xaml::Controls::PanelT<ViewPanel>;
private:
  // Constructors
  ViewPanel();

public:
  static winrt::com_ptr<ViewPanel> Create();

public:
  template <typename D, typename... Args> friend auto winrt::make_self(Args&&... args);

  // Overrides
  virtual winrt::Windows::Foundation::Size MeasureOverride(winrt::Windows::Foundation::Size availableSize);
  virtual winrt::Windows::Foundation::Size ArrangeOverride(winrt::Windows::Foundation::Size finalSize);

  // Public Methods
  void InsertAt(uint32_t const index, winrt::Windows::UI::Xaml::UIElement const& value) const;
  void RemoveAt(uint32_t const index) const;
  void Clear() const;

  void FinalizeProperties();
  winrt::Windows::UI::Xaml::Controls::Border GetOuterBorder();

  // Public Properties
  winrt::Windows::UI::Xaml::Media::Brush Background() { return GetValue(BackgroundProperty()).try_as<winrt::Windows::UI::Xaml::Media::Brush>(); }
  void Background(winrt::Windows::UI::Xaml::Media::Brush const& value);

  winrt::Windows::UI::Xaml::Thickness BorderThickness() { return winrt::unbox_value<winrt::Windows::UI::Xaml::Thickness>(GetValue(BorderThicknessProperty())); }
  void BorderThickness(winrt::Windows::UI::Xaml::Thickness const& value);

  winrt::Windows::UI::Xaml::Media::Brush BorderBrush() { return GetValue(BorderBrushProperty()).as<winrt::Windows::UI::Xaml::Media::Brush>(); }
  void BorderBrush(winrt::Windows::UI::Xaml::Media::Brush const& value);

  winrt::Windows::UI::Xaml::CornerRadius CornerRadius() { return winrt::unbox_value<winrt::Windows::UI::Xaml::CornerRadius>(GetValue(CornerRadiusProperty())); }
  void CornerRadius(winrt::Windows::UI::Xaml::CornerRadius const& value);

  bool ClipChildren() { return winrt::unbox_value<bool>(GetValue(ClipChildrenProperty())); }
  void ClipChildren(bool value);

public:
  // ViewPanel Properties
  static winrt::Windows::UI::Xaml::DependencyProperty BorderBrushProperty();
  static winrt::Windows::UI::Xaml::DependencyProperty BorderThicknessProperty();
  static winrt::Windows::UI::Xaml::DependencyProperty CornerRadiusProperty();
  static winrt::Windows::UI::Xaml::DependencyProperty BackgroundProperty();
  static winrt::Windows::UI::Xaml::DependencyProperty ClipChildrenProperty();

  // Attached Properties
  static winrt::Windows::UI::Xaml::DependencyProperty TopProperty();
  static void SetTop(winrt::Windows::UI::Xaml::UIElement& element, double value);
  static double GetTop(winrt::Windows::UI::Xaml::UIElement& element) { return winrt::unbox_value<double>(element.GetValue(TopProperty())); }

  static winrt::Windows::UI::Xaml::DependencyProperty LeftProperty();
  static void SetLeft(winrt::Windows::UI::Xaml::UIElement& element, double value);
  static double GetLeft(winrt::Windows::UI::Xaml::UIElement& element) { return winrt::unbox_value<double>(element.GetValue(LeftProperty())); }

private:
  void Remove(winrt::Windows::UI::Xaml::UIElement element) const;

  void UpdateClip(winrt::Windows::Foundation::Size& finalSize);

private:
  // Properties: Background is not managed as a DP so it won't conflict with the parent Background property.
  //std::optional<winrt::Windows::UI::Xaml::Media::Brush> m_optBackgroundBrush;
  bool m_propertiesChanged { false };

  // Child Elements
  winrt::Windows::UI::Xaml::Controls::Border m_border { nullptr };
  bool m_hasOuterBorder;

private:
  static void VisualPropertyChanged(winrt::Windows::UI::Xaml::DependencyObject sender, winrt::Windows::UI::Xaml::DependencyPropertyChangedEventArgs e);
  static void PositionPropertyChanged(winrt::Windows::UI::Xaml::DependencyObject sender, winrt::Windows::UI::Xaml::DependencyPropertyChangedEventArgs e);
};

} }
