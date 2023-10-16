// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include <pch.h>

#include "CompositionEventHandler.h"

#include <Fabric/FabricUIManagerModule.h>
#include <IReactContext.h>
#include <React.h>
#include <Views/DevMenu.h>
#include <Views/ShadowNodeBase.h>
#include <windows.h>
#include <windowsx.h>
#include <winrt/Windows.UI.Input.h>
#include "Composition.Input.h"
#include "CompositionRootView.h"
#include "CompositionViewComponentView.h"
#include "RootComponentView.h"

#ifdef USE_WINUI3
#include <winrt/Microsoft.UI.Input.h>
#endif

namespace Microsoft::ReactNative {

const PointerId MOUSE_POINTER_ID = 1;

bool IsMousePointerEvent(const facebook::react::PointerEvent &pointerEvent) {
  return pointerEvent.pointerId == MOUSE_POINTER_ID;
}

bool IsViewListeningToEvent(IComponentView *view, facebook::react::ViewEvents::Offset eventType) {
  if (view) {
    auto const &viewProps = *std::static_pointer_cast<facebook::react::ViewProps const>(view->props());
    return viewProps.events[eventType];
  }
  return false;
}

bool IsViewListeningToEvent(IComponentView *view, facebook::react::WindowsViewEvents::Offset eventType) {
  if (view) {
    auto const &viewProps = *std::static_pointer_cast<facebook::react::ViewProps const>(view->props());
    return viewProps.windowsEvents[eventType];
  }
  return false;
}

bool IsAnyViewInPathListeningToEvent(
    std::vector<IComponentView *> &path,
    facebook::react::ViewEvents::Offset eventType) {
  for (auto view : path) {
    if (IsViewListeningToEvent(view, eventType)) {
      return true;
    }
  }
  return false;
}

IComponentView *FindClosestFabricManagedTouchableView(IComponentView *componentView) {
  while (componentView) {
    if (componentView->eventEmitter()) {
      return componentView;
    }
    componentView = componentView->parent();
  }
  return nullptr;
}

facebook::react::SharedEventEmitter EventEmitterForComponent(
    std::shared_ptr<FabricUIManager> &uimanager,
    facebook::react::Tag tag) noexcept {
  auto &registry = uimanager->GetViewRegistry();

  auto descriptor = registry.componentViewDescriptorWithTag(tag);
  auto view = std::static_pointer_cast<CompositionBaseComponentView>(descriptor.view);
  auto emitter = view->GetEventEmitter();
  if (emitter)
    return emitter;

  for (IComponentView *it = view->parent(); it; it = it->parent()) {
    auto emitter = static_cast<CompositionBaseComponentView *>(it)->GetEventEmitter();
    if (emitter)
      return emitter;
  }

  return nullptr;
}

struct CompositionKeyboardSource
    : winrt::implements<CompositionKeyboardSource, winrt::Microsoft::ReactNative::Composition::Input::KeyboardSource> {
  CompositionKeyboardSource(CompositionEventHandler *outer) : m_outer(outer) {}

  winrt::Windows::UI::Core::CoreVirtualKeyStates GetKeyState(winrt::Windows::System::VirtualKey key) noexcept {
    if (!m_outer)
      return winrt::Windows::UI::Core::CoreVirtualKeyStates::None;
    return m_outer->GetKeyState(key);
  }

  void Disconnect() noexcept {
    m_outer = nullptr;
  }

 private:
  CompositionEventHandler *m_outer{nullptr};
};

#ifdef USE_WINUI3
struct CompositionInputKeyboardSource : winrt::implements<
                                            CompositionInputKeyboardSource,
                                            winrt::Microsoft::ReactNative::Composition::Input::KeyboardSource> {
  CompositionInputKeyboardSource(winrt::Microsoft::UI::Input::InputKeyboardSource source) : m_source(source) {}

  winrt::Windows::UI::Core::CoreVirtualKeyStates GetKeyState(winrt::Windows::System::VirtualKey key) noexcept {
    if (!m_source)
      return winrt::Windows::UI::Core::CoreVirtualKeyStates::None;

    static_assert(
        static_cast<winrt::Windows::UI::Core::CoreVirtualKeyStates>(
            winrt::Microsoft::UI::Input::VirtualKeyStates::Down) ==
        winrt::Windows::UI::Core::CoreVirtualKeyStates::Down);
    static_assert(
        static_cast<winrt::Windows::UI::Core::CoreVirtualKeyStates>(
            winrt::Microsoft::UI::Input::VirtualKeyStates::Locked) ==
        winrt::Windows::UI::Core::CoreVirtualKeyStates::Locked);
    static_assert(
        static_cast<winrt::Windows::UI::Core::CoreVirtualKeyStates>(
            winrt::Microsoft::UI::Input::VirtualKeyStates::None) ==
        winrt::Windows::UI::Core::CoreVirtualKeyStates::None);
    return static_cast<winrt::Windows::UI::Core::CoreVirtualKeyStates>(m_source.GetKeyState(key));
  }

  void Disconnect() noexcept {
    m_source = nullptr;
  }

 private:
  winrt::Microsoft::UI::Input::InputKeyboardSource m_source{nullptr};
};
#endif

CompositionEventHandler::CompositionEventHandler(
    const winrt::Microsoft::ReactNative::ReactContext &context,
    const winrt::Microsoft::ReactNative::CompositionRootView &CompositionRootView)
    : m_context(context) {
  m_compRootView = CompositionRootView;

#ifdef USE_WINUI3
  if (auto island = m_compRootView.Island()) {
    auto pointerSource = winrt::Microsoft::UI::Input::InputPointerSource::GetForIsland(island);

    m_pointerPressedToken =
        pointerSource.PointerPressed([this](
                                         winrt::Microsoft::UI::Input::InputPointerSource const &,
                                         winrt::Microsoft::UI::Input::PointerEventArgs const &args) {
          if (SurfaceId() == -1)
            return;

          auto pp = winrt::make<winrt::Microsoft::ReactNative::Composition::Input::implementation::PointerPoint>(
              args.CurrentPoint());
          onPointerPressed(pp, args.KeyModifiers());
        });

    m_pointerReleasedToken =
        pointerSource.PointerReleased([this](
                                          winrt::Microsoft::UI::Input::InputPointerSource const &,
                                          winrt::Microsoft::UI::Input::PointerEventArgs const &args) {
          if (SurfaceId() == -1)
            return;

          auto pp = winrt::make<winrt::Microsoft::ReactNative::Composition::Input::implementation::PointerPoint>(
              args.CurrentPoint());
          onPointerReleased(pp, args.KeyModifiers());
        });

    m_pointerMovedToken = pointerSource.PointerMoved([this](
                                                         winrt::Microsoft::UI::Input::InputPointerSource const &,
                                                         winrt::Microsoft::UI::Input::PointerEventArgs const &args) {
      auto pp = winrt::make<winrt::Microsoft::ReactNative::Composition::Input::implementation::PointerPoint>(
          args.CurrentPoint());
      onPointerMoved(pp, args.KeyModifiers());
    });

    m_pointerWheelChangedToken =
        pointerSource.PointerWheelChanged([this](
                                              winrt::Microsoft::UI::Input::InputPointerSource const &,
                                              winrt::Microsoft::UI::Input::PointerEventArgs const &args) {
          if (SurfaceId() == -1)
            return;

          auto pp = winrt::make<winrt::Microsoft::ReactNative::Composition::Input::implementation::PointerPoint>(
              args.CurrentPoint());
          onPointerWheelChanged(pp, args.KeyModifiers());
        });

    auto keyboardSource = winrt::Microsoft::UI::Input::InputKeyboardSource::GetForIsland(island);

    m_keyDownToken = keyboardSource.KeyDown([this](
                                                winrt::Microsoft::UI::Input::InputKeyboardSource const &source,
                                                winrt::Microsoft::UI::Input::KeyEventArgs const &args) {
      if (SurfaceId() == -1)
        return;

      auto focusedComponent = RootComponentView().GetFocusedComponent();
      auto keyArgs = winrt::make<winrt::Microsoft::ReactNative::Composition::Input::implementation::KeyRoutedEventArgs>(
          focusedComponent
              ? focusedComponent->tag()
              : static_cast<facebook::react::Tag>(
                    winrt::get_self<winrt::Microsoft::ReactNative::implementation::CompositionRootView>(m_compRootView)
                        ->GetTag()),
          args);
      auto keyboardSource = winrt::make<CompositionInputKeyboardSource>(source);
      onKeyDown(keyboardSource, keyArgs);
      winrt::get_self<CompositionInputKeyboardSource>(keyboardSource)->Disconnect();
    });

    m_keyUpToken = keyboardSource.KeyUp([this](
                                            winrt::Microsoft::UI::Input::InputKeyboardSource const &source,
                                            winrt::Microsoft::UI::Input::KeyEventArgs const &args) {
      if (SurfaceId() == -1)
        return;

      auto focusedComponent = RootComponentView().GetFocusedComponent();
      auto keyArgs = winrt::make<winrt::Microsoft::ReactNative::Composition::Input::implementation::KeyRoutedEventArgs>(
          focusedComponent
              ? focusedComponent->tag()
              : static_cast<facebook::react::Tag>(
                    winrt::get_self<winrt::Microsoft::ReactNative::implementation::CompositionRootView>(m_compRootView)
                        ->GetTag()),
          args);
      auto keyboardSource = winrt::make<CompositionInputKeyboardSource>(source);
      onKeyUp(keyboardSource, keyArgs);
      winrt::get_self<CompositionInputKeyboardSource>(keyboardSource)->Disconnect();
    });
  }
#endif
};

CompositionEventHandler::~CompositionEventHandler() {
#ifdef USE_WINUI3
  if (auto island = m_compRootView.Island()) {
    auto pointerSource = winrt::Microsoft::UI::Input::InputPointerSource::GetForIsland(island);
    pointerSource.PointerPressed(m_pointerPressedToken);
    pointerSource.PointerReleased(m_pointerReleasedToken);
    pointerSource.PointerMoved(m_pointerMovedToken);
    pointerSource.PointerWheelChanged(m_pointerWheelChangedToken);
    auto keyboardSource = winrt::Microsoft::UI::Input::InputKeyboardSource::GetForIsland(island);
    keyboardSource.KeyDown(m_keyDownToken);
    keyboardSource.KeyUp(m_keyUpToken);
  }
#endif
}

facebook::react::SurfaceId CompositionEventHandler::SurfaceId() noexcept {
  return static_cast<facebook::react::SurfaceId>(
      winrt::get_self<winrt::Microsoft::ReactNative::implementation::CompositionRootView>(m_compRootView)->GetTag());
}

::Microsoft::ReactNative::RootComponentView &CompositionEventHandler::RootComponentView() noexcept {
  auto rootComponentViewDescriptor = (::Microsoft::ReactNative::FabricUIManager::FromProperties(m_context.Properties()))
                                         ->GetViewRegistry()
                                         .componentViewDescriptorWithTag(SurfaceId());
  return static_cast<::Microsoft::ReactNative::RootComponentView &>(*(rootComponentViewDescriptor.view));
}

void CompositionEventHandler::onPointerWheelChanged(
    const winrt::Microsoft::ReactNative::Composition::Input::PointerPoint &pointerPoint,
    winrt::Windows::System::VirtualKeyModifiers keyModifiers) noexcept {
  if (std::shared_ptr<FabricUIManager> fabricuiManager =
          ::Microsoft::ReactNative::FabricUIManager::FromProperties(m_context.Properties())) {
    auto position = pointerPoint.Position();

    facebook::react::Point ptLocal;
    facebook::react::Point ptScaled = {static_cast<float>(position.X), static_cast<float>(position.Y)};

    auto tag = RootComponentView().hitTest(ptScaled, ptLocal);

    if (tag == -1)
      return;

    auto targetComponentView = fabricuiManager->GetViewRegistry().componentViewDescriptorWithTag(tag).view;
    auto args = winrt::make<winrt::Microsoft::ReactNative::Composition::Input::implementation::PointerRoutedEventArgs>(
        tag, pointerPoint, keyModifiers);

    std::static_pointer_cast<CompositionBaseComponentView>(targetComponentView)->onPointerWheelChanged(args);
  }
}

winrt::Windows::UI::Core::CoreVirtualKeyStates CompositionEventHandler::GetKeyState(
    winrt::Windows::System::VirtualKey key) noexcept {
  winrt::Windows::UI::Core::CoreVirtualKeyStates coreKeyState = winrt::Windows::UI::Core::CoreVirtualKeyStates::None;
  SHORT keyState = ::GetKeyState(static_cast<int>(key));
  if (keyState & 0x01) {
    coreKeyState = winrt::Windows::UI::Core::CoreVirtualKeyStates::Locked;
  }
  if (keyState & 0x8000) {
    coreKeyState = static_cast<winrt::Windows::UI::Core::CoreVirtualKeyStates>(
        static_cast<int>(coreKeyState) | static_cast<int>(winrt::Windows::UI::Core::CoreVirtualKeyStates::Down));
  }

  return coreKeyState;
}

winrt::Windows::System::VirtualKeyModifiers GetKeyModifiers(uint64_t wParam) {
  auto keyModifiers = winrt::Windows::System::VirtualKeyModifiers::None;
  if ((wParam & MK_CONTROL) == MK_CONTROL) {
    keyModifiers |= winrt::Windows::System::VirtualKeyModifiers::Control;
  }
  if ((wParam & MK_SHIFT) == MK_SHIFT) {
    keyModifiers |= winrt::Windows::System::VirtualKeyModifiers::Shift;
  }
  if (::GetKeyState(VK_MENU) < 0) {
    keyModifiers |= winrt::Windows::System::VirtualKeyModifiers::Menu;
  }
  if (::GetKeyState(VK_LWIN) < 0 || ::GetKeyState(VK_RWIN) < 0) {
    keyModifiers |= winrt::Windows::System::VirtualKeyModifiers::Windows;
  }
  return keyModifiers;
}

int64_t CompositionEventHandler::SendMessage(uint32_t msg, uint64_t wParam, int64_t lParam) noexcept {
  switch (msg) {
    case WM_LBUTTONDOWN: {
      auto pp = winrt::make<winrt::Microsoft::ReactNative::Composition::Input::implementation::PointerPoint>(
          msg, wParam, lParam, m_compRootView.ScaleFactor());
      onPointerPressed(pp, GetKeyModifiers(wParam));
      return 0;
    }
    case WM_POINTERDOWN: {
      auto pp = winrt::make<winrt::Microsoft::ReactNative::Composition::Input::implementation::PointerPoint>(
          msg, wParam, lParam, m_compRootView.ScaleFactor());
      onPointerPressed(pp, GetKeyModifiers(wParam));
      return 0;
    }
    case WM_LBUTTONUP: {
      auto pp = winrt::make<winrt::Microsoft::ReactNative::Composition::Input::implementation::PointerPoint>(
          msg, wParam, lParam, m_compRootView.ScaleFactor());
      onPointerReleased(pp, GetKeyModifiers(wParam));
      return 0;
    }
    case WM_POINTERUP: {
      auto pp = winrt::make<winrt::Microsoft::ReactNative::Composition::Input::implementation::PointerPoint>(
          msg, wParam, lParam, m_compRootView.ScaleFactor());
      onPointerReleased(pp, GetKeyModifiers(wParam));
      return 0;
    }
    case WM_MOUSEMOVE: {
      auto pp = winrt::make<winrt::Microsoft::ReactNative::Composition::Input::implementation::PointerPoint>(
          msg, wParam, lParam, m_compRootView.ScaleFactor());
      onPointerMoved(pp, GetKeyModifiers(wParam));
      return 0;
    }
    case WM_MOUSEWHEEL: {
      auto pp = winrt::make<winrt::Microsoft::ReactNative::Composition::Input::implementation::PointerPoint>(
          msg, wParam, lParam, m_compRootView.ScaleFactor());
      onPointerWheelChanged(pp, GetKeyModifiers(wParam));
    }
    case WM_CHAR:
    case WM_SYSCHAR: {
      // TODO full bubbling of events
      if (auto focusedComponent = RootComponentView().GetFocusedComponent()) {
        auto result = focusedComponent->sendMessage(msg, wParam, lParam);
        if (result)
          return result;
      }
      break;
    }
    case WM_KEYDOWN:
    case WM_KEYUP:
    case WM_SYSKEYDOWN:
    case WM_SYSKEYUP: {
      auto focusedComponent = RootComponentView().GetFocusedComponent();
      auto args = winrt::make<winrt::Microsoft::ReactNative::Composition::Input::implementation::KeyRoutedEventArgs>(
          focusedComponent
              ? focusedComponent->tag()
              : static_cast<facebook::react::Tag>(
                    winrt::get_self<winrt::Microsoft::ReactNative::implementation::CompositionRootView>(m_compRootView)
                        ->GetTag()),
          msg,
          wParam,
          lParam);
      auto keyboardSource = winrt::make<CompositionKeyboardSource>(this);
      if (msg == WM_KEYDOWN || msg == WM_SYSKEYDOWN) {
        onKeyDown(keyboardSource, args);
      } else {
        onKeyUp(keyboardSource, args);
      }
      winrt::get_self<CompositionKeyboardSource>(keyboardSource)->Disconnect();
    }
  }

  return 0;
}

void CompositionEventHandler::onKeyDown(
    const winrt::Microsoft::ReactNative::Composition::Input::KeyboardSource &source,
    const winrt::Microsoft::ReactNative::Composition::Input::KeyRoutedEventArgs &args) noexcept {
  if (auto focusedComponent = RootComponentView().GetFocusedComponent()) {
    focusedComponent->onKeyDown(source, args);

    if (args.Handled())
      return;
  }

  bool fShift = source.GetKeyState(winrt::Windows::System::VirtualKey::Shift) ==
      winrt::Windows::UI::Core::CoreVirtualKeyStates::Down;
  bool fCtrl =
      GetKeyState(winrt::Windows::System::VirtualKey::Control) == winrt::Windows::UI::Core::CoreVirtualKeyStates::Down;

  if (fShift && fCtrl && args.Key() == static_cast<winrt::Windows::System::VirtualKey>(VkKeyScanA('d')) &&
      Mso::React::ReactOptions::UseDeveloperSupport(m_context.Properties().Handle())) {
    auto contextSelf = winrt::get_self<React::implementation::ReactContext>(m_context.Handle());
    Microsoft::ReactNative::DevMenuManager::Show(Mso::CntPtr<Mso::React::IReactContext>(&contextSelf->GetInner()));
    args.Handled(true);
    return;
  }

  if (!fCtrl && args.Key() == winrt::Windows::System::VirtualKey::Tab) {
    if (RootComponentView().TryMoveFocus(!fShift)) {
      args.Handled(true);
    }

    // TODO notify rootview that host should move focus
  }
}

void CompositionEventHandler::onKeyUp(
    const winrt::Microsoft::ReactNative::Composition::Input::KeyboardSource &source,
    const winrt::Microsoft::ReactNative::Composition::Input::KeyRoutedEventArgs &args) noexcept {
  if (auto focusedComponent = RootComponentView().GetFocusedComponent()) {
    focusedComponent->onKeyUp(source, args);

    if (args.Handled())
      return;
  }
}

std::vector<IComponentView *> GetTouchableViewsInPathToRoot(IComponentView *view) {
  std::vector<IComponentView *> results;
  while (view) {
    if (view->eventEmitter()) {
      results.push_back(view);
    }
    view = view->parent();
  }
  return results;
}

/**
 * Private method which is used for tracking the location of pointer events to manage the entering/leaving events.
 * The primary idea is that a pointer's presence & movement is dictated by a variety of underlying events such as down,
 * move, and up — and they should all be treated the same when it comes to tracking the entering & leaving of pointers
 * to views. This method accomplishes that by receiving the pointer event, the target view (can be null in cases when
 * the event indicates that the pointer has left the screen entirely), and a block/callback where the underlying event
 * should be fired.
 */
void CompositionEventHandler::HandleIncomingPointerEvent(
    facebook::react::PointerEvent &event,
    IComponentView *targetView,
    const winrt::Microsoft::ReactNative::Composition::Input::PointerPoint &pointerPoint,
    winrt::Windows::System::VirtualKeyModifiers keyModifiers,
    std::function<void(std::vector<IComponentView *> &)> handler) {
  int pointerId = event.pointerId;
  // CGPoint clientLocation = CGPointMake(event.clientPoint.x, event.clientPoint.y);

  std::vector<IComponentView *> currentlyHoveredViews;
  auto itCurrentlyHoveredViews = m_currentlyHoveredViewsPerPointer.find(pointerId);
  if (itCurrentlyHoveredViews != m_currentlyHoveredViewsPerPointer.end()) {
    for (auto &taggedView : itCurrentlyHoveredViews->second) {
      if (auto view = taggedView.view()) {
        currentlyHoveredViews.push_back(view.get());
      }
    }
  }

  // RCTReactTaggedView *targetTaggedView = [RCTReactTaggedView wrap:targetView];
  auto prevTargetView = currentlyHoveredViews.empty() ? nullptr : currentlyHoveredViews[0];
  auto previousTargetTag = prevTargetView ? prevTargetView->tag() : -1;

  auto eventPathViews = GetTouchableViewsInPathToRoot(targetView);

  // Over
  if (targetView != nullptr && previousTargetTag != targetView->tag()) {
    bool shouldEmitOverEvent =
        IsAnyViewInPathListeningToEvent(eventPathViews, facebook::react::ViewEvents::Offset::PointerOver);
    const auto eventEmitter = targetView->eventEmitterAtPoint(event.offsetPoint);
    if (shouldEmitOverEvent && eventEmitter != nullptr) {
      eventEmitter->onPointerOver(event);
    }
  }

  // Entering

  // We only want to emit events to JS if there is a view that is currently listening to said event
  // so we only send those event to the JS side if the element which has been entered is itself listening,
  // or if one of its parents is listening in case those listeners care about the capturing phase. Adding the ability
  // for native to distinguish between capturing listeners and not could be an optimization to further reduce the
  // number of events we send to JS
  bool hasParentEnterListener = false;
  bool emittedNativeEnteredEvent = false;

  for (auto itComponentView = eventPathViews.rbegin(); itComponentView != eventPathViews.rend();
       itComponentView++) { //  for (RCTReactTaggedView *taggedView in [eventPathViews reverseObjectEnumerator]) {

    auto componentView = *itComponentView;
    bool shouldEmitEvent = componentView != nullptr &&
        (hasParentEnterListener ||
         IsViewListeningToEvent(componentView, facebook::react::ViewEvents::Offset::PointerEnter) ||
         IsViewListeningToEvent(componentView, facebook::react::WindowsViewEvents::Offset::MouseEnter));

    if ((shouldEmitEvent || !emittedNativeEnteredEvent) &&
        std::find(currentlyHoveredViews.begin(), currentlyHoveredViews.end(), componentView) ==
            currentlyHoveredViews.end()) {
      if (shouldEmitEvent) {
        const auto eventEmitter = componentView->eventEmitter();
        if (eventEmitter) {
          eventEmitter->onPointerEnter(event);
          if (IsMousePointerEvent(event)) {
            eventEmitter->onMouseEnter(event);
          }
        }
      }
      if (!emittedNativeEnteredEvent) {
        emittedNativeEnteredEvent = true;
        auto args =
            winrt::make<winrt::Microsoft::ReactNative::Composition::Input::implementation::PointerRoutedEventArgs>(
                componentView->tag(), pointerPoint, keyModifiers);
        static_cast<CompositionBaseComponentView *>(componentView)->onPointerEntered(args);
      }
    }

    if (shouldEmitEvent && !hasParentEnterListener) {
      hasParentEnterListener = true;
    }
  }

  // Call the underlaying pointer handler
  handler(eventPathViews);

  // Out
  if (previousTargetTag != -1 && previousTargetTag != (targetView ? targetView->tag() : -1)) {
    bool shouldEmitOutEvent =
        IsAnyViewInPathListeningToEvent(currentlyHoveredViews, facebook::react::ViewEvents::Offset::PointerOut);
    const auto eventEmitter = prevTargetView->eventEmitter();
    if (shouldEmitOutEvent && eventEmitter != nullptr) {
      eventEmitter->onPointerOut(event);
    }
  }

  // Leaving

  // pointerleave events need to be emitted from the deepest target to the root but
  // we also need to efficiently keep track of if a view has a parent which is listening to the leave events,
  // so we first iterate from the root to the target, collecting the views which need events fired for, of which
  // we reverse iterate (now from target to root), actually emitting the events.
  std::vector<IComponentView *> viewsToEmitLeaveEventsTo; // NSMutableOrderedSet<UIView *> *viewsToEmitLeaveEventsTo =
                                                          // [NSMutableOrderedSet orderedSet];

  IComponentView *viewToEmitNativeExitedEvent = nullptr;

  bool hasParentLeaveListener = false;
  for (auto itComponentView = currentlyHoveredViews.rbegin(); itComponentView != currentlyHoveredViews.rend();
       itComponentView++) { //  for (RCTReactTaggedView *taggedView in [currentlyHoveredViews
                            //  reverseObjectEnumerator])
                            //  {
    auto componentView = *itComponentView;

    bool shouldEmitEvent = componentView != nullptr &&
        (hasParentLeaveListener ||
         IsViewListeningToEvent(componentView, facebook::react::ViewEvents::Offset::PointerLeave) ||
         IsViewListeningToEvent(componentView, facebook::react::WindowsViewEvents::Offset::MouseLeave));

    if ((shouldEmitEvent || !viewToEmitNativeExitedEvent) &&
        std::find(eventPathViews.begin(), eventPathViews.end(), componentView) == eventPathViews.end()) {
      if (shouldEmitEvent) {
        viewsToEmitLeaveEventsTo.push_back(componentView);
      }
      if (!viewToEmitNativeExitedEvent) {
        viewToEmitNativeExitedEvent = componentView;
      }
    }

    if (shouldEmitEvent && !hasParentLeaveListener) {
      hasParentLeaveListener = true;
    }
  }

  if (viewToEmitNativeExitedEvent) {
    auto args = winrt::make<winrt::Microsoft::ReactNative::Composition::Input::implementation::PointerRoutedEventArgs>(
        viewToEmitNativeExitedEvent->tag(), pointerPoint, keyModifiers);
    static_cast<CompositionBaseComponentView *>(viewToEmitNativeExitedEvent)->onPointerExited(args);
  }

  for (auto itComponentView = viewsToEmitLeaveEventsTo.rbegin(); itComponentView != viewsToEmitLeaveEventsTo.rend();
       itComponentView++) { //  for (UIView *componentView in [viewsToEmitLeaveEventsTo reverseObjectEnumerator]) {
    auto componentView = *itComponentView;

    const auto eventEmitter = componentView->eventEmitter();
    if (eventEmitter) {
      eventEmitter->onPointerLeave(event);
      if (IsMousePointerEvent(event)) {
        eventEmitter->onMouseLeave(event);
      }
    }
  }

  std::vector<ReactTaggedView> hoveredViews;
  std::shared_ptr<FabricUIManager> fabricuiManager =
      ::Microsoft::ReactNative::FabricUIManager::FromProperties(m_context.Properties());
  auto &viewRegistry = fabricuiManager->GetViewRegistry();
  for (auto &view : eventPathViews) {
    auto componentViewDescriptor = viewRegistry.componentViewDescriptorWithTag(view->tag());
    hoveredViews.emplace_back(ReactTaggedView(componentViewDescriptor.view));
  }
  m_currentlyHoveredViewsPerPointer[pointerId] = std::move(hoveredViews);
}

void CompositionEventHandler::UpdateActiveTouch(
    ActiveTouch &activeTouch,
    facebook::react::Point ptScaled,
    facebook::react::Point ptLocal) noexcept {
  // TODO set all the activeTouch.touch properties
  // activeTouch.touch.isLeftButton = true;
  // activeTouch.touch.isRightButton = false;
  // activeTouch.touch.isMiddleButton = false;
  // activeTouch.touch.isHorizontalScrollWheel = false;
  // activeTouch.touch.isEraser = false;
  activeTouch.touch.pagePoint.x = ptScaled.x;
  activeTouch.touch.pagePoint.y = ptScaled.y;
  activeTouch.touch.screenPoint.x = ptLocal.x;
  activeTouch.touch.screenPoint.y = ptLocal.y;
  activeTouch.touch.offsetPoint.x = ptLocal.x;
  activeTouch.touch.offsetPoint.y = ptLocal.y;
  activeTouch.touch.timestamp = static_cast<facebook::react::Float>(
      std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch())
          .count());
  activeTouch.touch.force = 0;
  // activeTouch.touch.isBarrelButton = false;
  // activeTouch.touch.shiftKey = false;
  // activeTouch.touch.ctrlKey = false;
  // activeTouch.touch.altKey = false;

