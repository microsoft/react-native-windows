// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"

#include <IReactInstance.h>

#include <ViewManager.h>
#include <Views/ExpressionAnimationStore.h>
#include <Views/ShadowNodeBase.h>
#include <Views/ViewManagerBase.h>
#include <WindowsNumerics.h>
#include <winrt/Windows.UI.Composition.h>
#include "Views/KeyboardEventHandler.h"

using namespace std::placeholders;

namespace react {
namespace uwp {

ShadowNodeBase::ShadowNodeBase() : m_view(nullptr) {}

ViewManagerBase *ShadowNodeBase::GetViewManager() const {
  return static_cast<ViewManagerBase *>(m_viewManager);
}

void ShadowNodeBase::updateProperties(const folly::dynamic &&props) {
  GetViewManager()->UpdateProperties(this, props);
}

void ShadowNodeBase::createView() {
  m_view = GetViewManager()->CreateView(this->m_tag);
}

void ShadowNodeBase::dispatchCommand(
    int64_t commandId,
    const folly::dynamic &commandArgs) {
  GetViewManager()->DispatchCommand(GetView(), commandId, commandArgs);
}

void ShadowNodeBase::removeAllChildren() {
  GetViewManager()->RemoveAllChildren(GetView());
}

void ShadowNodeBase::AddView(ShadowNode &child, int64_t index) {
  this->GetViewManager()->AddView(
      GetView(), static_cast<ShadowNodeBase &>(child).GetView(), index);
}

void ShadowNodeBase::RemoveChildAt(int64_t indexToRemove) {
  GetViewManager()->RemoveChildAt(GetView(), indexToRemove);
}

void ShadowNodeBase::onDropViewInstance() {
  m_handledKeyboardEventHandler = nullptr;
}

void ShadowNodeBase::ReplaceView(XamlView view) {
  SetTag(view, GetTag(m_view));

  m_view = view;

  if (m_handledKeyboardEventHandler) {
    m_handledKeyboardEventHandler->unhook();
    m_handledKeyboardEventHandler->hook(view);
  }
}

void ShadowNodeBase::ReplaceChild(
    XamlView oldChildView,
    XamlView newChildView) {
  GetViewManager()->ReplaceChild(m_view, oldChildView, newChildView);
}

winrt::Windows::UI::Composition::CompositionPropertySet
ShadowNodeBase::EnsureTransformPS() {
  if (m_transformPS == nullptr) {
    m_transformPS = winrt::Window::Current().Compositor().CreatePropertySet();
    UpdateTransformPS();
  }

  return m_transformPS;
}

// Create a PropertySet that will hold two properties:
// "center":  This is the center of the UIElement
// "transform": This will hold the un-centered TransformMatrix we want to apply
void ShadowNodeBase::UpdateTransformPS() {
  if (m_transformPS != nullptr) {
    // First build up an ExpressionAnimation to compute the "center" property,
    // like so: The input to the expression is UIElement.ActualSize/2, output is
    // a vector3 with [cx, cy, 0].
    auto uielement = m_view.try_as<winrt::UIElement>();
    assert(uielement != nullptr);
    m_transformPS = EnsureTransformPS();
    m_transformPS.InsertVector3(L"center", {0, 0, 0});
    auto instance = GetViewManager()->GetReactInstance().lock();
    assert(instance != nullptr);
    auto centeringAnimation = instance->GetExpressionAnimationStore()
                                  .GetElementCenterPointExpression();
    centeringAnimation.SetExpressionReferenceParameter(L"uielement", uielement);
    m_transformPS.StartAnimation(L"center", centeringAnimation);

    // Now insert the "transform" property with an initial value of identity.
    // The caller will handle populating this with the appropriate value (either
    // a static or animated value).
    winrt::Windows::Foundation::Numerics::float4x4 unused;
    // Take care not to stomp over any transform value we currently have set, as
    // we will use this value in the scenario where a View changed its backing
    // XAML element, here we will just transfer existing value to a new backing
    // XAML element.
    if (m_transformPS.TryGetMatrix4x4(L"transform", unused) ==
        winrt::Windows::UI::Composition::CompositionGetValueStatus::NotFound) {
      m_transformPS.InsertMatrix4x4(
          L"transform",
          winrt::Windows::Foundation::Numerics::float4x4::identity());
    }
  }
}

void ShadowNodeBase::UpdateHandledKeyboardEvents(
    std::string const &propertyName,
    folly::dynamic const &value) {
  EnsureHandledKeyboardEventHandler();
  m_handledKeyboardEventHandler->UpdateHandledKeyboardEvents(
      propertyName, value);
}

void ShadowNodeBase::EnsureHandledKeyboardEventHandler() {
  if (!m_handledKeyboardEventHandler) {
    assert(m_view);
    m_handledKeyboardEventHandler =
        std::make_unique<HandledKeyboardEventHandler>();
    m_handledKeyboardEventHandler->hook(m_view);
  }
}

} // namespace uwp
} // namespace react
