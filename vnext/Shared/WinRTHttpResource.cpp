// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "WinRTHttpResource.h"

#include <Utils/CppWinrtLessExceptions.h>
#include <Utils/WinRTConversions.h>
#include <utilities.h>
#include "RuntimeOptions.h"

// Windows API
#include <winrt/Windows.Security.Cryptography.h>
#include <winrt/Windows.Storage.Streams.h>
#include <winrt/Windows.Web.Http.Filters.h>
#include <winrt/Windows.Web.Http.Headers.h>

// Standard Library
#include <set>

using std::function;
using std::scoped_lock;
using std::shared_ptr;
using std::string;

using winrt::fire_and_forget;
using winrt::hresult_error;
using winrt::to_hstring;
using winrt::to_string;
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
using winrt::Windows::Web::Http::Filters::IHttpFilter;
using winrt::Windows::Web::Http::Headers::HttpMediaTypeHeaderValue;

namespace Microsoft::React {

#if LOOOOOL

using std::weak_ptr;
using winrt::Windows::Foundation::IAsyncAction;
struct IHttpRequestFilter {
  virtual IAsyncAction ProcessRequest(
      int64_t requestId,
      HttpRequestMessage &request,
      IHttpResource::Headers &headers,
      IHttpResource::BodyData &bodyData,
      bool textResponse) noexcept = 0;
};

class PrototypeRequestFilter : public IHttpRequestFilter {
  // TODO: Remove. Redundant.
  typedef winrt::Windows::Foundation::IAsyncOperationWithProgress<
      winrt::Windows::Web::Http::HttpResponseMessage,
      winrt::Windows::Web::Http::HttpProgress>
      ResponseType;

  winrt::weak_ref<IHttpClient> m_weakClient;

 public:
  PrototypeRequestFilter(winrt::weak_ref<IHttpClient> wkClient) noexcept : m_weakClient{wkClient} {}

#pragma region IHttpRequestFilter

  IAsyncAction ProcessRequest(
      int64_t requestId,
      HttpRequestMessage &request,
      IHttpResource::Headers &headers,
      IHttpResource::BodyData &bodyData,
      bool textResponse) noexcept override {
    if (headers["Preflight"] != "Requested")
      co_return;

    auto client = m_weakClient.get();
    if (!client)
      co_return;

    string exceptionMessage;
    try {
      auto preflightRequest = HttpRequestMessage();
      preflightRequest.RequestUri(request.RequestUri());
      preflightRequest.Method(HttpMethod::Options());

      auto sendPreflightOp = client.SendRequestAsync(preflightRequest);
      co_await lessthrow_await_adapter<ResponseType>{sendPreflightOp};
      auto result = sendPreflightOp.ErrorCode();
      if (result < 0) {
        winrt::throw_hresult(result);
      }
      auto response = sendPreflightOp.GetResults();

      auto preflightStatus = response.Headers().Lookup(L"Preflight");
      if (preflightStatus != L"Approved")
        throw hresult_error{E_FAIL, L"Origin policy non-compliance"}; // TODO: Better HRESULT?

      for (auto header : response.Headers()) {
        headers.emplace(to_string(header.Key()), to_string(header.Value()));
      }

      co_return;
    } catch (const std::exception &e) {
      exceptionMessage = e.what();
    } catch (...) {
      exceptionMessage = "Unspecified error processing Origin Policy request";
    }

    throw hresult_error{E_FAIL, to_hstring(exceptionMessage)}; // TODO: Better HRESULT?
  }

#pragma endregion IHttpRequestFilter
};
#endif // LOOOOOL

class PrototypeHttpFilter : public winrt::implements<PrototypeHttpFilter, IHttpFilter> { // public IHttpFilter {
  // TODO: Remove. Redundant.
  typedef winrt::Windows::Foundation::IAsyncOperationWithProgress<
      winrt::Windows::Web::Http::HttpResponseMessage,
      winrt::Windows::Web::Http::HttpProgress>
      ResponseType;

  // https://fetch.spec.whatwg.org/#forbidden-method
  static inline std::set<const char *> s_forbiddenMethods{"CONNECT", "TRACE", "TRACK"};

