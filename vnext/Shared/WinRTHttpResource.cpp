// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "WinRTHttpResource.h"

//#include <Utils/WinRTConversions.h>
#include <Utils/CppWinrtLessExceptions.h>
#include <utilities.h>

// Windows API
#include <winrt/Windows.Security.Cryptography.h>
#include <winrt/Windows.Storage.Streams.h>
#include <winrt/Windows.Web.Http.Headers.h>

using std::function;
using std::shared_ptr;
using std::string;

using winrt::Windows::Foundation::IAsyncOperationWithProgress;
using winrt::Windows::Foundation::Uri;
using winrt::Windows::Security::Cryptography::CryptographicBuffer;
using winrt::Windows::Storage::Streams::DataReader;
using winrt::Windows::Storage::Streams::UnicodeEncoding;
using winrt::Windows::Storage::StorageFile;//
using winrt::Windows::Web::Http::Headers::HttpMediaTypeHeaderValue;
using winrt::Windows::Web::Http::HttpBufferContent;
using winrt::Windows::Web::Http::HttpClient;
using winrt::Windows::Web::Http::HttpMethod;
using winrt::Windows::Web::Http::HttpProgress;
using winrt::Windows::Web::Http::HttpRequestMessage;
using winrt::Windows::Web::Http::HttpResponseMessage;
using winrt::Windows::Web::Http::HttpStreamContent;//
using winrt::Windows::Web::Http::HttpStringContent;
using winrt::Windows::Web::Http::IHttpContent;
using winrt::fire_and_forget;
using winrt::to_hstring;

namespace {

} // namespace <anonymous>

