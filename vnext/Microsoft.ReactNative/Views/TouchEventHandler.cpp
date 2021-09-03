// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include <pch.h>

#include <Views/ShadowNodeBase.h>
#include "TouchEventHandler.h"

#ifdef USE_FABRIC
#include <Fabric/FabricUIManagerModule.h>
#include <Fabric/ViewComponentView.h>
#include <react/renderer/components/view/TouchEventEmitter.h>
#endif

#include <Modules/NativeUIManager.h>
#include <Modules/PaperUIManagerModule.h>
#include <UI.Xaml.Controls.h>
#include <UI.Xaml.Documents.h>
#include <UI.Xaml.Input.h>
#include <UI.Xaml.Media.h>
#include <Utils/ValueUtils.h>

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

TouchEventHandler::TouchEventHandler(const Mso::React::IReactContext &context, bool fabric)
    : m_xamlView(nullptr),
      m_context(&context),
      m_fabric(fabric),
      m_batchingEventEmitter{
          std::make_shared<winrt::Microsoft::ReactNative::BatchingEventEmitter>(Mso::CntPtr(&context))} {}

TouchEventHandler::~TouchEventHandler() {
  RemoveTouchHandlers();
}

void TouchEventHandler::AddTouchHandlers(XamlView xamlView) {
  auto uiElement(xamlView.as<xaml::UIElement>());
  if (uiElement == nullptr) {
    assert(false);
    return;
  }

  m_xamlView = xamlView;

  RemoveTouchHandlers();

  m_pressedRevoker = uiElement.PointerPressed(winrt::auto_revoke, {this, &TouchEventHandler::OnPointerPressed});
  m_releasedRevoker = uiElement.PointerReleased(winrt::auto_revoke, {this, &TouchEventHandler::OnPointerReleased});
  m_canceledRevoker = uiElement.PointerCanceled(winrt::auto_revoke, {this, &TouchEventHandler::OnPointerCanceled});
  m_captureLostRevoker =
      uiElement.PointerCaptureLost(winrt::auto_revoke, {this, &TouchEventHandler::OnPointerCaptureLost});
  m_exitedRevoker = uiElement.PointerExited(winrt::auto_revoke, {this, &TouchEventHandler::OnPointerExited});
  m_movedRevoker = uiElement.PointerMoved(winrt::auto_revoke, {this, &TouchEventHandler::OnPointerMoved});
}

void TouchEventHandler::RemoveTouchHandlers() {
  m_pressedRevoker.revoke();
  m_releasedRevoker.revoke();
  m_canceledRevoker.revoke();
  m_captureLostRevoker.revoke();
  m_exitedRevoker.revoke();
  m_movedRevoker.revoke();
}

winrt::Microsoft::ReactNative::BatchingEventEmitter &TouchEventHandler::BatchingEmitter() noexcept {
  return *m_batchingEventEmitter;
}