  // activeTouch.touch.isPrimary = true;
}

facebook::react::PointerEvent CreatePointerEventFromIncompleteHoverData(
    facebook::react::Point ptScaled,
    facebook::react::Point ptLocal) {
  facebook::react::PointerEvent pointerEvent = {};
  pointerEvent.pointerId = MOUSE_POINTER_ID;
  pointerEvent.pressure = 0.0;
  pointerEvent.pointerType = "mouse";

  pointerEvent.clientPoint = ptScaled;
  pointerEvent.screenPoint = ptScaled;
  pointerEvent.offsetPoint = ptLocal;
  pointerEvent.width = 1.0;
  pointerEvent.height = 1.0;
  pointerEvent.tiltX = 0;
  pointerEvent.tiltY = 0;
  pointerEvent.detail = 0;
  // pointerEvent.button = -1;
  pointerEvent.buttons = 0;
  // UpdatePointerEventModifierFlags(pointerEvent, modifierFlags);
  pointerEvent.tangentialPressure = 0.0;
  pointerEvent.twist = 0;
  // pointerEvent.isPrimary = true;

  return pointerEvent;
}

void CompositionEventHandler::onPointerMoved(
    const winrt::Microsoft::ReactNative::Composition::Input::PointerPoint &pointerPoint,
    winrt::Windows::System::VirtualKeyModifiers keyModifiers) noexcept {
  if (SurfaceId() == -1)
    return;

  int pointerId = pointerPoint.PointerId();

  auto position = pointerPoint.Position();

  if (std::shared_ptr<FabricUIManager> fabricuiManager =
          ::Microsoft::ReactNative::FabricUIManager::FromProperties(m_context.Properties())) {
    facebook::react::Point ptLocal;

    facebook::react::Point ptScaled = {position.X, position.Y};
    auto tag = RootComponentView().hitTest(ptScaled, ptLocal);

    if (tag == -1)
      return;

    auto args = winrt::make<winrt::Microsoft::ReactNative::Composition::Input::implementation::PointerRoutedEventArgs>(
        tag, pointerPoint, keyModifiers);
    IComponentView *targetComponentView =
        fabricuiManager->GetViewRegistry().componentViewDescriptorWithTag(tag).view.get();

    static_cast<CompositionBaseComponentView *>(targetComponentView)->onPointerMoved(args);

    auto targetView = FindClosestFabricManagedTouchableView(
        fabricuiManager->GetViewRegistry().componentViewDescriptorWithTag(tag).view.get());

    facebook::react::PointerEvent pointerEvent = CreatePointerEventFromIncompleteHoverData(ptScaled, ptLocal);

    auto handler = [targetView, &pointerEvent](std::vector<IComponentView *> &eventPathViews) {
      const auto eventEmitter = targetView ? targetView->eventEmitterAtPoint(pointerEvent.offsetPoint) : nullptr;
      bool hasMoveEventListeners =
          IsAnyViewInPathListeningToEvent(eventPathViews, facebook::react::ViewEvents::Offset::PointerMove) ||
          IsAnyViewInPathListeningToEvent(eventPathViews, facebook::react::ViewEvents::Offset::PointerMoveCapture);
      if (eventEmitter != nullptr && hasMoveEventListeners) {
        eventEmitter->onPointerMove(pointerEvent);
      }
    };

    HandleIncomingPointerEvent(pointerEvent, targetView, pointerPoint, keyModifiers, handler);
  }
}

