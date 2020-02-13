#include "pch.h"

#include "SnapPointManagingContentControl.h"

namespace react {
namespace uwp {

SnapPointManagingContentControl::SnapPointManagingContentControl() {
  IsTabStop(false);
}

/*static*/ winrt::com_ptr<SnapPointManagingContentControl> SnapPointManagingContentControl::Create() {
  return winrt::make_self<SnapPointManagingContentControl>();
}

void SnapPointManagingContentControl::SnapToInterval(float interval) {
  if (interval != m_interval) {
    m_interval = interval;
    NotifySnapPointsUpdated();
  }
}

void SnapPointManagingContentControl::SnapToStart(bool snapToStart) {
  m_snapToStart = snapToStart;
  NotifySnapPointsUpdated();
}

void SnapPointManagingContentControl::SnapToEnd(bool snapToEnd) {
  m_snapToEnd = snapToEnd;
  NotifySnapPointsUpdated();
}

void SnapPointManagingContentControl::PagingEnabled(bool pagingEnabled) {
  m_pagingEnabled = pagingEnabled;
  NotifySnapPointsUpdated();
}

void SnapPointManagingContentControl::SnapToOffsets(const winrt::IVectorView<float> &offsets) {
  m_offsets = offsets;
  if (m_offsets.Size() > 0) {
    m_interval = 0.0f;
    m_pagingEnabled = false;
  }
  NotifySnapPointsUpdated();
}

void SnapPointManagingContentControl::SetViewportSize(float scaledViewportWidth, float scaledviewportHeight) {
  m_viewportWidth = scaledViewportWidth;
  m_viewportHeight = scaledviewportHeight;
}

void SnapPointManagingContentControl::NotifySnapPointsUpdated() {
  m_horizontalSnapPointsChangedEventSource(*this, nullptr);
  m_verticalSnapPointsChangedEventSource(*this, nullptr);
}

bool SnapPointManagingContentControl::AreHorizontalSnapPointsRegular() {
  return (m_interval != 0.0f || m_pagingEnabled);
}

bool SnapPointManagingContentControl::AreVerticalSnapPointsRegular() {
  return (m_interval != 0.0f || m_pagingEnabled);
}

winrt::event_token SnapPointManagingContentControl::HorizontalSnapPointsChanged(
    winrt::EventHandler<winrt::IInspectable> const &value) {
  return m_horizontalSnapPointsChangedEventSource.add(value);
}
void SnapPointManagingContentControl::HorizontalSnapPointsChanged(winrt::event_token const &token) {
  m_horizontalSnapPointsChangedEventSource.remove(token);
}

winrt::event_token SnapPointManagingContentControl::VerticalSnapPointsChanged(
    winrt::EventHandler<winrt::IInspectable> const &value) {
  return m_verticalSnapPointsChangedEventSource.add(value);
}
void SnapPointManagingContentControl::VerticalSnapPointsChanged(winrt::event_token const &token) {
  m_verticalSnapPointsChangedEventSource.remove(token);
}

winrt::IVectorView<float> SnapPointManagingContentControl::GetIrregularSnapPoints(
    winrt::Orientation /*orientation*/,
    winrt::SnapPointsAlignment /*alignment*/) {
  const auto retVal = winrt::single_threaded_vector<float>();

  if (m_offsets && m_offsets.Size()) {
    if (m_snapToStart) {
      retVal.Append(m_horizontal ? m_startWidth : m_startHeight);
    }
    for (const auto offset : m_offsets) {
      retVal.Append(offset);
    }
    if (m_snapToEnd) {
      retVal.Append(m_horizontal ? m_endWidth : m_endHeight);
    }
  }

  return retVal.GetView();
}

float SnapPointManagingContentControl::GetRegularSnapPoints(
    winrt::Orientation /*orientation*/,
    winrt::SnapPointsAlignment /*alignment*/,
    float /*offset*/) {
  if (m_interval > 0.0f) {
    return m_interval;
  } else if (m_pagingEnabled) {
    return m_horizontal ? m_viewportWidth : m_viewportHeight;
  }

  return 0;
}

void SnapPointManagingContentControl::SetHorizontal(bool horizontal) {
  if (m_horizontal != horizontal) {
    m_horizontal = horizontal;

    if (m_endHeight != m_endWidth) {
      if (horizontal) {
        m_horizontalSnapPointsChangedEventSource(*this, nullptr);
      } else {
        m_verticalSnapPointsChangedEventSource(*this, nullptr);
      }
    }
  }
}

void SnapPointManagingContentControl::SetWidthBounds(float startWidth, float endWidth) {
  const auto update = [this, startWidth, endWidth]() {
    const auto endUpdated = [this, endWidth]() {
      if (m_endWidth != endWidth) {
        m_endWidth = endWidth;
        return true;
      }
      return false;
    }();

    const auto startUpdated = [this, startWidth]() {
      if (m_startWidth != startWidth) {
        m_startWidth = startWidth;
        return true;
      }
      return false;
    }();

    return endUpdated || startUpdated;
  }();

  if (update && m_horizontal && m_offsets && m_offsets.Size()) {
    m_horizontalSnapPointsChangedEventSource(*this, nullptr);
  }
}

void SnapPointManagingContentControl::SetHeightBounds(float startHeight, float endHeight) {
  const auto update = [this, startHeight, endHeight]() {
    const auto endUpdated = [this, endHeight]() {
      if (m_endHeight != endHeight) {
        m_endHeight = endHeight;
        return true;
      }
      return false;
    }();
    const auto startUpdated = [this, startHeight]() {
      if (m_startHeight != startHeight) {
        m_startHeight = startHeight;
        return true;
      }
      return false;
    }();

    return endUpdated || startUpdated;
  }();

  if (!m_horizontal && m_offsets && m_offsets.Size()) {
    m_verticalSnapPointsChangedEventSource(*this, nullptr);
  }
}

} // namespace uwp
} // namespace react
