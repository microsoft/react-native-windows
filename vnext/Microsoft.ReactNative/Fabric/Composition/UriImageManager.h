// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once
#include "Composition.UriImageManager.g.h"

#include <ReactPropertyBag.h>
#include <Utils/ImageUtils.h>
#include <react/renderer/imagemanager/primitives.h>
#include <winrt/Microsoft.ReactNative.Composition.h>

namespace winrt::Microsoft::ReactNative::Composition::implementation {

struct UriImageManager : UriImageManagerT<UriImageManager> {
  UriImageManager() = default;

  static void AddUriImageProvider(
      const winrt::Microsoft::ReactNative::IReactPropertyBag &properties,
      const IUriImageProvider &provider);

  static winrt::com_ptr<UriImageManager> GetOrCreate(
      const winrt::Microsoft::ReactNative::ReactPropertyBag &properties) noexcept;

  IUriImageProvider TryGetUriImageProvider(
      const IReactContext &context,
      winrt::Microsoft::ReactNative::Composition::ImageSource &source) noexcept;

 private:
  std::vector<IUriImageProvider> m_providers;
};

winrt::Microsoft::ReactNative::Composition::ImageSource MakeImageSource(
    const facebook::react::ImageSource &source) noexcept;

} // namespace winrt::Microsoft::ReactNative::Composition::implementation

namespace winrt::Microsoft::ReactNative::Composition::factory_implementation {
struct UriImageManager : UriImageManagerT<UriImageManager, implementation::UriImageManager> {};
} // namespace winrt::Microsoft::ReactNative::Composition::factory_implementation