void TouchEventHandler::OnPointerPressed(
    const winrt::IInspectable & /*sender*/,
    const winrt::PointerRoutedEventArgs &args) {
  // Short circuit all of this if we are in an error state
  if (m_context->State() == Mso::React::ReactInstanceState::HasError)
    return;

  if (IndexOfPointerWithId(args.Pointer().PointerId()) != std::nullopt) {
    // A pointer with this ID already exists
    assert(false);
    return;
  }

  // Only if the view has a Tag can we process this
  std::vector<int64_t> tagsForBranch;
  xaml::UIElement sourceElement(nullptr);
  const auto eventType = TouchEventType::Start;
  const auto kind = GetPointerEventKind(eventType);
  const auto reactArgs = winrt::make<winrt::Microsoft::ReactNative::implementation::ReactPointerEventArgs>(kind, args);
  const auto argsImpl =
      winrt::get_self<winrt::Microsoft::ReactNative::implementation::ReactPointerEventArgs>(reactArgs);
  if (!PropagatePointerEventAndFindReactTarget(reactArgs, &tagsForBranch, &sourceElement))
    return;

  // If this was caused by the user pressing the "back" hardware button, fire that event instead
  if (args.GetCurrentPoint(sourceElement).Properties().PointerUpdateKind() ==
      winrt::Windows::UI::Input::PointerUpdateKind::XButton1Pressed) {
    args.Handled(DispatchBackEvent());
    return;
  }

  if (!argsImpl->CaptureReleased() && !argsImpl->DefaultPrevented() &&
      m_xamlView.as<xaml::FrameworkElement>().CapturePointer(args.Pointer())) {
    // Pointer pressing updates the enter/leave state
    UpdatePointersInViews(args, sourceElement, std::move(tagsForBranch));

    assert(!tagsForBranch.empty());
    const auto tag = tagsForBranch.front();
    size_t pointerIndex = AddReactPointer(args, tag, sourceElement);

    // For now, when using the mouse we only want to send click events for the left button.
    // Finger and pen taps will also set isLeftButton.
    if (m_pointers[pointerIndex].isLeftButton) {
      DispatchTouchEvent(eventType, pointerIndex);
    }
  }
}

void TouchEventHandler::OnPointerReleased(
    const winrt::IInspectable & /*sender*/,
    const winrt::PointerRoutedEventArgs &args) {
  OnPointerConcluded(TouchEventType::End, args);
}

void TouchEventHandler::OnPointerCanceled(
    const winrt::IInspectable & /*sender*/,
    const winrt::PointerRoutedEventArgs &args) {
  OnPointerConcluded(TouchEventType::Cancel, args);
}

void TouchEventHandler::OnPointerCaptureLost(
    const winrt::IInspectable & /*sender*/,
    const winrt::PointerRoutedEventArgs &args) {
  OnPointerConcluded(TouchEventType::CaptureLost, args);
}

void TouchEventHandler::OnPointerExited(
    const winrt::IInspectable & /*sender*/,
    const winrt::PointerRoutedEventArgs &args) {
  // Short circuit all of this if we are in an error state
  if (m_context->State() == Mso::React::ReactInstanceState::HasError)
    return;

  std::vector<int64_t> tagsForBranch;
  UpdatePointersInViews(args, nullptr, std::move(tagsForBranch));
}

void TouchEventHandler::OnPointerMoved(
    const winrt::IInspectable & /*sender*/,
    const winrt::PointerRoutedEventArgs &args) {
  // Short circuit all of this if we are in an error state
  if (m_context->State() == Mso::React::ReactInstanceState::HasError)
    return;

  // Only if the view has a Tag can we process this
  std::vector<int64_t> tagsForBranch;
  xaml::UIElement sourceElement(nullptr);
  const auto eventType = TouchEventType::Move;
  const auto kind = GetPointerEventKind(eventType);
  const auto reactArgs = winrt::make<winrt::Microsoft::ReactNative::implementation::ReactPointerEventArgs>(kind, args);
  const auto argsImpl =
      winrt::get_self<winrt::Microsoft::ReactNative::implementation::ReactPointerEventArgs>(reactArgs);
  if (!PropagatePointerEventAndFindReactTarget(reactArgs, &tagsForBranch, &sourceElement))
    return;

  auto optPointerIndex = IndexOfPointerWithId(args.Pointer().PointerId());
  if (optPointerIndex) {
    if (argsImpl->CaptureReleased()) {
      m_xamlView.as<xaml::FrameworkElement>().ReleasePointerCapture(args.Pointer());
    } else if (!argsImpl->DefaultPrevented()) {
      UpdateReactPointer(m_pointers[*optPointerIndex], args, sourceElement);
      DispatchTouchEvent(eventType, *optPointerIndex);
    }
  } else if (!argsImpl->DefaultPrevented()) {
    // Move with no buttons pressed
    UpdatePointersInViews(args, sourceElement, std::move(tagsForBranch));
  }
}

