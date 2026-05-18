// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"
#include "UriImageManager.h"

#include "Composition.ImageSource.g.h"
#include <Composition.Experimental.UriBrushFactoryImageResponse.g.cpp>
#include <Composition.ImageFailedResponse.g.cpp>
#include <Composition.ImageResponse.g.cpp>
#include <Composition.StreamImageResponse.g.cpp>
#include <Composition.UriBrushFactoryImageResponse.g.cpp>
#include <AutoDraw.h>
#include <IBlobPersistor.h>
#include <Networking/NetworkPropertyIds.h>
#include <ReactPropertyBag.h>
#include <d2d1_3.h>
#include <shcore.h>
#include <winrt/Microsoft.ReactNative.Composition.Experimental.h>
#include <winrt/Microsoft.ReactNative.Composition.h>
#include <winrt/Windows.Security.Cryptography.h>
#include <winrt/Windows.Storage.Streams.h>

namespace winrt::Microsoft::ReactNative::Composition::implementation {

struct ImageSource : ImageSourceT<ImageSource> {
  ImageSource(const facebook::react::ImageSource &source)
      : m_size({source.size.width, source.size.height}),
        m_scale(source.scale),
        m_uri(::Microsoft::ReactNative::UriTryCreate(winrt::to_hstring(source.uri))) {}

  winrt::Windows::Foundation::Uri Uri() noexcept {
    return m_uri;
  }

  winrt::Windows::Foundation::Size Size() noexcept {
    return m_size;
  }

  float Scale() noexcept {
    return m_scale;
  }

