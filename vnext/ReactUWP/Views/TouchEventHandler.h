// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once
#include <folly/dynamic.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Input.h>
#include <optional>
#include <set>

#include <IReactInstance.h>
#include "XamlView.h"

namespace winrt {
using namespace Windows::UI;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::Foundation;
using namespace Windows::UI::Xaml::Media;
} // namespace winrt

namespace react {
namespace uwp {

class TouchEventHandler {
 public:
  TouchEventHandler(const std::weak_ptr<IReactInstance> &reactInstance);
  virtual ~TouchEventHandler();

  void AddTouchHandlers(XamlView xamlView);
  void RemoveTouchHandlers();

 private:
  void OnPointerPressed(const winrt::IInspectable &, const winrt::PointerRoutedEventArgs &args);
  void OnPointerReleased(const winrt::IInspectable &, const winrt::PointerRoutedEventArgs &args);
  void OnPointerCanceled(const winrt::IInspectable &, const winrt::PointerRoutedEventArgs &args);
  void OnPointerCaptureLost(const winrt::IInspectable &, const winrt::PointerRoutedEventArgs &args);
  void OnPointerExited(const winrt::IInspectable &, const winrt::PointerRoutedEventArgs &args);
  void OnPointerMoved(const winrt::IInspectable &, const winrt::PointerRoutedEventArgs &args);
  winrt::event_revoker<winrt::IUIElement> m_pressedRevoker;
  winrt::event_revoker<winrt::IUIElement> m_releasedRevoker;
  winrt::event_revoker<winrt::IUIElement> m_canceledRevoker;
  winrt::event_revoker<winrt::IUIElement> m_captureLostRevoker;
  winrt::event_revoker<winrt::IUIElement> m_exitedRevoker;
  winrt::event_revoker<winrt::IUIElement> m_movedRevoker;

  struct ReactPointer {
    int64_t target = 0;
    int64_t identifier = 0;
    uint32_t pointerId = 0;
    uint64_t timestamp = 0;
    winrt::Point positionRoot = {0, 0};
    winrt::Point positionView = {0, 0};
    winrt::Windows::Devices::Input::PointerDeviceType deviceType;
    float pressure = 0;
    bool isLeftButton = false;
    bool isRightButton = false;
    bool isMiddleButton = false;
    bool isBarrelButton = false;
    bool isHorizontalScrollWheel = false;
    bool isEraser = false;
    bool shiftKey = false;
    bool ctrlKey = false;
    bool altKey = false;
  };
  size_t AddReactPointer(const winrt::PointerRoutedEventArgs &args, int64_t tag, winrt::UIElement sourceElement);
  ReactPointer
  CreateReactPointer(const winrt::PointerRoutedEventArgs &args, int64_t tag, winrt::UIElement sourceElement);
  void
  UpdateReactPointer(ReactPointer &pointer, const winrt::PointerRoutedEventArgs &args, winrt::UIElement sourceElement);
  void UpdatePointersInViews(
      std::shared_ptr<IReactInstance> instance,
      const winrt::PointerRoutedEventArgs &args,
      int64_t tag,
      winrt::UIElement sourceElement);

  enum class TouchEventType { Start = 0, End, Move, Cancel, PointerEntered, PointerExited, PointerMove };
  void OnPointerConcluded(TouchEventType eventType, const winrt::PointerRoutedEventArgs &args);
  void DispatchTouchEvent(TouchEventType eventType, size_t pointerIndex);
  bool DispatchBackEvent();
  const char *GetPointerDeviceTypeName(winrt::Windows::Devices::Input::PointerDeviceType deviceType) noexcept;
  const char *GetTouchEventTypeName(TouchEventType eventType) noexcept;

  std::optional<size_t> IndexOfPointerWithId(uint32_t pointerId);
  folly::dynamic GetPointerJson(const ReactPointer &pointer, int64_t target);

  struct TagSet {
    std::unordered_set<int64_t> tags;
    std::vector<int64_t> orderedTags;
  };

  std::vector<ReactPointer> m_pointers;
  std::unordered_map<uint32_t /*pointerId*/, TagSet /*tags*/> m_pointersInViews;
  int64_t m_touchId = 0;

  bool
  TagFromOriginalSource(const winrt::PointerRoutedEventArgs &args, int64_t *pTag, winrt::UIElement *pSourceElement);

  XamlView m_xamlView;
  std::weak_ptr<IReactInstance> m_wkReactInstance;
};

} // namespace uwp
} // namespace react
