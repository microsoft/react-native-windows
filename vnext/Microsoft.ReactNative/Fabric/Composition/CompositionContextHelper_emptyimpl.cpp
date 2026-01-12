
#include "pch.h"
#include "CompositionContextHelper.h"
#if __has_include("Composition.Experimental.SystemCompositionContextHelper.g.cpp")
#include "Composition.Experimental.SystemCompositionContextHelper.g.cpp"
#endif
#if __has_include("Composition.Experimental.MicrosoftCompositionContextHelper.g.cpp")
#include "Composition.Experimental.MicrosoftCompositionContextHelper.g.cpp"
#endif

namespace winrt::Microsoft::ReactNative::Composition::Experimental::implementation {

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

ICompositionContext MicrosoftCompositionContextHelper::CreateContext(
    winrt::Microsoft::UI::Composition::Compositor const &) noexcept {
  return nullptr;
}

IVisual MicrosoftCompositionContextHelper::CreateVisual(winrt::Microsoft::UI::Composition::Visual const &) noexcept {
  return nullptr;
}

winrt::Microsoft::UI::Composition::Compositor MicrosoftCompositionContextHelper::InnerCompositor(
    ICompositionContext) noexcept {
  return nullptr;
}

winrt::Microsoft::UI::Composition::Visual MicrosoftCompositionContextHelper::InnerVisual(IVisual) noexcept {
  return nullptr;
}

winrt::Microsoft::UI::Composition::DropShadow MicrosoftCompositionContextHelper::InnerDropShadow(IDropShadow) noexcept {
  return nullptr;
}

winrt::Microsoft::UI::Composition::CompositionBrush MicrosoftCompositionContextHelper::InnerBrush(IBrush) noexcept {
  return nullptr;
}

winrt::Microsoft::UI::Composition::ICompositionSurface MicrosoftCompositionContextHelper::InnerSurface(
    IDrawingSurfaceBrush) noexcept {
  return nullptr;
}

} // namespace winrt::Microsoft::ReactNative::Composition::Experimental::implementation
