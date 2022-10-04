// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"

#include "WindowsImageManager.h"

#include <Utils/ImageUtils.h>
#include <shcore.h>
#include <wincodec.h>

extern "C" HRESULT WINAPI WICCreateImagingFactory_Proxy(UINT SDKVersion, IWICImagingFactory **ppIWICImagingFactory);

namespace Microsoft::ReactNative {

WindowsImageManager::WindowsImageManager() {}

winrt::com_ptr<IWICBitmapSource> wicBitmapSourceFromStream(
    const winrt::Windows::Storage::Streams::IRandomAccessStream &results) noexcept {
  if (!results) {
    return nullptr;
  }

  winrt::com_ptr<IWICImagingFactory> imagingFactory;
  winrt::check_hresult(WICCreateImagingFactory_Proxy(WINCODEC_SDK_VERSION, imagingFactory.put()));

  winrt::com_ptr<IStream> istream;
  winrt::check_hresult(
      CreateStreamOverRandomAccessStream(results.as<IUnknown>().get(), __uuidof(IStream), istream.put_void()));

  winrt::com_ptr<IWICBitmapDecoder> bitmapDecoder;
  if (imagingFactory->CreateDecoderFromStream(
          istream.get(), nullptr, WICDecodeMetadataCacheOnDemand, bitmapDecoder.put()) < 0) {
    return nullptr;
  }

  winrt::com_ptr<IWICBitmapFrameDecode> decodedFrame;
  winrt::check_hresult(bitmapDecoder->GetFrame(0, decodedFrame.put()));
  return decodedFrame;
}

void generateBitmap(
    std::weak_ptr<const facebook::react::ImageResponseObserverCoordinator> weakObserverCoordinator,
    const winrt::Windows::Storage::Streams::IRandomAccessStream &results) noexcept {
  auto observerCoordinator = weakObserverCoordinator.lock();
  if (!observerCoordinator) {
    return;
  }

  winrt::com_ptr<IWICBitmapSource> decodedFrame = wicBitmapSourceFromStream(results);

  if (!decodedFrame) {
    observerCoordinator->nativeImageResponseFailed();
    return;
  }

  winrt::com_ptr<IWICImagingFactory> imagingFactory;
  winrt::check_hresult(WICCreateImagingFactory_Proxy(WINCODEC_SDK_VERSION, imagingFactory.put()));
  winrt::com_ptr<IWICFormatConverter> converter;
  winrt::check_hresult(imagingFactory->CreateFormatConverter(converter.put()));

  winrt::check_hresult(converter->Initialize(
      decodedFrame.get(),
      GUID_WICPixelFormat32bppPBGRA,
      WICBitmapDitherTypeNone,
      nullptr,
      0.0f,
      WICBitmapPaletteTypeMedianCut));

  winrt::com_ptr<IWICBitmap> wicbmp;
  winrt::check_hresult(imagingFactory->CreateBitmapFromSource(converter.get(), WICBitmapCacheOnLoad, wicbmp.put()));

  // ImageResponse saves a shared_ptr<void> for its data, so we need to wrap the com_ptr in a shared_ptr...
  auto sharedwicbmp = std::make_shared<winrt::com_ptr<IWICBitmap>>(wicbmp);

  observerCoordinator->nativeImageResponseComplete(facebook::react::ImageResponse(sharedwicbmp, nullptr /*metadata*/));
}

facebook::react::ImageRequest WindowsImageManager::requestImage(
    const facebook::react::ImageSource &imageSource,
    facebook::react::SurfaceId surfaceId) const {
  auto imageRequest = facebook::react::ImageRequest(imageSource, nullptr);

  auto weakObserverCoordinator = (std::weak_ptr<const facebook::react::ImageResponseObserverCoordinator>)
                                     imageRequest.getSharedObserverCoordinator();

  ReactImageSource source;
  source.uri = imageSource.uri;
  source.height = imageSource.size.height;
  source.width = imageSource.size.width;
  source.sourceType = ImageSourceType::Download;

  auto task = GetImageStreamAsync(source);

  // TODO progress? - Can we register for progress off the download task?
  // observerCoordinator->nativeImageResponseProgress((float)progress / (float)total);

  task.Completed([weakObserverCoordinator](auto asyncOp, auto status) {
    auto observerCoordinator = weakObserverCoordinator.lock();
    if (!observerCoordinator) {
      return;
    }

    switch (status) {
      case winrt::Windows::Foundation::AsyncStatus::Completed: {
        generateBitmap(weakObserverCoordinator, asyncOp.GetResults());
        break;
      }
      case winrt::Windows::Foundation::AsyncStatus::Canceled: {
        observerCoordinator->nativeImageResponseFailed();
        break;
      }
      case winrt::Windows::Foundation::AsyncStatus::Error: {
        observerCoordinator->nativeImageResponseFailed();
        break;
      }
      case winrt::Windows::Foundation::AsyncStatus::Started: {
      }
    }
  });

  return imageRequest;
}

} // namespace Microsoft::ReactNative
