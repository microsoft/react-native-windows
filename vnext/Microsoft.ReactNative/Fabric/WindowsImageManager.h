// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"

#include <react/renderer/imagemanager/ImageRequest.h>

#include <Fabric/Composition/UriImageManager.h>
#include <ReactContext.h>

namespace Microsoft::ReactNative {

struct WindowsImageManager {
  WindowsImageManager(winrt::Microsoft::ReactNative::ReactContext reactContext);

  facebook::react::ImageRequest requestImage(
      const facebook::react::ImageSource &imageSource,
      facebook::react::SurfaceId surfaceId) const;

 private:
  winrt::Microsoft::ReactNative::ReactContext m_reactContext;
  winrt::com_ptr<winrt::Microsoft::ReactNative::Composition::implementation::UriImageManager> m_uriImageManager;
};

} // namespace Microsoft::ReactNative
