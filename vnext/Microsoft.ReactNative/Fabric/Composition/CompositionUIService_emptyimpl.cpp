// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"
#include "Composition.CompositionUIService.g.cpp"
#include "CompositionUIService.h"

namespace winrt::Microsoft::ReactNative::Composition::implementation {

void CompositionUIService::SetCompositionContext(IReactPropertyBag const &, ICompositionContext const &) noexcept {}

ICompositionContext CompositionUIService::GetCompositionContext(const IReactPropertyBag &) noexcept {
  return nullptr;
}

} // namespace winrt::Microsoft::ReactNative::Composition::implementation
