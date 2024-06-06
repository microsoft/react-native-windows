// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once
#include <Fabric/Composition/RootComponentView.h>
#include <Fabric/ReactTaggedView.h>
#include <IReactInstance.h>
#include <JSValue.h>
#include <ReactContext.h>
#include <react/renderer/components/view/PointerEvent.h>
#include <react/renderer/components/view/Touch.h>
#include <react/renderer/components/view/TouchEventEmitter.h>
#include <winrt/Microsoft.ReactNative.h>
#include <winrt/Windows.Devices.Input.h>
#include <optional>
#include <set>
#include "Utils/BatchingEventEmitter.h"

namespace winrt {
using namespace Windows::UI;
using namespace Windows::Foundation;
} // namespace winrt

namespace Microsoft::ReactNative {

struct FabricUIManager;
struct winrt::Microsoft::ReactNative::implementation::ComponentView;
typedef int PointerId;

class CompositionEventHandler {
 public:
  CompositionEventHandler(
      const winrt::Microsoft::ReactNative::ReactContext &context,
      const winrt::Microsoft::ReactNative::ReactNativeIsland &ReactNativeIsland);
  virtual ~CompositionEventHandler();

  int64_t SendMessage(HWND hwnd, uint32_t msg, uint64_t wParam, int64_t lParam) noexcept;
  void RemoveTouchHandlers();
  winrt::Windows::UI::Core::CoreVirtualKeyStates GetKeyState(winrt::Windows::System::VirtualKey key) noexcept;

  bool CapturePointer(
      const winrt::Microsoft::ReactNative::Composition::Input::Pointer &pointer,
      facebook::react::Tag tag) noexcept;
  bool ReleasePointerCapture(
      const winrt::Microsoft::ReactNative::Composition::Input::Pointer &pointer,
      facebook::react::Tag tag) noexcept;
  facebook::react::Tag PointerCapturingComponent() noexcept;

 private:
  void onPointerPressed(
      const winrt::Microsoft::ReactNative::Composition::Input::PointerPoint &pointerPoint,
      winrt::Windows::System::VirtualKeyModifiers keyModifiers) noexcept;
  void onPointerReleased(
      const winrt::Microsoft::ReactNative::Composition::Input::PointerPoint &pointerPoint,
      winrt::Windows::System::VirtualKeyModifiers keyModifiers) noexcept;
  void onPointerMoved(
      const winrt::Microsoft::ReactNative::Composition::Input::PointerPoint &pointerPoint,
      winrt::Windows::System::VirtualKeyModifiers keyModifiers) noexcept;
  void onPointerWheelChanged(
      const winrt::Microsoft::ReactNative::Composition::Input::PointerPoint &pointerPoint,
      winrt::Windows::System::VirtualKeyModifiers keyModifiers) noexcept;
  void onPointerCaptureLost(
      const winrt::Microsoft::ReactNative::Composition::Input::PointerPoint &pointerPoint,
      winrt::Windows::System::VirtualKeyModifiers keyModifiers) noexcept;
  void onKeyDown(
      const winrt::Microsoft::ReactNative::Composition::Input::KeyboardSource &source,
      const winrt::Microsoft::ReactNative::Composition::Input::KeyRoutedEventArgs &args) noexcept;
  void onKeyUp(
      const winrt::Microsoft::ReactNative::Composition::Input::KeyboardSource &source,
      const winrt::Microsoft::ReactNative::Composition::Input::KeyRoutedEventArgs &args) noexcept;
  void onCharacterReceived(
      const winrt::Microsoft::ReactNative::Composition::Input::KeyboardSource &source,
      const winrt::Microsoft::ReactNative::Composition::Input::CharacterReceivedRoutedEventArgs &args) noexcept;

