// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"

#include <ViewManager.h>
#include <Views/ShadowNodeBase.h>
#include <Views/ViewManagerBase.h>

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

}}
