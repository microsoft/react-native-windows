// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"
#include "ImageUtils.h"

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

winrt::IAsyncOperation<winrt::IRandomAccessStream> GetImageStreamAsync(ReactImageSource source) {
  try {
    co_await winrt::resume_background();

    auto httpMethod{
        source.method.empty() ? winrt::HttpMethod::Get() : winrt::HttpMethod{winrt::to_hstring(source.method)}};

    winrt::Uri uri = UriTryCreate(winrt::to_hstring(source.uri));
    if (!uri) {
      co_return nullptr;
    }
    winrt::HttpRequestMessage request{httpMethod, uri};

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

winrt::IAsyncOperation<winrt::IRandomAccessStream> GetImageMemoryStreamAsync(ReactImageSource source) {
  switch (source.sourceType) {
    case ImageSourceType::Download:
      co_return co_await GetImageStreamAsync(source);
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
