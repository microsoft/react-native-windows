
#include "pch.h"
#include "CompositionContextHelper.h"
#if __has_include("Composition.CompositionContextHelper.g.cpp")
#include "Composition.CompositionContextHelper.g.cpp"
#endif

namespace winrt::Microsoft::ReactNative::Composition::implementation {

ICompositionContext CompositionContextHelper::CreateContext(
    winrt::Windows::UI::Composition::Compositor const &) noexcept {
  return nullptr;
}

IVisual CompositionContextHelper::CreateVisual(winrt::Windows::UI::Composition::Visual const &) noexcept {
  return nullptr;
}

winrt::Windows::UI::Composition::Compositor CompositionContextHelper::InnerCompositor(ICompositionContext) noexcept {
  return nullptr;
}

winrt::Windows::UI::Composition::Visual CompositionContextHelper::InnerVisual(IVisual) noexcept {
  return nullptr;
}

winrt::Windows::UI::Composition::DropShadow CompositionContextHelper::InnerDropShadow(IDropShadow) noexcept {
  return nullptr;
}

winrt::Windows::UI::Composition::CompositionBrush CompositionContextHelper::InnerBrush(IBrush) noexcept {
  return nullptr;
}

winrt::Windows::UI::Composition::ICompositionSurface CompositionContextHelper::InnerSurface(
    IDrawingSurfaceBrush) noexcept {
  return nullptr;
}

} // namespace winrt::Microsoft::ReactNative::Composition::implementation
