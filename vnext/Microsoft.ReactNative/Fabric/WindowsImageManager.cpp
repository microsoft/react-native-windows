// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"

#include "WindowsImageManager.h"

#include <CppRuntimeOptions.h>
#include <Fabric/Composition/CompositionContextHelper.h>
#include <Fabric/Composition/ImageResponseImage.h>
#include <Fabric/Composition/UriImageManager.h>
#include <Networking/NetworkPropertyIds.h>
#include <Utils/ImageUtils.h>
#include <fmt/format.h>
#include <functional/functor.h>
#include <shcore.h>
#include <wincodec.h>
#include <winrt/Microsoft.ReactNative.Composition.h>
#include <winrt/Windows.Web.Http.Headers.h>
#include <winrt/Windows.Web.Http.h>

extern "C" HRESULT WINAPI WICCreateImagingFactory_Proxy(UINT SDKVersion, IWICImagingFactory **ppIWICImagingFactory);

namespace Microsoft::ReactNative {

std::string FormatHResultError(winrt::hresult_error const &ex) {
  return fmt::format("[0x{:0>8x}] {}", static_cast<uint32_t>(ex.code()), winrt::to_string(ex.message()));
}

WindowsImageManager::WindowsImageManager(winrt::Microsoft::ReactNative::ReactContext reactContext)
    : m_reactContext(reactContext) {
  m_uriImageManager =
      winrt::Microsoft::ReactNative::Composition::implementation::UriImageManager::Get(reactContext.Properties());

  // Ideally we'd just set m_httpClient.DefaultRequestHeaders().UserAgent().ParseAdd(m_defaultUserAgent), but when we do
  // we start hitting E_STATE_CHANGED errors. Which appears to be this issue
  // https://github.com/MicrosoftDocs/winrt-api/issues/2410 So instead we apply the header to each request
  auto userAgent = Microsoft::React::GetRuntimeOptionString("Http.UserAgent");
  if (userAgent.size() > 0) {
    m_defaultUserAgent = winrt::to_hstring(userAgent);
  } else if (auto userAgentProp = reactContext.Properties().Get(::Microsoft::React::DefaultUserAgentPropertyId())) {
    m_defaultUserAgent = *userAgentProp;
  }
}

std::tuple<
    winrt::com_ptr<IWICBitmapSource>,
    winrt::com_ptr<IWICImagingFactory>,
    std::shared_ptr<facebook::react::ImageErrorInfo>>
wicBitmapSourceFromStream(const winrt::Windows::Storage::Streams::IRandomAccessStream &stream) noexcept {
  try {
    winrt::com_ptr<IWICImagingFactory> imagingFactory;
    winrt::check_hresult(WICCreateImagingFactory_Proxy(WINCODEC_SDK_VERSION, imagingFactory.put()));

    winrt::com_ptr<IStream> istream;
    winrt::check_hresult(
        CreateStreamOverRandomAccessStream(stream.as<IUnknown>().get(), __uuidof(IStream), istream.put_void()));

    winrt::com_ptr<IWICBitmapDecoder> bitmapDecoder;
    winrt::check_hresult(imagingFactory->CreateDecoderFromStream(
        istream.get(), nullptr, WICDecodeMetadataCacheOnDemand, bitmapDecoder.put()));

    if (!bitmapDecoder) {
      auto errorInfo = std::make_shared<facebook::react::ImageErrorInfo>();
      errorInfo->error = "Failed to decode the image.";
      return {nullptr, nullptr, errorInfo};
    }

    winrt::com_ptr<IWICBitmapFrameDecode> decodedFrame;
    winrt::check_hresult(bitmapDecoder->GetFrame(0, decodedFrame.put()));
    return {decodedFrame, imagingFactory, nullptr};
  } catch (winrt::hresult_error const &ex) {
    auto errorInfo = std::make_shared<facebook::react::ImageErrorInfo>();
    errorInfo = std::make_shared<facebook::react::ImageErrorInfo>();
    errorInfo->error = ::Microsoft::ReactNative::FormatHResultError(winrt::hresult_error(ex));
    return {nullptr, nullptr, errorInfo};
  }
}

winrt::Windows::Foundation::IAsyncOperation<winrt::Microsoft::ReactNative::Composition::ImageResponse>
WindowsImageManager::GetImageRandomAccessStreamAsync(
    ReactImageSource source,
    std::function<void(uint64_t loaded, uint64_t total)> progressCallback) const {
  co_await winrt::resume_background();

  winrt::Windows::Foundation::Uri uri(winrt::to_hstring(source.uri));
  bool isFile = (uri.SchemeName() == L"file");
  bool isAppx = (uri.SchemeName() == L"ms-appx");

  if (isFile || isAppx) {
    winrt::Windows::Foundation::IAsyncOperation<winrt::Windows::Storage::StorageFile> getFileSync{nullptr};

    if (isFile) {
      getFileSync = winrt::Windows::Storage::StorageFile::GetFileFromPathAsync(uri.AbsoluteCanonicalUri());
    } else {
      getFileSync = winrt::Windows::Storage::StorageFile::GetFileFromApplicationUriAsync(uri);
    }

    winrt::Windows::Storage::StorageFile file(co_await getFileSync);

    if (!file) {
      co_return winrt::Microsoft::ReactNative::Composition::ImageFailedResponse(L"Failed to get file.");
    }

    co_return winrt::Microsoft::ReactNative::Composition::StreamImageResponse(co_await file.OpenReadAsync());
  }

  auto httpMethod{
      source.method.empty() ? winrt::Windows::Web::Http::HttpMethod::Get()
                            : winrt::Windows::Web::Http::HttpMethod{winrt::to_hstring(source.method)}};

  winrt::Windows::Web::Http::HttpRequestMessage request{httpMethod, uri};

  if (!m_defaultUserAgent.empty()) {
    request.Headers().Append(L"User-Agent", m_defaultUserAgent);
  }

  if (!source.headers.empty()) {
    for (auto &header : source.headers) {
      if (_stricmp(header.first.c_str(), "authorization") == 0) {
        request.Headers().TryAppendWithoutValidation(winrt::to_hstring(header.first), winrt::to_hstring(header.second));
      } else {
        request.Headers().Append(winrt::to_hstring(header.first), winrt::to_hstring(header.second));
      }
    }
  }

  if (!source.body.empty()) {
    auto bodyContent = winrt::Windows::Web::Http::HttpStringContent(
        winrt::to_hstring(source.body), winrt::Windows::Storage::Streams::UnicodeEncoding::Utf8, L"application/json");
    request.Content(bodyContent);
  }

  winrt::Windows::Web::Http::HttpResponseMessage response(co_await m_httpClient.SendRequestAsync(request));

  if (!response.IsSuccessStatusCode()) {
    co_return winrt::Microsoft::ReactNative::Composition::ImageFailedResponse(
        response.ReasonPhrase(), response.StatusCode(), response.Headers());
  }

  auto inputStream = co_await response.Content().ReadAsInputStreamAsync();
  auto contentLengthRef = response.Content().Headers().ContentLength();
  uint64_t total = contentLengthRef ? contentLengthRef.GetUInt64() : 0;
  uint64_t loaded = 0;

  winrt::Windows::Storage::Streams::InMemoryRandomAccessStream memoryStream;
  winrt::Windows::Storage::Streams::DataReader reader(inputStream);
  constexpr uint32_t bufferSize = 16 * 1024;

  while (true) {
    uint32_t loadedBuffer = co_await reader.LoadAsync(bufferSize);
    if (loadedBuffer == 0)
      break;

    auto buffer = reader.ReadBuffer(loadedBuffer);
    co_await memoryStream.WriteAsync(buffer);
    loaded += loadedBuffer;

    if (progressCallback) {
      progressCallback(loaded, total);
    }
  }

  memoryStream.Seek(0);

  co_return winrt::Microsoft::ReactNative::Composition::StreamImageResponse(memoryStream.CloneStream());
}

facebook::react::ImageRequest WindowsImageManager::requestImage(
    const facebook::react::ImageSource &imageSource,
    facebook::react::SurfaceId surfaceId) const {
  auto imageRequest = facebook::react::ImageRequest(imageSource, nullptr, {});

  auto weakObserverCoordinator = (std::weak_ptr<const facebook::react::ImageResponseObserverCoordinator>)
                                     imageRequest.getSharedObserverCoordinator();

  auto rnImageSource = winrt::Microsoft::ReactNative::Composition::implementation::MakeImageSource(imageSource);
  auto provider = m_uriImageManager->TryGetUriImageProvider(m_reactContext.Handle(), rnImageSource);

  winrt::Windows::Foundation::IAsyncOperation<winrt::Microsoft::ReactNative::Composition::ImageResponse>
      imageResponseTask{nullptr};

  if (provider) {
    imageResponseTask = provider.GetImageResponseAsync(m_reactContext.Handle(), rnImageSource);
  } else {
    ReactImageSource source;
    source.uri = imageSource.uri;
    source.height = imageSource.size.height;
    source.width = imageSource.size.width;
    source.sourceType = ImageSourceType::Download;

    auto progressCallback = [weakObserverCoordinator](int64_t loaded, int64_t total) {
      if (auto observerCoordinator = weakObserverCoordinator.lock()) {
        float progress = total > 0 ? static_cast<float>(loaded) / static_cast<float>(total) : 1.0f;
        observerCoordinator->nativeImageResponseProgress(progress, loaded, total);
      }
    };
    imageResponseTask = GetImageRandomAccessStreamAsync(source, progressCallback);
  }

  imageResponseTask.Completed([weakObserverCoordinator](auto asyncOp, auto status) {
    auto observerCoordinator = weakObserverCoordinator.lock();
    if (!observerCoordinator) {
      return;
    }

    switch (status) {
      case winrt::Windows::Foundation::AsyncStatus::Completed: {
        auto imageResponse = asyncOp.GetResults();
        auto selfImageResponse =
            winrt::get_self<winrt::Microsoft::ReactNative::Composition::implementation::ImageResponse>(imageResponse);
        auto imageResultOrError = selfImageResponse->ResolveImage();
        if (imageResultOrError.image) {
          observerCoordinator->nativeImageResponseComplete(
              facebook::react::ImageResponse(imageResultOrError.image, nullptr /*metadata*/));
        } else {
          observerCoordinator->nativeImageResponseFailed(facebook::react::ImageLoadError(imageResultOrError.errorInfo));
        }
        break;
      }
      case winrt::Windows::Foundation::AsyncStatus::Canceled: {
        auto errorInfo = std::make_shared<facebook::react::ImageErrorInfo>();
        errorInfo->error = FormatHResultError(winrt::hresult_error(asyncOp.ErrorCode()));
        observerCoordinator->nativeImageResponseFailed(facebook::react::ImageLoadError(errorInfo));
        break;
      }
      case winrt::Windows::Foundation::AsyncStatus::Error: {
        auto errorInfo = std::make_shared<facebook::react::ImageErrorInfo>();
        errorInfo->error = FormatHResultError(winrt::hresult_error(asyncOp.ErrorCode()));
        observerCoordinator->nativeImageResponseFailed(facebook::react::ImageLoadError(errorInfo));
        break;
      }
    }
  });
  return imageRequest;
}

} // namespace Microsoft::ReactNative

