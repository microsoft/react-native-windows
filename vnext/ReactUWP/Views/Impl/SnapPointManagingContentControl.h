#pragma once

#include "winrt/Windows.Foundation.Collections.h"
#include "winrt/Windows.Foundation.h"
#include "winrt/Windows.UI.Xaml.Controls.Primitives.h"
#include "winrt/Windows.UI.Xaml.Controls.h"
#include "winrt/Windows.UI.Xaml.h"

namespace winrt {
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Primitives;
} // namespace winrt

namespace react {
namespace uwp {

class SnapPointManagingContentControl
    : public winrt::ContentControlT<SnapPointManagingContentControl, winrt::IScrollSnapPointsInfo> {
 public:
  SnapPointManagingContentControl();

  static winrt::com_ptr<SnapPointManagingContentControl> Create();

  // ScrollView Implementation
  void SnapToInterval(float interval);
  void SnapToOffsets(const winrt::IVectorView<float> &offsets);
  void SnapToStart(bool snapToStart);
  void SnapToEnd(bool snapToEnd);
  void NotifySnapPointsUpdated();
  void PagingEnabled(bool pagingEnabled);

  // IScrollSnapPointsInfo Implementation
  bool AreHorizontalSnapPointsRegular();
  bool AreVerticalSnapPointsRegular();

  winrt::event_token HorizontalSnapPointsChanged(winrt::EventHandler<winrt::IInspectable> const &value);
  void HorizontalSnapPointsChanged(winrt::event_token const &token);

  winrt::event_token VerticalSnapPointsChanged(winrt::EventHandler<winrt::IInspectable> const &value);
  void VerticalSnapPointsChanged(winrt::event_token const &token);

  winrt::IVectorView<float> GetIrregularSnapPoints(
      winrt::Orientation orientation,
      winrt::SnapPointsAlignment alignment);

  float GetRegularSnapPoints(winrt::Orientation orientation, winrt::SnapPointsAlignment alignment, float offset);

  // Helpers
  void SetHorizontal(bool horizontal);
  void SetHeightBounds(float startHeight, float endHeight);
  void SetWidthBounds(float startWidth, float endWidth);
  void SetViewportSize(float scaledViewportWidth, float scaledviewportHeight);

  bool IsHorizontal() {
    return m_horizontal;
  }

 private:
  float m_interval{0.0f};
  winrt::IVectorView<float> m_offsets{};
  bool m_snapToStart{true};
  bool m_snapToEnd{true};
  bool m_pagingEnabled{false};
  winrt::event<winrt::EventHandler<winrt::IInspectable>> m_horizontalSnapPointsChangedEventSource;
  winrt::event<winrt::EventHandler<winrt::IInspectable>> m_verticalSnapPointsChangedEventSource;

  bool m_horizontal{false};
  float m_startHeight{0};
  float m_startWidth{0};
  float m_endHeight{INFINITY};
  float m_endWidth{INFINITY};
  float m_viewportHeight{0};
  float m_viewportWidth{0};
};
} // namespace uwp
} // namespace react
