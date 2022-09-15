// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once
#include <winrt/Windows.Storage.Streams.h>
#include <string>

namespace Microsoft::ReactNative {

enum class ImageSourceType { Uri = 0, Download = 1, InlineData = 2 };
enum class ImageSourceFormat { Bitmap = 0, Svg = 1 };

struct ReactImageSource {
  std::string uri;
  std::string method;
  std::string bundleRootPath;
  std::vector<std::pair<std::string, std::string>> headers;
  double width = 0;
  double height = 0;
  double scale = 1.0;
  bool packagerAsset = false;
  ImageSourceType sourceType = ImageSourceType::Uri;
  ImageSourceFormat sourceFormat = ImageSourceFormat::Bitmap;
};

winrt::Windows::Foundation::IAsyncOperation<winrt::Windows::Storage::Streams::IRandomAccessStream>
GetImageMemoryStreamAsync(ReactImageSource source);

winrt::Windows::Foundation::IAsyncOperation<winrt::Windows::Storage::Streams::IRandomAccessStream> GetImageStreamAsync(
    ReactImageSource source);

winrt::Windows::Foundation::IAsyncOperation<winrt::Windows::Storage::Streams::IRandomAccessStream>
GetImageInlineDataAsync(ReactImageSource source);

} // namespace Microsoft::ReactNative