void CompositionEventHandler::onPointerPressed(
    const winrt::Microsoft::ReactNative::Composition::Input::PointerPoint &pointerPoint,
    winrt::Windows::System::VirtualKeyModifiers keyModifiers) noexcept {
  PointerId pointerId = pointerPoint.PointerId();

  auto staleTouch = std::find_if(m_activeTouches.begin(), m_activeTouches.end(), [pointerId](const auto &pair) {
    return pair.second.touch.identifier == pointerId;
  });

  if (staleTouch != m_activeTouches.end()) {
    // A pointer with this ID already exists - Should we fire a button cancel or something?
    // assert(false);
    return;
  }

  auto position = pointerPoint.Position();
  const auto eventType = TouchEventType::Start;

  if (std::shared_ptr<FabricUIManager> fabricuiManager =
          ::Microsoft::ReactNative::FabricUIManager::FromProperties(m_context.Properties())) {
    facebook::react::Point ptLocal;

    facebook::react::Point ptScaled = {position.X, position.Y};
    auto tag = RootComponentView().hitTest(ptScaled, ptLocal);

    if (tag == -1)
      return;

    IComponentView *targetComponentView =
        fabricuiManager->GetViewRegistry().componentViewDescriptorWithTag(tag).view.get();
    auto args = winrt::make<winrt::Microsoft::ReactNative::Composition::Input::implementation::PointerRoutedEventArgs>(
        tag, pointerPoint, keyModifiers);
    static_cast<CompositionBaseComponentView *>(targetComponentView)->onPointerPressed(args);

    ActiveTouch activeTouch{0};
    activeTouch.touchType = UITouchType::Mouse;

    auto componentView = targetComponentView;
    while (componentView) {
      if (auto eventEmitter = componentView->eventEmitterAtPoint(ptLocal)) {
        activeTouch.eventEmitter = eventEmitter;
        activeTouch.touch.target = componentView->tag();
        // activeTouch.componentView = componentView;
        break;
      }
      componentView = componentView->parent();
    }

    UpdateActiveTouch(activeTouch, ptScaled, ptLocal);

    // activeTouch.touch.isPrimary = true;
    activeTouch.touch.identifier = pointerId;

    // If the pointer has not been marked as hovering over views before the touch started, we register
    // that the activeTouch should not maintain its hovered state once the pointer has been lifted.
    auto currentlyHoveredTags = m_currentlyHoveredViewsPerPointer.find(activeTouch.touch.identifier);
    if (currentlyHoveredTags == m_currentlyHoveredViewsPerPointer.end() || currentlyHoveredTags->second.empty()) {
      activeTouch.shouldLeaveWhenReleased = true;
    }

    m_activeTouches.emplace(pointerId, activeTouch);

    DispatchTouchEvent(eventType, pointerId, pointerPoint, keyModifiers);
  }
}

