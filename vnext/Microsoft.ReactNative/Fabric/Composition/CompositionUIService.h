// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once
#include "Composition.CompositionUIService.g.h"
#include <winrt/Microsoft.ReactNative.Composition.Experimental.h>

namespace winrt::Microsoft::ReactNative::Composition::implementation {

struct CompositionUIService : CompositionUIServiceT<CompositionUIService> {
  CompositionUIService() = default;

  static void SetCompositor(
      const winrt::Microsoft::ReactNative::ReactInstanceSettings &settings,
      const winrt::Microsoft::UI::Composition::Compositor &compositor) noexcept;
  static winrt::Microsoft::UI::Composition::Compositor GetCompositor(const IReactPropertyBag &properties) noexcept;

  static Experimental::ICompositionContext GetCompositionContext(const IReactPropertyBag &properties) noexcept;

  static winrt::Microsoft::ReactNative::ComponentView ComponentFromReactTag(
      const winrt::Microsoft::ReactNative::IReactContext &context,
      int64_t reactTag) noexcept;
};

} // namespace winrt::Microsoft::ReactNative::Composition::implementation

namespace winrt::Microsoft::ReactNative::Composition::factory_implementation {
struct CompositionUIService : CompositionUIServiceT<CompositionUIService, implementation::CompositionUIService> {};
} // namespace winrt::Microsoft::ReactNative::Composition::factory_implementation
