// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once
#include <IReactInstance.h>
#include <JSValue.h>
#include <ReactPointerEventArgs.h>
#include <winrt/Windows.Devices.Input.h>
#include <optional>
#include <set>
#include "Utils/BatchingEventEmitter.h"
#include <winrt/Microsoft.ReactNative.h>

#ifdef USE_FABRIC
#include <react/renderer/components/view/Touch.h>
#endif

namespace winrt {
using namespace Windows::UI;
using namespace Windows::Foundation;
} // namespace winrt

namespace Microsoft::ReactNative {

class CompEventHandler {
 public:
  CompEventHandler(const Mso::React::IReactContext &context, bool fabric);
  CompEventHandler(
      const Mso::React::IReactContext &context,
      bool fabric,
      const winrt::Microsoft::ReactNative::CompRootView &compRootView);
  virtual ~CompEventHandler();


  int64_t SendMessage(facebook::react::SurfaceId surfaceId, uint32_t msg, uint64_t wParam, int64_t lParam) noexcept;
  void ScrollWheel(facebook::react::SurfaceId surfaceId, facebook::react::Point pt, uint32_t delta);
  void RemoveTouchHandlers();
  winrt::Microsoft::ReactNative::BatchingEventEmitter &BatchingEmitter() noexcept;

 private:

  void PointerPressed(
    facebook::react::SurfaceId surfaceId,
    uint32_t msg,
    uint64_t wParam,
    int64_t lParam);
  void PointerUp(
    facebook::react::SurfaceId surfaceId,
    uint32_t msg,
    uint64_t wParam,
    int64_t lParam);

  enum class TouchEventType { Start = 0, End, Move, Cancel, CaptureLost, PointerEntered, PointerExited, PointerMove };
  static bool IsEndishEventType(TouchEventType eventType) noexcept;
  void DispatchTouchEvent(TouchEventType eventType, int64_t pointerId);
  bool DispatchBackEvent();

  struct TagSet {
    std::unordered_set<int64_t> tags;
    std::vector<int64_t> orderedTags;
  };

  std::vector<facebook::react::Touch> m_touches;
  int64_t m_touchId = 0;

  winrt::Microsoft::ReactNative::CompRootView m_compRootView{nullptr};
  Mso::CntPtr<const Mso::React::IReactContext> m_context;
  bool m_fabric;
  std::shared_ptr<winrt::Microsoft::ReactNative::BatchingEventEmitter> m_batchingEventEmitter;
};

} // namespace Microsoft::ReactNative
