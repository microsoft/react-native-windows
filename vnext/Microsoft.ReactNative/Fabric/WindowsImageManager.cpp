// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"

#include "WindowsImageManager.h"

#include <Fabric/Composition/CompositionContextHelper.h>
#include <Fabric/Composition/ImageResponseImage.h>
#include <Fabric/Composition/UriImageManager.h>
#include <Utils/ImageUtils.h>
#include <functional/functor.h>
#include <shcore.h>
#include <wincodec.h>

extern "C" HRESULT WINAPI WICCreateImagingFactory_Proxy(UINT SDKVersion, IWICImagingFactory **ppIWICImagingFactory);

namespace Microsoft::ReactNative {

WindowsImageManager::WindowsImageManager(winrt::Microsoft::ReactNative::ReactContext reactContext)
    : m_reactContext(reactContext) {
  m_uriImageManager =
      winrt::Microsoft::ReactNative::Composition::implementation::UriImageManager::Get(reactContext.Properties());
}

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

std::shared_ptr<winrt::Microsoft::ReactNative::Composition::implementation::ImageResponseImage> generateBitmap(
    const winrt::Windows::Storage::Streams::IRandomAccessStream &results) noexcept {
  winrt::com_ptr<IWICBitmapSource> decodedFrame = wicBitmapSourceFromStream(results);

  if (!decodedFrame) {
    return nullptr;
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

  auto image = std::make_shared<winrt::Microsoft::ReactNative::Composition::implementation::ImageResponseImage>();
  image->m_wicbmp = wicbmp;
  return image;
}

template <typename T>
void ProcessImageRequestTask(
    std::weak_ptr<const facebook::react::ImageResponseObserverCoordinator> &weakObserverCoordinator,
    const winrt::Windows::Foundation::IAsyncOperation<T> &task,
    Mso::Functor<std::shared_ptr<winrt::Microsoft::ReactNative::Composition::implementation::ImageResponseImage>(
        const T &result)> &&onSuccess) {
  task.Completed([weakObserverCoordinator, onSuccess = std::move(onSuccess)](auto asyncOp, auto status) {
    auto observerCoordinator = weakObserverCoordinator.lock();
    if (!observerCoordinator) {
      return;
    }

    auto error = facebook::react::ImageLoadError(nullptr);

    switch (status) {
      case winrt::Windows::Foundation::AsyncStatus::Completed: {
        auto imageResponseImage = onSuccess(asyncOp.GetResults());
        if (imageResponseImage)
          observerCoordinator->nativeImageResponseComplete(
              facebook::react::ImageResponse(imageResponseImage, nullptr /*metadata*/));
        else
          observerCoordinator->nativeImageResponseFailed(error);
        break;
      }
      case winrt::Windows::Foundation::AsyncStatus::Canceled: {
        observerCoordinator->nativeImageResponseFailed(error);
        break;
      }
      case winrt::Windows::Foundation::AsyncStatus::Error: {
        observerCoordinator->nativeImageResponseFailed(error);
        break;
      }
      case winrt::Windows::Foundation::AsyncStatus::Started: {
      }
    }
  });
}

facebook::react::ImageRequest WindowsImageManager::requestImage(
    const facebook::react::ImageSource &imageSource,
    facebook::react::SurfaceId surfaceId) const {
  auto imageRequest = facebook::react::ImageRequest(imageSource, nullptr, {});

  auto weakObserverCoordinator = (std::weak_ptr<const facebook::react::ImageResponseObserverCoordinator>)
                                     imageRequest.getSharedObserverCoordinator();

  auto rnImageSource = winrt::Microsoft::ReactNative::Composition::implementation::MakeImageSource(imageSource);
  auto provider = m_uriImageManager->TryGetUriImageProvider(m_reactContext.Handle(), rnImageSource);

  if (auto bProvider = provider.try_as<winrt::Microsoft::ReactNative::Composition::IUriBrushProvider>()) {
    ProcessImageRequestTask<winrt::Microsoft::ReactNative::Composition::UriBrushFactory>(
        weakObserverCoordinator,
        bProvider.GetSourceAsync(m_reactContext.Handle(), rnImageSource),
        [](const winrt::Microsoft::ReactNative::Composition::UriBrushFactory &factory) noexcept {
          auto image =
              std::make_shared<winrt::Microsoft::ReactNative::Composition::implementation::ImageResponseImage>();

          // Wrap the UriBrushFactory to provide the internal CompositionContext types
          image->m_brushFactory =
              [factory](
                  const winrt::Microsoft::ReactNative::IReactContext &context,
                  const winrt::Microsoft::ReactNative::Composition::Experimental::ICompositionContext
                      &compositionContext) {
                auto compositor = winrt::Microsoft::ReactNative::Composition::Experimental::
                    MicrosoftCompositionContextHelper::InnerCompositor(compositionContext);
                auto brush = factory(context, compositor);
                return winrt::Microsoft::ReactNative::Composition::Experimental::implementation::
                    MicrosoftCompositionContextHelper::WrapBrush(brush);
              };
          return image;
        });

    return imageRequest;
  }

  if (auto brushProvider =
          provider.try_as<winrt::Microsoft::ReactNative::Composition::Experimental::IUriBrushProvider>()) {
    ProcessImageRequestTask<winrt::Microsoft::ReactNative::Composition::Experimental::UriBrushFactory>(
        weakObserverCoordinator,
        brushProvider.GetSourceAsync(m_reactContext.Handle(), rnImageSource),
        [](const winrt::Microsoft::ReactNative::Composition::Experimental::UriBrushFactory &factory) noexcept {
          auto image =
              std::make_shared<winrt::Microsoft::ReactNative::Composition::implementation::ImageResponseImage>();
          image->m_brushFactory = factory;
          return image;
        });

    return imageRequest;
  };

  winrt::Windows::Foundation::IAsyncOperation<winrt::Windows::Storage::Streams::IRandomAccessStream> task;
  if (auto imageStreamProvider =
          provider.try_as<winrt::Microsoft::ReactNative::Composition::IUriImageStreamProvider>()) {
    task = imageStreamProvider.GetSourceAsync(m_reactContext.Handle(), rnImageSource);
  } else {
    ReactImageSource source;
    source.uri = imageSource.uri;
    source.height = imageSource.size.height;
    source.width = imageSource.size.width;
    source.sourceType = ImageSourceType::Download;

    task = GetImageStreamAsync(source);
  }

  // TODO progress? - Can we register for progress off the download task?
  // observerCoordinator->nativeImageResponseProgress((float)progress / (float)total);

  ProcessImageRequestTask<winrt::Windows::Storage::Streams::IRandomAccessStream>(
      weakObserverCoordinator, task, [](const winrt::Windows::Storage::Streams::IRandomAccessStream &stream) {
        return generateBitmap(stream);
      });

  return imageRequest;
}

} // namespace Microsoft::ReactNative
