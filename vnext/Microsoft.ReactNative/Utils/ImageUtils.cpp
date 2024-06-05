// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"
#include "ImageUtils.h"

#include <CppRuntimeOptions.h>
#include <Networking/NetworkPropertyIds.h>
#include <Shared/cdebug.h>
#include <Utils/CppWinrtLessExceptions.h>
#include <windows.Web.Http.h>
#include <winrt/Windows.Security.Cryptography.h>
#include <winrt/Windows.Web.Http.Headers.h>
#include <winrt/Windows.Web.Http.h>

namespace winrt {
using namespace Windows::Foundation;
using namespace Windows::Storage::Streams;
using namespace Windows::Web::Http;
} // namespace winrt

namespace Microsoft::ReactNative {

winrt::IAsyncOperation<winrt::IRandomAccessStream> GetImageStreamAsync(
    winrt::Microsoft::ReactNative::IReactPropertyBag properties,
    ReactImageSource source) {
  try {
    co_await winrt::resume_background();

    winrt::Uri uri = UriTryCreate(winrt::to_hstring(source.uri));
    if (!uri) {
      co_return nullptr;
    }

    bool isFile = (uri.SchemeName() == L"file");
    bool isAppx = (uri.SchemeName() == L"ms-appx");

    if (isFile || isAppx) {
      winrt::Windows::Foundation::IAsyncOperation<winrt::Windows::Storage::StorageFile> getFileSync{nullptr};

      if (isFile) {
        getFileSync = winrt::Windows::Storage::StorageFile::GetFileFromPathAsync(uri.AbsoluteCanonicalUri());
      } else {
        getFileSync = winrt::Windows::Storage::StorageFile::GetFileFromApplicationUriAsync(uri);
      }

      co_await lessthrow_await_adapter<
          winrt::Windows::Foundation::IAsyncOperation<winrt::Windows::Storage::StorageFile>>{getFileSync};

      if (FAILED(getFileSync.ErrorCode())) {
        co_return nullptr;
      }

      winrt::Windows::Storage::StorageFile file{getFileSync.GetResults()};

      auto openReadAsync = file.OpenReadAsync();
      co_await lessthrow_await_adapter<winrt::Windows::Foundation::IAsyncOperation<
          winrt::Windows::Storage::Streams::IRandomAccessStreamWithContentType>>{openReadAsync};

      if (FAILED(openReadAsync.ErrorCode())) {
        co_return nullptr;
      }

      winrt::Windows::Storage::Streams::IRandomAccessStreamWithContentType stream{openReadAsync.GetResults()};
      co_return stream;
    }

    auto httpMethod{
        source.method.empty() ? winrt::HttpMethod::Get() : winrt::HttpMethod{winrt::to_hstring(source.method)}};

    winrt::HttpRequestMessage request{httpMethod, uri};

    // Ideally we'd just set m_httpClient.DefaultRequestHeaders().UserAgent().ParseAdd(m_defaultUserAgent), but when we
    // do we start hitting E_STATE_CHANGED errors. Which appears to be this issue
    // https://github.com/MicrosoftDocs/winrt-api/issues/2410 So instead we apply the header to each request
    auto userAgent = Microsoft::React::GetRuntimeOptionString("Http.UserAgent");
    if (userAgent.size() > 0) {
      request.Headers().Append(L"User-Agent", winrt::to_hstring(userAgent));
    } else {
      if (auto userAgentProp = winrt::Microsoft::ReactNative::ReactPropertyBag(properties)
                                   .Get(::Microsoft::React::DefaultUserAgentPropertyId())) {
        request.Headers().Append(L"User-Agent", winrt::to_hstring(*userAgentProp));
      }
    }

    if (!source.headers.empty()) {
      for (auto &header : source.headers) {
        if (_stricmp(header.first.c_str(), "authorization") == 0) {
          request.Headers().TryAppendWithoutValidation(
              winrt::to_hstring(header.first), winrt::to_hstring(header.second));
        } else {
          request.Headers().Append(winrt::to_hstring(header.first), winrt::to_hstring(header.second));
        }
      }
    }

    winrt::HttpClient httpClient;
    auto httpClientAbi = reinterpret_cast<ABI::Windows::Web::Http::IHttpClient *>(winrt::get_abi(httpClient));

    winrt::Windows::Foundation::IAsyncOperationWithProgress<
        winrt::Windows::Web::Http::HttpResponseMessage,
        winrt::Windows::Web::Http::HttpProgress>
        asyncRequest{nullptr};

    if (FAILED(httpClientAbi->SendRequestAsync(
            reinterpret_cast<ABI::Windows::Web::Http::IHttpRequestMessage *>(winrt::get_abi(request)),
            reinterpret_cast<
                ABI::Windows::Foundation::
                    __FIAsyncOperationWithProgress_2_Windows__CWeb__CHttp__CHttpResponseMessage_Windows__CWeb__CHttp__CHttpProgress_t
                        **>(winrt::put_abi(asyncRequest))))) {
      co_return nullptr;
    }

    if (!asyncRequest) {
      co_return nullptr;
    }

    co_await lessthrow_await_adapter<winrt::Windows::Foundation::IAsyncOperationWithProgress<
        winrt::Windows::Web::Http::HttpResponseMessage,
        winrt::Windows::Web::Http::HttpProgress>>{asyncRequest};

    if (FAILED(asyncRequest.ErrorCode())) {
      co_return nullptr;
    }

    winrt::HttpResponseMessage response{asyncRequest.GetResults()};

    if (response && response.StatusCode() == winrt::HttpStatusCode::Ok) {
      auto asyncRead = response.Content().ReadAsInputStreamAsync();
      co_await lessthrow_await_adapter<winrt::Windows::Foundation::IAsyncOperationWithProgress<
          winrt::Windows::Storage::Streams::IInputStream,
          uint64_t>>{asyncRead};
      if (FAILED(asyncRead.ErrorCode())) {
        co_return nullptr;
      }

      winrt::IInputStream inputStream{asyncRead.GetResults()};
      winrt::InMemoryRandomAccessStream memoryStream;

      auto asyncCopy = winrt::RandomAccessStream::CopyAsync(inputStream, memoryStream);
      co_await lessthrow_await_adapter<winrt::Windows::Foundation::IAsyncOperationWithProgress<uint64_t, uint64_t>>{
          asyncCopy};
      if (FAILED(asyncCopy.ErrorCode())) {
        co_return nullptr;
      }

      memoryStream.Seek(0);
      co_return memoryStream;
    }
  } catch (winrt::hresult_error const &e) {
    DEBUG_HRESULT_ERROR(e);
  }

  co_return nullptr;
}

winrt::IAsyncOperation<winrt::IRandomAccessStream> GetImageInlineDataAsync(ReactImageSource source) {
  size_t start = source.uri.find(',');
  if (start == std::string::npos || start + 1 > source.uri.length())
    co_return nullptr;

  try {
    co_await winrt::resume_background();

    std::string_view base64String(source.uri.c_str() + start + 1, source.uri.length() - start - 1);
    auto buffer = winrt::Windows::Security::Cryptography::CryptographicBuffer::DecodeFromBase64String(
        winrt::to_hstring(base64String));

    winrt::InMemoryRandomAccessStream memoryStream;
    co_await memoryStream.WriteAsync(buffer);
    memoryStream.Seek(0);

    co_return memoryStream;
  } catch (winrt::hresult_error const &) {
    // Base64 decode failed
  }

  co_return nullptr;
}

winrt::IAsyncOperation<winrt::IRandomAccessStream> GetImageMemoryStreamAsync(
    winrt::Microsoft::ReactNative::IReactPropertyBag properties,
    ReactImageSource source) {
  switch (source.sourceType) {
    case ImageSourceType::Download:
      co_return co_await GetImageStreamAsync(properties, source);
    case ImageSourceType::InlineData:
      co_return co_await GetImageInlineDataAsync(source);
    default: // ImageSourceType::Uri
      co_return nullptr;
  }
}

// C# provides System.Uri.TryCreate, but no native equivalent seems to exist
winrt::Uri UriTryCreate(winrt::param::hstring const &uri) {
  auto factory = winrt::
      get_activation_factory<winrt::Windows::Foundation::Uri, winrt::Windows::Foundation::IUriRuntimeClassFactory>();
  auto abiFactory = static_cast<ABI::Windows::Foundation::IUriRuntimeClassFactory *>(winrt::get_abi(factory));

  const winrt::hstring &localUri = uri;
  winrt::Windows::Foundation::Uri returnValue{nullptr};
  if (FAILED(abiFactory->CreateUri(
          static_cast<HSTRING>(winrt::get_abi(localUri)),
          reinterpret_cast<ABI::Windows::Foundation::IUriRuntimeClass **>(winrt::put_abi(returnValue))))) {
    return winrt::Windows::Foundation::Uri{nullptr};
  }
  return returnValue;
}

} // namespace Microsoft::ReactNative
