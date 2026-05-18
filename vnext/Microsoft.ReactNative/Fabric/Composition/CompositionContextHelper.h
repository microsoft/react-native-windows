// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.
#pragma once

#include "Composition.Experimental.MicrosoftCompositionContextHelper.g.h"
#include "Composition.Experimental.SystemCompositionContextHelper.g.h"

#include <d2d1_1.h>
#include <windows.ui.composition.interop.h>
#include <winrt/Windows.UI.Composition.h>
#include "CompositionHelpers.h"

namespace winrt::Microsoft::ReactNative::Composition::Experimental {
using CompositionContextHelper = MicrosoftCompositionContextHelper;
} // namespace winrt::Microsoft::ReactNative::Composition::Experimental

namespace winrt::Microsoft::ReactNative::Composition::Experimental::implementation {

struct SystemCompositionContextHelper : SystemCompositionContextHelperT<SystemCompositionContextHelper> {
  SystemCompositionContextHelper() = default;

  static ICompositionContext CreateContext(winrt::Windows::UI::Composition::Compositor const &compositor) noexcept;
  static IVisual CreateVisual(winrt::Windows::UI::Composition::Visual const &visual) noexcept;
  static winrt::Windows::UI::Composition::Compositor InnerCompositor(ICompositionContext context) noexcept;
  static winrt::Windows::UI::Composition::Visual InnerVisual(IVisual visual) noexcept;
  static winrt::Windows::UI::Composition::DropShadow InnerDropShadow(IDropShadow shadow) noexcept;
  static winrt::Windows::UI::Composition::CompositionBrush InnerBrush(IBrush brush) noexcept;
  static winrt::Windows::UI::Composition::ICompositionSurface InnerSurface(IDrawingSurfaceBrush surface) noexcept;
};

struct MicrosoftCompositionContextHelper : MicrosoftCompositionContextHelperT<MicrosoftCompositionContextHelper> {
  MicrosoftCompositionContextHelper() = default;

  static ICompositionContext CreateContext(winrt::Microsoft::UI::Composition::Compositor const &compositor) noexcept;
  static IVisual CreateVisual(winrt::Microsoft::UI::Composition::Visual const &visual) noexcept;
  static winrt::Microsoft::UI::Composition::Compositor InnerCompositor(ICompositionContext context) noexcept;
  static winrt::Microsoft::UI::Composition::Visual InnerVisual(IVisual visual) noexcept;
  static winrt::Microsoft::UI::Composition::DropShadow InnerDropShadow(IDropShadow shadow) noexcept;
  static winrt::Microsoft::UI::Composition::CompositionBrush InnerBrush(IBrush brush) noexcept;
  static winrt::Microsoft::UI::Composition::ICompositionSurface InnerSurface(IDrawingSurfaceBrush surface) noexcept;

  static IBrush WrapBrush(const winrt::Microsoft::UI::Composition::CompositionBrush &brush) noexcept;
};

} // namespace winrt::Microsoft::ReactNative::Composition::Experimental::implementation

namespace winrt::Microsoft::ReactNative::Composition::Experimental::factory_implementation {

struct SystemCompositionContextHelper
    : SystemCompositionContextHelperT<SystemCompositionContextHelper, implementation::SystemCompositionContextHelper> {
};

struct MicrosoftCompositionContextHelper : MicrosoftCompositionContextHelperT<
                                               MicrosoftCompositionContextHelper,
                                               implementation::MicrosoftCompositionContextHelper> {};

} // namespace winrt::Microsoft::ReactNative::Composition::Experimental::factory_implementation
