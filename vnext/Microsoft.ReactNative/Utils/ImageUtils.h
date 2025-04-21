// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once
#include <ReactContext.h>
#include <winrt/Windows.Storage.Streams.h>
#include <string>

namespace Microsoft::ReactNative {

enum class ImageSourceType { Uri = 0, Download = 1, InlineData = 2 };
enum class ImageSourceFormat { Bitmap = 0, Svg = 1 };

struct ReactImageSource {
  std::string uri;
  std::string method;
  std::vector<std::pair<std::string, std::string>> headers;
  std::string body;
  double width = 0;
  double height = 0;
  double scale = 1.0;
  bool packagerAsset = false;
  ImageSourceType sourceType = ImageSourceType::Uri;
  ImageSourceFormat sourceFormat = ImageSourceFormat::Bitmap;
};

winrt::Windows::Foundation::IAsyncOperation<winrt::Windows::Storage::Streams::IRandomAccessStream>
GetImageMemoryStreamAsync(winrt::Microsoft::ReactNative::IReactPropertyBag properties, ReactImageSource source);

winrt::Windows::Foundation::IAsyncOperation<winrt::Windows::Storage::Streams::IRandomAccessStream> GetImageStreamAsync(
    winrt::Microsoft::ReactNative::IReactPropertyBag properties,
    ReactImageSource source);

winrt::Windows::Foundation::IAsyncOperation<winrt::Windows::Storage::Streams::IRandomAccessStream>
GetImageInlineDataAsync(ReactImageSource source);

winrt::Windows::Foundation::Uri UriTryCreate(winrt::param::hstring const &uri);

} // namespace Microsoft::ReactNative
