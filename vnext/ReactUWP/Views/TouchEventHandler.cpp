// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include <pch.h>

#include <Views/ShadowNodeBase.h>
#include "TouchEventHandler.h"

#include <Modules/NativeUIManager.h>
#include <Utils/ValueUtils.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Input.h>
#include <winrt/Windows.UI.Xaml.Media.h>

#include <winrt/Windows.ApplicationModel.Core.h>
#include <winrt/Windows.Devices.Input.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.UI.Core.h>
#include <winrt/Windows.UI.Input.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Input.h>
#include <winrt/Windows.UI.Xaml.Media.h>
#include <winrt/Windows.UI.Xaml.h>

namespace react {
namespace uwp {

std::vector<int64_t> GetTagsForBranch(facebook::react::INativeUIManagerHost *host, int64_t tag);

TouchEventHandler::TouchEventHandler(const std::weak_ptr<IReactInstance> &reactInstance)
    : m_xamlView(nullptr), m_wkReactInstance(reactInstance) {}

TouchEventHandler::~TouchEventHandler() {
  RemoveTouchHandlers();
}

void TouchEventHandler::AddTouchHandlers(XamlView xamlView) {
  auto uiElement(xamlView.as<winrt::UIElement>());
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

void TouchEventHandler::OnPointerPressed(
    const winrt::IInspectable & /*sender*/,
    const winrt::PointerRoutedEventArgs &args) {
  // Short circuit all of this if we are in an error state
  auto instance = m_wkReactInstance.lock();
  if (!instance || instance->IsInError())
    return;

  if (IndexOfPointerWithId(args.Pointer().PointerId()) != std::nullopt) {
    // A pointer with this ID already exists
    assert(false);
    return;
  }

  // Only if the view has a Tag can we process this
  int64_t tag;
  winrt::UIElement sourceElement(nullptr);
  if (!TagFromOriginalSource(args, &tag, &sourceElement))
    return;

  // If this was caused by the user pressing the "back" hardware button, fire that event instead
  if (args.GetCurrentPoint(sourceElement).Properties().PointerUpdateKind() ==
      winrt::Windows::UI::Input::PointerUpdateKind::XButton1Pressed) {
    args.Handled(DispatchBackEvent());
    return;
  }

  if (m_xamlView.as<winrt::FrameworkElement>().CapturePointer(args.Pointer())) {
    // Pointer pressing updates the enter/leave state
    UpdatePointersInViews(instance, args, tag, sourceElement);

    size_t pointerIndex = AddReactPointer(args, tag, sourceElement);
    DispatchTouchEvent(TouchEventType::Start, pointerIndex);
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
  OnPointerConcluded(TouchEventType::Cancel, args);
}

void TouchEventHandler::OnPointerExited(
    const winrt::IInspectable & /*sender*/,
    const winrt::PointerRoutedEventArgs &args) {
  // Short circuit all of this if we are in an error state
  auto instance = m_wkReactInstance.lock();
  if (!instance || instance->IsInError())
    return;

  UpdatePointersInViews(instance, args, -1, nullptr);
}

void TouchEventHandler::OnPointerMoved(
    const winrt::IInspectable & /*sender*/,
    const winrt::PointerRoutedEventArgs &args) {
  // Short circuit all of this if we are in an error state
  auto instance = m_wkReactInstance.lock();
  if (!instance || instance->IsInError())
    return;

  // Only if the view has a Tag can we process this
  int64_t tag;
  winrt::UIElement sourceElement(nullptr);
  if (!TagFromOriginalSource(args, &tag, &sourceElement))
    return;

  auto optPointerIndex = IndexOfPointerWithId(args.Pointer().PointerId());
  if (optPointerIndex) {
    UpdateReactPointer(m_pointers[*optPointerIndex], args, sourceElement);
    DispatchTouchEvent(TouchEventType::Move, *optPointerIndex);
  } else {
    // Move with no buttons pressed
    UpdatePointersInViews(instance, args, tag, sourceElement);
    // MouseMove support: (Not yet enabled, requires adding to ViewPropTypes.js)
    // SendPointerMove(args, tag, sourceElement);
  }
}

void TouchEventHandler::OnPointerConcluded(TouchEventType eventType, const winrt::PointerRoutedEventArgs &args) {
  // Short circuit all of this if we are in an error state
  auto instance = m_wkReactInstance.lock();
  if (!instance || instance->IsInError())
    return;

  auto optPointerIndex = IndexOfPointerWithId(args.Pointer().PointerId());
  if (!optPointerIndex)
    return;

  // if the view has a Tag, update the pointer info.
  // Regardless of that, ensure we Dispatch & cleanup the pointer
  int64_t tag;
  winrt::UIElement sourceElement(nullptr);
  if (TagFromOriginalSource(args, &tag, &sourceElement))
    UpdateReactPointer(m_pointers[*optPointerIndex], args, sourceElement);

  DispatchTouchEvent(eventType, *optPointerIndex);

  m_pointers.erase(cbegin(m_pointers) + *optPointerIndex);
  if (m_pointers.size() == 0)
    m_touchId = 0;

  m_xamlView.as<winrt::FrameworkElement>().ReleasePointerCapture(args.Pointer());
}

size_t TouchEventHandler::AddReactPointer(
    const winrt::PointerRoutedEventArgs &args,
    int64_t tag,
    winrt::UIElement sourceElement) {
  ReactPointer pointer = CreateReactPointer(args, tag, sourceElement);
  m_pointers.emplace_back(std::move(pointer));
  return m_pointers.size() - 1;
}

TouchEventHandler::ReactPointer TouchEventHandler::CreateReactPointer(
    const winrt::PointerRoutedEventArgs &args,
    int64_t tag,
    winrt::UIElement sourceElement) {
  auto point = args.GetCurrentPoint(sourceElement);
  auto props = point.Properties();

  ReactPointer pointer;
  pointer.target = tag;
  pointer.identifier = m_touchId++;
  pointer.pointerId = point.PointerId();
  pointer.deviceType = point.PointerDevice().PointerDeviceType();
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
    winrt::UIElement sourceElement) {
  auto rootPoint = args.GetCurrentPoint(m_xamlView.as<winrt::FrameworkElement>());
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
    std::shared_ptr<IReactInstance> instance,
    const winrt::PointerRoutedEventArgs &args,
    int64_t tag,
    winrt::UIElement sourceElement) {
  auto nativeUiManager = static_cast<NativeUIManager *>(instance->NativeUIManager());
  auto puiManagerHost = nativeUiManager->getHost();
  int32_t pointerId = args.Pointer().PointerId();

  // m_pointers is tracking the pointers that are 'down', for moves we usually
  // don't have any pointers down and should reset the touchId back to zero
  if (m_pointers.size() == 0)
    m_touchId = 0;

  // Get the branch of views under the pointer in leaf to root order
  std::vector<int64_t> newViews;
  if (tag != -1)
    newViews = GetTagsForBranch(puiManagerHost, tag);

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
    pointer = CreateReactPointer(args, tag, sourceElement);
  }

  // Walk through existingViews from innermost to outer, firing mouseLeave events if they are not in newViews
  if (existingViews) {
    for (int64_t existingTag : existingViews->orderedTags) {
      if (newViewsSet.count(existingTag)) {
        continue;
      }

      ShadowNodeBase *node = static_cast<ShadowNodeBase *>(puiManagerHost->FindShadowNodeForTag(existingTag));
      if (node != nullptr && node->m_onMouseLeave)
        instance->DispatchEvent(existingTag, "topMouseLeave", GetPointerJson(pointer, existingTag));
    }
  }

  // Walk through newViews from outermost to inner, firing mouseEnter events if they are not in existingViews
  for (auto iter = newViews.rbegin(); iter != newViews.rend(); ++iter) {
    int64_t newTag = *iter;
    if (existingViews && existingViews->tags.count(newTag)) {
      continue;
    }

    ShadowNodeBase *node = static_cast<ShadowNodeBase *>(puiManagerHost->FindShadowNodeForTag(newTag));
    if (node != nullptr && node->m_onMouseEnter)
      instance->DispatchEvent(newTag, "topMouseEnter", GetPointerJson(pointer, newTag));
  }

  m_pointersInViews[pointerId] = {std::move(newViewsSet), std::move(newViews)};
}

folly::dynamic TouchEventHandler::GetPointerJson(const ReactPointer &pointer, int64_t target) {
  folly::dynamic json =
      folly::dynamic::object()("target", target)("identifier", pointer.identifier)("pageX", pointer.positionRoot.X)(
          "pageY", pointer.positionRoot.Y)("locationX", pointer.positionView.X)("locationY", pointer.positionView.Y)(
          "timestamp", pointer.timestamp)("pointerType", GetPointerDeviceTypeName(pointer.deviceType))(
          "force", pointer.pressure)("isLeftButton", pointer.isLeftButton)("isRightButton", pointer.isRightButton)(
          "isMiddleButton", pointer.isMiddleButton)("isBarrelButtonPressed", pointer.isBarrelButton)(
          "isHorizontalScrollWheel", pointer.isHorizontalScrollWheel)("isEraser", pointer.isEraser)(
          "shiftKey", pointer.shiftKey)("ctrlKey", pointer.ctrlKey)("altKey", pointer.altKey);
  return json;
}

void TouchEventHandler::DispatchTouchEvent(TouchEventType eventType, size_t pointerIndex) {
  folly::dynamic changedIndices = folly::dynamic::array();
  changedIndices.push_back(pointerIndex);

  auto touches = folly::dynamic::array();
  for (const auto &pointer : m_pointers) {
    folly::dynamic touch = GetPointerJson(pointer, pointer.target);
    touches.push_back(touch);
  }

  // Package up parameters and invoke the JS event emitter
  const char *eventName = GetTouchEventTypeName(eventType);
  if (eventName == nullptr)
    return;
  folly::dynamic params = folly::dynamic::array(eventName, std::move(touches), std::move(changedIndices));

  auto instance = m_wkReactInstance.lock();
  instance->CallJsFunction("RCTEventEmitter", "receiveTouches", std::move(params));
}

bool TouchEventHandler::DispatchBackEvent() {
  auto instance = m_wkReactInstance.lock();
  if (instance != nullptr && !instance->IsInError()) {
    instance->CallJsFunction("RCTDeviceEventEmitter", "emit", folly::dynamic::array("hardwareBackPress"));
    return true;
  }

  return false;
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

const char *TouchEventHandler::GetTouchEventTypeName(TouchEventType eventType) noexcept {
  const char *eventName = nullptr;
  switch (eventType) {
    case TouchEventType::Start:
      eventName = "topTouchStart";
      break;
    case TouchEventType::End:
      eventName = "topTouchEnd";
      break;
    case TouchEventType::Move:
      eventName = "topTouchMove";
      break;
    case TouchEventType::Cancel:
      eventName = "topTouchCancel";
      break;
    default:
      assert(false);
      break;
  }
  return eventName;
}

bool TouchEventHandler::TagFromOriginalSource(
    const winrt::PointerRoutedEventArgs &args,
    int64_t *pTag,
    winrt::UIElement *pSourceElement) {
  assert(pTag != nullptr);
  assert(pSourceElement != nullptr);

  // Find the React element that triggered the input event
  winrt::UIElement sourceElement = args.OriginalSource().try_as<winrt::UIElement>();
  winrt::IPropertyValue tag(nullptr);
  while (sourceElement) {
    tag = sourceElement.GetValue(winrt::FrameworkElement::TagProperty()).try_as<winrt::IPropertyValue>();
    if (tag) {
      break;
    }
    sourceElement = winrt::VisualTreeHelper::GetParent(sourceElement).try_as<winrt::UIElement>();
  }

  if (tag == nullptr) {
    // If the root view fails to be fully created, then the Tag property will
    // never be set. This can happen,
    //  for example, when the red box error box is shown.
    return false;
  }

  *pTag = tag.GetInt64();
  *pSourceElement = sourceElement;
  return true;
}

//
// Retreives the path of nodes from an element to the root.
// The order of the returned list is from child to parent.
//
std::vector<int64_t> GetTagsForBranch(facebook::react::INativeUIManagerHost *host, int64_t tag) {
  std::vector<int64_t> tags;

  auto *shadowNode = host->FindShadowNodeForTag(tag);
  while (shadowNode != nullptr && tag != -1) {
    tags.push_back(tag);

    tag = shadowNode->m_parent;
    shadowNode = host->FindShadowNodeForTag(tag);
  }

  return tags;
}

} // namespace uwp
} // namespace react
