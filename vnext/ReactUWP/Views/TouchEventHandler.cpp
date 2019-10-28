// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include <pch.h>

#include <Views/ShadowNodeBase.h>
#include "TouchEventHandler.h"

#include <Modules/NativeUIManager.h>
#include <Utils/ValueUtils.h>

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

  m_pressedRevoker.revoke();
  m_pressedRevoker = uiElement.PointerPressed(winrt::auto_revoke, {this, &TouchEventHandler::OnPointerPressed});
  m_releasedRevoker.revoke();
  m_releasedRevoker = uiElement.PointerReleased(winrt::auto_revoke, {this, &TouchEventHandler::OnPointerReleased});
  m_canceledRevoker.revoke();
  m_canceledRevoker = uiElement.PointerCanceled(winrt::auto_revoke, {this, &TouchEventHandler::OnPointerCanceled});
  m_captureLostRevoker.revoke();
  m_captureLostRevoker =
      uiElement.PointerCaptureLost(winrt::auto_revoke, {this, &TouchEventHandler::OnPointerCaptureLost});
  m_exitedRevoker.revoke();
  m_exitedRevoker = uiElement.PointerExited(winrt::auto_revoke, {this, &TouchEventHandler::OnPointerExited});
  m_movedRevoker.revoke();
  m_movedRevoker = uiElement.PointerMoved(winrt::auto_revoke, {this, &TouchEventHandler::OnPointerMoved});
}

void TouchEventHandler::RemoveTouchHandlers() {
  m_pressedRevoker.revoke();
  m_releasedRevoker.revoke();
}

void TouchEventHandler::OnPointerPressed(const winrt::IInspectable &sender, const winrt::PointerRoutedEventArgs &args) {
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
  winrt::FrameworkElement sourceElement(nullptr);
  if (!TagFromOriginalSource(args, &tag, &sourceElement))
    return;

  if (m_xamlView.as<winrt::FrameworkElement>().CapturePointer(args.Pointer())) {
    // Pointer pressing updates the enter/leave state
    UpdatePointersInViews(args, tag, sourceElement);

    size_t pointerIndex = AddReactPointer(args, tag, sourceElement);
    DispatchTouchEvent(TouchEventType::Start, pointerIndex);
  }
}

void TouchEventHandler::OnPointerReleased(
    const winrt::IInspectable &sender,
    const winrt::PointerRoutedEventArgs &args) {
  OnPointerConcluded(TouchEventType::End, args);
}

void TouchEventHandler::OnPointerCanceled(
    const winrt::IInspectable &sender,
    const winrt::PointerRoutedEventArgs &args) {
  OnPointerConcluded(TouchEventType::Cancel, args);
}

void TouchEventHandler::OnPointerCaptureLost(
    const winrt::IInspectable &sender,
    const winrt::PointerRoutedEventArgs &args) {
  OnPointerConcluded(TouchEventType::Cancel, args);
}

void TouchEventHandler::OnPointerExited(const winrt::IInspectable &sender, const winrt::PointerRoutedEventArgs &args) {
  UpdatePointersInViews(args, -1, nullptr);
}

