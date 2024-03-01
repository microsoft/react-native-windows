// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"
#include "CompositionUIService.h"
#include "Composition.CompositionUIService.g.cpp"
#include <QuirkSettings.h>

#include <ReactPropertyBag.h>

namespace winrt::Microsoft::ReactNative::Composition::implementation {

static const ReactPropertyId<ICompositionContext> &CompositionContextPropertyId() noexcept {
  static const ReactPropertyId<ICompositionContext> prop{L"ReactNative.Composition", L"CompositionContext"};
  return prop;
}

void CompositionUIService::SetCompositionContext(
    IReactPropertyBag const &properties,
    ICompositionContext const &compositionContext) noexcept {
  ReactPropertyBag(properties).Set(CompositionContextPropertyId(), compositionContext);
  // Default to using Bridgeless mode when using fabric
  winrt::Microsoft::ReactNative::implementation::QuirkSettings::SetIsBridgeless(ReactPropertyBag(properties), true);
}

ICompositionContext CompositionUIService::GetCompositionContext(const IReactPropertyBag &properties) noexcept {
  return ReactPropertyBag(properties).Get(CompositionContextPropertyId());
}

} // namespace winrt::Microsoft::ReactNative::Composition::implementation
