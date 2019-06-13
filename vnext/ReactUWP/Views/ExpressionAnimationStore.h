// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include <winrt/Windows.UI.Composition.h>

namespace react { namespace uwp {

class ExpressionAnimationStore
{
public:
  winrt::Windows::UI::Composition::ExpressionAnimation GetElementCenterPointExpression();
  winrt::Windows::UI::Composition::ExpressionAnimation GetTransformCenteringExpression();

private:
  winrt::Windows::UI::Composition::ExpressionAnimation m_elementCenterPointExpression { nullptr };
  winrt::Windows::UI::Composition::ExpressionAnimation m_transformCenteringExpression { nullptr };
};

} }
