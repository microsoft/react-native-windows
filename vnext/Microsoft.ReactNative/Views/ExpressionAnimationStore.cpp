// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"

#include "ExpressionAnimationStore.h"
#include "XamlFeatures.h"

#include <UI.Composition.h>

namespace react::uwp {

// Expression for computing the center point of a UIElement, produces a vector3
// with 2D translation to center.
comp::ExpressionAnimation ExpressionAnimationStore::GetElementCenterPointExpression(comp::Compositor compositor) {
  /*

  // Because of a bug in the Composition system, we cannot cache this
  expression.
  // The pattern we need to avoid is caching any animation that we call
  // SetExpressionReferenceParameter on is unexpectedly taking a strong
  reference
  // on the reference parameter, keeping it alive much too long and causing a
  crash
  // when reusing the cached animation.

  if (m_elementCenterPointExpression == nullptr) {
    m_elementCenterPointExpression =
        xaml::Window::Current().Compositor().CreateExpressionAnimation(
            L"vector3(0.5 * uielement.ActualSize.x, 0.5 *
  uielement.ActualSize.y, 0)");
  }

  return m_elementCenterPointExpression;
*/

  // In React Native, animations use the element's center as the center or rotation/scale
  // However XAML uses CenterPoint which is usually not set and therefore remains at 0,0,0
  // This means Rotate animations would rotate around the top left corner instead of around the center of the element
  // In order to fix that we need a transform (see GetTransformCenteringExpression)
  // The way we obtain the center of an element is by using the ActualSize façade. However this was only added in 19h1
  // An expression animation that refers to a non-existent property (e.g. in RS5) will crash, so use the CenterPoint as
  // a fallback. This might be wrong but at least we won't crash.
  return compositor.CreateExpressionAnimation(
      g_HasActualSizeProperty == TriBit::Set
          ? L"vector3(0.5 * uielement.ActualSize.x, 0.5 * uielement.ActualSize.y, 0)"
          : L"vector3(uielement.CenterPoint.X, uielement.CenterPoint.Y, uielement.CenterPoint.Z)");
}

// Expression for applying a TransformMatrix about the centerpoint of a
// UIElement, produces a Matrix4x4 with overall transform.
comp::ExpressionAnimation ExpressionAnimationStore::GetTransformCenteringExpression(comp::Compositor compositor) {
  if (m_transformCenteringExpression == nullptr) {
    m_transformCenteringExpression = compositor.CreateExpressionAnimation(
        L"Matrix4x4.CreateFromTranslation(-PS.center) * PS.transform * Matrix4x4.CreateFromTranslation(PS.center)");
  }

  return m_transformCenteringExpression;
}

} // namespace react::uwp
