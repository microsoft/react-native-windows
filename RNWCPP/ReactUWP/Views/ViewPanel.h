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
public:
  // Constructors
  ViewPanel();

  // Overrides
  virtual winrt::Windows::Foundation::Size MeasureOverride(winrt::Windows::Foundation::Size availableSize);
  virtual winrt::Windows::Foundation::Size ArrangeOverride(winrt::Windows::Foundation::Size finalSize);

  // Public Methods
  void InsertAt(uint32_t const index, winrt::Windows::UI::Xaml::UIElement const& value) const;
  void RemoveAt(uint32_t const index) const;
  void Clear() const;

  winrt::Windows::UI::Xaml::UIElement GetAt(uint32_t const index) const;
  uint32_t Size() const;

  void FinalizeProperties();

  // Public Properties
  void ClearValue(winrt::Windows::UI::Xaml::DependencyProperty dp) const;

  winrt::Windows::UI::Xaml::Media::Brush Background() { return m_optBackgroundBrush.has_value() ? m_optBackgroundBrush.value() : winrt::Windows::UI::Xaml::Media::SolidColorBrush(); }
  void Background(winrt::Windows::UI::Xaml::Media::Brush const& value);

  winrt::Windows::UI::Xaml::Thickness BorderThickness() { return winrt::unbox_value<winrt::Windows::UI::Xaml::Thickness>(GetValue(BorderThicknessProperty())); }
  void BorderThickness(winrt::Windows::UI::Xaml::Thickness const& value);

  winrt::Windows::UI::Xaml::Media::Brush BorderBrush() { return GetValue(BorderBrushProperty()).as<winrt::Windows::UI::Xaml::Media::Brush>(); }
  void BorderBrush(winrt::Windows::UI::Xaml::Media::Brush const& value);

  winrt::Windows::UI::Xaml::CornerRadius CornerRadius() { return winrt::unbox_value<winrt::Windows::UI::Xaml::CornerRadius>(GetValue(CornerRadiusProperty())); }
  void CornerRadius(winrt::Windows::UI::Xaml::CornerRadius const& value);

  void ClipChildren(bool value);

public:
  static winrt::Windows::UI::Xaml::DependencyProperty BorderBrushProperty();
  static winrt::Windows::UI::Xaml::DependencyProperty BorderThicknessProperty();
  static winrt::Windows::UI::Xaml::DependencyProperty CornerRadiusProperty();
  static winrt::Windows::UI::Xaml::DependencyProperty BackgroundProperty();

private:
  // Managing the Border & Background Elements
  void EnsureBorder();
  void EnsureInnerPanel();
  ViewPanel* GetInnerPanel() const;

  void Remove(winrt::Windows::UI::Xaml::UIElement element) const;

  void UpdateClip();

private:

  // Properties: Background is not managed as a DP so it won't conflict with the parent Background property.
  std::optional<winrt::Windows::UI::Xaml::Media::Brush> m_optBackgroundBrush;
  bool m_clipChildren { false };
  bool m_propertiesChanged { false };

  // Child Elements
  winrt::Windows::UI::Xaml::Controls::Border m_border { nullptr };
  winrt::Windows::UI::Xaml::UIElement m_innerElement { nullptr };

private:
  static void VisualPropertyChanged(winrt::Windows::UI::Xaml::DependencyObject sender, winrt::Windows::UI::Xaml::DependencyPropertyChangedEventArgs e);
};

} }
