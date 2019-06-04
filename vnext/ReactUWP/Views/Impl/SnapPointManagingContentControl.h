#pragma once

#include "pch.h"

namespace winrt {
  using namespace Windows::Foundation;
  using namespace Windows::Foundation::Collections;
  using namespace Windows::UI::Xaml;
  using namespace Windows::UI::Xaml::Controls;
  using namespace Windows::UI::Xaml::Controls::Primitives;
}

namespace react {
namespace uwp {

  class SnapPointManagingContentControl : public winrt::ContentControlT<SnapPointManagingContentControl, winrt::IScrollSnapPointsInfo>
  {
  private:
    SnapPointManagingContentControl();

  public:
    static winrt::com_ptr<SnapPointManagingContentControl> Create();
    template <typename D, typename... Args> friend auto winrt::make_self(Args&& ... args);

    // ScrollView Implementation
    void SnapToInterval(float interval);
    void SnapToOffsets(const winrt::IVectorView<float>& offsets);

    // IScrollSnapPointsInfo Implementation
    bool AreHorizontalSnapPointsRegular();
    bool AreVerticalSnapPointsRegular();

    winrt::event_token HorizontalSnapPointsChanged(winrt::EventHandler<winrt::IInspectable> const& value);
    void HorizontalSnapPointsChanged(winrt::event_token const& token);

    winrt::event_token VerticalSnapPointsChanged(winrt::EventHandler<winrt::IInspectable> const& value);
    void VerticalSnapPointsChanged(winrt::event_token const& token);

    winrt::IVectorView<float> GetIrregularSnapPoints(winrt::Orientation orientation, winrt::SnapPointsAlignment alignment);

    float GetRegularSnapPoints(winrt::Orientation orientation, winrt::SnapPointsAlignment alignment, float offset);

    // Helpers
    void SetHorizontal(bool horizontal);
    void SetHeightBounds(float startHeight, float endHeight);
    void SetWidthBounds(float startWidth, float endWidth);

  private:
    float m_interval{ 0 };
    winrt::IVectorView<float> m_offsets{};
    bool m_snapToStart{ true };
    bool m_snapToEnd{ true };
    winrt::event<winrt::EventHandler<winrt::IInspectable>> m_horizontalSnapPointsChangedEventSource;
    winrt::event<winrt::EventHandler<winrt::IInspectable>> m_verticalSnapPointsChangedEventSource;

    bool m_horizontal{ false };
    float m_startHeight{ 0 };
    float m_startWidth{ 0 };
    float m_endHeight{ INFINITY };
    float m_endWidth{ INFINITY };
  };
} }
