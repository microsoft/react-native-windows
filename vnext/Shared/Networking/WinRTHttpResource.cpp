// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "WinRTHttpResource.h"

#include <CppRuntimeOptions.h>
#include <ReactPropertyBag.h>
#include <Utils/CppWinrtLessExceptions.h>
#include <Utils/WinRTConversions.h>
#include <utilities.h>
#include "OriginPolicyHttpFilter.h"

// Boost Libraries
#include <boost/algorithm/string.hpp>

// Windows API
#include <winrt/Windows.Security.Cryptography.h>
#include <winrt/Windows.Storage.Streams.h>
#include <winrt/Windows.Web.Http.Headers.h>

using folly::dynamic;

using std::function;
using std::scoped_lock;
using std::shared_ptr;
using std::string;
using std::weak_ptr;

using winrt::fire_and_forget;
using winrt::hresult_error;
using winrt::to_hstring;
using winrt::to_string;
using winrt::Windows::Foundation::IInspectable;
using winrt::Windows::Foundation::Uri;
using winrt::Windows::Security::Cryptography::CryptographicBuffer;
using winrt::Windows::Storage::StorageFile;
using winrt::Windows::Storage::Streams::DataReader;
using winrt::Windows::Storage::Streams::UnicodeEncoding;
using winrt::Windows::Web::Http::HttpBufferContent;
using winrt::Windows::Web::Http::HttpMethod;
using winrt::Windows::Web::Http::HttpRequestMessage;
using winrt::Windows::Web::Http::HttpStreamContent;
using winrt::Windows::Web::Http::HttpStringContent;
using winrt::Windows::Web::Http::IHttpClient;
using winrt::Windows::Web::Http::IHttpContent;
using winrt::Windows::Web::Http::Headers::HttpMediaTypeHeaderValue;