void CompositionEventHandler::onPointerReleased(
    const winrt::Microsoft::ReactNative::Composition::Input::PointerPoint &pointerPoint,
    winrt::Windows::System::VirtualKeyModifiers keyModifiers) noexcept {
  int pointerId = pointerPoint.PointerId();

  auto activeTouch = std::find_if(m_activeTouches.begin(), m_activeTouches.end(), [pointerId](const auto &pair) {
    return pair.second.touch.identifier == pointerId;
  });

  if (activeTouch == m_activeTouches.end()) {
    return;
  }

  auto position = pointerPoint.Position();

  if (std::shared_ptr<FabricUIManager> fabricuiManager =
          ::Microsoft::ReactNative::FabricUIManager::FromProperties(m_context.Properties())) {
    facebook::react::Point ptLocal;

    facebook::react::Point ptScaled = {position.X, position.Y};
    auto tag = RootComponentView().hitTest(ptScaled, ptLocal);

    if (tag == -1)
      return;

    auto targetComponentView = fabricuiManager->GetViewRegistry().componentViewDescriptorWithTag(tag).view;
    auto args = winrt::make<winrt::Microsoft::ReactNative::Composition::Input::implementation::PointerRoutedEventArgs>(
        tag, pointerPoint, keyModifiers);

    std::static_pointer_cast<CompositionBaseComponentView>(targetComponentView)->onPointerReleased(args);

    UpdateActiveTouch(activeTouch->second, ptScaled, ptLocal);
    DispatchTouchEvent(TouchEventType::End, pointerId, pointerPoint, keyModifiers);
    m_activeTouches.erase(pointerId);
  }
}

