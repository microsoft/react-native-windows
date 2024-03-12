
#include "pch.h"
#include "CompositionContextHelper.h"
#if __has_include("Composition.SystemCompositionContextHelper.g.cpp")
#include "Composition.SystemCompositionContextHelper.g.cpp"
#endif
#ifdef USE_WINUI3
#if __has_include("Composition.MicrosoftCompositionContextHelper.g.cpp")
#include "Composition.MicrosoftCompositionContextHelper.g.cpp"
#endif
#endif

namespace winrt::Microsoft::ReactNative::Composition::implementation {

ICompositionContext SystemCompositionContextHelper::CreateContext(
    winrt::Windows::UI::Composition::Compositor const &) noexcept {
  return nullptr;
}

IVisual SystemCompositionContextHelper::CreateVisual(winrt::Windows::UI::Composition::Visual const &) noexcept {
  return nullptr;
}

winrt::Windows::UI::Composition::Compositor SystemCompositionContextHelper::InnerCompositor(
    ICompositionContext) noexcept {
  return nullptr;
}

winrt::Windows::UI::Composition::Visual SystemCompositionContextHelper::InnerVisual(IVisual) noexcept {
  return nullptr;
}

winrt::Windows::UI::Composition::DropShadow SystemCompositionContextHelper::InnerDropShadow(IDropShadow) noexcept {
  return nullptr;
}

winrt::Windows::UI::Composition::CompositionBrush SystemCompositionContextHelper::InnerBrush(IBrush) noexcept {
  return nullptr;
}

winrt::Windows::UI::Composition::ICompositionSurface SystemCompositionContextHelper::InnerSurface(
    IDrawingSurfaceBrush) noexcept {
  return nullptr;
}

#ifdef USE_WINUI3
ICompositionContext MicrosoftCompositionContextHelper::CreateContext(
    winrt::Windows::UI::Composition::Compositor const &) noexcept {
  return nullptr;
}

IVisual MicrosoftCompositionContextHelper::CreateVisual(winrt::Windows::UI::Composition::Visual const &) noexcept {
  return nullptr;
}

winrt::Windows::UI::Composition::Compositor MicrosoftCompositionContextHelper::InnerCompositor(
    ICompositionContext) noexcept {
  return nullptr;
}

winrt::Windows::UI::Composition::Visual MicrosoftCompositionContextHelper::InnerVisual(IVisual) noexcept {
  return nullptr;
}

winrt::Windows::UI::Composition::DropShadow MicrosoftCompositionContextHelper::InnerDropShadow(IDropShadow) noexcept {
  return nullptr;
}

winrt::Windows::UI::Composition::CompositionBrush MicrosoftCompositionContextHelper::InnerBrush(IBrush) noexcept {
  return nullptr;
}

winrt::Windows::UI::Composition::ICompositionSurface MicrosoftCompositionContextHelper::InnerSurface(
    IDrawingSurfaceBrush) noexcept {
  return nullptr;
}
#endif

} // namespace winrt::Microsoft::ReactNative::Composition::implementation