void TouchEventHandler::OnPointerConcluded(TouchEventType eventType, const winrt::PointerRoutedEventArgs &args) {
  // Short circuit all of this if we are in an error state
  if (m_context->State() == Mso::React::ReactInstanceState::HasError)
    return;

  auto optPointerIndex = IndexOfPointerWithId(args.Pointer().PointerId());
  if (!optPointerIndex)
    return;

  // if the view has a Tag, update the pointer info.
  // Regardless of that, ensure we Dispatch & cleanup the pointer
  std::vector<int64_t> tagsForBranch;
  xaml::UIElement sourceElement(nullptr);
  const auto kind = GetPointerEventKind(eventType);
  const auto reactArgs = winrt::make<winrt::Microsoft::ReactNative::implementation::ReactPointerEventArgs>(kind, args);
  const auto argsImpl =
      winrt::get_self<winrt::Microsoft::ReactNative::implementation::ReactPointerEventArgs>(reactArgs);
  if (PropagatePointerEventAndFindReactTarget(reactArgs, &tagsForBranch, &sourceElement))
    UpdateReactPointer(m_pointers[*optPointerIndex], args, sourceElement);

  if (m_pointers[*optPointerIndex].isLeftButton) {
    if (argsImpl->CaptureReleased()) {
      // Force cancellation if ReleaseCapture is called ReactPointerEventArgs
      DispatchTouchEvent(TouchEventType::Cancel, *optPointerIndex);
    } else if (!argsImpl->DefaultPrevented()) {
      DispatchTouchEvent(eventType, *optPointerIndex);
    }
  }

  // If PreventDefault is called when CaptureLost occurs, do not clean up
  if (eventType != TouchEventType::CaptureLost || !argsImpl->DefaultPrevented()) {
    m_pointers.erase(cbegin(m_pointers) + *optPointerIndex);
    if (m_pointers.size() == 0)
      m_touchId = 0;

    const auto frameworkElement = m_xamlView.as<xaml::FrameworkElement>();
    auto wasCaptured = false;
    if (frameworkElement.PointerCaptures()) {
      for (auto pointer : frameworkElement.PointerCaptures()) {
        wasCaptured |= pointer.PointerId() == args.Pointer().PointerId();
      }
    }

    frameworkElement.ReleasePointerCapture(args.Pointer());

    // Updates the enter/leave state when pointer was previously captured
    if (wasCaptured) {
      UpdatePointersInViews(args, sourceElement, std::move(tagsForBranch));
    }
  }
}
size_t TouchEventHandler::AddReactPointer(
    const winrt::PointerRoutedEventArgs &args,
    int64_t tag,
    xaml::UIElement sourceElement) {
  ReactPointer pointer = CreateReactPointer(args, tag, sourceElement);
  m_pointers.emplace_back(std::move(pointer));
  return m_pointers.size() - 1;
}

TouchEventHandler::ReactPointer TouchEventHandler::CreateReactPointer(
    const winrt::PointerRoutedEventArgs &args,
    int64_t tag,
    xaml::UIElement sourceElement) {
  auto point = args.GetCurrentPoint(sourceElement);
  auto props = point.Properties();

  ReactPointer pointer;
  pointer.target = tag;
  pointer.identifier = m_touchId++;
  pointer.pointerId = point.PointerId();
#ifndef USE_WINUI3
  pointer.deviceType = point.PointerDevice().PointerDeviceType();
#else
  pointer.deviceType = point.PointerDeviceType();
#endif
  pointer.isLeftButton = props.IsLeftButtonPressed();
  pointer.isRightButton = props.IsRightButtonPressed();
  pointer.isMiddleButton = props.IsMiddleButtonPressed();
  pointer.isHorizontalScrollWheel = props.IsHorizontalMouseWheel();
  pointer.isEraser = props.IsEraser();

  UpdateReactPointer(pointer, args, sourceElement);

  return pointer;
}

