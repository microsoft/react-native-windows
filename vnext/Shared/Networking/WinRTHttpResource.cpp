// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "WinRTHttpResource.h"

#include <RuntimeOptions.h>
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

using std::function;
using std::scoped_lock;
using std::shared_ptr;
using std::string;

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

// TODO: Check for multi-thread issues if there are multiple instances.
/*static*/ int64_t WinRTHttpResource::s_lastRequestId = 0;

WinRTHttpResource::WinRTHttpResource(IHttpClient &&client) noexcept : m_client{std::move(client)} {}

WinRTHttpResource::WinRTHttpResource() noexcept : WinRTHttpResource(winrt::Windows::Web::Http::HttpClient()) {}

#pragma region IHttpResource

void WinRTHttpResource::SendRequest(
    string &&method,
    string &&url,
    Headers &&headers,
    BodyData &&bodyData,
    string &&responseType,
    bool useIncrementalUpdates,
    int64_t timeout,
    bool withCredentials,
    std::function<void(int64_t)> &&callback) noexcept /*override*/ {
  auto requestId = ++s_lastRequestId;

  // Enforce supported args
  assert(responseType == "text" || responseType == "base64");

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
    concreteArgs->Body = std::move(bodyData);
    concreteArgs->IncrementalUpdates = useIncrementalUpdates;
    concreteArgs->WithCredentials = withCredentials;
    concreteArgs->IsText = responseType == "text";
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

void WinRTHttpResource::SetOnRequest(function<void(int64_t requestId)> &&handler) noexcept /*override*/ {
  m_onRequest = std::move(handler);
}

void WinRTHttpResource::SetOnResponse(function<void(int64_t requestId, Response &&response)> &&handler) noexcept
/*override*/ {
  m_onResponse = std::move(handler);
}

void WinRTHttpResource::SetOnData(function<void(int64_t requestId, string &&responseData)> &&handler) noexcept
/*override*/ {
  m_onData = std::move(handler);
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
  if (BodyData::Type::String == coReqArgs->Body.Type) {
    content = HttpStringContent{to_hstring(coReqArgs->Body.Data)};
  } else if (BodyData::Type::Base64 == coReqArgs->Body.Type) {
    auto buffer = CryptographicBuffer::DecodeFromBase64String(to_hstring(coReqArgs->Body.Data));
    content = HttpBufferContent{buffer};
  } else if (BodyData::Type::Uri == coReqArgs->Body.Type) {
    auto file = co_await StorageFile::GetFileFromApplicationUriAsync(Uri{to_hstring(coReqArgs->Body.Data)});
    auto stream = co_await file.OpenReadAsync();
    content = HttpStreamContent{stream};
  } else if (BodyData::Type::Form == coReqArgs->Body.Type) {
    // #9535 - HTTP form data support
  } else {
    // BodyData::Type::Empty
    // TODO: Error => unsupported??
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

      if (coReqArgs->IsText) {
        reader.UnicodeEncoding(UnicodeEncoding::Utf8);
      }

      // #9510 - 10mb limit on fetch
      co_await reader.LoadAsync(10 * 1024 * 1024);
      auto length = reader.UnconsumedBufferLength();

      if (coReqArgs->IsText) {
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
}

#pragma endregion WinRTHttpResource

#pragma region IHttpResource

/*static*/ shared_ptr<IHttpResource> IHttpResource::Make() noexcept {
  if (static_cast<OriginPolicy>(Microsoft_React_GetRuntimeOptionInt("Http.OriginPolicy")) == OriginPolicy::None) {
    return std::make_shared<WinRTHttpResource>();
  } else {
    auto globalOrigin = Microsoft_React_GetRuntimeOptionString("Http.GlobalOrigin");
    OriginPolicyHttpFilter::SetStaticOrigin(globalOrigin);
    delete globalOrigin;
    auto opFilter = winrt::make<OriginPolicyHttpFilter>();
    auto client = winrt::Windows::Web::Http::HttpClient{opFilter};

    return std::make_shared<WinRTHttpResource>(std::move(client));
  }
}

#pragma endregion IHttpResource

} // namespace Microsoft::React::Networking
