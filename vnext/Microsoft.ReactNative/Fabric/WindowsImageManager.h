// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"

#include <react/renderer/imagemanager/ImageRequest.h>

#include <Fabric/Composition/UriImageManager.h>
#include <ReactContext.h>
#include <Utils/ImageUtils.h>
#include <winrt/Microsoft.ReactNative.Composition.h>
#include <winrt/Windows.Web.Http.h>

namespace Microsoft::ReactNative {

struct WindowsImageManager {
  WindowsImageManager(winrt::Microsoft::ReactNative::ReactContext reactContext);

  facebook::react::ImageRequest requestImage(
      const facebook::react::ImageSource &imageSource,
      facebook::react::SurfaceId surfaceId) const;

 private:
  winrt::Windows::Foundation::IAsyncOperation<winrt::Microsoft::ReactNative::Composition::ImageResponse>
  GetImageRandomAccessStreamAsync(ReactImageSource source) const;

  winrt::Windows::Web::Http::HttpClient m_httpClient;
  winrt::Microsoft::ReactNative::ReactContext m_reactContext;
  std::shared_ptr<winrt::Microsoft::ReactNative::Composition::implementation::UriImageManager> m_uriImageManager;
};

} // namespace Microsoft::ReactNative
