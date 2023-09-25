
#include "pch.h"
#include "CompositionContextHelper.h"
#if __has_include("Composition.WindowsCompositionContextHelper.g.cpp")
#include "Composition.WindowsCompositionContextHelper.g.cpp"
#endif
#ifdef USE_WINUI3
#if __has_include("Composition.MicrosoftCompositionContextHelper.g.cpp")
#include "Composition.MicrosoftCompositionContextHelper.g.cpp"
#endif
#endif

namespace winrt::Microsoft::ReactNative::Composition::implementation {

ICompositionContext WindowsCompositionContextHelper::CreateContext(
    winrt::Windows::UI::Composition::Compositor const &) noexcept {
  return nullptr;
}

IVisual WindowsCompositionContextHelper::CreateVisual(winrt::Windows::UI::Composition::Visual const &) noexcept {
  return nullptr;
}

winrt::Windows::UI::Composition::Compositor WindowsCompositionContextHelper::InnerCompositor(
    ICompositionContext) noexcept {
  return nullptr;
}

winrt::Windows::UI::Composition::Visual WindowsCompositionContextHelper::InnerVisual(IVisual) noexcept {
  return nullptr;
}

winrt::Windows::UI::Composition::DropShadow WindowsCompositionContextHelper::InnerDropShadow(IDropShadow) noexcept {
  return nullptr;
}

winrt::Windows::UI::Composition::CompositionBrush WindowsCompositionContextHelper::InnerBrush(IBrush) noexcept {
  return nullptr;
}

winrt::Windows::UI::Composition::ICompositionSurface WindowsCompositionContextHelper::InnerSurface(
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