void TouchEventHandler::OnPointerMoved(const winrt::IInspectable &sender, const winrt::PointerRoutedEventArgs &args) {
  // Short circuit all of this if we are in an error state
  auto instance = m_wkReactInstance.lock();
  if (!instance || instance->IsInError())
    return;

  // Only if the view has a Tag can we process this
  int64_t tag;
  winrt::FrameworkElement sourceElement(nullptr);
  if (!TagFromOriginalSource(args, &tag, &sourceElement))
    return;

  auto optPointerIndex = IndexOfPointerWithId(args.Pointer().PointerId());
  if (optPointerIndex) {
    UpdateReactPointer(m_pointers[*optPointerIndex], args, sourceElement);
    DispatchTouchEvent(TouchEventType::Move, *optPointerIndex);
  } else {
    // Move with no buttons pressed
    UpdatePointersInViews(args, tag, sourceElement);
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

  const auto pointerIndex = *optPointerIndex;
  // if the view has a Tag, update the pointer info.
  // Regardless of that, ensure we Dispatch & cleanup the pointer
  int64_t tag;
  winrt::FrameworkElement sourceElement(nullptr);
  if (TagFromOriginalSource(args, &tag, &sourceElement))
    UpdateReactPointer(m_pointers[pointerIndex], args, sourceElement);

  DispatchTouchEvent(eventType, pointerIndex);

  m_pointers.erase(cbegin(m_pointers) + pointerIndex);
  if (m_pointers.size() == 0)
    m_touchId = 0;

  m_xamlView.as<winrt::FrameworkElement>().ReleasePointerCapture(args.Pointer());
}

size_t TouchEventHandler::AddReactPointer(
    const winrt::PointerRoutedEventArgs &args,
    int64_t tag,
    winrt::FrameworkElement sourceElement) {
  ReactPointer pointer = CreateReactPointer(args, tag, sourceElement);
  m_pointers.emplace_back(std::move(pointer));
  return m_pointers.size() - 1;
}

TouchEventHandler::ReactPointer TouchEventHandler::CreateReactPointer(
    const winrt::PointerRoutedEventArgs &args,
    int64_t tag,
    winrt::FrameworkElement sourceElement) {
  auto point = args.GetCurrentPoint(sourceElement);

  ReactPointer pointer;
  pointer.target = tag;
  pointer.identifier = m_touchId++;
  pointer.pointerId = point.PointerId();
  pointer.deviceType = point.PointerDevice().PointerDeviceType();
  pointer.isLeftButton = point.Properties().IsLeftButtonPressed();
  pointer.isRightButton = point.Properties().IsRightButtonPressed();
  pointer.isMiddleButton = point.Properties().IsMiddleButtonPressed();
  pointer.isHorizontalScrollWheel = point.Properties().IsHorizontalMouseWheel();
  pointer.isEraser = point.Properties().IsEraser();
  UpdateReactPointer(pointer, args, sourceElement);

  return pointer;
}

void TouchEventHandler::UpdateReactPointer(
    ReactPointer &pointer,
    const winrt::PointerRoutedEventArgs &args,
    winrt::FrameworkElement sourceElement) {
  auto rootPoint = args.GetCurrentPoint(m_xamlView.as<winrt::FrameworkElement>());
  auto point = args.GetCurrentPoint(sourceElement);

  pointer.positionRoot = rootPoint.Position();
  pointer.positionView = point.Position();
  pointer.timestamp = point.Timestamp() / 1000; // us -> ms
  pointer.pressure = point.Properties().Pressure();
  pointer.isBarrelButton = point.Properties().IsBarrelButtonPressed();
  pointer.shiftKey = !!static_cast<uint32_t>(args.KeyModifiers() & winrt::Windows::System::VirtualKeyModifiers::Shift);
  pointer.ctrlKey = !!static_cast<uint32_t>(args.KeyModifiers() & winrt::Windows::System::VirtualKeyModifiers::Control);
  pointer.altKey = !!static_cast<uint32_t>(args.KeyModifiers() & winrt::Windows::System::VirtualKeyModifiers::Menu);
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
    int64_t tag,
    winrt::FrameworkElement sourceElement) {
  auto instance = m_wkReactInstance.lock();
  if (!instance || instance->IsInError())
    return;

  int32_t pointerId = args.Pointer().PointerId();
  auto optPointerIndex = IndexOfPointerWithId(pointerId);
  ReactPointer pointer =
      (optPointerIndex) ? m_pointers[*optPointerIndex] : CreateReactPointer(args, tag, sourceElement);
  // m_pointers is tracking the pointers that are 'down', for moves we usually
  // don't have any pointers down and should reset the touchId back to zero
  if (m_pointers.size() == 0)
    m_touchId = 0;

  std::set<int64_t> newViews;
  if (tag != -1)
    newViews = GetTagsAtPoint(args);
  std::set<int64_t> existingViews;
  auto it = m_pointersInViews.find(pointerId);
  if (it != m_pointersInViews.end())
    existingViews = it->second;
  if (newViews == existingViews)
    return;

  auto nativeUiManager = static_cast<NativeUIManager *>(instance->NativeUIManager());
  facebook::react::INativeUIManagerHost *puiManagerHost = nativeUiManager->getHost();

  // Notify tags that are no longer in the list
  for (int64_t existingTag : existingViews) {
    if (newViews.count(existingTag))
      continue;

    ShadowNodeBase *node = static_cast<ShadowNodeBase *>(puiManagerHost->FindShadowNodeForTag(existingTag));
    if (node != nullptr && node->m_onMouseLeave)
      instance->DispatchEvent(existingTag, "topMouseLeave", GetPointerJson(pointer, existingTag));
  }

  // Notify tags new in the list
  for (int64_t newTag : newViews) {
    if (existingViews.count(newTag))
      continue;

    ShadowNodeBase *node = static_cast<ShadowNodeBase *>(puiManagerHost->FindShadowNodeForTag(newTag));
    if (node != nullptr && node->m_onMouseEnter)
      instance->DispatchEvent(newTag, "topMouseEnter", GetPointerJson(pointer, newTag));
  }

  m_pointersInViews[pointerId] = std::move(newViews);
}

void TouchEventHandler::SendPointerMove(
    const winrt::PointerRoutedEventArgs &args,
    int64_t tag,
    winrt::FrameworkElement sourceElement) {
  auto instance = m_wkReactInstance.lock();
  auto nativeUiManager = static_cast<NativeUIManager *>(instance->NativeUIManager());
  facebook::react::INativeUIManagerHost *puiManagerHost = nativeUiManager->getHost();

  // Look for an element subscribed to onMouseMove
  ShadowNodeBase *node = static_cast<ShadowNodeBase *>(puiManagerHost->FindShadowNodeForTag(tag));
  while (node != nullptr && !node->m_onMouseMove) {
    tag = node->m_parent;
    node = tag == -1 ? nullptr : node = static_cast<ShadowNodeBase *>(puiManagerHost->FindShadowNodeForTag(tag));
  }
  if (node == nullptr || !node->m_onMouseMove)
    return;
  sourceElement = node->GetView().try_as<winrt::FrameworkElement>();
  if (sourceElement == nullptr)
    return;

  auto optPointerIndex = IndexOfPointerWithId(args.Pointer().PointerId());
  ReactPointer pointer =
      (optPointerIndex) ? m_pointers[*optPointerIndex] : CreateReactPointer(args, tag, sourceElement);
  if (m_pointers.size() == 0) // If we created a reactPointer, reset the touchId back to zero
    m_touchId = 0;

  folly::dynamic touch = GetPointerJson(pointer, pointer.target);
  instance->DispatchEvent(tag, "topMouseEnter", GetPointerJson(pointer, tag));
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
    winrt::FrameworkElement *pSourceElement) {
  assert(pTag != nullptr);

  if (args.OriginalSource() == nullptr)
    return false;

  auto sourceElement = args.OriginalSource().try_as<winrt::FrameworkElement>();
  if (sourceElement == nullptr) {
    // TODO: Do we need to handle this for non FrameworkElement views?
    return false;
  } else {
    auto tag = sourceElement.Tag();
    while (tag == nullptr && sourceElement && winrt::VisualTreeHelper::GetParent(sourceElement)) {
      sourceElement = winrt::VisualTreeHelper::GetParent(sourceElement).try_as<winrt::FrameworkElement>();
      tag = sourceElement.Tag();
    }

    if (tag == nullptr) {
      // If the root view fails to be fully created, then the Tag property will
      // never be set. This can happen,
      //  for example, when the red box error box is shown.
      return false;
    }

    assert(tag.as<winrt::IPropertyValue>() != nullptr);

    *pTag = tag.as<winrt::IPropertyValue>().GetInt64();
    *pSourceElement = sourceElement;
    return true;
  }
}

std::set<int64_t> TouchEventHandler::GetTagsAtPoint(const winrt::PointerRoutedEventArgs &e) {
  std::set<int64_t> tags;
  winrt::UIElement root(m_xamlView.as<winrt::UIElement>());

  winrt::Point point = e.GetCurrentPoint(root).Position();
  auto transform = root.TransformToVisual(nullptr);
  point = transform.TransformPoint(point);

  auto elements = winrt::VisualTreeHelper::FindElementsInHostCoordinates(point, root);
  for (const auto &elem : elements) {
    auto element = elem.try_as<winrt::FrameworkElement>();
    if (element != nullptr) {
      auto tag = element.Tag();
      if (tag != nullptr)
        tags.insert(tag.as<winrt::IPropertyValue>().GetInt64());
    }
  }

  return tags;
}

} // namespace uwp
} // namespace react
