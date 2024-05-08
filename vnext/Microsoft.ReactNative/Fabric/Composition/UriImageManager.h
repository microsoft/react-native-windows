// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include <ReactPropertyBag.h>
#include <Utils/ImageUtils.h>
#include <react/renderer/imagemanager/primitives.h>
#include <winrt/Microsoft.ReactNative.Composition.h>

namespace winrt::Microsoft::ReactNative::Composition::implementation {

struct UriImageManager {
  UriImageManager();

  void AddUriImageProvider(const IUriImageProvider &provider);

  static void Install(
      const winrt::Microsoft::ReactNative::ReactPropertyBag &properties,
      const std::shared_ptr<UriImageManager> &manager) noexcept;

  static std::shared_ptr<UriImageManager> Get(
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