void TouchEventHandler::UpdateReactPointer(
    ReactPointer &pointer,
    const winrt::PointerRoutedEventArgs &args,
    xaml::UIElement sourceElement) {
  auto rootPoint = args.GetCurrentPoint(m_xamlView.as<xaml::FrameworkElement>());
  auto point = args.GetCurrentPoint(sourceElement);
  auto props = point.Properties();
  auto keyModifiers = static_cast<uint32_t>(args.KeyModifiers());

  pointer.positionRoot = rootPoint.Position();
  pointer.positionView = point.Position();
  pointer.timestamp = point.Timestamp() / 1000; // us -> ms
  pointer.pressure = props.Pressure();
  pointer.isBarrelButton = props.IsBarrelButtonPressed();
  pointer.shiftKey = 0 != (keyModifiers & static_cast<uint32_t>(winrt::Windows::System::VirtualKeyModifiers::Shift));
  pointer.ctrlKey = 0 != (keyModifiers & static_cast<uint32_t>(winrt::Windows::System::VirtualKeyModifiers::Control));
  pointer.altKey = 0 != (keyModifiers & static_cast<uint32_t>(winrt::Windows::System::VirtualKeyModifiers::Menu));
}

std::optional<size_t> TouchEventHandler::IndexOfPointerWithId(uint32_t pointerId) {
  for (size_t i = 0; i < m_pointers.size(); ++i) {
    if (m_pointers[i].pointerId == pointerId)
      return i;
  }

  return std::nullopt;
}

void TouchEventHandler::UpdatePointersInViews(
    const winrt::PointerRoutedEventArgs &args,
    xaml::UIElement sourceElement,
    std::vector<int64_t> &&newViews) {
  if (auto nativeUiManager = GetNativeUIManager(*m_context).lock()) {
    auto puiManagerHost = nativeUiManager->getHost();
    int32_t pointerId = args.Pointer().PointerId();

    // m_pointers is tracking the pointers that are 'down', for moves we usually
    // don't have any pointers down and should reset the touchId back to zero
    if (m_pointers.size() == 0)
      m_touchId = 0;

    // Get the results of the last time we calculated the path
    auto it = m_pointersInViews.find(pointerId);
    TagSet *existingViews;
    if (it != m_pointersInViews.end()) {
      existingViews = &it->second;
    } else {
      existingViews = nullptr;
    }

    // Short-circuit if the hierarchy hasn't changed
    if ((existingViews == nullptr && newViews.size() == 0) ||
        (existingViews != nullptr && existingViews->orderedTags == newViews))
      return;

    // Prep to fire pointer events
    std::unordered_set<int64_t> newViewsSet(newViews.begin(), newViews.end());
    ReactPointer pointer;

    auto optPointerIndex = IndexOfPointerWithId(pointerId);
    if (optPointerIndex) {
      pointer = m_pointers[*optPointerIndex];
      UpdateReactPointer(pointer, args, sourceElement);
    } else {
      // newViews is empty when UpdatePointersInViews is called from outside
      // the root view, in this case use -1 for the JS event pointer target
      const auto tag = !newViews.empty() ? newViews.front() : -1;
      pointer = CreateReactPointer(args, tag, sourceElement);
    }

    // Walk through existingViews from innermost to outer, firing mouseLeave events if they are not in newViews
    if (existingViews) {
      for (int64_t existingTag : existingViews->orderedTags) {
        if (newViewsSet.count(existingTag)) {
          continue;
        }

        ShadowNodeBase *node = static_cast<ShadowNodeBase *>(puiManagerHost->FindShadowNodeForTag(existingTag));
        if (node != nullptr && node->m_onMouseLeaveRegistered)
          BatchingEmitter().DispatchEvent(
              existingTag,
              L"topMouseLeave",
              winrt::Microsoft::ReactNative::MakeJSValueWriter(GetPointerJson(pointer, existingTag)));
      }
    }

    // Walk through newViews from outermost to inner, firing mouseEnter events if they are not in existingViews
    for (auto iter = newViews.rbegin(); iter != newViews.rend(); ++iter) {
      int64_t newTag = *iter;
      if (existingViews && existingViews->tags.count(newTag)) {
        continue;
      }

      ShadowNodeBase *node = static_cast<ShadowNodeBase *>(puiManagerHost->FindShadowNodeForTag(newTag));
      if (node != nullptr && node->m_onMouseEnterRegistered)
        BatchingEmitter().DispatchEvent(
            newTag,
            L"topMouseEnter",
            winrt::Microsoft::ReactNative::MakeJSValueWriter(GetPointerJson(pointer, newTag)));
    }

    m_pointersInViews[pointerId] = {std::move(newViewsSet), std::move(newViews)};
  }
}