  static inline std::set<const char *> s_simpleCorsRequestHeaderNames{
      "Accept",
      "Accept-Language",
      "Content-Language",
      "Content-Type",
      "DPR",
      "Downlink",
      "Save-Data",
      "Viewport-Width",
      "Width"};

  static inline std::set<const char *> s_simpleCorsResponseHeaderNames{
      "Cache-Control",
      "Content-Language",
      "Content-Type",
      "Expires",
      "Last-Modified",
      "Pragma"};

  static inline std::set<const char *> s_simpleCorsContentTypeValues{
      "application/x-www-form-urlencoded",
      "multipart/form-data",
      "text/plain"};

  // https://fetch.spec.whatwg.org/#forbidden-header-name
  // Chromium still bans "User-Agent" due to https://crbug.com/571722 //TODO: Remove?
  static inline std::set<const char *> s_corsForbiddenRequestHeaderNames{
      "Accept-Charset",
      "Accept-Encoding",
      "Access-Control-Request-Headers",
      "Access-Control-Request-Method",
      "Connection",
      "Content-Length",
      "Cookie",
      "Cookie2",
      "Date",
      "DNT",
      "Expect",
      "Host",
      "Keep-Alive",
      "Origin",
      "Referer",
      "TE",
      "Trailer",
      "Transfer-Encoding",
      "Upgrade",
      "Via"};

  static inline std::set<const char *> s_corsForbiddenRequestHeaderNamePrefixes{"Proxy-", "Sec-"};

  IHttpFilter m_innerFilter;
  OriginPolicy m_originPolicy;
  // Uri m_origin;

 public:
  PrototypeHttpFilter(OriginPolicy originPolicy, IHttpFilter &&innerFilter)
      : m_originPolicy{m_originPolicy}, m_innerFilter{std::move(innerFilter)} {}

  bool ValidateRequest(Uri &url) {
    // case CORS:
    // https://developer.mozilla.org/en-US/docs/Web/HTTP/CORS#simple_requests
    // Refer to CorsURLLoaderFactory::IsValidRequest in chrome\src\services\network\cors\cors_url_loader_factory.cc.
    // Forbidden headers should be blocked regardless of origins.
    // It is correct to check for forbidden header first.
    // It is verified in Edge when I try to set the "host" header to a XHR. The browser rejected it as unsafe.
    // In fact, this check probably should apply to all networking security policy.
    // https://fetch.spec.whatwg.org/#forbidden-header-name
    /// Is mixed content? (incoming URL vs origin URL)
    //// => false
    /// AreRequestHeadersSafe(headers)
    //// => false
    /// IsForbiddenMethod(method)
    //// => false
    /// IsSameOrigin(m_securitySettings.origin, destinationOrigin)
    //// => SOP; Reset the validated policy to SOP policy and no need to involve simple CORS which is for cross origins.
    /// IsSimpleCors(meth, headers)
    //// => SimpleCORS; Cross origins but meet Simple CORS conditions. No need to go through Preflight.
    /// else => CORS

    return true;
  }

  // Mso::React::HttpResource::SendPreflight
  ResponseType SendPreflightAsync(HttpRequestMessage const &request) const { // TODO: const& ??

    co_return {};
  }

  ResponseType SendRequestAsync(HttpRequestMessage const &request) const {
    // Ensure absolute URL

    // If fetch is in CORS mode, ValidateSecurityOnRequest() determines if it is a simple request by inspecting origin,
    // header, and method
    /// ValidateRequest()

    // const bool isSameOrigin =
    /// (validatedSecurityPolicy != NetworkingSecurityPolicy::SimpleCORS) &&
    /// (validatedSecurityPolicy != NetworkingSecurityPolicy::CORS);
    // !isSameOrigin => CrossOrigin => RemoveUserNamePasswordFromUrl || FAIL

    // If CORS && !inCache => Preflight! { cache() }
    //

    auto coRequest = request;
    // Prototype
    try {
      auto preflightRequest = HttpRequestMessage();
      preflightRequest.RequestUri(request.RequestUri());
      preflightRequest.Method(HttpMethod::Options());

      auto response = co_await m_innerFilter.SendRequestAsync(preflightRequest);
      auto preflightStatus = response.Headers().Lookup(L"Preflight");
      if (L"Approved" != preflightStatus)
        throw hresult_error{E_FAIL, L"Origin policy non-compliance"}; // TODO: Better HRESULT?

      for (auto header : response.Headers()) {
        coRequest.Headers().Insert(header.Key(), header.Value());
      }
    } catch (hresult_error const &e) {
      throw e;
    } catch (const std::exception &e) {
      throw hresult_error{E_FAIL, to_hstring(e.what())};
    } catch (...) {
      throw hresult_error{E_FAIL, L"Unspecified error processing Origin Policy request"};
    }

    co_return {co_await m_innerFilter.SendRequestAsync(coRequest)};
  }
};

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