namespace Microsoft::React {

#pragma region WinRTHttpResource


#pragma region IHttpResource

void WinRTHttpResource::SendRequest(
    const string &method,
    const string &url,
    const Headers &headers,
    BodyData &&bodyData,
    const string &responseType,
    bool useIncrementalUpdates,
    int64_t timeout,
    bool withCredentials,
    std::function<void(int64_t)> &&callback) noexcept /*override*/ {
  // TODO: Mange request ID?

  // Enforce supported args
  assert(responseType == "text" || responseType == "base64");

  // TODO:Callback?

  // TODO: Use exception->hresult_error conversion
  try {
    HttpMethod httpMethod{to_hstring(method)};
    Uri uri{to_hstring(url)};
    HttpRequestMessage request{httpMethod, uri};
    HttpMediaTypeHeaderValue contentType{nullptr};
    string contentEncoding;
    string contentLength;

    // TODO: Check casing
    for (auto &header : headers) {
      if (header.first == "content-type") {
        bool success = HttpMediaTypeHeaderValue::TryParse(to_hstring(header.first), contentType);
        // TODO: Do something with result.
      } else if (header.first == "content-encoding") {
        contentEncoding = header.second;
      } else if (header.first == "content-length") {
        contentLength = header.second;
      } else if (header.first == "authorization") {
        bool success =
            request.Headers().TryAppendWithoutValidation(to_hstring(header.first), to_hstring(header.second));
      } else {
        request.Headers().Append(to_hstring(header.first), to_hstring(header.second));
      }
    }

    IHttpContent content{nullptr};
    if (BodyData::Type::String == bodyData.Type) {
      content = HttpStringContent{to_hstring(bodyData.Data)};
    } else if (BodyData::Type::Base64 == bodyData.Type) {
      auto buffer = CryptographicBuffer::DecodeFromBase64String(to_hstring(bodyData.Data));
      content = HttpBufferContent{buffer};
    } else if (BodyData::Type::Uri == bodyData.Type) {
      auto file = StorageFile::GetFileFromApplicationUriAsync(Uri{to_hstring(bodyData.Data)}).get(); // TODO: async??
      auto stream = file.OpenReadAsync().get();
      content = HttpStreamContent{stream};
    } else if (BodyData::Type::Form == bodyData.Type) {
      // TODO: Add support
    } else {
      // BodyData::Type::Empty
    }

    if (content != nullptr) {
      // TODO: Attach headers to content
      request.Content(content);
    }

    // TODO: PerformSendRequest

  } catch (...) { // TODO: Delcare specific exception types
    // TODO: OnRequestError
  }
}

void WinRTHttpResource::AbortRequest() noexcept /*override*/ {}

void WinRTHttpResource::ClearCookies() noexcept /*override*/ {}

void WinRTHttpResource::SetOnRequest(function<void(int64_t requestId)> &&handler) noexcept /*override*/ {
  m_onRequest = std::move(handler);
}

void WinRTHttpResource::SetOnResponse(function<void(int64_t requestId, Response&& response)> &&handler) noexcept /*override*/ {
  m_onResponse = std::move(handler);
}

void WinRTHttpResource::SetOnData(function<void(int64_t requestId, std::string&& responseData)> &&handler) noexcept /*override*/ {
  m_onData = std::move(handler);
}

void WinRTHttpResource::SetOnError(function<void(int64_t requestId, string && message)> &&handler) noexcept /*override*/ {
  m_onError = std::move(handler);
}

#pragma endregion IHttpResource

fire_and_forget WinRTHttpResource::PerformSendRequest(HttpClient client, HttpRequestMessage request, bool textResponse) noexcept {
  auto self = shared_from_this();
  //TODO: Set timeout?

  try {
    auto async = client.SendRequestAsync(request);
    //TODO: Add request?

    co_await lessthrow_await_adapter<IAsyncOperationWithProgress<HttpResponseMessage, HttpProgress>>{async};
    auto result = async.ErrorCode();
    if (result < 0) {
      //TODO: OnRequestError
      //TODO: Remove request
      co_return;
    }

    auto response = async.GetResults();
    if (response) {//TODO: check nullptr?
      //TODO: OnResponseReceived
      if (self->m_onResponse) {
        Headers headers;
        for (auto header : response.Headers()) {
          headers.emplace(winrt::to_string(header.Key()), winrt::to_string(header.Value()));
        }
        string url = winrt::to_string(response.RequestMessage().RequestUri().AbsoluteUri());
        self->m_onResponse(0 /*requestId*/, {static_cast<int32_t>(response.StatusCode()), std::move(headers), std::move(url)});
      }
    }

    //TODO: Incremental updates?
    if (response && response.Content()) {//TODO: check nullptr?
      auto inputStream = co_await response.Content().ReadAsInputStreamAsync();
      auto reader = DataReader{inputStream};

      if (textResponse) {
        reader.UnicodeEncoding(UnicodeEncoding::Utf8);
      }

      // Only support response sizes up to 10MB.
      //TODO: WHY????
      co_await reader.LoadAsync(10 * 1024 * 1024);
      auto length = reader.UnconsumedBufferLength();

      if (textResponse) {
        std::vector<uint8_t> data(length);
        reader.ReadBytes(data);
        string responseData =
            string(Common::Utilities::CheckedReinterpretCast<char *>(data.data()), data.size());

        if (self->m_onData) {
          self->m_onData(0 /*requestId*/, std::move(responseData));
        }
      } else {
        auto buffer = reader.ReadBuffer(length);
        auto data = CryptographicBuffer::EncodeToBase64String(buffer);
        auto responseData = winrt::to_string(data);//TODO: string view???

        if (self->m_onData) {
          self->m_onData(0 /*requestId*/, std::move(responseData));
        }
      }

      //TODO: self->OnRequestSuccess OR, do request success inside OnData.
    } else {
      if (self->m_onError) {
        self->m_onError(0 /*requestId*/, "Unhandled exception during request" /*, isTimeout*/);
      }
    }
  } catch (...) {
    // TODO: self->OnRequestError
    //TODO: Lose generic catch
  }

  //TODO: Remove request
  co_return;//TODO: keep?
}

#pragma endregion WinRTHttpResource

#pragma region IHttpResource

/*static*/ shared_ptr<IHttpResource> IHttpResource::Make() noexcept {
  return std::make_shared<WinRTHttpResource>();
}

#pragma endregion IHttpResource

}// namespace
