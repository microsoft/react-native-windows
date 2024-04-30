// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.
#include "pch.h"

#include "CompositionRootView.h"

#include "CompositionRootView.g.cpp"
#include "FocusNavigationRequest.g.cpp"

namespace winrt::Microsoft::ReactNative::implementation {

//! This class ensures that we access ReactRootView from UI thread.
struct CompositionReactViewInstance
    : public winrt::implements<CompositionReactViewInstance, winrt::Microsoft::ReactNative::IReactViewInstance> {
  CompositionReactViewInstance(
      winrt::weak_ref<winrt::Microsoft::ReactNative::implementation::CompositionRootView> &&) noexcept {}

  void InitRootView(
      winrt::Microsoft::ReactNative::IReactContext,
      winrt::Microsoft::ReactNative::ReactViewOptions) noexcept {}

  void UpdateRootView() noexcept {}
  void UninitRootView() noexcept {}
};

//===========================================================================
// ReactViewInstance inline implementation
//===========================================================================

CompositionRootView::CompositionRootView() noexcept {}
CompositionRootView::~CompositionRootView() noexcept {}

CompositionRootView::CompositionRootView(const winrt::Microsoft::UI::Composition::Compositor &compositor) noexcept {}

ReactNative::IReactViewHost CompositionRootView::ReactViewHost() noexcept {
  return nullptr;
}

void CompositionRootView::ReactViewHost(winrt::Microsoft::ReactNative::IReactViewHost const &) noexcept {}

winrt::Microsoft::UI::Composition::Visual CompositionRootView::RootVisual() noexcept {
  return nullptr;
}

winrt::Microsoft::ReactNative::Composition::Experimental::IVisual CompositionRootView::InternalRootVisual() noexcept {
  return nullptr;
}

void CompositionRootView::InternalRootVisual(
    const winrt::Microsoft::ReactNative::Composition::Experimental::IVisual &) noexcept {}

winrt::Microsoft::UI::Content::ContentIsland CompositionRootView::Island() noexcept {
  return nullptr;
}

winrt::Windows::Foundation::Size CompositionRootView::Size() noexcept {
  return {};
}

void CompositionRootView::Size(winrt::Windows::Foundation::Size) noexcept {}

float CompositionRootView::ScaleFactor() noexcept {
  return 0;
}

void CompositionRootView::ScaleFactor(float) noexcept {}

winrt::Microsoft::ReactNative::Composition::Theme CompositionRootView::Theme() noexcept {
  return nullptr;
}
void CompositionRootView::Theme(const winrt::Microsoft::ReactNative::Composition::Theme &) noexcept {}

winrt::Microsoft::ReactNative::Composition::ICustomResourceLoader CompositionRootView::Resources() noexcept {
  return nullptr;
}
void CompositionRootView::Resources(
    const winrt::Microsoft::ReactNative::Composition::ICustomResourceLoader &) noexcept {}

winrt::IInspectable CompositionRootView::GetUiaProvider() noexcept {
  return nullptr;
}

std::string CompositionRootView::JSComponentName() const noexcept {
  return {};
}

int64_t CompositionRootView::GetActualHeight() const noexcept {
  return 0;
}

int64_t CompositionRootView::GetActualWidth() const noexcept {
  return 0;
}

int64_t CompositionRootView::GetTag() const noexcept {
  return 0;
}

void CompositionRootView::SetTag(int64_t) noexcept {}

void CompositionRootView::SetWindow(uint64_t) noexcept {}

int64_t CompositionRootView::SendMessage(uint32_t, uint64_t, int64_t) noexcept {
  return 0;
}

void CompositionRootView::InitRootView(
    winrt::Microsoft::ReactNative::IReactContext &&,
    winrt::Microsoft::ReactNative::ReactViewOptions &&) noexcept {}

void CompositionRootView::UpdateRootView() noexcept {}

void CompositionRootView::UpdateRootViewInternal() noexcept {}

void CompositionRootView::UninitRootView() noexcept {}

void CompositionRootView::ClearLoadingUI() noexcept {}

void CompositionRootView::EnsureLoadingUI() noexcept {}

void CompositionRootView::ShowInstanceLoaded() noexcept {}

void CompositionRootView::ShowInstanceError() noexcept {}

void CompositionRootView::ShowInstanceLoading() noexcept {}

winrt::Windows::Foundation::Size CompositionRootView::Measure(winrt::Windows::Foundation::Size const &) const {
  return {};
}

winrt::Windows::Foundation::Size CompositionRootView::Arrange(winrt::Windows::Foundation::Size) const {
  return {};
}

winrt::Microsoft::ReactNative::Composition::implementation::RootComponentView *
CompositionRootView::GetComponentView() noexcept {
  return nullptr;
}

winrt::Microsoft::ReactNative::FocusNavigationResult CompositionRootView::NavigateFocus(
    const winrt::Microsoft::ReactNative::FocusNavigationRequest &) noexcept {
  return nullptr;
}

} // namespace winrt::Microsoft::ReactNative::implementation
