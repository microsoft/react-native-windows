// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"

#include "ShadowNodeBase.h"

namespace react { namespace uwp {

ShadowNodeBase::ShadowNodeBase()
  : m_view(nullptr)
{
}

void ShadowNodeBase::updateProperties(const folly::dynamic&& props)
{
  GetViewManager()->UpdateProperties(this, m_view, props);
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

}}
