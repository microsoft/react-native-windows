#include "pch.h"

#include "SnapPointManagingContentControl.h"

namespace react {
namespace uwp {

SnapPointManagingContentControl::SnapPointManagingContentControl()
{

}

void SnapPointManagingContentControl::SnapToInterval(float interval)
{
  if (interval != m_interval)
  {
    m_interval = interval;
    m_horizontalSnapPointsChangedEventSource(*this, nullptr);
    m_verticalSnapPointsChangedEventSource(*this, nullptr);
  }
}

void SnapPointManagingContentControl::SnapToOffsets(const winrt::IVectorView<float>& offsets)
{
  m_offsets = offsets;
  m_interval = 0;
  m_horizontalSnapPointsChangedEventSource(*this, nullptr);
  m_verticalSnapPointsChangedEventSource(*this, nullptr);
}

bool SnapPointManagingContentControl::AreHorizontalSnapPointsRegular()
{
  return m_interval;
}

bool SnapPointManagingContentControl::AreVerticalSnapPointsRegular()
{
  return m_interval;
}

winrt::event_token SnapPointManagingContentControl::HorizontalSnapPointsChanged(winrt::EventHandler<winrt::IInspectable> const& value)
{
  return m_horizontalSnapPointsChangedEventSource.add(value);
}
void SnapPointManagingContentControl::HorizontalSnapPointsChanged(winrt::event_token const& token)
{
  m_horizontalSnapPointsChangedEventSource.remove(token);
}

winrt::event_token SnapPointManagingContentControl::VerticalSnapPointsChanged(winrt::EventHandler<winrt::IInspectable> const& value)
{
  return m_verticalSnapPointsChangedEventSource.add(value);
}
void SnapPointManagingContentControl::VerticalSnapPointsChanged(winrt::event_token const& token)
{
  m_verticalSnapPointsChangedEventSource.remove(token);
}

winrt::IVectorView<float> SnapPointManagingContentControl::GetIrregularSnapPoints(winrt::Orientation orientation, winrt::SnapPointsAlignment alignment)
{
  auto retVal = winrt::single_threaded_vector<float>();

  if (m_offsets && m_offsets.Size())
  {
    if (m_snapToStart)
    {
      retVal.Append(m_horizontal ? m_startWidth : m_startHeight);
    }
    for (auto offset : m_offsets)
    {
      retVal.Append(offset);
    }
    if (m_snapToEnd)
    {
      retVal.Append(m_horizontal ? m_endWidth : m_endHeight);
    }
  }

  return retVal.GetView();
}

float SnapPointManagingContentControl::GetRegularSnapPoints(winrt::Orientation orientation, winrt::SnapPointsAlignment alignment, float offset)
{
  return m_interval;
}

void SnapPointManagingContentControl::SetHorizontal(bool horizontal)
{
  if (m_horizontal != horizontal)
  {
    m_horizontal = horizontal;

    if (m_endHeight != m_endWidth)
    {
      if (horizontal)
      {
        m_horizontalSnapPointsChangedEventSource(*this, nullptr);
      }
      else
      {
        m_verticalSnapPointsChangedEventSource(*this, nullptr);
      }
    }
  }
}

void SnapPointManagingContentControl::SetWidthBounds(float startWidth, float endWidth)
{
  auto const update = [this, startWidth, endWidth]()
  {
    auto const endUpdated = [this, endWidth]()
    {
      if (m_endWidth != endWidth)
      {
        m_endWidth = endWidth;
        return true;
      }
      return false;
    }();
    auto const startUpdated = [this, startWidth]()
    {
      if (m_startWidth != startWidth)
      {
        m_startWidth = startWidth;
        return true;
      }
      return false;
    }();

    return endUpdated || startUpdated;
  }();

  if (update && m_horizontal && m_offsets && m_offsets.Size())
  {
    m_horizontalSnapPointsChangedEventSource(*this, nullptr);
  }
}

void SnapPointManagingContentControl::SetHeightBounds(float startHeight, float endHeight)
{
  auto const update = [this, startHeight, endHeight]()
  {
    auto const endUpdated = [this, endHeight]()
    {
      if (m_endHeight != endHeight)
      {
        m_endHeight = endHeight;
        return true;
      }
      return false;
    }();
    auto const startUpdated = [this, startHeight]()
    {
      if (m_startHeight != startHeight)
      {
        m_startHeight = startHeight;
        return true;
      }
      return false;
    }();

    return endUpdated || startUpdated;
  }();

  if (!m_horizontal && m_offsets && m_offsets.Size())
  {
    m_verticalSnapPointsChangedEventSource(*this, nullptr);
  }
}

} }
