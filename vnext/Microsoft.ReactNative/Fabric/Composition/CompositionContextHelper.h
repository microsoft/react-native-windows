// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.
#pragma once

#ifdef USE_WINUI3
#include "Composition.MicrosoftCompositionContextHelper.g.h"
#endif
#include "Composition.WindowsCompositionContextHelper.g.h"

#include <d2d1_1.h>
#include <windows.ui.composition.interop.h>
#include <winrt/Windows.UI.Composition.h>
#include "CompositionHelpers.h"

namespace winrt::Microsoft::ReactNative::Composition {
#ifdef USE_WINUI3
using CompositionContextHelper = MicrosoftCompositionContextHelper;
#else
using CompositionContextHelper = WindowsCompositionContextHelper;
#endif
} // namespace winrt::Microsoft::ReactNative::Composition

namespace winrt::Microsoft::ReactNative::Composition::implementation {

struct WindowsCompositionContextHelper : WindowsCompositionContextHelperT<WindowsCompositionContextHelper> {
  WindowsCompositionContextHelper() = default;

  static ICompositionContext CreateContext(winrt::Windows::UI::Composition::Compositor const &compositor) noexcept;
  static IVisual CreateVisual(winrt::Windows::UI::Composition::Visual const &visual) noexcept;
  static winrt::Windows::UI::Composition::Compositor InnerCompositor(ICompositionContext context) noexcept;
  static winrt::Windows::UI::Composition::Visual InnerVisual(IVisual visual) noexcept;
  static winrt::Windows::UI::Composition::DropShadow InnerDropShadow(IDropShadow shadow) noexcept;
  static winrt::Windows::UI::Composition::CompositionBrush InnerBrush(IBrush brush) noexcept;
  static winrt::Windows::UI::Composition::ICompositionSurface InnerSurface(IDrawingSurfaceBrush surface) noexcept;
};

#ifdef USE_WINUI3
struct MicrosoftCompositionContextHelper : MicrosoftCompositionContextHelperT<MicrosoftCompositionContextHelper> {
  MicrosoftCompositionContextHelper() = default;

  static ICompositionContext CreateContext(winrt::Microsoft::UI::Composition::Compositor const &compositor) noexcept;
  static IVisual CreateVisual(winrt::Microsoft::UI::Composition::Visual const &visual) noexcept;
  static winrt::Microsoft::UI::Composition::Compositor InnerCompositor(ICompositionContext context) noexcept;
  static winrt::Microsoft::UI::Composition::Visual InnerVisual(IVisual visual) noexcept;
  static winrt::Microsoft::UI::Composition::DropShadow InnerDropShadow(IDropShadow shadow) noexcept;
  static winrt::Microsoft::UI::Composition::CompositionBrush InnerBrush(IBrush brush) noexcept;
  static winrt::Microsoft::UI::Composition::ICompositionSurface InnerSurface(IDrawingSurfaceBrush surface) noexcept;
};
#endif

} // namespace winrt::Microsoft::ReactNative::Composition::implementation

namespace winrt::Microsoft::ReactNative::Composition::factory_implementation {

struct WindowsCompositionContextHelper : WindowsCompositionContextHelperT<
                                             WindowsCompositionContextHelper,
                                             implementation::WindowsCompositionContextHelper> {};

#ifdef USE_WINUI3
struct MicrosoftCompositionContextHelper : MicrosoftCompositionContextHelperT<
                                               MicrosoftCompositionContextHelper,
                                               implementation::MicrosoftCompositionContextHelper> {};
#endif

} // namespace winrt::Microsoft::ReactNative::Composition::factory_implementation