winrt::Microsoft::ReactNative::JSValue TouchEventHandler::GetPointerJson(const ReactPointer &pointer, int64_t target) {
  return winrt::Microsoft::ReactNative::JSValueObject{
      {"target", target},
      {"identifier", pointer.identifier},
      {"pageX", pointer.positionRoot.X},
      {"pageY", pointer.positionRoot.Y},
      {"locationX", pointer.positionView.X},
      {"locationY", pointer.positionView.Y},
      {"timestamp", pointer.timestamp},
      {
          "pointerType",
          GetPointerDeviceTypeName(pointer.deviceType),
      },
      {"force", pointer.pressure},
      {"isLeftButton", pointer.isLeftButton},
      {"isRightButton", pointer.isRightButton},
      {"isMiddleButton", pointer.isMiddleButton},
      {"isBarrelButtonPressed", pointer.isBarrelButton},
      {"isHorizontalScrollWheel", pointer.isHorizontalScrollWheel},
      {"isEraser", pointer.isEraser},
      {"shiftKey", pointer.shiftKey},
      {"ctrlKey", pointer.ctrlKey},
      {"altKey", pointer.altKey}};
}

#ifdef USE_FABRIC
// This work should probably be delegated to the ComponentViews
facebook::react::SharedEventEmitter EventEmitterForElement(
    std::shared_ptr<FabricUIManager> &uimanager,
    facebook::react::Tag tag) noexcept {
  auto &registry = uimanager->GetViewRegistry();

  auto descriptor = registry.componentViewDescriptorWithTag(tag);
  auto view = std::static_pointer_cast<BaseComponentView const>(descriptor.view);
  auto emitter = view->GetEventEmitter();
  if (emitter)
    return emitter;

  auto element = view->Element();
  while (auto parent = element.Parent()) {
    if (element = parent.try_as<xaml::FrameworkElement>()) {
      auto boxedTag = element.Tag();
      if (boxedTag) {
        if (tag = winrt::unbox_value<facebook::react::Tag>(element.Tag()))
          return EventEmitterForElement(uimanager, tag);
      }
    }
  }
  return nullptr;
}

facebook::react::Touch TouchEventHandler::TouchForPointer(const ReactPointer &pointer) noexcept {
  facebook::react::Touch t;
  t.force = pointer.pressure;
  t.identifier = static_cast<int>(pointer.identifier);
  t.pagePoint.x = pointer.positionRoot.X; // TODO: This should be relative to the rootview, not the XAML tree
  t.pagePoint.y = pointer.positionRoot.Y; // TODO: This should be relative to the rootview, not the XAML tree
  t.screenPoint.x = pointer.positionRoot.X;
  t.screenPoint.y = pointer.positionRoot.Y;
  t.offsetPoint.x = pointer.positionView.X;
  t.offsetPoint.y = pointer.positionView.Y;
  t.target = static_cast<facebook::react::Tag>(pointer.target);
  t.timestamp = static_cast<facebook::react::Float>(pointer.timestamp);
  return t;
}
#endif