// defines button payload, follows https://developer.mozilla.org/docs/Web/API/MouseEvent/button
enum class MouseEventButtonKind { None = -1, Main = 0, Auxiliary = 1, Secondary = 2, Eraser = 5 };

bool CompositionEventHandler::IsEndishEventType(TouchEventType eventType) noexcept {
  switch (eventType) {
    case TouchEventType::End:
    case TouchEventType::Cancel:
    case TouchEventType::CaptureLost:
      return true;
    default:
      return false;
  }
}

const char *CompositionEventHandler::PointerTypeCStringFromUITouchType(UITouchType type) noexcept {
  switch (type) {
    case UITouchType::Mouse:
      return "mouse";
    case UITouchType::Pen:
      return "pen";
    case UITouchType::Touch:
      return "touch";
    default:
      assert(false);
      return "";
  }
}

facebook::react::PointerEvent CompositionEventHandler::CreatePointerEventFromActiveTouch(
    const ActiveTouch &activeTouch,
    TouchEventType eventType) noexcept {
  facebook::react::Touch touch = activeTouch.touch;

  facebook::react::PointerEvent event = {};
  event.pointerId = touch.identifier;
  event.pointerType = PointerTypeCStringFromUITouchType(activeTouch.touchType);
  event.clientPoint = touch.pagePoint;
  event.screenPoint = touch.screenPoint;
  event.offsetPoint = touch.offsetPoint;

  event.pressure = touch.force;
  if (activeTouch.touchType == UITouchType::Mouse) {
    // pointer events with a mouse button pressed should report a pressure of 0.5
    // when the touch is down and 0.0 when it is lifted regardless of how it is reported by the OS
    event.pressure = eventType != TouchEventType::End ? 0.5f : 0.0f;
  }

  /*
  float pointerSize = activeTouch.majorRadius * 2.0;
  if (activeTouch.touchType == UITouchType::Mouse) {
    // mouse type pointers should always report a size of 1
    pointerSize = 1.0;
  }
  event.width = pointerSize;
  event.height = pointerSize;
  */

  // CGPoint tilt = SphericalToTilt(activeTouch.altitudeAngle, activeTouch.azimuthAngle);
  // event.tiltX = RadsToDegrees(tilt.x);
  // event.tiltY = RadsToDegrees(tilt.y);

  event.detail = 0;

  // event.button = activeTouch.button;
  // event.buttons = ButtonMaskToButtons(activeTouch.buttonMask);

  // UpdatePointerEventModifierFlags(event, activeTouch.modifierFlags);

  // event.tangentialPressure = 0.0;
  // event.twist = 0;
  // event.isPrimary = activeTouch.isPrimary;

  return event;
}

