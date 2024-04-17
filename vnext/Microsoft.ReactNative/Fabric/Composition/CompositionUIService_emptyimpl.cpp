// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"

#include "CompositionUIService.h"

#include "Composition.CompositionUIService.g.cpp"

namespace winrt::Microsoft::ReactNative::Composition::implementation {

void CompositionUIService::SetCompositionContext(IReactPropertyBag const &, ICompositionContext const &) noexcept {}

winrt::Microsoft::UI::Compositor CompositionUIService::GetCompositor(const IReactPropertyBag &) noexcept {
  return nullptr;
}

void CompositionUIService::SetCompositor(const winrt::Microsoft::UI::Compositor &) noexcept {
  return nullptr;
}

} // namespace winrt::Microsoft::ReactNative::Composition::implementation
