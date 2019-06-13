// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include "ExpressionAnimationStore.h"

namespace winrt
{
  using namespace Windows::UI::Xaml;
  using namespace Windows::UI::Composition;
}

namespace react {  namespace uwp {

winrt::ExpressionAnimation ExpressionAnimationStore::GetElementCenterPointExpression()
{
  if (m_elementCenterPointExpression == nullptr)
  {
    m_elementCenterPointExpression = winrt::Window::Current().Compositor().CreateExpressionAnimation(L"vector3(0.5 * uielement.ActualSize.x, 0.5 * uielement.ActualSize.y, 0)");
  }

  return m_elementCenterPointExpression;
}

winrt::ExpressionAnimation ExpressionAnimationStore::GetTransformCenteringExpression()
{
  if (m_transformCenteringExpression == nullptr)
  {
    m_transformCenteringExpression = winrt::Window::Current().Compositor().CreateExpressionAnimation(L"Matrix4x4.CreateFromTranslation(-PS.center) * PS.transform * Matrix4x4.CreateFromTranslation(PS.center)");
  }

  return m_transformCenteringExpression;
}

} }
