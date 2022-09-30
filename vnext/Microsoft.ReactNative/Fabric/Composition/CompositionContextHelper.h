// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.
#pragma once

#include "Composition.CompositionContextHelper.g.h"

#include <d2d1_1.h>
#include <windows.ui.composition.interop.h>
#include <winrt/Windows.UI.Composition.h>
#include "CompositionHelpers.h"

namespace winrt::Microsoft::ReactNative::Composition::implementation {

struct CompositionContextHelper : CompositionContextHelperT<CompositionContextHelper> {
  CompositionContextHelper() = default;

  static ICompositionContext CreateContext(winrt::Windows::UI::Composition::Compositor const &compositor) noexcept;
  static IVisual CreateVisual(winrt::Windows::UI::Composition::Visual const &visual) noexcept;
  static winrt::Windows::UI::Composition::Compositor InnerCompositor(ICompositionContext context) noexcept;
  static winrt::Windows::UI::Composition::Visual InnerVisual(IVisual visual) noexcept;
  static winrt::Windows::UI::Composition::DropShadow InnerDropShadow(IDropShadow shadow) noexcept;
  static winrt::Windows::UI::Composition::CompositionBrush InnerBrush(IBrush brush) noexcept;
  static winrt::Windows::UI::Composition::ICompositionSurface InnerSurface(ICompositionDrawingSurface surface) noexcept;
};

} // namespace winrt::Microsoft::ReactNative::Composition::implementation

namespace winrt::Microsoft::ReactNative::Composition::factory_implementation {

struct CompositionContextHelper
    : CompositionContextHelperT<CompositionContextHelper, implementation::CompositionContextHelper> {};

} // namespace winrt::Microsoft::ReactNative::Composition::factory_implementation
