// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"
#include "CompositionUIService.h"
#include "Composition.CompositionUIService.g.cpp"
#include <QuirkSettings.h>

#include <ReactPropertyBag.h>

namespace winrt::Microsoft::ReactNative::Composition::implementation {

static const ReactPropertyId<Experimental::ICompositionContext> &CompositionContextPropertyId() noexcept {
  static const ReactPropertyId<Experimental::ICompositionContext> prop{
      L"ReactNative.Composition", L"CompositionContext"};
  return prop;
}

void CompositionUIService::SetCompositor(
    const winrt::Microsoft::ReactNative::ReactInstanceSettings &settings,
    const winrt::Microsoft::UI::Composition::Compositor &compositor) noexcept {
  ReactPropertyBag properties(settings.Properties());
  properties.Set(
      CompositionContextPropertyId(),
      winrt::Microsoft::ReactNative::Composition::Experimental::MicrosoftCompositionContextHelper::CreateContext(
          compositor));
  // Default to using Bridgeless mode when using fabric
  winrt::Microsoft::ReactNative::implementation::QuirkSettings::SetIsBridgeless(properties, !!compositor);
}

winrt::Microsoft::UI::Composition::Compositor CompositionUIService::GetCompositor(
    const IReactPropertyBag &properties) noexcept {
  return winrt::Microsoft::ReactNative::Composition::Experimental::MicrosoftCompositionContextHelper::InnerCompositor(
      GetCompositionContext(properties));
}

Experimental::ICompositionContext CompositionUIService::GetCompositionContext(
    const IReactPropertyBag &properties) noexcept {
  return ReactPropertyBag(properties).Get(CompositionContextPropertyId());
}

} // namespace winrt::Microsoft::ReactNative::Composition::implementation
