// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include <Composition.Experimental.UriBrushFactoryImageResponse.g.h>
#include <Composition.ImageFailedResponse.g.h>
#include <Composition.ImageResponse.g.h>
#include <Composition.StreamImageResponse.g.h>
#include <Composition.UriBrushFactoryImageResponse.g.h>
#include <ReactPropertyBag.h>
#include <Utils/ImageUtils.h>
#include <react/renderer/imagemanager/primitives.h>
#include <winrt/Microsoft.ReactNative.Composition.h>
#include "ImageResponseImage.h"

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

struct ImageResponseOrImageErrorInfo {
  std::shared_ptr<winrt::Microsoft::ReactNative::Composition::implementation::ImageResponseImage> image;
  std::shared_ptr<facebook::react::ImageErrorInfo> errorInfo;
};

struct ImageResponse : ImageResponseT<ImageResponse /*, IResolveImage*/> {
  ImageResponse() noexcept = default;
  virtual ImageResponseOrImageErrorInfo ResolveImage();
};

struct ImageFailedResponse : ImageFailedResponseT<ImageFailedResponse, ImageResponse /*, IResolveImage*/> {
  ImageFailedResponse(winrt::hstring errorMessage) noexcept : base_type(), m_errorMessage(errorMessage) {}
  ImageFailedResponse(
      winrt::hstring errorMessage,
      winrt::Windows::Web::Http::HttpStatusCode statusCode,
      const winrt::Windows::Web::Http::Headers::HttpResponseHeaderCollection &responseHeaders) noexcept
      : base_type(), m_errorMessage(errorMessage), m_statusCode(statusCode), m_responseHeaders(responseHeaders) {}

  virtual ImageResponseOrImageErrorInfo ResolveImage();

 private:
  const winrt::hstring m_errorMessage;
  const winrt::Windows::Web::Http::HttpStatusCode m_statusCode{winrt::Windows::Web::Http::HttpStatusCode::None};
  const winrt::Windows::Web::Http::Headers::HttpResponseHeaderCollection m_responseHeaders{nullptr};
};

struct StreamImageResponse : StreamImageResponseT<StreamImageResponse, ImageResponse /*, IResolveImage*/> {
  StreamImageResponse(const winrt::Windows::Storage::Streams::IRandomAccessStream &stream) noexcept
      : base_type(), m_stream(stream) {}
  virtual ImageResponseOrImageErrorInfo ResolveImage();

 private:
  const winrt::Windows::Storage::Streams::IRandomAccessStream m_stream;
};

struct UriBrushFactoryImageResponse
    : UriBrushFactoryImageResponseT<UriBrushFactoryImageResponse, ImageResponse /*, IResolveImage*/> {
  UriBrushFactoryImageResponse(const winrt::Microsoft::ReactNative::Composition::UriBrushFactory &factory) noexcept
      : base_type(), m_factory(factory) {}
  virtual ImageResponseOrImageErrorInfo ResolveImage();

 private:
  const winrt::Microsoft::ReactNative::Composition::UriBrushFactory m_factory;
};

} // namespace winrt::Microsoft::ReactNative::Composition::implementation

namespace winrt::Microsoft::ReactNative::Composition::Experimental::implementation {
struct UriBrushFactoryImageResponse
    : UriBrushFactoryImageResponseT<
          UriBrushFactoryImageResponse,
          winrt::Microsoft::ReactNative::Composition::implementation::
              ImageResponse /*, winrt::Microsoft::ReactNative::Composition::implementation::IResolveImage*/> {
  UriBrushFactoryImageResponse(
      const winrt::Microsoft::ReactNative::Composition::Experimental::UriBrushFactory &factory) noexcept
      : base_type(), m_factory(factory) {}
  virtual winrt::Microsoft::ReactNative::Composition::implementation::ImageResponseOrImageErrorInfo ResolveImage();

 private:
  const winrt::Microsoft::ReactNative::Composition::Experimental::UriBrushFactory m_factory;
};
} // namespace winrt::Microsoft::ReactNative::Composition::Experimental::implementation

namespace winrt::Microsoft::ReactNative::Composition::factory_implementation {
struct ImageFailedResponse : ImageFailedResponseT<ImageFailedResponse, implementation::ImageFailedResponse> {};
struct StreamImageResponse : StreamImageResponseT<StreamImageResponse, implementation::StreamImageResponse> {};
struct UriBrushFactoryImageResponse
    : UriBrushFactoryImageResponseT<UriBrushFactoryImageResponse, implementation::UriBrushFactoryImageResponse> {};
} // namespace winrt::Microsoft::ReactNative::Composition::factory_implementation

namespace winrt::Microsoft::ReactNative::Composition::Experimental::factory_implementation {
struct UriBrushFactoryImageResponse
    : UriBrushFactoryImageResponseT<UriBrushFactoryImageResponse, implementation::UriBrushFactoryImageResponse> {};
} // namespace winrt::Microsoft::ReactNative::Composition::Experimental::factory_implementation
