// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include <winrt/Windows.UI.Composition.h>

namespace react {
namespace uwp {

// Holds a cache of unique ExpressionAnimations.
// It's better for performance to cache each unique ExpressionAnimation, as
// there's a one-time cost to parse the string down to tokens.  The Composition
// team designed ExpressionAnimations for reusability, each call to
// StartAnimation stamps out an animation with reference parameters fully
// resolved, so they can be reused.
class ExpressionAnimationStore {
 public:
  winrt::Windows::UI::Composition::ExpressionAnimation
  GetElementCenterPointExpression();
  winrt::Windows::UI::Composition::ExpressionAnimation
  GetTransformCenteringExpression();

 private:
  // Compositor bug, see notes in GetElementCenterPointExpression()
  //  winrt::Windows::UI::Composition::ExpressionAnimation
  //      m_elementCenterPointExpression{nullptr};
  winrt::Windows::UI::Composition::ExpressionAnimation
      m_transformCenteringExpression{nullptr};
};

} // namespace uwp
} // namespace react
