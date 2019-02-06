// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include <pch.h>
#include "ReactControl.h"

#include <ReactUWP/InstanceFactory.h>
#include <CxxMessageQueue.h>
#include "UnicodeConversion.h"
#include <Utils/ValueUtils.h>

#include <INativeUIManager.h>
#include <Views/ShadowNodeBase.h>

#include <winrt/Windows.ApplicationModel.Core.h>
#include <winrt/Windows.Devices.Input.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.UI.Core.h>
#include <winrt/Windows.UI.Input.h>
#include <winrt/Windows.UI.Xaml.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Input.h>
#include <winrt/Windows.UI.Xaml.Media.h>

namespace react {
namespace uwp {

ReactControl::~ReactControl()
{
  if (m_reactInstance != nullptr)
  {
    m_reactInstance->UnregisterLiveReloadCallback(m_liveReloadCallbackCookie);
    m_reactInstance->UnregisterErrorCallback(m_errorCallbackCookie);
  }
}

std::shared_ptr<IReactInstance> ReactControl::GetReactInstance() const noexcept
{
  return m_reactInstance;
}

void ReactControl::HandleInstanceError()
{
  m_reactInstance->DefaultNativeMessageQueueThread()->runOnQueue([this]()
  {
    if (m_reactInstance->IsInError())
    {
      auto xamlRootGrid(m_xamlRootView.as<winrt::Grid>());

      // Remove existing children from root view (from the hosted app)
      xamlRootGrid.Children().Clear();

      // Create Grid & TextBlock to hold error text
      if (m_errorTextBlock == nullptr)
      {
        m_errorTextBlock = winrt::TextBlock();
        m_redBoxGrid = winrt::Grid();
        m_redBoxGrid.Background(winrt::SolidColorBrush(winrt::Colors::Crimson()));
        m_redBoxGrid.Children().Append(m_errorTextBlock);
      }

      // Add red box grid to root view
      xamlRootGrid.Children().Append(m_redBoxGrid);

      // Place error message into TextBlock
      std::wstring wstrErrorMessage(L"ERROR: Instance failed to start.\n\n");
      wstrErrorMessage += facebook::react::UnicodeConversion::Utf8ToUtf16(m_reactInstance->LastErrorMessage()).c_str();
      m_errorTextBlock.Text(wstrErrorMessage);

      // Format TextBlock
      m_errorTextBlock.TextAlignment(winrt::TextAlignment::Center);
      m_errorTextBlock.FontFamily(winrt::FontFamily(L"Consolas"));
      m_errorTextBlock.Foreground(winrt::SolidColorBrush(winrt::Colors::White()));
      winrt::Thickness margin = { 10.0f, 10.0f, 10.0f, 10.0f };
      m_errorTextBlock.Margin(margin);
    }
  });
}

void ReactControl::AttachRoot() noexcept
{
  if (m_isAttached)
    return;

  if (!m_reactInstance)
    m_reactInstance = m_instanceCreator->getInstance();

  // Handle any errors that occurred during creation before we add our callback
  if (m_reactInstance->IsInError())
    HandleInstanceError();

  // Register callback from instance for live reload
  m_errorCallbackCookie = m_reactInstance->RegisterErrorCallback([this]()
  {
    HandleInstanceError();
  });

  // Register callback from instance for live reload
  m_liveReloadCallbackCookie = m_reactInstance->RegisterLiveReloadCallback([this]()
  {
    m_reactInstance->DefaultNativeMessageQueueThread()->runOnQueue([this]()
    {
      Reload(true);
    });
  });

  // Schedule initialization that must happen on the UI thread
  m_reactInstance->DefaultNativeMessageQueueThread()->runOnQueue([this]() {
    AddTouchHandlers();
    auto initialProps = m_initialProps;
    m_reactInstance->AttachMeasuredRootView(m_pParent, std::move(initialProps));
  });

  m_isAttached = true;
}

void ReactControl::DetachRoot() noexcept
{
  if (!m_isAttached)
    return;

  m_pressedRevoker.revoke();
  m_releasedRevoker.revoke();

  if (m_reactInstance != nullptr)
  {
    m_reactInstance->DetachRootView(m_pParent);

    // If the redbox error UI is shown we need to remove it, otherwise let the natural teardown process do this
    if (m_reactInstance->IsInError())
    {
      auto grid(m_xamlRootView.as<winrt::Grid>());
      if (grid != nullptr)
        grid.Children().Clear();

      m_redBoxGrid = nullptr;
      m_errorTextBlock = nullptr;
    }
  }

  m_isAttached = false;
}

void ReactControl::DetachInstance()
{
  if (m_reactInstance != nullptr)
  {
    m_reactInstance->UnregisterLiveReloadCallback(m_liveReloadCallbackCookie);
    m_reactInstance->UnregisterErrorCallback(m_errorCallbackCookie);
    m_reactInstance.reset();
  }
}

void ReactControl::Reload(bool shouldRetireCurrentInstance)
{
  // DetachRoot the current view and detach it
  DetachRoot();

  m_reactInstance->DefaultNativeMessageQueueThread()->runOnQueue([this, shouldRetireCurrentInstance]() {

    if (shouldRetireCurrentInstance && m_reactInstance != nullptr)
      m_instanceCreator->markAsNeedsReload();

    // Detach ourselves from the instance
    DetachInstance();

    // Restart the view which will re-create the instance and attach the root view
    AttachRoot();
  });
}

void ReactControl::AddTouchHandlers()
{
  auto grid(m_xamlRootView.as<winrt::Grid>());
  if (grid == nullptr)
  {
    assert(false);
    return;
  }

  m_pressedRevoker.revoke();
  m_pressedRevoker = grid.PointerPressed(winrt::auto_revoke, { this, &ReactControl::OnPointerPressed });
  m_releasedRevoker.revoke();
  m_releasedRevoker = grid.PointerReleased(winrt::auto_revoke, { this, &ReactControl::OnPointerReleased });
  m_canceledRevoker.revoke();
  m_canceledRevoker = grid.PointerCanceled(winrt::auto_revoke, { this, &ReactControl::OnPointerCanceled });
  m_captureLostRevoker.revoke();
  m_captureLostRevoker = grid.PointerCaptureLost(winrt::auto_revoke, { this, &ReactControl::OnPointerCaptureLost });
  m_exitedRevoker.revoke();
  m_exitedRevoker = grid.PointerExited(winrt::auto_revoke, { this, &ReactControl::OnPointerExited });
  m_movedRevoker.revoke();
  m_movedRevoker = grid.PointerMoved(winrt::auto_revoke, { this, &ReactControl::OnPointerMoved });
}

bool ReactControl::TagFromOriginalSource(const winrt::PointerRoutedEventArgs& args, int64_t* pTag, winrt::FrameworkElement* pSourceElement)
{
  assert(pTag != nullptr);

  if (args.OriginalSource() == nullptr)
    return false;

  auto sourceElement = args.OriginalSource().try_as<winrt::FrameworkElement>();
  if (sourceElement == nullptr)
  {
    // TODO: Do we need to handle this for non FrameworkElement views?
    return false;
  }
  else
  {
    auto tag = sourceElement.Tag();
    while (tag == nullptr && sourceElement && winrt::VisualTreeHelper::GetParent(sourceElement))
    {
      sourceElement = winrt::VisualTreeHelper::GetParent(sourceElement).try_as<winrt::FrameworkElement>();
      tag = sourceElement.Tag();
    }

    if (tag == nullptr)
    {
      // If the root view fails to be fully created, then the Tag property will never be set. This can happen,
      //  for example, when the red box error box is shown.
      return false;
    }

    assert(tag.as<winrt::IPropertyValue>() != nullptr);

    *pTag = tag.as<winrt::IPropertyValue>().GetInt64();
    *pSourceElement = sourceElement;
    return true;
  }
}

std::set<int64_t> ReactControl::GetTagsAtPoint(const winrt::PointerRoutedEventArgs& e)
{
  std::set<int64_t> tags;
  winrt::UIElement root(m_xamlRootView.as<winrt::UIElement>());

  winrt::Point point = e.GetCurrentPoint(root).Position();
  auto transform = root.TransformToVisual(winrt::Window::Current().Content());
  point = transform.TransformPoint(point);

  auto elements = winrt::VisualTreeHelper::FindElementsInHostCoordinates(point, root);
  for (const auto& elem : elements)
  {
    auto element = elem.try_as<winrt::FrameworkElement>();
    if (element != nullptr)
    {
      auto tag = element.Tag();
      if (tag != nullptr)
        tags.insert(tag.as<winrt::IPropertyValue>().GetInt64());
    }
  }

  return tags;
}

void ReactControl::OnPointerPressed(const winrt::IInspectable& sender, const winrt::PointerRoutedEventArgs& args)
{
  // Short circuit all of this if we are in an error state
  if (m_reactInstance->IsInError())
    return;

  if (IndexOfPointerWithId(args.Pointer().PointerId()) != s_InvalidPointerId)
  {
    // A pointer with this ID already exists
    assert(false);
    return;
  }

  // Only if the view has a Tag can we process this
  int64_t tag;
  winrt::FrameworkElement sourceElement(nullptr);
  if (!TagFromOriginalSource(args, &tag, &sourceElement))
    return;

  if (m_xamlRootView.as<winrt::FrameworkElement>().CapturePointer(args.Pointer()))
  {
    // Pointer pressing updates the enter/leave state
    UpdatePointersInViews(args, tag, sourceElement);

    size_t pointerIndex = AddReactPointer(args, tag, sourceElement);
    DispatchTouchEvent(TouchEventType::Start, pointerIndex);
  }
}

void ReactControl::OnPointerReleased(const winrt::IInspectable& sender, const winrt::PointerRoutedEventArgs& args)
{
  OnPointerConcluded(TouchEventType::End, args);
}

void ReactControl::OnPointerCanceled(const winrt::IInspectable& sender, const winrt::PointerRoutedEventArgs& args)
{
  OnPointerConcluded(TouchEventType::Cancel, args);
}

void ReactControl::OnPointerCaptureLost(const winrt::IInspectable& sender, const winrt::PointerRoutedEventArgs& args)
{
  OnPointerConcluded(TouchEventType::Cancel, args);
}

void ReactControl::OnPointerExited(const winrt::IInspectable& sender, const winrt::PointerRoutedEventArgs& args)
{
  UpdatePointersInViews(args, -1, nullptr);
}

void ReactControl::OnPointerMoved(const winrt::IInspectable& sender, const winrt::PointerRoutedEventArgs& args)
{
  // Short circuit all of this if we are in an error state
  if (m_reactInstance->IsInError())
    return;

  // Only if the view has a Tag can we process this
  int64_t tag;
  winrt::FrameworkElement sourceElement(nullptr);
  if (!TagFromOriginalSource(args, &tag, &sourceElement))
    return;

  size_t pointerIndex = IndexOfPointerWithId(args.Pointer().PointerId());
  if (pointerIndex != s_InvalidPointerId)
  {
    UpdateReactPointer(m_pointers[pointerIndex], args, sourceElement);
    DispatchTouchEvent(TouchEventType::Move, pointerIndex);
  }
  else
  {
    // Move with no buttons pressed
    UpdatePointersInViews(args, tag, sourceElement);
    // MouseMove support: (Not yet enabled, requires adding to ViewPropTypes.js)
    // SendPointerMove(args, tag, sourceElement);
  }
}

void ReactControl::OnPointerConcluded(TouchEventType eventType, const winrt::PointerRoutedEventArgs& args)
{
  // Short circuit all of this if we are in an error state
  if (m_reactInstance->IsInError())
    return;

  size_t pointerIndex = IndexOfPointerWithId(args.Pointer().PointerId());
  if (pointerIndex == s_InvalidPointerId)
    return;

  // Only if the view has a Tag can we process this
  int64_t tag;
  winrt::FrameworkElement sourceElement(nullptr);
  if (!TagFromOriginalSource(args, &tag, &sourceElement))
    return;

  UpdateReactPointer(m_pointers[pointerIndex], args, sourceElement);
  DispatchTouchEvent(eventType, pointerIndex);

  m_pointers.erase(cbegin(m_pointers) + pointerIndex);
  if (m_pointers.size() == 0)
    m_touchId = 0;

  m_xamlRootView.as<winrt::FrameworkElement>().ReleasePointerCapture(args.Pointer());
}


size_t ReactControl::AddReactPointer(const winrt::PointerRoutedEventArgs& args, int64_t tag, winrt::FrameworkElement sourceElement)
{
  ReactPointer pointer = CreateReactPointer(args, tag, sourceElement);
  m_pointers.emplace_back(std::move(pointer));
  return m_pointers.size() - 1;
}

ReactControl::ReactPointer ReactControl::CreateReactPointer(const winrt::PointerRoutedEventArgs& args, int64_t tag, winrt::FrameworkElement sourceElement)
{
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


void ReactControl::UpdateReactPointer(ReactPointer& pointer, const winrt::PointerRoutedEventArgs& args, winrt::FrameworkElement sourceElement)
{
  auto rootPoint = args.GetCurrentPoint(m_xamlRootView.as<winrt::FrameworkElement>());
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

size_t ReactControl::IndexOfPointerWithId(uint32_t pointerId)
{
  for (size_t i = 0; i < m_pointers.size(); ++i)
  {
    if (m_pointers[i].pointerId == pointerId)
      return i;
  }

  return s_InvalidPointerId;
}

void ReactControl::UpdatePointersInViews(const winrt::PointerRoutedEventArgs& args, int64_t tag, winrt::FrameworkElement sourceElement)
{
    int32_t pointerId = args.Pointer().PointerId();
    size_t pointerIndex = IndexOfPointerWithId(pointerId);
    ReactPointer pointer = (pointerIndex == s_InvalidPointerId) ? CreateReactPointer(args, tag, sourceElement) : m_pointers[pointerIndex];
    // m_pointers is tracking the pointers that are 'down', for moves we usually don't have any pointers
    // down and should reset the touchId back to zero
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

    facebook::react::INativeUIManagerHost* puiManagerHost = m_reactInstance->NativeUIManager()->getHost();

    // Notify tags that are no longer in the list
    for (int64_t existingTag : existingViews)
    {
      if (newViews.count(existingTag))
        continue;

      ShadowNodeBase* node = static_cast<ShadowNodeBase*>(puiManagerHost->FindShadowNodeForTag(existingTag));
      if (node != nullptr && node->m_onMouseLeave)
        m_reactInstance->DispatchEvent(existingTag, "topMouseLeave", GetPointerJson(pointer, existingTag));
    }

    // Notify tags new in the list
    for (int64_t newTag : newViews)
    {
      if (existingViews.count(newTag))
        continue;

      ShadowNodeBase* node = static_cast<ShadowNodeBase*>(puiManagerHost->FindShadowNodeForTag(newTag));
      if (node != nullptr && node->m_onMouseEnter)
        m_reactInstance->DispatchEvent(newTag, "topMouseEnter", GetPointerJson(pointer, newTag));
    }

    m_pointersInViews[pointerId] = std::move(newViews);
}

void ReactControl::SendPointerMove(const winrt::PointerRoutedEventArgs& args, int64_t tag, winrt::FrameworkElement sourceElement)
{
  facebook::react::INativeUIManagerHost* puiManagerHost = m_reactInstance->NativeUIManager()->getHost();

  // Look for an element subscribed to onMouseMove
  ShadowNodeBase* node = static_cast<ShadowNodeBase*>(puiManagerHost->FindShadowNodeForTag(tag));
  while (node != nullptr && !node->m_onMouseMove)
  {
    tag = node->m_parent;
    node = tag == -1 ? nullptr : node = static_cast<ShadowNodeBase*>(puiManagerHost->FindShadowNodeForTag(tag));
  }
  if (node == nullptr || !node->m_onMouseMove)
    return;
  sourceElement = node->GetView().try_as<winrt::FrameworkElement>();
  if (sourceElement == nullptr)
    return;

  size_t pointerIndex = IndexOfPointerWithId(args.Pointer().PointerId());
  ReactPointer pointer = (pointerIndex == s_InvalidPointerId) ? CreateReactPointer(args, tag, sourceElement) : m_pointers[pointerIndex];
  if (m_pointers.size() == 0) // If we created a reactPointer, reset the touchId back to zero
    m_touchId = 0;

  folly::dynamic touch = GetPointerJson(pointer, s_InvalidPointerId);
  m_reactInstance->DispatchEvent(tag, "topMouseEnter", GetPointerJson(pointer, tag));
}


folly::dynamic ReactControl::GetPointerJson(const ReactPointer& pointer, size_t targetOverride)
{
  folly::dynamic json = folly::dynamic::object()
    ("target", targetOverride != s_InvalidPointerId ? targetOverride : pointer.target)
    ("identifier", pointer.identifier)
    ("pageX", pointer.positionRoot.X)
    ("pageY", pointer.positionRoot.Y)
    ("locationX", pointer.positionView.X)
    ("locationY", pointer.positionView.Y)
    ("timestamp", pointer.timestamp)
    ("pointerType", GetPointerDeviceTypeName(pointer.deviceType))
    ("force", pointer.pressure)
    ("isLeftButton", pointer.isLeftButton)
    ("isRightButton", pointer.isRightButton)
    ("isMiddleButton", pointer.isMiddleButton)
    ("isBarrelButtonPressed", pointer.isBarrelButton)
    ("isHorizontalScrollWheel", pointer.isHorizontalScrollWheel)
    ("isEraser", pointer.isEraser)
    ("shiftKey", pointer.shiftKey)
    ("ctrlKey", pointer.ctrlKey)
    ("altKey", pointer.altKey)
    ;
  return json;
}

void ReactControl::DispatchTouchEvent(TouchEventType eventType, size_t pointerIndex)
{
  folly::dynamic changedIndices = folly::dynamic::array();
  changedIndices.push_back(pointerIndex);

  auto touches = folly::dynamic::array();
  for (const auto& pointer : m_pointers)
  {
    folly::dynamic touch = GetPointerJson(pointer, s_InvalidPointerId);
    touches.push_back(touch);
  }

  // Package up parameters and invoke the JS event emitter
  const char* eventName = GetTouchEventTypeName(eventType);
  if (eventName == nullptr)
    return;
  folly::dynamic params = folly::dynamic::array(eventName, std::move(touches), std::move(changedIndices));
  m_reactInstance->CallJsFunction("RCTEventEmitter", "receiveTouches", std::move(params));
}

const char* ReactControl::GetPointerDeviceTypeName(winrt::Windows::Devices::Input::PointerDeviceType deviceType) noexcept
{
  const char* deviceTypeName = "unknown";
  switch (deviceType)
  {
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

const char* ReactControl::GetTouchEventTypeName(TouchEventType eventType) noexcept
{
  const char* eventName = nullptr;
  switch (eventType)
  {
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


XamlView ReactControl::GetXamlView() const noexcept
{
  return m_xamlRootView;
}

void ReactControl::SetJSComponentName(std::string&& jsComponentName) noexcept
{
  m_jsComponentName = std::move(jsComponentName);

  if (m_reactInstance != nullptr)
    m_reactInstance->SetAsNeedsReload();
}

void ReactControl::SetInstanceCreator(const ReactInstanceCreator& instanceCreator) noexcept
{
  // TODO - Handle swapping this out after the control is running
  m_instanceCreator = instanceCreator;
}

void ReactControl::SetInitialProps(folly::dynamic&& initialProps) noexcept
{
  m_initialProps = initialProps;
}

std::string ReactControl::JSComponentName() const noexcept
{
  return m_jsComponentName;
}

int64_t ReactControl::GetActualHeight() const
{
  auto element = m_xamlRootView.as<winrt::FrameworkElement>();
  assert(element != nullptr);

  return static_cast<int64_t>(element.ActualHeight());
}

int64_t ReactControl::GetActualWidth() const
{
  auto element = m_xamlRootView.as<winrt::FrameworkElement>();
  assert(element != nullptr);

  return static_cast<int64_t>(element.ActualWidth());
}

}
}
