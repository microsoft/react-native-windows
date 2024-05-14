// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"

#include "CompositionUIService.h"

#include "Composition.CompositionUIService.g.cpp"
#include <winrt/Microsoft.UI.Composition.h>

namespace winrt::Microsoft::ReactNative::Composition::implementation {

void CompositionUIService::SetCompositor(
    ReactInstanceSettings const &,
    winrt::Microsoft::UI::Composition::Compositor const &) noexcept {}

winrt::Microsoft::UI::Composition::Compositor CompositionUIService::GetCompositor(const IReactPropertyBag &) noexcept {
  return nullptr;
}

winrt::Microsoft::ReactNative::ComponentView CompositionUIService::ComponentFromReactTag(
    const winrt::Microsoft::ReactNative::IReactContext &,
    int64_t) noexcept {
  return nullptr;
}

} // namespace winrt::Microsoft::ReactNative::Composition::implementation