    PerformSendRequest(requestId, std::move(request), std::move(headers), std::move(bodyData), responseType == "text");
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
  ResponseType request{nullptr};

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

void WinRTHttpResource::SetOnData(function<void(int64_t requestId, std::string &&responseData)> &&handler) noexcept
/*override*/ {
  m_onData = std::move(handler);
}

void WinRTHttpResource::SetOnError(function<void(int64_t requestId, string &&errorMessage)> &&handler) noexcept
/*override*/ {
  m_onError = std::move(handler);
}

#pragma endregion IHttpResource

void WinRTHttpResource::TrackResponse(int64_t requestId, ResponseType response) noexcept {
  scoped_lock lock{m_mutex};
  m_responses[requestId] = response;
}

void WinRTHttpResource::UntrackResponse(int64_t requestId) noexcept {
  scoped_lock lock{m_mutex};
  m_responses.erase(requestId);
}

fire_and_forget WinRTHttpResource::PerformSendRequest(
    int64_t requestId,
    HttpRequestMessage &&request,
    Headers &&headers,
    BodyData &&bodyData,
    bool textResponse) noexcept {
  // Keep references after coroutine suspension.
  auto self = shared_from_this();
  auto coRequest = std::move(request);
  auto coHeaders = std::move(headers);
  auto coBodyData = std::move(bodyData);

  // Ensure background thread
  co_await winrt::resume_background();

  HttpMediaTypeHeaderValue contentType{nullptr};
  string contentEncoding;
  string contentLength;

  // Headers are generally case-insensitive
  // https://www.ietf.org/rfc/rfc2616.txt section 4.2
  for (auto &header : coHeaders) {
    if (_stricmp(header.first.c_str(), "content-type") == 0) {
      bool success = HttpMediaTypeHeaderValue::TryParse(to_hstring(header.second), contentType);
      if (!success && m_onError) {
        co_return m_onError(requestId, "Failed to parse Content-Type");
      }
    } else if (_stricmp(header.first.c_str(), "content-encoding") == 0) {
      contentEncoding = header.second;
    } else if (_stricmp(header.first.c_str(), "content-length") == 0) {
      contentLength = header.second;
    } else if (_stricmp(header.first.c_str(), "authorization") == 0) {
      bool success =
          coRequest.Headers().TryAppendWithoutValidation(to_hstring(header.first), to_hstring(header.second));
      if (!success && m_onError) {
        co_return m_onError(requestId, "Failed to append Authorization");
      }
    } else {
      coRequest.Headers().Append(to_hstring(header.first), to_hstring(header.second));
    }
  }

  IHttpContent content{nullptr};
  if (BodyData::Type::String == coBodyData.Type) {
    content = HttpStringContent{to_hstring(coBodyData.Data)};
  } else if (BodyData::Type::Base64 == coBodyData.Type) {
    auto buffer = CryptographicBuffer::DecodeFromBase64String(to_hstring(coBodyData.Data));
    content = HttpBufferContent{buffer};
  } else if (BodyData::Type::Uri == coBodyData.Type) {
    auto file = co_await StorageFile::GetFileFromApplicationUriAsync(Uri{to_hstring(coBodyData.Data)});
    auto stream = co_await file.OpenReadAsync();
    content = HttpStreamContent{stream};
  } else if (BodyData::Type::Form == coBodyData.Type) {
    // TODO: Add support
  } else {
    // BodyData::Type::Empty
    // TODO: Error 'cause unsupported??
  }

  if (content != nullptr) {
    // Attach content headers
    if (contentType) {
      content.Headers().ContentType(contentType);
    }
    if (!contentEncoding.empty()) {
      if (!content.Headers().ContentEncoding().TryParseAdd(to_hstring(contentEncoding))) {
        if (m_onError) {
          m_onError(requestId, "Failed to parse Content-Encoding");
        }
        co_return;
      }
    }
    if (!contentLength.empty()) {
      const auto contentLengthHeader = _atoi64(contentLength.c_str()); // TODO: Alternatives to _atoi64?
      content.Headers().ContentLength(contentLengthHeader);
    }

    coRequest.Content(content);
  }

  try {
    // TODO: Remove
    ////FILTER!
    // auto filter = PrototypeRequestFilter(winrt::make_weak<IHttpClient>(self->m_client));
    // auto preflightOp = filter.ProcessRequest(requestId, coRequest, coHeaders, coBodyData, textResponse);
    // co_await lessthrow_await_adapter<IAsyncAction>{preflightOp};
    // auto preResult = preflightOp.ErrorCode();
    // if (preResult < 0) {
    //  if (self->m_onError) {
    //    co_return self->m_onError(requestId, Utilities::HResultToString(std::move(preResult)));
    //  }
    //}
    //// FILTER

    auto sendRequestOp = self->m_client.SendRequestAsync(coRequest);
    self->TrackResponse(requestId, sendRequestOp);

    co_await lessthrow_await_adapter<ResponseType>{sendRequestOp};
    auto result = sendRequestOp.ErrorCode();
    if (result < 0) {
      if (self->m_onError) {
        self->m_onError(requestId, Utilities::HResultToString(std::move(result)));
      }
      co_return self->UntrackResponse(requestId);
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
            requestId, {static_cast<int32_t>(response.StatusCode()), std::move(url), std::move(responseHeaders)});
      }
    }