void TouchEventHandler::DispatchTouchEvent(TouchEventType eventType, size_t pointerIndex) {
  winrt::Microsoft::ReactNative::JSValueArray changedIndices;
  changedIndices.push_back(pointerIndex);

#ifdef USE_FABRIC
  std::shared_ptr<FabricUIManager> fabricuiManager;
  if (m_fabric &&
      !!(fabricuiManager = ::Microsoft::ReactNative::FabricUIManager::FromProperties(
             winrt::Microsoft::ReactNative::ReactPropertyBag(m_context->Properties())))) {
    std::unordered_set<facebook::react::SharedTouchEventEmitter> uniqueEventEmitters = {};
    std::vector<facebook::react::SharedTouchEventEmitter> emittersForIndex;

    facebook::react::TouchEvent te;

    size_t index = 0;
    for (const auto &pointer : m_pointers) {
      te.touches.insert(TouchForPointer(pointer));
      if (pointerIndex == index++)
        te.changedTouches.insert(TouchForPointer(pointer));

      auto emitter = std::static_pointer_cast<facebook::react::TouchEventEmitter>(
          std::const_pointer_cast<facebook::react::EventEmitter>(
              EventEmitterForElement(fabricuiManager, static_cast<facebook::react::Tag>(pointer.target))));
      emittersForIndex.push_back(emitter);
      if (emitter)
        uniqueEventEmitters.insert(emitter);
    }

    for (const auto emitter : uniqueEventEmitters) {
      te.targetTouches.clear();
      index = 0;
      for (const auto &pointer : m_pointers) {
        auto pointerEmitter = emittersForIndex[index++];
        if (emitter == pointerEmitter)
          te.targetTouches.insert(TouchForPointer(pointer));
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
  } else
#endif // USE_FABRIC
  {
    winrt::Microsoft::ReactNative::JSValueArray touches;
    for (const auto &pointer : m_pointers) {
      touches.push_back(GetPointerJson(pointer, pointer.target));
    }

    // Package up parameters and invoke the JS event emitter
    const wchar_t *eventName = GetTouchEventTypeName(eventType);
    if (eventName == nullptr)
      return;

    const auto paramsWriter = MakeJSValueArgWriter(eventName, std::move(touches), std::move(changedIndices));
    if (eventType == TouchEventType::Move || eventType == TouchEventType::PointerMove) {
      BatchingEmitter().EmitCoalescingJSEvent(
          L"RCTEventEmitter",
          L"receiveTouches",
          std::move(eventName),
          m_pointers[pointerIndex].pointerId,
          paramsWriter);
    } else {
      BatchingEmitter().EmitJSEvent(L"RCTEventEmitter", L"receiveTouches", paramsWriter);
    }
  }
}

bool TouchEventHandler::DispatchBackEvent() {
  if (m_context->State() != Mso::React::ReactInstanceState::Loaded)
    return false;

  BatchingEmitter().EmitJSEvent(
      L"RCTDeviceEventEmitter", L"emit", winrt::Microsoft::ReactNative::MakeJSValueArgWriter(L"hardwardBackPress"));

  return true;
}

const char *TouchEventHandler::GetPointerDeviceTypeName(
    winrt::Windows::Devices::Input::PointerDeviceType deviceType) noexcept {
  const char *deviceTypeName = "unknown";
  switch (deviceType) {
    case winrt::Windows::Devices::Input::PointerDeviceType::Mouse:
      deviceTypeName = "mouse";
      break;
    case winrt::Windows::Devices::Input::PointerDeviceType::Pen:
      deviceTypeName = "pen";
      break;
    case winrt::Windows::Devices::Input::PointerDeviceType::Touch:
      deviceTypeName = "touch";
      break;
    default:
      break;
  }
  return deviceTypeName;
}

winrt::Microsoft::ReactNative::PointerEventKind TouchEventHandler::GetPointerEventKind(
    TouchEventType eventType) noexcept {
  auto kind = winrt::Microsoft::ReactNative::PointerEventKind::None;
  switch (eventType) {
    case TouchEventType::Start:
      kind = winrt::Microsoft::ReactNative::PointerEventKind::Start;
      break;
    case TouchEventType::End:
      kind = winrt::Microsoft::ReactNative::PointerEventKind::End;
      break;
    case TouchEventType::Move:
      kind = winrt::Microsoft::ReactNative::PointerEventKind::Move;
      break;
    case TouchEventType::Cancel:
      kind = winrt::Microsoft::ReactNative::PointerEventKind::Cancel;
      break;
    case TouchEventType::CaptureLost:
      kind = winrt::Microsoft::ReactNative::PointerEventKind::CaptureLost;
      break;
    default:
      assert(false);
      break;
  }
  return kind;
}

const wchar_t *TouchEventHandler::GetTouchEventTypeName(TouchEventType eventType) noexcept {
  const wchar_t *eventName = nullptr;
  switch (eventType) {
    case TouchEventType::Start:
      eventName = L"topTouchStart";
      break;
    case TouchEventType::End:
      eventName = L"topTouchEnd";
      break;
    case TouchEventType::Move:
      eventName = L"topTouchMove";
      break;
    case TouchEventType::Cancel:
    case TouchEventType::CaptureLost:
      eventName = L"topTouchCancel";
      break;
    default:
      assert(false);
      break;
  }
  return eventName;
}

bool TouchEventHandler::PropagatePointerEventAndFindReactTarget(
    const winrt::Microsoft::ReactNative::ReactPointerEventArgs &args,
    std::vector<int64_t> *pTagsForBranch,
    xaml::UIElement *pSourceElement) {
  assert(pTagsForBranch != nullptr);
  assert(pSourceElement != nullptr);

  if (const auto uiManager = GetNativeUIManager(*m_context).lock()) {
    xaml::UIElement sourceElement = args.Args().OriginalSource().try_as<xaml::UIElement>();
    ShadowNodeBase *node = nullptr;
    std::vector<int64_t> tagsForBranch;

    // Find the "deepest" React element that triggered the input event
    while (sourceElement) {
      node = static_cast<ShadowNodeBase *>(uiManager->getHost()->FindShadowNodeForTag(GetTag(sourceElement)));
      if (node) {
        args.Target(sourceElement);
        break;
      } else {
        sourceElement = winrt::VisualTreeHelper::GetParent(sourceElement).try_as<xaml::UIElement>();
      }
    }

    // Walk to root to find refined React target view
    const auto argsImpl = winrt::get_self<winrt::Microsoft::ReactNative::implementation::ReactPointerEventArgs>(args);
    while (node) {
      if (args.Target() == nullptr) {
        args.Target(node->GetView());
      }

      const auto previousTarget = args.Target();

      node->GetViewManager()->OnPointerEvent(node, args);

      if (args.Target() != previousTarget) {
        tagsForBranch.clear();
        if (const auto target = args.Target().try_as<XamlView>()) {
          // We assume that if a ViewManager is going to change the target, it
          // can only update the target to one of its descendants.
          const auto tagsToCurrentTarget = GetTagsForBranch(uiManager->getHost(), GetTag(target), node->m_tag);
          for (auto tag : tagsToCurrentTarget) {
            tagsForBranch.push_back(tag);
          }
        }
      }

      if (args.Target() != nullptr) {
        tagsForBranch.push_back(node->m_tag);
      }

      node = static_cast<ShadowNodeBase *>(uiManager->getHost()->FindShadowNodeForTag(node->m_parent));
    }

    if (args.Target() != nullptr) {
      sourceElement = args.Target().try_as<xaml::UIElement>();

      // Find the first parent UIElement of the React target for pointer positioning
      if (!sourceElement) {
        node = static_cast<ShadowNodeBase *>(uiManager->getHost()->FindShadowNodeForTag(tagsForBranch.front()));
      }

      while (!sourceElement && node) {
        node = static_cast<ShadowNodeBase *>(uiManager->getHost()->FindShadowNodeForTag(node->m_parent));
        sourceElement = node->GetView().try_as<xaml::UIElement>();
      }

      if (sourceElement) {
        // If a TextBlock was the UIElement event source, perform a more accurate hit test,
        // searching for the tag of the nested Run/Span XAML elements that the user actually clicked.
        // This is to support nested <Text> elements in React.
        // Nested React <Text> elements get translated into nested XAML <Span> elements,
        // while the content of the <Text> becomes a list of XAML <Run> elements.
        // However, we should report the Text element as the target, not the contexts of the text.
        if (auto tag = GetTagAsPropertyValue(args.Target().as<XamlView>())) {
          if (const auto textBlock = sourceElement.try_as<xaml::Controls::TextBlock>()) {
            const auto pointerPos = args.Args().GetCurrentPoint(textBlock).RawPosition();
            const auto inlines = textBlock.Inlines().GetView();
            bool isHit = false;
            const auto finerTag = TestHit(inlines, pointerPos, isHit);
            if (finerTag) {
              const auto tagsToCurrentTarget = GetTagsForBranch(uiManager->getHost(), GetTag(finerTag), GetTag(tag));
              for (auto tag : tagsToCurrentTarget) {
                tagsForBranch.insert(tagsForBranch.begin(), tag);
              }
            }
          }
        }

        *pTagsForBranch = std::move(tagsForBranch);
        *pSourceElement = sourceElement;
        return true;
      }
    }
  }

  // If the root view is not fully created, then the Tag property will never
  // be set. This can happen, e.g., when the red box error box is shown.
  return false;
}

winrt::IPropertyValue TouchEventHandler::TestHit(
    const winrt::Collections::IVectorView<xaml::Documents::Inline> &inlines,
    const winrt::Point &pointerPos,
    bool &isHit) {
  winrt::IPropertyValue tag(nullptr);

  for (const auto &el : inlines) {
    if (const auto span = el.try_as<xaml::Documents::Span>()) {
      auto resTag = TestHit(span.Inlines().GetView(), pointerPos, isHit);

      if (resTag)
        return resTag;

      if (isHit) {
        tag = el.GetValue(xaml::FrameworkElement::TagProperty()).try_as<winrt::IPropertyValue>();
        if (tag) {
          return tag;
        }
      }
    } else if (const auto run = el.try_as<xaml::Documents::Run>()) {
      const auto start = el.ContentStart();
      const auto end = el.ContentEnd();

      auto startRect = start.GetCharacterRect(xaml::Documents::LogicalDirection::Forward);
      auto endRect = end.GetCharacterRect(xaml::Documents::LogicalDirection::Forward);

      // Swap rectangles in RTL scenarios.
      if (startRect.X > endRect.X) {
        const auto tempRect = startRect;
        startRect = endRect;
        endRect = tempRect;
      }

      // Approximate the bounding rect (for now, don't account for text wrapping).
      if ((startRect.X <= pointerPos.X) && (endRect.X + endRect.Width >= pointerPos.X) &&
          (startRect.Y <= pointerPos.Y) && (endRect.Y + endRect.Height >= pointerPos.Y)) {
        isHit = true;
        return nullptr;
      }
    }
  }

  return tag;
}

//
// Retreives the path of nodes from an element to the root.
// The order of the returned list is from child to parent.
//
std::vector<int64_t> GetTagsForBranch(INativeUIManagerHost *host, int64_t tag, int64_t rootTag) {
  std::vector<int64_t> tags;

  auto *shadowNode = host->FindShadowNodeForTag(tag);
  while (shadowNode != nullptr && tag != -1) {
    if (tag == rootTag) {
      break;
    }

    tags.push_back(tag);
    tag = shadowNode->m_parent;
    shadowNode = host->FindShadowNodeForTag(tag);
  }

  return tags;
}

} // namespace Microsoft::ReactNative
