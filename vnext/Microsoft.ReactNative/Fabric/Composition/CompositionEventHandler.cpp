// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include <pch.h>

#include "CompositionEventHandler.h"

#include <Fabric/Composition/CompositionViewComponentView.h>
#include <Fabric/FabricUIManagerModule.h>
#include <Views/ShadowNodeBase.h>
#include <windows.h>
#include <windowsx.h>

namespace Microsoft::ReactNative {

const PointerId MOUSE_POINTER_ID = 1; // TODO ensure this is something that does not conflict with pointer point IDs.

bool IsViewListeningToEvent(IComponentView *view, facebook::react::ViewEvents::Offset eventType) {
  if (view) {
    auto const &viewProps = *std::static_pointer_cast<facebook::react::ViewProps const>(view->props());
    return true;
    // TODO why arn't the events bits being set?
    // return viewProps.events[eventType];
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
    if (componentView->touchEventEmitter()) {
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

CompositionEventHandler::CompositionEventHandler(const winrt::Microsoft::ReactNative::ReactContext &context)
    : m_context(context) {}

CompositionEventHandler::CompositionEventHandler(
    const winrt::Microsoft::ReactNative::ReactContext &context,
    const winrt::Microsoft::ReactNative::CompositionRootView &CompositionRootView)
    : CompositionEventHandler(context) {
  m_compRootView = CompositionRootView;
};

CompositionEventHandler::~CompositionEventHandler() {}

// For DM
/*
void CompositionEventHandler::PointerDown(facebook::react::SurfaceId surfaceId, uint32_t pointerId) {
  if (std::shared_ptr<FabricUIManager> fabricuiManager = ::Microsoft::ReactNative::FabricUIManager::FromProperties(
          winrt::Microsoft::ReactNative::ReactPropertyBag(m_context->Properties()))) {
    facebook::react::Point ptLocal;


  auto pp = winrt::Windows::UI::Input::PointerPoint::GetCurrentPoint(pointerId);


  auto rootComponentViewDescriptor = fabricuiManager->GetViewRegistry().componentViewDescriptorWithTag(surfaceId);
  auto tag = static_cast<CompositionBaseComponentView &>(*rootComponentViewDescriptor.view)
                 .hitTest({pp.Position().X, pp.Position().Y}, ptLocal);

  if (tag != -1) {
  auto hitComponentViewDescriptor = fabricuiManager->GetViewRegistry().componentViewDescriptorWithTag(tag);
    static_cast<CompositionBaseComponentView &>(*hitComponentViewDescriptor.view).OnPointerDown(pp);
  }

  //fabricuiManager

  }

}
*/

void CompositionEventHandler::ScrollWheel(
    facebook::react::SurfaceId surfaceId,
    facebook::react::Point pt,
    uint32_t delta) {
  if (std::shared_ptr<FabricUIManager> fabricuiManager =
          ::Microsoft::ReactNative::FabricUIManager::FromProperties(m_context.Properties())) {
    facebook::react::Point ptLocal;

    auto rootComponentViewDescriptor = fabricuiManager->GetViewRegistry().componentViewDescriptorWithTag(surfaceId);

    static_cast<CompositionBaseComponentView &>(*rootComponentViewDescriptor.view)
        .ScrollWheel(
            {static_cast<float>(pt.x / m_compRootView.ScaleFactor()),
             static_cast<float>(pt.y / m_compRootView.ScaleFactor())},
            delta);
  }
}

int64_t CompositionEventHandler::SendMessage(
    facebook::react::SurfaceId surfaceId,
    uint32_t msg,
    uint64_t wParam,
    int64_t lParam) noexcept {
  switch (msg) {
    case WM_LBUTTONDOWN: {
      ButtonDown(surfaceId, msg, wParam, lParam);
      return 0;
    }
    case WM_POINTERDOWN: {
      PointerPressed(surfaceId, msg, wParam, lParam);
      return 0;
    }
    case WM_LBUTTONUP: {
      ButtonUp(surfaceId, msg, wParam, lParam);
      return 0;
    }
    case WM_POINTERUP: {
      PointerUp(surfaceId, msg, wParam, lParam);
      return 0;
    }
    case WM_MOUSEMOVE: {
      MouseMove(surfaceId, msg, wParam, lParam);
      return 0;
    }
    case WM_KEYDOWN:
    case WM_KEYUP:
    case WM_CHAR:
    case WM_SYSCHAR:
    case WM_SYSKEYDOWN:
    case WM_SYSKEYUP: {
      if (auto focusedComponent = GetFocusedComponent()) {
        auto result = focusedComponent->SendMessage(msg, wParam, lParam);
        if (result)
          return result;
      }
      return 0;
    }
  }

  return 0;
}

std::vector<IComponentView *> GetTouchableViewsInPathToRoot(IComponentView *view) {
  std::vector<IComponentView *> results;
  while (view) {
    if (view->touchEventEmitter()) {
      results.push_back(view);
    }
    view = view->parent();
  }
  return results;
}

/**
 * Private method which is used for tracking the location of pointer events to manage the entering/leaving events.
 * The primary idea is that a pointer's presence & movement is dicated by a variety of underlying events such as down,
 * move, and up — and they should all be treated the same when it comes to tracking the entering & leaving of pointers
 * to views. This method accomplishes that by recieving the pointer event, the target view (can be null in cases when
 * the event indicates that the pointer has left the screen entirely), and a block/callback where the underlying event
 * should be fired.
 */
void CompositionEventHandler::HandleIncomingPointerEvent(
    facebook::react::PointerEvent &event,
    IComponentView *targetView,
    std::function<void(std::vector<IComponentView *> &)> handler) {
  int pointerId = event.pointerId;
  // CGPoint clientLocation = CGPointMake(event.clientPoint.x, event.clientPoint.y);

  std::vector<IComponentView *> currentlyHoveredViews;
  auto itCurrentlyHoveredViews = m_currentlyHoveredViewsPerPointer.find(pointerId);
  if (itCurrentlyHoveredViews != m_currentlyHoveredViewsPerPointer.end()) {
    for (auto &taggedView : itCurrentlyHoveredViews->second) {
      if (auto &view = taggedView.view()) {
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
    facebook::react::SharedTouchEventEmitter eventEmitter = targetView->touchEventEmitter();
    if (shouldEmitOverEvent && eventEmitter != nullptr) {
      eventEmitter->onPointerOver(event);
    }
  }

  // Entering

  // We only want to emit events to JS if there is a view that is currently listening to said event
  // so we only send those event to the JS side if the element which has been entered is itself listening,
  // or if one of its parents is listening in case those listeners care about the capturing phase. Adding the ability
  // for native to distingusih between capturing listeners and not could be an optimization to futher reduce the number
  // of events we send to JS
  bool hasParentEnterListener = false;

  for (auto itComponentView = eventPathViews.rbegin(); itComponentView != eventPathViews.rend();
       itComponentView++) { //  for (RCTReactTaggedView *taggedView in [eventPathViews reverseObjectEnumerator]) {

    auto componentView = *itComponentView;
    bool shouldEmitEvent = componentView != nullptr &&
        (hasParentEnterListener ||
         IsViewListeningToEvent(componentView, facebook::react::ViewEvents::Offset::PointerEnter));

    if (shouldEmitEvent &&
        std::find(currentlyHoveredViews.begin(), currentlyHoveredViews.end(), componentView) ==
            currentlyHoveredViews.end()) {
      facebook::react::SharedTouchEventEmitter eventEmitter = componentView->touchEventEmitter();
      if (eventEmitter) {
        eventEmitter->onPointerEnter(event);
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
    facebook::react::SharedTouchEventEmitter eventEmitter = prevTargetView->touchEventEmitter();
    if (shouldEmitOutEvent && eventEmitter != nullptr) {
      eventEmitter->onPointerOut(event);
    }
  }

  // Leaving

  // pointerleave events need to be emited from the deepest target to the root but
  // we also need to efficiently keep track of if a view has a parent which is listening to the leave events,
  // so we first iterate from the root to the target, collecting the views which need events fired for, of which
  // we reverse iterate (now from target to root), actually emitting the events.
  std::vector<IComponentView *> viewsToEmitLeaveEventsTo; // NSMutableOrderedSet<UIView *> *viewsToEmitLeaveEventsTo =
                                                          // [NSMutableOrderedSet orderedSet];

  bool hasParentLeaveListener = false;
  for (auto itComponentView = currentlyHoveredViews.rbegin(); itComponentView != currentlyHoveredViews.rend();
       itComponentView++) { //  for (RCTReactTaggedView *taggedView in [currentlyHoveredViews reverseObjectEnumerator])
                            //  {
    auto componentView = *itComponentView;

    bool shouldEmitEvent = componentView != nullptr &&
        (hasParentLeaveListener ||
         IsViewListeningToEvent(componentView, facebook::react::ViewEvents::Offset::PointerLeave));

    if (shouldEmitEvent &&
        std::find(eventPathViews.begin(), eventPathViews.end(), componentView) == eventPathViews.end()) {
      viewsToEmitLeaveEventsTo.push_back(componentView);
    }

    if (shouldEmitEvent && !hasParentLeaveListener) {
      hasParentLeaveListener = true;
    }
  }

  for (auto itComponentView = viewsToEmitLeaveEventsTo.rbegin(); itComponentView != viewsToEmitLeaveEventsTo.rend();
       itComponentView++) { //  for (UIView *componentView in [viewsToEmitLeaveEventsTo reverseObjectEnumerator]) {
    auto componentView = *itComponentView;

    facebook::react::SharedTouchEventEmitter eventEmitter = componentView->touchEventEmitter();
    if (eventEmitter) {
      eventEmitter->onPointerLeave(event);
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
  // pointerEvent.offsetPoint = ptLocal;
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

void CompositionEventHandler::MouseMove(
    facebook::react::SurfaceId surfaceId,
    uint32_t msg,
    uint64_t wParam,
    int64_t lParam) {
  int pointerId = MOUSE_POINTER_ID; // TODO pointerId

  auto x = GET_X_LPARAM(lParam);
  auto y = GET_Y_LPARAM(lParam);

  if (std::shared_ptr<FabricUIManager> fabricuiManager =
          ::Microsoft::ReactNative::FabricUIManager::FromProperties(m_context.Properties())) {
    facebook::react::Point ptLocal;

    auto rootComponentViewDescriptor = fabricuiManager->GetViewRegistry().componentViewDescriptorWithTag(surfaceId);
    facebook::react::Point ptScaled = {
        static_cast<float>(x / m_compRootView.ScaleFactor()), static_cast<float>(y / m_compRootView.ScaleFactor())};
    auto tag =
        static_cast<CompositionBaseComponentView &>(*rootComponentViewDescriptor.view).hitTest(ptScaled, ptLocal);

    if (tag == -1)
      return;

    auto targetView = FindClosestFabricManagedTouchableView(
        fabricuiManager->GetViewRegistry().componentViewDescriptorWithTag(tag).view.get());

    facebook::react::PointerEvent pointerEvent = CreatePointerEventFromIncompleteHoverData(ptScaled, ptLocal);

    auto handler = [targetView, &pointerEvent](std::vector<IComponentView *> &eventPathViews) {
      facebook::react::SharedTouchEventEmitter eventEmitter = targetView ? targetView->touchEventEmitter() : nullptr;
      bool hasMoveEventListeners =
          IsAnyViewInPathListeningToEvent(eventPathViews, facebook::react::ViewEvents::Offset::PointerMove) ||
          IsAnyViewInPathListeningToEvent(eventPathViews, facebook::react::ViewEvents::Offset::PointerMoveCapture);
      if (eventEmitter != nullptr && hasMoveEventListeners) {
        eventEmitter->onPointerMove(pointerEvent);
      }
    };

    HandleIncomingPointerEvent(pointerEvent, targetView, handler);
  }
}

void CompositionEventHandler::PointerPressed(
    facebook::react::SurfaceId surfaceId,
    uint32_t msg,
    uint64_t wParam,
    int64_t lParam) {
  POINTER_INFO pi;
  PointerId pointerId = GET_POINTERID_WPARAM(wParam);
  GetPointerInfo(pointerId, &pi);
  POINT pt = {GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
  ::ScreenToClient(pi.hwndTarget, &pt);

  auto staleTouch = std::find_if(m_activeTouches.begin(), m_activeTouches.end(), [pointerId](const auto &pair) {
    return pair.second.touch.identifier == pointerId;
  });

  if (staleTouch != m_activeTouches.end()) {
    // A pointer with this ID already exists - Should we fire a button cancel or something?
    assert(false);
    return;
  }

  const auto eventType = TouchEventType::Start;

  if (std::shared_ptr<FabricUIManager> fabricuiManager =
          ::Microsoft::ReactNative::FabricUIManager::FromProperties(m_context.Properties())) {
    facebook::react::Point ptLocal;

    auto rootComponentViewDescriptor = fabricuiManager->GetViewRegistry().componentViewDescriptorWithTag(surfaceId);
    facebook::react::Point ptScaled = {
        static_cast<float>(pt.x / m_compRootView.ScaleFactor()),
        static_cast<float>(pt.y / m_compRootView.ScaleFactor())};
    auto tag =
        static_cast<CompositionBaseComponentView &>(*rootComponentViewDescriptor.view).hitTest(ptScaled, ptLocal);

    if (tag == -1)
      return;

    IComponentView *targetComponentView =
        fabricuiManager->GetViewRegistry().componentViewDescriptorWithTag(tag).view.get();
    static_cast<CompositionBaseComponentView *>(targetComponentView)->SendMessage(msg, wParam, lParam);

    ActiveTouch activeTouch{0};
    switch (pi.pointerType) {
      case PT_POINTER:
      case PT_TOUCH:
        activeTouch.touchType = UITouchType::Touch;
        break;
      case PT_PEN:
        activeTouch.touchType = UITouchType::Pen;
        break;
      case PT_TOUCHPAD:
      case PT_MOUSE:
        activeTouch.touchType = UITouchType::Mouse;
        break;
    }

    auto componentView = targetComponentView;
    while (componentView) {
      if (auto eventEmitter = componentView->touchEventEmitter()) {
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

    DispatchTouchEvent(surfaceId, eventType, pointerId);
  }
}

void CompositionEventHandler::ButtonDown(
    facebook::react::SurfaceId surfaceId,
    uint32_t msg,
    uint64_t wParam,
    int64_t lParam) {
  PointerId pointerId = MOUSE_POINTER_ID;

  auto staleTouch = std::find_if(m_activeTouches.begin(), m_activeTouches.end(), [pointerId](const auto &pair) {
    return pair.second.touch.identifier == pointerId;
  });

  if (staleTouch != m_activeTouches.end()) {
    // A pointer with this ID already exists - Should we fire a button cancel or something?
    // assert(false);
    return;
  }

  auto x = GET_X_LPARAM(lParam);
  auto y = GET_Y_LPARAM(lParam);
  const auto eventType = TouchEventType::Start;

  if (std::shared_ptr<FabricUIManager> fabricuiManager =
          ::Microsoft::ReactNative::FabricUIManager::FromProperties(m_context.Properties())) {
    facebook::react::Point ptLocal;

    auto rootComponentViewDescriptor = fabricuiManager->GetViewRegistry().componentViewDescriptorWithTag(surfaceId);
    facebook::react::Point ptScaled = {
        static_cast<float>(x / m_compRootView.ScaleFactor()), static_cast<float>(y / m_compRootView.ScaleFactor())};
    auto tag = rootComponentViewDescriptor.view->hitTest(ptScaled, ptLocal);

    if (tag == -1)
      return;

    IComponentView *targetComponentView =
        fabricuiManager->GetViewRegistry().componentViewDescriptorWithTag(tag).view.get();
    static_cast<CompositionBaseComponentView *>(targetComponentView)->SendMessage(msg, wParam, lParam);

    ActiveTouch activeTouch{0};
    activeTouch.touchType = UITouchType::Mouse;

    auto componentView = targetComponentView;
    while (componentView) {
      if (auto eventEmitter = componentView->touchEventEmitter()) {
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

    DispatchTouchEvent(surfaceId, eventType, pointerId);
  }
}

void CompositionEventHandler::PointerUp(
    facebook::react::SurfaceId surfaceId,
    uint32_t msg,
    uint64_t wParam,
    int64_t lParam) {
  POINTER_INFO pi;
  PointerId pointerId = GET_POINTERID_WPARAM(wParam);
  GetPointerInfo(pointerId, &pi);
  POINT pt = {GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
  ::ScreenToClient(pi.hwndTarget, &pt);

  auto activeTouch = std::find_if(m_activeTouches.begin(), m_activeTouches.end(), [pointerId](const auto &pair) {
    return pair.second.touch.identifier == pointerId;
  });

  if (activeTouch == m_activeTouches.end()) {
    return;
  }

  const auto eventType = TouchEventType::Start;

  if (std::shared_ptr<FabricUIManager> fabricuiManager =
          ::Microsoft::ReactNative::FabricUIManager::FromProperties(m_context.Properties())) {
    facebook::react::Point ptLocal;

    auto rootComponentViewDescriptor = fabricuiManager->GetViewRegistry().componentViewDescriptorWithTag(surfaceId);
    facebook::react::Point ptScaled = {
        static_cast<float>(pt.x / m_compRootView.ScaleFactor()),
        static_cast<float>(pt.y / m_compRootView.ScaleFactor())};
    auto tag =
        static_cast<CompositionBaseComponentView &>(*rootComponentViewDescriptor.view).hitTest(ptScaled, ptLocal);

    if (tag == -1)
      return;

    auto targetComponentView = fabricuiManager->GetViewRegistry().componentViewDescriptorWithTag(tag).view;
    std::static_pointer_cast<CompositionBaseComponentView>(targetComponentView)->SendMessage(msg, wParam, lParam);

    UpdateActiveTouch(activeTouch->second, ptScaled, ptLocal);
    DispatchTouchEvent(surfaceId, TouchEventType::End, pointerId);
    m_activeTouches.erase(pointerId);
  }
}

void CompositionEventHandler::ButtonUp(
    facebook::react::SurfaceId surfaceId,
    uint32_t msg,
    uint64_t wParam,
    int64_t lParam) {
  int pointerId = MOUSE_POINTER_ID;

  auto activeTouch = std::find_if(m_activeTouches.begin(), m_activeTouches.end(), [pointerId](const auto &pair) {
    return pair.second.touch.identifier == pointerId;
  });

  if (activeTouch == m_activeTouches.end()) {
    return;
  }

  auto x = GET_X_LPARAM(lParam);
  auto y = GET_Y_LPARAM(lParam);

  if (std::shared_ptr<FabricUIManager> fabricuiManager =
          ::Microsoft::ReactNative::FabricUIManager::FromProperties(m_context.Properties())) {
    facebook::react::Point ptLocal;

    auto rootComponentViewDescriptor = fabricuiManager->GetViewRegistry().componentViewDescriptorWithTag(surfaceId);
    facebook::react::Point ptScaled = {
        static_cast<float>(x / m_compRootView.ScaleFactor()), static_cast<float>(y / m_compRootView.ScaleFactor())};
    auto tag =
        static_cast<CompositionBaseComponentView &>(*rootComponentViewDescriptor.view).hitTest(ptScaled, ptLocal);

    if (tag == -1)
      return;

    auto targetComponentView = fabricuiManager->GetViewRegistry().componentViewDescriptorWithTag(tag).view;
    std::static_pointer_cast<CompositionBaseComponentView>(targetComponentView)->SendMessage(msg, wParam, lParam);

    UpdateActiveTouch(activeTouch->second, ptScaled, ptLocal);
    DispatchTouchEvent(surfaceId, TouchEventType::End, pointerId);
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
  // event.offsetPoint = touch.offsetPoint;

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
    facebook::react::SurfaceId surfaceId,
    TouchEventType eventType,
    PointerId pointerId) {
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
      auto rootComponentViewDescriptor = viewRegistry.componentViewDescriptorWithTag(surfaceId);
      facebook::react::Point ptLocal;
      auto targetTag = rootComponentViewDescriptor.view->hitTest(pointerEvent.clientPoint, ptLocal);
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

    HandleIncomingPointerEvent(pointerEvent, targetView, handler);
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
