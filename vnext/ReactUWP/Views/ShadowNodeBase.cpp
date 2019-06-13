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

namespace react { namespace uwp {

ShadowNodeBase::ShadowNodeBase()
  : m_view(nullptr)
{
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

void ShadowNodeBase::UpdateTransformPS()
{
  if (m_transformPS != nullptr)
  {
    auto uielement = m_view.try_as<winrt::UIElement>();
    assert(uielement != nullptr);
    m_transformPS = EnsureTransformPS();
    m_transformPS.InsertVector3(L"center", { 0, 0, 0 });
    auto instance = GetViewManager()->GetReactInstance().lock();
    assert(instance != nullptr);
    auto centeringAnimation = instance->GetExpressionAnimationStore().GetElementCenterPointExpression();
    centeringAnimation.SetExpressionReferenceParameter(L"uielement", uielement);
    m_transformPS.StartAnimation(L"center", centeringAnimation);
    winrt::Windows::Foundation::Numerics::float4x4 unused;
    if (m_transformPS.TryGetMatrix4x4(L"transform", unused) == winrt::Windows::UI::Composition::CompositionGetValueStatus::NotFound)
    {
      m_transformPS.InsertMatrix4x4(L"transform", winrt::Windows::Foundation::Numerics::float4x4::identity());
    }
  }
}

}}