    // TODO: Incremental updates?
    if (response && response.Content()) {
      auto inputStream = co_await response.Content().ReadAsInputStreamAsync();
      auto reader = DataReader{inputStream};

      if (textResponse) {
        reader.UnicodeEncoding(UnicodeEncoding::Utf8);
      }

      // Only support response sizes up to 10MB.
      // TODO: WHY????
      co_await reader.LoadAsync(10 * 1024 * 1024);
      auto length = reader.UnconsumedBufferLength();

      if (textResponse) {
        std::vector<uint8_t> data(length);
        reader.ReadBytes(data);
        string responseData = string(Common::Utilities::CheckedReinterpretCast<char *>(data.data()), data.size());

        if (self->m_onData) {
          self->m_onData(requestId, std::move(responseData));
        }
      } else {
        auto buffer = reader.ReadBuffer(length);
        auto data = CryptographicBuffer::EncodeToBase64String(buffer);
        auto responseData = to_string(std::wstring_view(data));

        if (self->m_onData) {
          self->m_onData(requestId, std::move(responseData));
        }
      }
    } else {
      if (self->m_onError) {
        self->m_onError(requestId, response == nullptr ? "request failed" : "No response content");
      }
    }
  } catch (std::exception const &e) {
    if (self->m_onError) {
      self->m_onError(requestId, e.what());
    }
  } catch (hresult_error const &e) {
    if (self->m_onError) {
      self->m_onError(requestId, Utilities::HResultToString(e));
    }
  } catch (...) {
    if (self->m_onError) {
      self->m_onError(requestId, "Unhandled exception during request");
    }
  }

  self->UntrackResponse(requestId);
}

#pragma endregion WinRTHttpResource

#pragma region IHttpResource

/*static*/ shared_ptr<IHttpResource> IHttpResource::Make() noexcept {
  auto originPolicy = static_cast<OriginPolicy>(GetRuntimeOptionInt("Http.OriginPolicy"));
  if (originPolicy == OriginPolicy::None) {
    return std::make_shared<WinRTHttpResource>();
  } else {
    auto baseFilter = winrt::Windows::Web::Http::Filters::HttpBaseProtocolFilter{};
    auto opFilter = winrt::make<PrototypeHttpFilter>(originPolicy, std::move(baseFilter));
    auto client = winrt::Windows::Web::Http::HttpClient{opFilter};

    return std::make_shared<WinRTHttpResource>(std::move(client));
  }
}

#pragma endregion IHttpResource

} // namespace Microsoft::React
