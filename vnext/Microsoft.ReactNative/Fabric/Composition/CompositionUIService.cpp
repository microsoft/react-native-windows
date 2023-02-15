// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"
#include "CompositionUIService.h"
#include "Composition.CompositionUIService.g.cpp"

#include <ReactPropertyBag.h>

namespace winrt::Microsoft::ReactNative::Composition::implementation {

static const ReactPropertyId<ICompositionContext> &CompositionContextPropertyId() noexcept {
  static const ReactPropertyId<ICompositionContext> prop{L"ReactNative.Composition", L"CompositionContext"};
  return prop;
}

static const ReactPropertyId<uint64_t> &TopLevelHwndPropertyId() noexcept {
  static const ReactPropertyId<uint64_t> prop{L"ReactNative.Composition", L"TopLevelHwnd"};
  return prop;
}

void CompositionUIService::SetCompositionContext(
    IReactPropertyBag const &properties,
    ICompositionContext const &compositionContext) noexcept {
  ReactPropertyBag(properties).Set(CompositionContextPropertyId(), compositionContext);
}

ICompositionContext CompositionUIService::GetCompositionContext(const IReactPropertyBag &properties) noexcept {
  return ReactPropertyBag(properties).Get(CompositionContextPropertyId());
}

uint64_t CompositionUIService::GetTopLevelWindowHandle(const IReactPropertyBag &properties) noexcept {
  return ReactPropertyBag(properties).Get(TopLevelHwndPropertyId()).value_or(0);
}

void CompositionUIService::SetTopLevelWindowHandle(
    const IReactPropertyBag &properties,
    uint64_t windowHandle) noexcept {
  ReactPropertyBag(properties).Set(TopLevelHwndPropertyId(), windowHandle);
}

} // namespace winrt::Microsoft::ReactNative::Composition::implementation