  void getTargetPointerArgs(
      const std::shared_ptr<FabricUIManager> &fabricuiManager,
      const winrt::Microsoft::ReactNative::Composition::Input::PointerPoint &pointerPoint,
      facebook::react::Tag &tag,
      facebook::react::Point &ptScaled,
      facebook::react::Point &ptLocal) const noexcept;
  bool releasePointerCapture(PointerId pointerId, facebook::react::Tag tag) noexcept;

  facebook::react::SurfaceId SurfaceId() const noexcept;
  winrt::Microsoft::ReactNative::Composition::implementation::RootComponentView &RootComponentView() const noexcept;

  enum class UITouchType {
    Mouse,
    Pen,
    Touch,
  };
  enum class TouchEventType { Start = 0, End, Move, Cancel, CaptureLost, PointerEntered, PointerExited, PointerMove };
  void DispatchTouchEvent(
      TouchEventType eventType,
      PointerId pointerId,
      const winrt::Microsoft::ReactNative::Composition::Input::PointerPoint &pointerPoint,
      winrt::Windows::System::VirtualKeyModifiers keyModifiers);
  void HandleIncomingPointerEvent(
      facebook::react::PointerEvent &pe,
      const winrt::Microsoft::ReactNative::ComponentView &targetView,
      const winrt::Microsoft::ReactNative::Composition::Input::PointerPoint &pointerPoint,
      winrt::Windows::System::VirtualKeyModifiers keyModifiers,
      std::function<void(std::vector<winrt::Microsoft::ReactNative::ComponentView> &)> handler);

  struct ActiveTouch {
    facebook::react::Touch touch;
    facebook::react::SharedTouchEventEmitter eventEmitter;

    /*
     * The type of touch received.
     */
    UITouchType touchType;

    /*
     * The button mask of the touch
     */
    // UIEventButtonMask buttonMask;

    /*
     * The bit mask of modifier flags in the gesture represented by the receiver.
     */
    // UIKeyModifierFlags modifierFlags;

    /*
     * Indicates if the active touch represents the primary pointer of this pointer type.
     */
    bool isPrimary;

    /*
     * The button number that was pressed (if applicable) when the event was fired.
     */
    int button;

    /*
     * Informs the event system that when the touch is released it should be treated as the
     * pointer leaving the screen entirely.
     */
    bool shouldLeaveWhenReleased;

    /*
     * A component view on which the touch was begun.
     */
    //__strong UIView<RCTComponentViewProtocol> *componentView = nil;
  };

  static bool IsEndishEventType(TouchEventType eventType) noexcept;
  static const char *PointerTypeCStringFromUITouchType(UITouchType type) noexcept;
  static facebook::react::PointerEvent CreatePointerEventFromActiveTouch(
      const ActiveTouch &activeTouch,
      TouchEventType eventType) noexcept;
  static void
  UpdateActiveTouch(ActiveTouch &activeTouch, facebook::react::Point ptScaled, facebook::react::Point ptLocal) noexcept;

  std::map<PointerId, ActiveTouch> m_activeTouches; // iOS is map of touch event args to ActiveTouch..?
  PointerId m_touchId = 0;

  std::map<PointerId, std::vector<ReactTaggedView>> m_currentlyHoveredViewsPerPointer;
  winrt::weak_ref<winrt::Microsoft::ReactNative::ReactNativeIsland> m_wkRootView;
  winrt::Microsoft::ReactNative::ReactContext m_context;

  facebook::react::Tag m_pointerCapturingComponentTag{-1}; // Component that has captured input
  std::vector<PointerId> m_capturedPointers;

#ifdef USE_WINUI3
  winrt::event_token m_pointerPressedToken;
  winrt::event_token m_pointerReleasedToken;
  winrt::event_token m_pointerMovedToken;
  winrt::event_token m_pointerWheelChangedToken;
  winrt::event_token m_pointerCaptureLostToken;
  winrt::event_token m_keyDownToken;
  winrt::event_token m_keyUpToken;
  winrt::event_token m_characterReceivedToken;
#endif
};

} // namespace Microsoft::ReactNative