// If we have events that include multiple pointer updates, we should change arg from pointerId to vector<pointerId>
void CompositionEventHandler::DispatchTouchEvent(
    TouchEventType eventType,
    PointerId pointerId,
    const winrt::Microsoft::ReactNative::Composition::Input::PointerPoint &pointerPoint,
    winrt::Windows::System::VirtualKeyModifiers keyModifiers) {
  auto fabricuiManager = ::Microsoft::ReactNative::FabricUIManager::FromProperties(m_context.Properties());

  if (!fabricuiManager)
    return;

  std::unordered_set<facebook::react::SharedTouchEventEmitter> uniqueEventEmitters = {};
  std::vector<facebook::react::SharedTouchEventEmitter> emittersForIndex;

  facebook::react::TouchEvent event;

  size_t index = 0;
  for (const auto &pair : m_activeTouches) {
    const auto &activeTouch = pair.second;

    if (!activeTouch.eventEmitter) {
      continue;
    }

    if (activeTouch.touch.identifier == pointerId) {
      event.changedTouches.insert(activeTouch.touch);
    }
    uniqueEventEmitters.insert(activeTouch.eventEmitter);

    facebook::react::PointerEvent pointerEvent = CreatePointerEventFromActiveTouch(activeTouch, eventType);

    IComponentView *targetView = nullptr;
    bool shouldLeave = (eventType == TouchEventType::End && activeTouch.shouldLeaveWhenReleased) ||
        eventType == TouchEventType::Cancel;
    if (!shouldLeave) {
      const auto &viewRegistry = fabricuiManager->GetViewRegistry();
      facebook::react::Point ptLocal;
      auto targetTag = RootComponentView().hitTest(pointerEvent.clientPoint, ptLocal);
      auto targetComponentViewDescriptor = viewRegistry.componentViewDescriptorWithTag(targetTag);
      targetView = FindClosestFabricManagedTouchableView(targetComponentViewDescriptor.view.get());
    }

    auto handler = [&activeTouch, eventType, &pointerEvent](std::vector<IComponentView *> &eventPathViews) {
      switch (eventType) {
        case TouchEventType::Start:
          activeTouch.eventEmitter->onPointerDown(pointerEvent);
          break;
        case TouchEventType::Move: {
          bool hasMoveEventListeners =
              IsAnyViewInPathListeningToEvent(eventPathViews, facebook::react::ViewEvents::Offset::PointerMove) ||
              IsAnyViewInPathListeningToEvent(eventPathViews, facebook::react::ViewEvents::Offset::PointerMoveCapture);
          if (hasMoveEventListeners) {
            activeTouch.eventEmitter->onPointerMove(pointerEvent);
          }
          break;
        }
        case TouchEventType::End:
          activeTouch.eventEmitter->onPointerUp(pointerEvent);
          break;
        case TouchEventType::Cancel:
        case TouchEventType::CaptureLost:
          activeTouch.eventEmitter->onPointerCancel(pointerEvent);
          break;
      }
    };

    HandleIncomingPointerEvent(pointerEvent, targetView, pointerPoint, keyModifiers, handler);
  }

  for (const auto &pair : m_activeTouches) {
    if (!pair.second.eventEmitter) {
      continue;
    }

    if (IsEndishEventType(eventType) && event.changedTouches.find(pair.second.touch) != event.changedTouches.end()) {
      continue;
    }

    event.touches.insert(pair.second.touch);
  }

  for (const auto &eventEmitter : uniqueEventEmitters) {
    event.targetTouches.clear();

    for (const auto &pair : m_activeTouches) {
      if (pair.second.eventEmitter == eventEmitter) {
        event.targetTouches.insert(pair.second.touch);
      }
    }

    switch (eventType) {
      case TouchEventType::Start:
        eventEmitter->onTouchStart(event);
        break;
      case TouchEventType::Move:
        eventEmitter->onTouchMove(event);
        break;
      case TouchEventType::End:
        eventEmitter->onTouchEnd(event);
        break;
      case TouchEventType::Cancel:
      case TouchEventType::CaptureLost:
        eventEmitter->onTouchCancel(event);
        break;
    }
  }
}

} // namespace Microsoft::ReactNative
