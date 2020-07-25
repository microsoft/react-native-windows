// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

namespace react::uwp {

// Holds a cache of unique ExpressionAnimations.
// It's better for performance to cache each unique ExpressionAnimation, as
// there's a one-time cost to parse the string down to tokens.  The Composition
// team designed ExpressionAnimations for reusability, each call to
// StartAnimation stamps out an animation with reference parameters fully
// resolved, so they can be reused.
class ExpressionAnimationStore {
 public:
  comp::ExpressionAnimation GetElementCenterPointExpression(comp::Compositor compositor);
  comp::ExpressionAnimation GetTransformCenteringExpression(comp::Compositor compositor);

 private:
  // Compositor bug, see notes in GetElementCenterPointExpression()
  //  comp::ExpressionAnimation
  //      m_elementCenterPointExpression{nullptr};
  comp::ExpressionAnimation m_transformCenteringExpression{nullptr};
};

} // namespace react::uwp
