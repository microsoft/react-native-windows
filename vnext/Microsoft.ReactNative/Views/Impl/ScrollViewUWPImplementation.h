#pragma once

#include "SnapPointManagingContentControl.h"

#include <Utils/PropertyUtils.h>
#include <Utils/ValueUtils.h>

#include <IReactInstance.h>

#include <winrt/Windows.Foundation.h>

namespace winrt {
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace xaml;
using namespace xaml::Controls;
using namespace xaml::Controls::Primitives;
} // namespace winrt

namespace react::uwp {

class ScrollViewUWPImplementation {
 public:
  ScrollViewUWPImplementation(const winrt::ScrollViewer &scrollViewer);

  void SetHorizontal(bool isHorizontal);
  void SnapToInterval(float interval);
  void SnapToStart(bool snapToStart);
  void SnapToEnd(bool snapToEnd);
  void SnapToOffsets(const winrt::IVectorView<float> &offsets);
  void SnapPointAlignment(const winrt::SnapPointsAlignment alignment);
  void PagingEnabled(bool pagingEnabled);

  void UpdateScrollableSize() const;

  winrt::ScrollViewer ScrollViewer() const;
  winrt::com_ptr<SnapPointManagingContentControl> ScrollViewerSnapPointManager() const;

 private:
  winrt::weak_ref<winrt::ScrollViewer> m_scrollViewer{};
};

} // namespace react::uwp