 private:
  const winrt::Windows::Foundation::Uri m_uri;
  const winrt::Windows::Foundation::Size m_size;
  const float m_scale;
};

winrt::Microsoft::ReactNative::Composition::ImageSource MakeImageSource(
    const facebook::react::ImageSource &source) noexcept {
  return winrt::make<ImageSource>(source);
}

/**
 * This ImageHandler will handle uri types with svgxaml base64 encoded data
 *
 *   <Image
 *      style={{width: 400, height: 200}}
 *      source={{uri:
 *       'data:image/svg+xml;base64,PD94bWwgdmVyc2lvbj0iMS4wIiBlbmNvZGluZz0iVVRGLTgiPz4gPHN2ZyB4bWxucz0iaHR0cDovL3d3dy53My5vcmcvMjAwMC9zdmciIHZpZXdCb3g9IjAgMCAyOCAyOCIgZmlsbD0ibm9uZSI+PHBhdGggZD0iTTEzLjEyNSAwSDBWMTMuMTI1SDEzLjEyNVYwWiIgZmlsbD0iI0YyNTAyMiI+PC9wYXRoPjxwYXRoIGQ9Ik0yOCAwSDE0Ljg3NVYxMy4xMjVIMjhWMFoiIGZpbGw9IiM3RkJBMDAiPjwvcGF0aD48cGF0aCBkPSJNMTMuMTI1IDE0Ljg3NUgwVjI4SDEzLjEyNVYxNC44NzVaIiBmaWxsPSIjMDBBNEVGIj48L3BhdGg+PHBhdGggZD0iTTI4IDE0Ljg3NUgxNC44NzVWMjhIMjhWMTQuODc1WiIgZmlsbD0iI0ZGQjkwMCI+PC9wYXRoPjwvc3ZnPiA='}}
 *   />
 *
 */
struct SvgDataImageHandler
    : winrt::implements<SvgDataImageHandler, winrt::Microsoft::ReactNative::Composition::IUriImageProvider> {
  bool CanLoadImageUri(winrt::Microsoft::ReactNative::IReactContext context, winrt::Windows::Foundation::Uri uri) {
    return uri.SchemeName() == L"data" && std::wstring_view(uri.Path()).starts_with(L"image/svg+xml;base64,");
  }

  winrt::Windows::Foundation::IAsyncOperation<winrt::Microsoft::ReactNative::Composition::ImageResponse>
  GetImageResponseAsync(
      const winrt::Microsoft::ReactNative::IReactContext &context,
      const winrt::Microsoft::ReactNative::Composition::ImageSource &imageSource) {
    auto path = winrt::to_string(imageSource.Uri().Path());
    auto size = imageSource.Size();
    auto scale = imageSource.Scale();

    size_t start = path.find(',');
    if (start == std::string::npos || start + 1 > path.length())
      co_return nullptr;

    try {
      co_await winrt::resume_background();

      std::string_view base64String(path.c_str() + start + 1, path.length() - start - 1);
      auto buffer = winrt::Windows::Security::Cryptography::CryptographicBuffer::DecodeFromBase64String(
          winrt::to_hstring(base64String));

      winrt::Windows::Storage::Streams::InMemoryRandomAccessStream memoryStream;
      co_await memoryStream.WriteAsync(buffer);
      memoryStream.Seek(0);

      co_return winrt::Microsoft::ReactNative::Composition::Experimental::UriBrushFactoryImageResponse(
          [memoryStream, size, scale](
              const winrt::Microsoft::ReactNative::IReactContext &reactContext,
              const winrt::Microsoft::ReactNative::Composition::Experimental::ICompositionContext &compositionContext)
              -> winrt::Microsoft::ReactNative::Composition::Experimental::IBrush {
            auto drawingBrush = compositionContext.CreateDrawingSurfaceBrush(
                size,
                winrt::Windows::Graphics::DirectX::DirectXPixelFormat::B8G8R8A8UIntNormalized,
                winrt::Windows::Graphics::DirectX::DirectXAlphaMode::Premultiplied);
            POINT pt;
            ::Microsoft::ReactNative::Composition::AutoDrawDrawingSurface autoDraw(drawingBrush, 1.0, &pt);
            auto renderTarget = autoDraw.GetRenderTarget();

            // Defensive check: ensure device context is valid before D2D operations
            if (!renderTarget) {
              return nullptr;
            }

            winrt::com_ptr<ID2D1DeviceContext5> deviceContext5;
            winrt::check_hresult(renderTarget->QueryInterface(IID_ID2D1DeviceContext5, deviceContext5.put_void()));

            winrt::com_ptr<IStream> nativeStream;

            winrt::check_hresult(CreateStreamOverRandomAccessStream(
                memoryStream.as<::IUnknown>().get(), IID_PPV_ARGS(nativeStream.put())));

            winrt::com_ptr<ID2D1SvgDocument> svgDocument;
            if (FAILED(deviceContext5->CreateSvgDocument(
                    nativeStream.get(), {size.Width, size.Height}, svgDocument.put()))) {
              return nullptr;
            }

            D2D1::Matrix3x2F originalTransform;
            D2D1::Matrix3x2F translationTransform =
                D2D1::Matrix3x2F::Translation(static_cast<float>(pt.x), static_cast<float>(pt.y));

            renderTarget->GetTransform(&originalTransform);
            translationTransform = originalTransform * translationTransform;

            renderTarget->SetTransform(translationTransform);

            deviceContext5->DrawSvgDocument(svgDocument.get());

            renderTarget->SetTransform(originalTransform);

            return drawingBrush;
          });

    } catch (winrt::hresult_error const &ex) {
      co_return winrt::Microsoft::ReactNative::Composition::ImageFailedResponse(ex.message());
    }

    winrt::throw_hresult(E_UNEXPECTED);
  }
};

/**
 * This ImageHandler will handle uri types of base64 encoded data
 *
 *   <Image
 *      style={{width: 400, height: 200}}
 *      source={{uri:
 * 'data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAAEsAAABLCAQAAACSR7JhAAADtUlEQVR4Ac3YA2Bj6QLH0XPT1Fzbtm29tW3btm3bfLZtv7e2ObZnms7d8Uw098tuetPzrxv8wiISrtVudrG2JXQZ4VOv+qUfmqCGGl1mqLhoA52oZlb0mrjsnhKpgeUNEs91Z0pd1kvihA3ULGVHiQO2narKSHKkEMulm9VgUyE60s1aWoMQUbpZOWE+kaqs4eLEjdIlZTcFZB0ndc1+lhB1lZrIuk5P2aib1NBpZaL+JaOGIt0ls47SKzLC7CqrlGF6RZ09HGoNy1lYl2aRSWL5GuzqWU1KafRdoRp0iOQEiDzgZPnG6DbldcomadViflnl/cL93tOoVbsOLVM2jylvdWjXolWX1hmfZbGR/wjypDjFLSZIRov09BgYmtUqPQPlQrPapecLgTIy0jMgPKtTeob2zWtrGH3xvjUkPCtNg/tm1rjwrMa+mdUkPd3hWbH0jArPGiU9ufCsNNWFZ40wpwn+62/66R2RUtoso1OB34tnLOcy7YB1fUdc9e0q3yru8PGM773vXsuZ5YIZX+5xmHwHGVvlrGPN6ZSiP1smOsMMde40wKv2VmwPPVXNut4sVpUreZiLBHi0qln/VQeI/LTMYXpsJtFiclUN+5HVZazim+Ky+7sAvxWnvjXrJFneVtLWLyPJu9K3cXLWeOlbMTlrIelbMDlrLenrjEQOtIF+fuI9xRp9ZBFp6+b6WT8RrxEpdK64BuvHgDk+vUy+b5hYk6zfyfs051gRoNO1usU12WWRWL73/MMEy9pMi9qIrR4ZpV16Rrvduxazmy1FSvuFXRkqTnE7m2kdb5U8xGjLw/spRr1uTov4uOgQE+0N/DvFrG/Jt7i/FzwxbA9kDanhf2w+t4V97G8lrT7wc08aA2QNUkuTfW/KimT01wdlfK4yEw030VfT0RtZbzjeMprNq8m8tnSTASrTLti64oBNdpmMQm0eEwvfPwRbUBywG5TzjPCsdwk3IeAXjQblLCoXnDVeoAz6SfJNk5TTzytCNZk/POtTSV40NwOFWzw86wNJRpubpXsn60NJFlHeqlYRbslqZm2jnEZ3qcSKgm0kTli3zZVS7y/iivZTweYXJ26Y+RTbV1zh3hYkgyFGSTKPfRVbRqWWVReaxYeSLarYv1Qqsmh1s95S7G+eEWK0f3jYKTbV6bOwepjfhtafsvUsqrQvrGC8YhmnO9cSCk3yuY984F1vesdHYhWJ5FvASlacshUsajFt2mUM9pqzvKGcyNJW0arTKN1GGGzQlH0tXwLDgQTurS8eIQAAAABJRU5ErkJggg=='}}
 *   />
 *
 */
struct DataImageHandler
    : winrt::implements<DataImageHandler, winrt::Microsoft::ReactNative::Composition::IUriImageProvider> {
  bool CanLoadImageUri(winrt::Microsoft::ReactNative::IReactContext context, winrt::Windows::Foundation::Uri uri) {
    return uri.SchemeName() == L"data";
  }

  winrt::Windows::Foundation::IAsyncOperation<winrt::Microsoft::ReactNative::Composition::ImageResponse>
  GetImageResponseAsync(
      const winrt::Microsoft::ReactNative::IReactContext &context,
      const winrt::Microsoft::ReactNative::Composition::ImageSource &imageSource) {
    auto path = winrt::to_string(imageSource.Uri().Path());

    size_t start = path.find(',');
    if (start == std::string::npos || start + 1 > path.length())
      co_return nullptr;

    try {
      co_await winrt::resume_background();

      std::string_view base64String(path.c_str() + start + 1, path.length() - start - 1);
      auto buffer = winrt::Windows::Security::Cryptography::CryptographicBuffer::DecodeFromBase64String(
          winrt::to_hstring(base64String));

      winrt::Windows::Storage::Streams::InMemoryRandomAccessStream memoryStream;
      co_await memoryStream.WriteAsync(buffer);
      memoryStream.Seek(0);

      co_return winrt::Microsoft::ReactNative::Composition::StreamImageResponse(memoryStream);
    } catch (winrt::hresult_error const &) {
      // Base64 decode failed
      co_return winrt::Microsoft::ReactNative::Composition::ImageFailedResponse(
          L"Invalid base64 encoding in inline image data");
    }

    winrt::throw_hresult(E_UNEXPECTED);
  }
};

/**
 * This ImageHandler will handle uri loading data from blobs
 *
 *   <Image
 *      style={{width: 400, height: 200}}
 *      source={{uri:'blob:<guid>?offset=<offset>&size=<size>'}}
 *   />
 *
 */
struct BlobImageHandler
    : winrt::implements<BlobImageHandler, winrt::Microsoft::ReactNative::Composition::IUriImageProvider> {
  bool CanLoadImageUri(winrt::Microsoft::ReactNative::IReactContext context, winrt::Windows::Foundation::Uri uri) {
    return uri.SchemeName() == L"blob";
  }

  winrt::Windows::Foundation::IAsyncOperation<winrt::Microsoft::ReactNative::Composition::ImageResponse>
  GetImageResponseAsync(
      const winrt::Microsoft::ReactNative::IReactContext &context,
      const winrt::Microsoft::ReactNative::Composition::ImageSource &imageSource) {
    if (auto prop = winrt::Microsoft::ReactNative::ReactPropertyBag(context.Properties())
                        .Get(::Microsoft::React::BlobModulePersistorPropertyId())) {
      auto weakBlobPersistor = prop.Value();
      if (auto persistor = weakBlobPersistor.lock()) {
        auto queryParsed = imageSource.Uri().QueryParsed();
        auto guid = winrt::to_string(imageSource.Uri().Path());
        int64_t offset = _atoi64(winrt::to_string(queryParsed.GetFirstValueByName(L"offset")).c_str());
        int64_t size = _atoi64(winrt::to_string(queryParsed.GetFirstValueByName(L"size")).c_str());

        auto arr = persistor->ResolveMessage(std::move(guid), offset, size);
        winrt::Windows::Storage::Streams::InMemoryRandomAccessStream memoryStream;
        winrt::Windows::Storage::Streams::DataWriter dataWriter{memoryStream};
        dataWriter.WriteBytes(arr);
        co_await dataWriter.StoreAsync();
        memoryStream.Seek(0);

        co_return winrt::Microsoft::ReactNative::Composition::StreamImageResponse(memoryStream.CloneStream());
      }
    }

    co_return winrt::Microsoft::ReactNative::Composition::ImageFailedResponse(L"Failed to load image from blob");
  }
};

static const ReactPropertyId<ReactNonAbiValue<std::shared_ptr<UriImageManager>>> &UriImageManagerPropertyId() noexcept {
  static const ReactPropertyId<ReactNonAbiValue<std::shared_ptr<UriImageManager>>> prop{
      L"ReactNative", L"UriImageManager"};
  return prop;
}

UriImageManager::UriImageManager() {
  m_providers.push_back(winrt::make<SvgDataImageHandler>());
  m_providers.push_back(winrt::make<DataImageHandler>());
  m_providers.push_back(winrt::make<BlobImageHandler>());
}

void UriImageManager::Install(
    const winrt::Microsoft::ReactNative::ReactPropertyBag &properties,
    const std::shared_ptr<UriImageManager> &manager) noexcept {
  properties.Set(UriImageManagerPropertyId(), manager);
}

std::shared_ptr<UriImageManager> UriImageManager::Get(
    const winrt::Microsoft::ReactNative::ReactPropertyBag &properties) noexcept {
  return winrt::Microsoft::ReactNative::ReactPropertyBag(properties).Get(UriImageManagerPropertyId()).Value();
}

void UriImageManager::AddUriImageProvider(const IUriImageProvider &provider) {
  if (!provider)
    winrt::throw_hresult(E_INVALIDARG);
  m_providers.push_back(provider);
}

IUriImageProvider UriImageManager::TryGetUriImageProvider(
    const IReactContext &context,
    winrt::Microsoft::ReactNative::Composition::ImageSource &source) noexcept {
  auto uri = source.Uri();
  if (!uri) {
    return nullptr;
  }

  for (auto &provider : m_providers) {
    if (provider.CanLoadImageUri(context, source.Uri())) {
      return provider;
    }
  }

  return nullptr;
}

ImageResponseOrImageErrorInfo ImageResponse::ResolveImage() {
  winrt::throw_hresult(E_NOTIMPL);
}

ImageResponseOrImageErrorInfo ImageFailedResponse::ResolveImage() {
  ImageResponseOrImageErrorInfo imageOrError;
  imageOrError.errorInfo = std::make_shared<facebook::react::ImageErrorInfo>();
  imageOrError.errorInfo->responseCode = static_cast<int>(m_statusCode);
  imageOrError.errorInfo->error = winrt::to_string(m_errorMessage);
  if (imageOrError.errorInfo->error.empty()) {
    imageOrError.errorInfo->error = "Failed to load image.";
  }
  if (m_responseHeaders) {
    for (auto &&[header, value] : m_responseHeaders) {
      imageOrError.errorInfo->httpResponseHeaders.push_back(
          std::make_pair<std::string, std::string>(winrt::to_string(header), winrt::to_string(value)));
    }
  }
  return imageOrError;
}

} // namespace winrt::Microsoft::ReactNative::Composition::implementation