namespace winrt::Microsoft::ReactNative::Composition::implementation {

ImageResponseOrImageErrorInfo StreamImageResponse::ResolveImage() {
  ImageResponseOrImageErrorInfo imageOrError;
  try {
    auto result = ::Microsoft::ReactNative::wicBitmapSourceFromStream(m_stream);

    if (auto errorInfo = std::get<std::shared_ptr<facebook::react::ImageErrorInfo>>(result)) {
      imageOrError.errorInfo = errorInfo;
      return imageOrError;
    }

    auto imagingFactory = std::get<winrt::com_ptr<IWICImagingFactory>>(result);
    auto decodedFrame = std::get<winrt::com_ptr<IWICBitmapSource>>(result);

    winrt::com_ptr<IWICFormatConverter> converter;
    winrt::check_hresult(imagingFactory->CreateFormatConverter(converter.put()));

    winrt::check_hresult(converter->Initialize(
        decodedFrame.get(),
        GUID_WICPixelFormat32bppPBGRA,
        WICBitmapDitherTypeNone,
        nullptr,
        0.0f,
        WICBitmapPaletteTypeMedianCut));

    imageOrError.image =
        std::make_shared<winrt::Microsoft::ReactNative::Composition::implementation::ImageResponseImage>();
    winrt::check_hresult(imagingFactory->CreateBitmapFromSource(
        converter.get(), WICBitmapCacheOnLoad, imageOrError.image->m_wicbmp.put()));
  } catch (winrt::hresult_error const &ex) {
    imageOrError.errorInfo = std::make_shared<facebook::react::ImageErrorInfo>();
    imageOrError.errorInfo->error = ::Microsoft::ReactNative::FormatHResultError(winrt::hresult_error(ex));
  }
  return imageOrError;
}

ImageResponseOrImageErrorInfo UriBrushFactoryImageResponse::ResolveImage() {
  ImageResponseOrImageErrorInfo imageOrError;
  imageOrError.image =
      std::make_shared<winrt::Microsoft::ReactNative::Composition::implementation::ImageResponseImage>();

  // Wrap the UriBrushFactory to provide the internal CompositionContext types
  imageOrError.image
      ->m_brushFactory = [factory = m_factory](
                             const winrt::Microsoft::ReactNative::IReactContext &context,
                             const winrt::Microsoft::ReactNative::Composition::Experimental::ICompositionContext
                                 &compositionContext) {
    auto compositor =
        winrt::Microsoft::ReactNative::Composition::Experimental::MicrosoftCompositionContextHelper::InnerCompositor(
            compositionContext);
    auto brush = factory(context, compositor);
    return winrt::Microsoft::ReactNative::Composition::Experimental::implementation::MicrosoftCompositionContextHelper::
        WrapBrush(brush);
  };
  return imageOrError;
}

} // namespace winrt::Microsoft::ReactNative::Composition::implementation

namespace winrt::Microsoft::ReactNative::Composition::Experimental::implementation {

winrt::Microsoft::ReactNative::Composition::implementation::ImageResponseOrImageErrorInfo
UriBrushFactoryImageResponse::ResolveImage() {
  winrt::Microsoft::ReactNative::Composition::implementation::ImageResponseOrImageErrorInfo imageOrError;
  imageOrError.image =
      std::make_shared<winrt::Microsoft::ReactNative::Composition::implementation::ImageResponseImage>();
  imageOrError.image->m_brushFactory = m_factory;
  return imageOrError;
}

} // namespace winrt::Microsoft::ReactNative::Composition::Experimental::implementation
