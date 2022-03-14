// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include <pch.h>

#include <Views/ShadowNodeBase.h>
#include "CompEventHandler.h"

#ifdef USE_FABRIC
#include <Fabric/CompViewComponentView.h>
#include <Fabric/FabricUIManagerModule.h>
#include <react/renderer/components/view/TouchEventEmitter.h>
#endif

#include <Modules/NativeUIManager.h>
#include <Utils/ValueUtils.h>

#include <windows.h>
#include <windowsx.h>
#include <winrt/Windows.ApplicationModel.Core.h>
#include <winrt/Windows.Devices.Input.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.UI.Core.h>
#include <winrt/Windows.UI.Input.h>

#ifdef USE_WINUI3
#include <winrt/Microsoft.UI.Input.Experimental.h>
#endif

namespace Microsoft::ReactNative {

std::vector<int64_t> GetTagsForBranch(INativeUIManagerHost *host, int64_t tag, int64_t rootTag);

facebook::react::SharedEventEmitter EventEmitterForElement(
    std::shared_ptr<FabricUIManager> &uimanager,
    facebook::react::Tag tag) noexcept;

CompEventHandler::CompEventHandler(const Mso::React::IReactContext &context, bool fabric)
    : m_context(&context),
      m_fabric(fabric),
      m_batchingEventEmitter{
          std::make_shared<winrt::Microsoft::ReactNative::BatchingEventEmitter>(Mso::CntPtr(&context))} {}

CompEventHandler::CompEventHandler(
    const Mso::React::IReactContext &context,
    bool fabric,
    const winrt::Microsoft::ReactNative::CompRootView &compRootView)
    : CompEventHandler(context, fabric) {
  m_compRootView = compRootView;
};

CompEventHandler::~CompEventHandler() {}

winrt::Microsoft::ReactNative::BatchingEventEmitter &CompEventHandler::BatchingEmitter() noexcept {
  return *m_batchingEventEmitter;
}

// For DM
/*
void CompEventHandler::PointerDown(facebook::react::SurfaceId surfaceId, uint32_t pointerId) {
  if (std::shared_ptr<FabricUIManager> fabricuiManager = ::Microsoft::ReactNative::FabricUIManager::FromProperties(
          winrt::Microsoft::ReactNative::ReactPropertyBag(m_context->Properties()))) {
    facebook::react::Point ptLocal;


  auto pp = winrt::Windows::UI::Input::PointerPoint::GetCurrentPoint(pointerId);


  auto rootComponentViewDescriptor = fabricuiManager->GetViewRegistry().componentViewDescriptorWithTag(surfaceId);
  auto tag = static_cast<CompBaseComponentView &>(*rootComponentViewDescriptor.view)
                 .hitTest({pp.Position().X, pp.Position().Y}, ptLocal);

  if (tag != -1) {
  auto hitComponentViewDescriptor = fabricuiManager->GetViewRegistry().componentViewDescriptorWithTag(tag);
    static_cast<CompBaseComponentView &>(*hitComponentViewDescriptor.view).OnPointerDown(pp);
  }

  //fabricuiManager

  }

}
*/

void CompEventHandler::ScrollWheel(facebook::react::SurfaceId surfaceId, facebook::react::Point pt, uint32_t delta) {
  if (std::shared_ptr<FabricUIManager> fabricuiManager = ::Microsoft::ReactNative::FabricUIManager::FromProperties(
          winrt::Microsoft::ReactNative::ReactPropertyBag(m_context->Properties()))) {
    facebook::react::Point ptLocal;

    auto rootComponentViewDescriptor = fabricuiManager->GetViewRegistry().componentViewDescriptorWithTag(surfaceId);

    static_cast<CompBaseComponentView &>(*rootComponentViewDescriptor.view)
        .ScrollWheel(
            {static_cast<float>(pt.x / m_compRootView.ScaleFactor()),
             static_cast<float>(pt.y / m_compRootView.ScaleFactor())},
            delta);
  }
}

int64_t CompEventHandler::SendMessage(
    facebook::react::SurfaceId surfaceId,
    uint32_t msg,
    uint64_t wParam,
    int64_t lParam) noexcept {
  switch (msg) {
    case WM_LBUTTONDOWN: {
      // TODO why are we not getting WM_POINTERDOWN instead?
      PointerPressed(surfaceId, msg, wParam, lParam);
      return 0;
    }

    case WM_POINTERDOWN: {
      PointerPressed(surfaceId, msg, wParam, lParam);
      return 0;
    }
    case WM_LBUTTONUP: {
      // TODO why are we not getting WM_POINTERUP instead?
      PointerUp(surfaceId, msg, wParam, lParam);
      return 0;
    }
    case WM_POINTERUP: {
      PointerUp(surfaceId, msg, wParam, lParam);
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

void CompEventHandler::PointerPressed(
    facebook::react::SurfaceId surfaceId,
    uint32_t msg,
    uint64_t wParam,
    int64_t lParam) {
  int pointerId = 1; // TODO pointerId

  auto touch = std::find_if(
      m_touches.begin(), m_touches.end(), [pointerId](facebook::react::Touch &t) { return t.identifier == pointerId; });

  if (touch != m_touches.end()) {
    // A pointer with this ID already exists
    assert(false);
    return;
  }

  auto x = GET_X_LPARAM(lParam);
  auto y = GET_Y_LPARAM(lParam);

  const auto eventType = TouchEventType::Start;

  if (std::shared_ptr<FabricUIManager> fabricuiManager = ::Microsoft::ReactNative::FabricUIManager::FromProperties(
          winrt::Microsoft::ReactNative::ReactPropertyBag(m_context->Properties()))) {
    facebook::react::Point ptLocal;

    auto rootComponentViewDescriptor = fabricuiManager->GetViewRegistry().componentViewDescriptorWithTag(surfaceId);
    facebook::react::Point ptScaled = {
        static_cast<float>(x / m_compRootView.ScaleFactor()), static_cast<float>(y / m_compRootView.ScaleFactor())};
    auto tag = static_cast<CompBaseComponentView &>(*rootComponentViewDescriptor.view).hitTest(ptScaled, ptLocal);

    if (tag == -1)
      return;

    auto &targetComponentView = static_cast<CompBaseComponentView &>(
        *fabricuiManager->GetViewRegistry().componentViewDescriptorWithTag(tag).view);

    targetComponentView.SendMessage(msg, wParam, lParam);

    facebook::react::Touch pointer;

    pointer.target = tag;
    pointer.identifier = pointerId;
    // pointer.deviceType = winrt::Windows::Devices::Input::PointerDeviceType::Mouse;
    // pointer.isLeftButton = true;
    // pointer.isRightButton = false;
    // pointer.isMiddleButton = false;
    // pointer.isHorizontalScrollWheel = false;
    // pointer.isEraser = false;

    pointer.pagePoint.x = ptScaled.x;
    pointer.pagePoint.y = ptScaled.y;
    pointer.screenPoint.x = ptLocal.x;
    pointer.screenPoint.y = ptLocal.y;
    pointer.offsetPoint.x = ptLocal.x;
    pointer.offsetPoint.y = ptLocal.y;
    pointer.timestamp = static_cast<facebook::react::Float>(
        std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch())
            .count());
    pointer.force = 0;
    // pointer.isBarrelButton = false;
    // pointer.shiftKey = false;
    // pointer.ctrlKey = false;
    // pointer.altKey = false;

    m_touches.emplace_back(std::move(pointer));
    DispatchTouchEvent(eventType, pointerId);
  }
}

void CompEventHandler::PointerUp(facebook::react::SurfaceId surfaceId, uint32_t msg, uint64_t wParam, int64_t lParam) {
  int pointerId = 1; // TODO pointerId

  auto touch = std::find_if(
      m_touches.begin(), m_touches.end(), [pointerId](facebook::react::Touch &t) { return t.identifier == pointerId; });

  if (touch == m_touches.end())
    return;

  auto x = GET_X_LPARAM(lParam);
  auto y = GET_Y_LPARAM(lParam);

  if (std::shared_ptr<FabricUIManager> fabricuiManager = ::Microsoft::ReactNative::FabricUIManager::FromProperties(
          winrt::Microsoft::ReactNative::ReactPropertyBag(m_context->Properties()))) {
    facebook::react::Point ptLocal;

    auto rootComponentViewDescriptor = fabricuiManager->GetViewRegistry().componentViewDescriptorWithTag(surfaceId);
    facebook::react::Point ptScaled = {
        static_cast<float>(x / m_compRootView.ScaleFactor()), static_cast<float>(y / m_compRootView.ScaleFactor())};
    auto tag = static_cast<CompBaseComponentView &>(*rootComponentViewDescriptor.view).hitTest(ptScaled, ptLocal);

    if (tag == -1)
      return;

    auto &targetComponentView = static_cast<CompBaseComponentView &>(
        *fabricuiManager->GetViewRegistry().componentViewDescriptorWithTag(tag).view);

    targetComponentView.SendMessage(msg, wParam, lParam);

    auto &pointer = *touch;
    pointer.target = tag;
    // pointer.isLeftButton = true;
    // pointer.isRightButton = false;
    // pointer.isMiddleButton = false;
    // pointer.isHorizontalScrollWheel = false;
    // pointer.isEraser = false;
    pointer.pagePoint.x = ptScaled.x;
    pointer.pagePoint.y = ptScaled.y;
    pointer.screenPoint.x = ptLocal.x;
    pointer.screenPoint.y = ptLocal.y;
    pointer.offsetPoint.x = ptLocal.x;
    pointer.offsetPoint.y = ptLocal.y;
    pointer.timestamp = static_cast<facebook::react::Float>(
        std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch())
            .count());
    pointer.force = 0;
    // pointer.isBarrelButton = false;
    // pointer.shiftKey = false;
    // pointer.ctrlKey = false;
    // pointer.altKey = false;

    DispatchTouchEvent(TouchEventType::End, pointerId);

    m_touches.erase(touch);
  }
}

// defines button payload, follows https://developer.mozilla.org/docs/Web/API/MouseEvent/button
enum class MouseEventButtonKind { None = -1, Main = 0, Auxiliary = 1, Secondary = 2, Eraser = 5 };

#ifdef USE_FABRIC

bool CompEventHandler::IsEndishEventType(TouchEventType eventType) noexcept {
  switch (eventType) {
    case TouchEventType::End:
    case TouchEventType::Cancel:
    case TouchEventType::CaptureLost:
      return true;
    default:
      return false;
  }
}
#endif

void CompEventHandler::DispatchTouchEvent(TouchEventType eventType, int64_t pointerId) {
  std::shared_ptr<FabricUIManager> fabricuiManager;
  if (m_fabric &&
      !!(fabricuiManager = ::Microsoft::ReactNative::FabricUIManager::FromProperties(
             winrt::Microsoft::ReactNative::ReactPropertyBag(m_context->Properties())))) {
    std::unordered_set<facebook::react::SharedTouchEventEmitter> uniqueEventEmitters = {};
    std::vector<facebook::react::SharedTouchEventEmitter> emittersForIndex;

    facebook::react::TouchEvent te;

    size_t index = 0;
    for (const auto &pointer : m_touches) {
      bool isChangedPointer = pointer.identifier == pointerId;

      if (!isChangedPointer || !IsEndishEventType(eventType)) {
        te.touches.insert(pointer);
      }

      if (isChangedPointer)
        te.changedTouches.insert(pointer);

      auto emitter = std::static_pointer_cast<facebook::react::TouchEventEmitter>(
          std::const_pointer_cast<facebook::react::EventEmitter>(
              EventEmitterForElement(fabricuiManager, pointer.target)));
      emittersForIndex.push_back(emitter);
      if (emitter)
        uniqueEventEmitters.insert(emitter);
    }

    for (const auto emitter : uniqueEventEmitters) {
      te.targetTouches.clear();
      index = 0;
      for (const auto &pointer : m_touches) {
        auto pointerEmitter = emittersForIndex[index++];
        if (emitter == pointerEmitter)
          te.targetTouches.insert(pointer);
      }

      switch (eventType) {
        case TouchEventType::Start:
          emitter->onTouchStart(te);
          break;
        case TouchEventType::Move:
          emitter->onTouchMove(te);
          break;
        case TouchEventType::End:
          emitter->onTouchEnd(te);
          break;
        case TouchEventType::Cancel:
        case TouchEventType::CaptureLost:
          emitter->onTouchCancel(te);
          break;
      }
    }
  }
}

bool CompEventHandler::DispatchBackEvent() {
  if (m_context->State() != Mso::React::ReactInstanceState::Loaded)
    return false;

  BatchingEmitter().EmitJSEvent(
      L"RCTDeviceEventEmitter", L"emit", winrt::Microsoft::ReactNative::MakeJSValueArgWriter(L"hardwardBackPress"));

  return true;
}

} // namespace Microsoft::ReactNative
