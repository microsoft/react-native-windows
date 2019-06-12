// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"

#include <IReactInstance.h>

#include <ViewManager.h>
#include <Views/ShadowNodeBase.h>
#include <Views/ViewManagerBase.h>
#include <Views/ExpressionAnimationStore.h>
#include <winrt/Windows.UI.Composition.h>
#include <WindowsNumerics.h>

using namespace std::placeholders;

namespace react { namespace uwp {

ShadowNodeBase::ShadowNodeBase()
  : m_view(nullptr)
{
  m_previewKeyboardEventHandler = make_unique<PreviewKeyboardEventHandler>(
    std::bind(&ShadowNodeBase::KeyboardEventHandledHandler, this, KeyboardEventPhase::PreviewKeyDown, _1, _2),
    std::bind(&ShadowNodeBase::KeyboardEventHandledHandler, this, KeyboardEventPhase::PreviewKeyUp, _1, _2)
    );

  m_keyboardEventHandler = make_unique<KeyboardEventHandler>(
    std::bind(&ShadowNodeBase::KeyboardEventHandledHandler, this, KeyboardEventPhase::KeyDown, _1, _2),
    std::bind(&ShadowNodeBase::KeyboardEventHandledHandler, this, KeyboardEventPhase::KeyUp, _1, _2)
    );
}

ViewManagerBase* ShadowNodeBase::GetViewManager() const
{
  return static_cast<ViewManagerBase*>(m_viewManager);
}

void ShadowNodeBase::updateProperties(const folly::dynamic&& props)
{
  GetViewManager()->UpdateProperties(this, props);
}

void ShadowNodeBase::createView()
{
  m_view = GetViewManager()->CreateView(this->m_tag);
}

void ShadowNodeBase::dispatchCommand(int64_t commandId, const folly::dynamic& commandArgs)
{
  GetViewManager()->DispatchCommand(GetView(), commandId, commandArgs);
}

void ShadowNodeBase::removeAllChildren()
{
  GetViewManager()->RemoveAllChildren(GetView());
}

void ShadowNodeBase::AddView(ShadowNode& child, int64_t index)
{
  this->GetViewManager()->AddView(GetView(), static_cast<ShadowNodeBase&>(child).GetView(), index);
}

void ShadowNodeBase::RemoveChildAt(int64_t indexToRemove)
{
  GetViewManager()->RemoveChildAt(GetView(), indexToRemove);
}

void ShadowNodeBase::onDropViewInstance()
{
}

void ShadowNodeBase::ReplaceView(XamlView view)
{

  SetTag(view, GetTag(m_view));

  m_view = view;

  // Force unhook from old view
  UpdateKeyboardEventHooks(nullptr);
  UpdateKeyboardEventHooks(m_view);

}

void ShadowNodeBase::ReplaceChild(XamlView oldChildView, XamlView newChildView)
{
  GetViewManager()->ReplaceChild(m_view, oldChildView, newChildView);
}

winrt::Windows::UI::Composition::CompositionPropertySet ShadowNodeBase::EnsureTransformPS()
{
  if (m_transformPS == nullptr)
  {
    m_transformPS = winrt::Window::Current().Compositor().CreatePropertySet();
    UpdateTransformPS();
  }

  return m_transformPS;
}

// Create a PropertySet that will hold two properties:
// "center":  This is the center of the UIElement
// "transform": This will hold the un-centered TransformMatrix we want to apply
void ShadowNodeBase::UpdateTransformPS()
{
  if (m_transformPS != nullptr)
  {
    // First build up an ExpressionAnimation to compute the "center" property, like so:
    // The input to the expression is UIElement.ActualSize/2, output is a vector3 with [cx, cy, 0].
    auto uielement = m_view.try_as<winrt::UIElement>();
    assert(uielement != nullptr);
    m_transformPS = EnsureTransformPS();
    m_transformPS.InsertVector3(L"center", { 0, 0, 0 });
    auto instance = GetViewManager()->GetReactInstance().lock();
    assert(instance != nullptr);
    auto centeringAnimation = instance->GetExpressionAnimationStore().GetElementCenterPointExpression();
    centeringAnimation.SetExpressionReferenceParameter(L"uielement", uielement);
    m_transformPS.StartAnimation(L"center", centeringAnimation);

    // Now insert the "transform" property with an initial value of identity.
    // The caller will handle populating this with the appropriate value (either a static or animated value).
    winrt::Windows::Foundation::Numerics::float4x4 unused;
    // Take care not to stomp over any transform value we currently have set, as we will use this value in the scenario
    // where a View changed its backing XAML element, here we will just transfer existing value to a new backing XAML element.
    if (m_transformPS.TryGetMatrix4x4(L"transform", unused) == winrt::Windows::UI::Composition::CompositionGetValueStatus::NotFound)
    {
      m_transformPS.InsertMatrix4x4(L"transform", winrt::Windows::Foundation::Numerics::float4x4::identity());
    }
  }
  
void ShadowNodeBase::UpdateHandledKeyboardEvents(KeyboardType keyboardType, folly::dynamic const& value)
{
  if (keyboardType == KeyboardType::KeyDown) {
    m_handledKeyDownKeyboardEvents = KeyboardHelper::FromJS(value);
  }
  else
  {
    m_handledKeyUpKeyboardEvents = KeyboardHelper::FromJS(value);
  }

  UpdateKeyboardEventHooks(m_view);
}

void ShadowNodeBase::UpdateKeyboardEventHooks(XamlView const& xamlView)
{
  auto shouldHook = ShouldHookKeyboardEvent(m_view);
  if (shouldHook != m_keyboardEventHooked)
  {
    if (shouldHook && m_view)
    {
      m_previewKeyboardEventHandler->hook(m_view);
      m_keyboardEventHandler->hook(m_view);
      m_keyboardEventHooked = true;
    }
    else
    {
      m_previewKeyboardEventHandler->unhook();
      m_keyboardEventHandler->unhook();
      m_keyboardEventHooked = false;
    }
  }
}

bool ShadowNodeBase::ShouldHookKeyboardEvent(XamlView const& xamlView)
{
  return (xamlView && (m_handledKeyDownKeyboardEvents.size() > 0 || m_handledKeyUpKeyboardEvents.size() > 0));
}

void ShadowNodeBase::KeyboardEventHandledHandler(KeyboardEventPhase phase, winrt::IInspectable const& sender, winrt::KeyRoutedEventArgs const& args)
{
  EventPhase currentEventPhase = (phase == KeyboardEventPhase::PreviewKeyUp || phase == KeyboardEventPhase::PreviewKeyDown)
    ? EventPhase::CAPTURING : EventPhase::BUBBLING;

  auto ev = KeyboardHelper::CreateKeyboardEvent(currentEventPhase, args);

  bool shouldMarkHandled = false;
  if (phase == KeyboardEventPhase::PreviewKeyDown || phase == KeyboardEventPhase::KeyDown)
    shouldMarkHandled = ShouldMarkKeyboardHandled(m_handledKeyDownKeyboardEvents, ev);
  else
    shouldMarkHandled = ShouldMarkKeyboardHandled(m_handledKeyUpKeyboardEvents, ev);

  if (shouldMarkHandled)
    args.Handled(true);
}

bool ShadowNodeBase::ShouldMarkKeyboardHandled(std::vector<KeyboardEvent> const& handledEvents, KeyboardEvent currentEvent)
{
  for (auto event : handledEvents)
  {
    if (event.key == currentEvent.key &&
      (!event.altKey || (event.altKey && currentEvent.altKey)) &&
      (!event.ctrlKey || (event.ctrlKey && currentEvent.ctrlKey)) &&
      (!event.shiftKey || (event.shiftKey && currentEvent.shiftKey)) &&
      (!event.metaKey || (event.metaKey && currentEvent.metaKey)) &&
      event.eventPhase == currentEvent.eventPhase)
      return true;
  }
  return false;
}

}}