namespace Microsoft::React::Networking {

#pragma region WinRTHttpResource

WinRTHttpResource::WinRTHttpResource(IHttpClient &&client) noexcept : m_client{std::move(client)} {}

WinRTHttpResource::WinRTHttpResource() noexcept : WinRTHttpResource(winrt::Windows::Web::Http::HttpClient{}) {}

#pragma region IHttpResource

void WinRTHttpResource::SendRequest(
    string &&method,
    string &&url,
    int64_t requestId,
    Headers &&headers,
    dynamic &&data,
    string &&responseType,
    bool useIncrementalUpdates,
    int64_t timeout,
    bool withCredentials,
    std::function<void(int64_t)> &&callback) noexcept /*override*/ {
  // Enforce supported args
  assert(responseType == "text" || responseType == "base64" | responseType == "blob");

  if (callback) {
    callback(requestId);
  }

  try {
    HttpMethod httpMethod{to_hstring(std::move(method))};
    Uri uri{to_hstring(std::move(url))};
    HttpRequestMessage request{httpMethod, uri};

    auto args = winrt::make<RequestArgs>();
    auto concreteArgs = args.as<RequestArgs>();
    concreteArgs->RequestId = requestId;
    concreteArgs->Headers = std::move(headers);
    concreteArgs->Data = std::move(data);
    concreteArgs->IncrementalUpdates = useIncrementalUpdates;
    concreteArgs->WithCredentials = withCredentials;
    concreteArgs->ResponseType = std::move(responseType);
    concreteArgs->Timeout = timeout;

    PerformSendRequest(std::move(request), args);
  } catch (std::exception const &e) {
    if (m_onError) {
      m_onError(requestId, e.what());
    }
  } catch (hresult_error const &e) {
    if (m_onError) {
      m_onError(requestId, Utilities::HResultToString(e));
    }
  } catch (...) {
    m_onError(requestId, "Unidentified error sending HTTP request");
  }
}

void WinRTHttpResource::AbortRequest(int64_t requestId) noexcept /*override*/ {
  ResponseOperation request{nullptr};

  {
    scoped_lock lock{m_mutex};
    auto iter = m_responses.find(requestId);
    if (iter == std::end(m_responses)) {
      return;
    }
    request = iter->second;
  }

  try {
    request.Cancel();
  } catch (hresult_error const &e) {
    m_onError(requestId, Utilities::HResultToString(e));
  }
}

void WinRTHttpResource::ClearCookies() noexcept /*override*/ {
  assert(false);
  // NOT IMPLEMENTED
}

void WinRTHttpResource::SetOnRequestSuccess(function<void(int64_t requestId)> &&handler) noexcept /*override*/ {
  m_onRequestSuccess = std::move(handler);
}

void WinRTHttpResource::SetOnResponse(function<void(int64_t requestId, Response &&response)> &&handler) noexcept
/*override*/ {
  m_onResponse = std::move(handler);
}

void WinRTHttpResource::SetOnData(function<void(int64_t requestId, string &&responseData)> &&handler) noexcept
/*override*/ {
  m_onData = std::move(handler);
}

void WinRTHttpResource::SetOnBlobData(function<void(int64_t requestId, dynamic &&responseData)> &&handler) noexcept
/*override*/
{
  m_onBlobData = std::move(handler);
}

void WinRTHttpResource::SetOnError(function<void(int64_t requestId, string &&errorMessage)> &&handler) noexcept
/*override*/ {
  m_onError = std::move(handler);
}

#pragma endregion IHttpResource

void WinRTHttpResource::TrackResponse(int64_t requestId, ResponseOperation response) noexcept {
  scoped_lock lock{m_mutex};
  m_responses[requestId] = response;
}

void WinRTHttpResource::UntrackResponse(int64_t requestId) noexcept {
  scoped_lock lock{m_mutex};
  m_responses.erase(requestId);
}

fire_and_forget WinRTHttpResource::PerformSendRequest(HttpRequestMessage &&request, IInspectable const &args) noexcept {
  // Keep references after coroutine suspension.
  auto self = shared_from_this();
  auto coRequest = std::move(request);
  auto coArgs = args;
  auto coReqArgs = coArgs.as<RequestArgs>();

  // Ensure background thread
  co_await winrt::resume_background();

  // If URI handler is available, it takes over request processing.
  if (auto uriHandler = self->m_uriHandler.lock()) {
    auto uri = winrt::to_string(coRequest.RequestUri().ToString());
    try {
      if (uriHandler->Supports(uri, coReqArgs->ResponseType)) {
        auto blob = uriHandler->Fetch(uri);
        if (self->m_onBlobData && self->m_onRequestSuccess)
          co_return self->m_onBlobData(coReqArgs->RequestId, std::move(blob));
      }
    } catch (const hresult_error &e) {
      if (self->m_onError)
        co_return self->m_onError(coReqArgs->RequestId, Utilities::HResultToString(e));
    } catch (const std::exception &e) {
      if (self->m_onError)
        co_return self->m_onError(coReqArgs->RequestId, e.what());
    }
  }

  HttpMediaTypeHeaderValue contentType{nullptr};
  string contentEncoding;
  string contentLength;

  // Headers are generally case-insensitive
  // https://www.ietf.org/rfc/rfc2616.txt section 4.2
  for (auto &header : coReqArgs->Headers) {
    if (boost::iequals(header.first.c_str(), "Content-Type")) {
      bool success = HttpMediaTypeHeaderValue::TryParse(to_hstring(header.second), contentType);
      if (!success && m_onError) {
        co_return m_onError(coReqArgs->RequestId, "Failed to parse Content-Type");
      }
    } else if (boost::iequals(header.first.c_str(), "Content-Encoding")) {
      contentEncoding = header.second;
    } else if (boost::iequals(header.first.c_str(), "Content-Length")) {
      contentLength = header.second;
    } else if (boost::iequals(header.first.c_str(), "Authorization")) {
      bool success =
          coRequest.Headers().TryAppendWithoutValidation(to_hstring(header.first), to_hstring(header.second));
      if (!success && m_onError) {
        co_return m_onError(coReqArgs->RequestId, "Failed to append Authorization");
      }
    } else {
      try {
        coRequest.Headers().Append(to_hstring(header.first), to_hstring(header.second));
      } catch (hresult_error const &e) {
        if (self->m_onError) {
          co_return self->m_onError(coReqArgs->RequestId, Utilities::HResultToString(e));
        }
      }
    }
  }

  IHttpContent content{nullptr};
  auto &data = coReqArgs->Data;
  if (!data.isNull()) {
    if (!data["string"].empty()) {
      content = HttpStringContent{to_hstring(data["string"].asString())};
    } else if (!data["base64"].empty()) {
      auto buffer = CryptographicBuffer::DecodeFromBase64String(to_hstring(data["base64"].asString()));
      content = HttpBufferContent{std::move(buffer)};
    } else if (!data["uri"].empty()) {
      auto file = co_await StorageFile::GetFileFromApplicationUriAsync(Uri{to_hstring(data["uri"].asString())});
      auto stream = co_await file.OpenReadAsync();
      content = HttpStreamContent{std::move(stream)};
    } else if (!data["form"].empty()) {
      // #9535 - HTTP form data support
      // winrt::Windows::Web::Http::HttpMultipartFormDataContent()
    } else {
      // Assume empty request body.
      // content = HttpStringContent{L""};
    }
  }

  if (content != nullptr) {
    // Attach content headers
    if (contentType) {
      content.Headers().ContentType(contentType);
    }
    if (!contentEncoding.empty()) {
      if (!content.Headers().ContentEncoding().TryParseAdd(to_hstring(contentEncoding))) {
        if (m_onError) {
          m_onError(coReqArgs->RequestId, "Failed to parse Content-Encoding");
        }
        co_return;
      }
    }
    if (!contentLength.empty()) {
      const auto contentLengthHeader = _atoi64(contentLength.c_str());
      content.Headers().ContentLength(contentLengthHeader);
    }

    coRequest.Content(content);
  }

  try {
    coRequest.Properties().Insert(L"RequestArgs", coArgs);
    auto sendRequestOp = self->m_client.SendRequestAsync(coRequest);
    self->TrackResponse(coReqArgs->RequestId, sendRequestOp);

    co_await lessthrow_await_adapter<ResponseOperation>{sendRequestOp};
    auto result = sendRequestOp.ErrorCode();
    if (result < 0) {
      if (self->m_onError) {
        self->m_onError(coReqArgs->RequestId, Utilities::HResultToString(std::move(result)));
      }
      co_return self->UntrackResponse(coReqArgs->RequestId);
    }

    auto response = sendRequestOp.GetResults();
    if (response) {
      if (self->m_onResponse) {
        string url = to_string(response.RequestMessage().RequestUri().AbsoluteUri());

        // Gather headers for both the response content and the response itself
        // See Invoke-WebRequest PowerShell cmdlet or Chromium response handling
        Headers responseHeaders;
        for (auto header : response.Headers()) {
          responseHeaders.emplace(to_string(header.Key()), to_string(header.Value()));
        }
        for (auto header : response.Content().Headers()) {
          responseHeaders.emplace(to_string(header.Key()), to_string(header.Value()));
        }

        self->m_onResponse(
            coReqArgs->RequestId,
            {static_cast<int32_t>(response.StatusCode()), std::move(url), std::move(responseHeaders)});
      }
    }

    // #9534 - Support HTTP incremental updates
    if (response && response.Content()) {
      auto inputStream = co_await response.Content().ReadAsInputStreamAsync();
      auto reader = DataReader{inputStream};

      auto isText = coReqArgs->ResponseType == "text";
      if (isText) {
        reader.UnicodeEncoding(UnicodeEncoding::Utf8);
      }

      // #9510 - 10mb limit on fetch
      co_await reader.LoadAsync(10 * 1024 * 1024);
      auto length = reader.UnconsumedBufferLength();

      if (isText) {
        std::vector<uint8_t> data(length);
        reader.ReadBytes(data);
        string responseData = string(Common::Utilities::CheckedReinterpretCast<char *>(data.data()), data.size());

        if (self->m_onData) {
          self->m_onData(coReqArgs->RequestId, std::move(responseData));
        }
      } else {
        auto buffer = reader.ReadBuffer(length);
        auto data = CryptographicBuffer::EncodeToBase64String(buffer);
        auto responseData = to_string(std::wstring_view(data));

        if (self->m_onData) {
          self->m_onData(coReqArgs->RequestId, std::move(responseData));
        }
      }
    } else {
      if (self->m_onError) {
        self->m_onError(coReqArgs->RequestId, response == nullptr ? "request failed" : "No response content");
      }
    }
  } catch (std::exception const &e) {
    if (self->m_onError) {
      self->m_onError(coReqArgs->RequestId, e.what());
    }
  } catch (hresult_error const &e) {
    if (self->m_onError) {
      self->m_onError(coReqArgs->RequestId, Utilities::HResultToString(e));
    }
  } catch (...) {
    if (self->m_onError) {
      self->m_onError(coReqArgs->RequestId, "Unhandled exception during request");
    }
  }

  self->UntrackResponse(coReqArgs->RequestId);
} // PerformSendRequest

#pragma region IHttpModuleProxy

void WinRTHttpResource::AddUriHandler(shared_ptr<IUriHandler> uriHandler) noexcept /*override*/
{
  m_uriHandler = weak_ptr<IUriHandler>(uriHandler);
}

void WinRTHttpResource::AddRequestBodyHandler(shared_ptr<IRequestBodyHandler> requestBodyHandler) noexcept /*override*/
{
  m_requestBodyHandler = weak_ptr<IRequestBodyHandler>(requestBodyHandler);
}

void WinRTHttpResource::AddResponseHandler(shared_ptr<IResponseHandler> responseHandler) noexcept /*override*/
{
  m_responseHandler = weak_ptr<IResponseHandler>(responseHandler);
}

#pragma endregion IHttpModuleProxy

#pragma endregion WinRTHttpResource

#pragma region IHttpResource

/*static*/ shared_ptr<IHttpResource> IHttpResource::Make(
    winrt::Windows::Foundation::IInspectable &inspectableProperties) noexcept {
  using namespace winrt::Microsoft::ReactNative;
  using winrt::Windows::Web::Http::HttpClient;

  shared_ptr<WinRTHttpResource> result;

  if (static_cast<OriginPolicy>(GetRuntimeOptionInt("Http.OriginPolicy")) == OriginPolicy::None) {
    result = std::make_shared<WinRTHttpResource>();
  } else {
    auto globalOrigin = GetRuntimeOptionString("Http.GlobalOrigin");
    OriginPolicyHttpFilter::SetStaticOrigin(std::move(globalOrigin));
    auto opFilter = winrt::make<OriginPolicyHttpFilter>();
    auto client = HttpClient{opFilter};

    result = std::make_shared<WinRTHttpResource>(std::move(client));
  }

  // Register resource as HTTP module proxy.
  if (inspectableProperties) {
    auto propId = ReactPropertyId<ReactNonAbiValue<weak_ptr<IHttpModuleProxy>>>{L"HttpModule.Proxy"};
    auto propBag = ReactPropertyBag{inspectableProperties.try_as<IReactPropertyBag>()};
    auto moduleProxy = weak_ptr<IHttpModuleProxy>{result};
    propBag.Set(propId, std::move(moduleProxy));
  }

  return result;
}

/*static*/ shared_ptr<IHttpResource> IHttpResource::Make() noexcept {
  auto inspectableProperties = IInspectable{nullptr};
  return Make(inspectableProperties);
}

#pragma endregion IHttpResource

} // namespace Microsoft::React::Networking
