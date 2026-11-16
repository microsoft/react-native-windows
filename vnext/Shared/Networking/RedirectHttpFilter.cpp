// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#undef WINRT_LEAN_AND_MEAN

#include "RedirectHttpFilter.h"

// React Native Windows
#include <CppRuntimeOptions.h>
#include "WinRTTypes.h"

// Windows API
#include <winapifamily.h>
#include <winrt/Windows.Web.Http.Headers.h>
#if WINAPI_FAMILY_PARTITION(WINAPI_PARTITION_DESKTOP)
#include <WinInet.h>
#else
#define INTERNET_ERROR_BASE 12000
#define ERROR_HTTP_REDIRECT_FAILED (INTERNET_ERROR_BASE + 156)
#endif

namespace {
constexpr size_t DefaultMaxRedirects = 20;

bool IsSameOrigin(winrt::Windows::Foundation::Uri const &u1, winrt::Windows::Foundation::Uri const &u2) noexcept {
  return u1 && u2 && _wcsicmp(u1.SchemeName().c_str(), u2.SchemeName().c_str()) == 0 &&
      _wcsicmp(u1.Host().c_str(), u2.Host().c_str()) == 0 && u1.Port() == u2.Port();
}

bool IsMethod(winrt::Windows::Web::Http::HttpMethod const &method, wchar_t const *name) noexcept {
  return method && _wcsicmp(method.Method().c_str(), name) == 0;
}

void RemoveHeader(winrt::Windows::Web::Http::HttpRequestMessage const &request, wchar_t const *name) {
  auto headers = request.Headers();
  if (headers.HasKey(name)) {
    headers.Remove(name);
  }
}
} // namespace

using winrt::Windows::Foundation::Uri;
using winrt::Windows::Foundation::Collections::IVector;
using winrt::Windows::Security::Credentials::PasswordCredential;
using winrt::Windows::Security::Cryptography::Certificates::Certificate;
using winrt::Windows::Security::Cryptography::Certificates::ChainValidationResult;
using winrt::Windows::Web::Http::HttpMethod;
using winrt::Windows::Web::Http::HttpRequestMessage;
using winrt::Windows::Web::Http::HttpResponseMessage;
using winrt::Windows::Web::Http::HttpStatusCode;
using winrt::Windows::Web::Http::Filters::IHttpBaseProtocolFilter;
using winrt::Windows::Web::Http::Filters::IHttpFilter;

namespace Microsoft::React::Networking {

#pragma region RedirectHttpFilter

RedirectHttpFilter::RedirectHttpFilter(
    size_t maxRedirects,
    IHttpFilter &&innerFilter,
    IHttpFilter &&innerFilterWithNoCredentials,
    winrt::hstring defaultUserAgent) noexcept
    : m_maximumRedirects{maxRedirects},
      m_innerFilter{std::move(innerFilter)},
      m_innerFilterWithNoCredentials{std::move(innerFilterWithNoCredentials)},
      m_defaultUserAgent(defaultUserAgent) {
  // Prevent automatic redirections.
  if (auto baseFilter = m_innerFilter.try_as<IHttpBaseProtocolFilter>()) {
    baseFilter.AllowAutoRedirect(false);
    baseFilter.AllowUI(false);
  }
  if (auto baseFilter = m_innerFilterWithNoCredentials.try_as<IHttpBaseProtocolFilter>()) {
    baseFilter.AllowAutoRedirect(false);
    baseFilter.AllowUI(false);
  }
}

RedirectHttpFilter::RedirectHttpFilter(
    IHttpFilter &&innerFilter,
    IHttpFilter &&innerFilterWithNoCredentials,
    winrt::hstring defaultUserAgent) noexcept
    : RedirectHttpFilter(
          DefaultMaxRedirects,
          std::move(innerFilter),
          std::move(innerFilterWithNoCredentials),
          defaultUserAgent) {}

RedirectHttpFilter::RedirectHttpFilter(winrt::hstring defaultUserAgent) noexcept
    : RedirectHttpFilter(
          winrt::Windows::Web::Http::Filters::HttpBaseProtocolFilter{},
          winrt::Windows::Web::Http::Filters::HttpBaseProtocolFilter{},
          defaultUserAgent) {}

void RedirectHttpFilter::SetRequestFactory(std::weak_ptr<IWinRTHttpRequestFactory> factory) noexcept {
  m_requestFactory = factory;
}

void RedirectHttpFilter::SetRedirectSource(
    winrt::com_ptr<Microsoft::React::Networking::IRedirectEventSource> const &eventSrc) noexcept {
  m_redirEventSrc = eventSrc;
}

#pragma region IHttpBaseProtocolFilter

bool RedirectHttpFilter::AllowAutoRedirect() const {
  return m_allowAutoRedirect;
}

void RedirectHttpFilter::AllowAutoRedirect(bool value) {
  m_allowAutoRedirect = value;
}

bool RedirectHttpFilter::AllowUI() const {
  return false;
}
void RedirectHttpFilter::AllowUI(bool /*value*/) const {
  throw winrt::hresult_error{HRESULT_FROM_WIN32(ERROR_NOT_SUPPORTED)};
}

bool RedirectHttpFilter::AutomaticDecompression() const {
  if (auto baseFilter = m_innerFilter.try_as<IHttpBaseProtocolFilter>()) {
    return baseFilter.AutomaticDecompression();
  }

  return false;
}
void RedirectHttpFilter::AutomaticDecompression(bool value) const {
  if (auto baseFilter = m_innerFilter.try_as<IHttpBaseProtocolFilter>()) {
    baseFilter.AutomaticDecompression(value);
  }
}

winrt::Windows::Web::Http::Filters::HttpCacheControl RedirectHttpFilter::CacheControl() const {
  if (auto baseFilter = m_innerFilter.try_as<IHttpBaseProtocolFilter>()) {
    return baseFilter.CacheControl();
  }

  return nullptr;
}

winrt::Windows::Web::Http::HttpCookieManager RedirectHttpFilter::CookieManager() const {
  if (auto baseFilter = m_innerFilter.try_as<IHttpBaseProtocolFilter>()) {
    return baseFilter.CookieManager();
  }

  return nullptr;
}

Certificate RedirectHttpFilter::ClientCertificate() const {
  if (auto baseFilter = m_innerFilter.try_as<IHttpBaseProtocolFilter>()) {
    return baseFilter.ClientCertificate();
  }

  return nullptr;
}
void RedirectHttpFilter::ClientCertificate(Certificate const &value) const {
  if (auto baseFilter = m_innerFilter.try_as<IHttpBaseProtocolFilter>()) {
    baseFilter.ClientCertificate(value);
  }
}

IVector<ChainValidationResult> RedirectHttpFilter::IgnorableServerCertificateErrors() const {
  if (auto baseFilter = m_innerFilter.try_as<IHttpBaseProtocolFilter>()) {
    return baseFilter.IgnorableServerCertificateErrors();
  }

  return nullptr;
}

uint32_t RedirectHttpFilter::MaxConnectionsPerServer() const {
  if (auto baseFilter = m_innerFilter.try_as<IHttpBaseProtocolFilter>()) {
    return baseFilter.MaxConnectionsPerServer();
  }

  return 0;
}
void RedirectHttpFilter::MaxConnectionsPerServer(uint32_t value) const {
  if (auto baseFilter = m_innerFilter.try_as<IHttpBaseProtocolFilter>()) {
    baseFilter.MaxConnectionsPerServer(value);
  }
}

PasswordCredential RedirectHttpFilter::ProxyCredential() const {
  if (auto baseFilter = m_innerFilter.try_as<IHttpBaseProtocolFilter>()) {
    return baseFilter.ProxyCredential();
  }

  return nullptr;
}
void RedirectHttpFilter::ProxyCredential(PasswordCredential const &value) const {
  if (auto baseFilter = m_innerFilter.try_as<IHttpBaseProtocolFilter>()) {
    baseFilter.ProxyCredential(value);
  }
}

PasswordCredential RedirectHttpFilter::ServerCredential() const {
  if (auto baseFilter = m_innerFilter.try_as<IHttpBaseProtocolFilter>()) {
    return baseFilter.ServerCredential();
  }

  return nullptr;
}
void RedirectHttpFilter::ServerCredential(PasswordCredential const &value) const {
  if (auto baseFilter = m_innerFilter.try_as<IHttpBaseProtocolFilter>()) {
    baseFilter.ServerCredential(value);
  }
}

bool RedirectHttpFilter::UseProxy() const {
  if (auto baseFilter = m_innerFilter.try_as<IHttpBaseProtocolFilter>()) {
    return baseFilter.UseProxy();
  }

  return false;
}
void RedirectHttpFilter::UseProxy(bool value) const {
  if (auto baseFilter = m_innerFilter.try_as<IHttpBaseProtocolFilter>()) {
    baseFilter.UseProxy(value);
  }
}

#pragma endregion IHttpBaseProtocolFilter

#pragma region IHttpFilter

///
/// See https://github.com/dotnet/corefx/pull/22702
ResponseOperation RedirectHttpFilter::SendRequestAsync(HttpRequestMessage const &request) {
  size_t redirectCount = 0;
  HttpMethod method{nullptr};
  HttpResponseMessage response{nullptr};

  auto coRequest = request;
  auto coAllowAutoRedirect = m_allowAutoRedirect;
  auto coMaxRedirects = m_maximumRedirects;
  auto coRequestFactory = m_requestFactory;
  auto coEventSrc = m_redirEventSrc;

  method = coRequest.Method();
  auto originalUri = coRequest.RequestUri();

  // Per the Fetch standard, Authorization/Cookie headers are dropped once a redirect changes origin. This applies to
  // every later hop too, because each hop may rebuild the request from the original arguments.
  bool removeCredentials = false;
  bool removeBody = false;

  do {
    // Set User-Agent
    // See https://fetch.spec.whatwg.org/#http-network-or-cache-fetch
    if (!m_defaultUserAgent.empty()) {
      coRequest.Headers().Append(L"User-Agent", m_defaultUserAgent);
    }

    // Send subsequent requests through the filter that doesn't have the credentials included in the first request
    response =
        co_await (redirectCount > 0 ? m_innerFilterWithNoCredentials : m_innerFilter).SendRequestAsync(coRequest);

    // Stop redirecting when a non-redirect status is responded.
    if (response.StatusCode() != HttpStatusCode::MultipleChoices &&
        response.StatusCode() != HttpStatusCode::MovedPermanently &&
        response.StatusCode() != HttpStatusCode::Found && // Redirect
        response.StatusCode() != HttpStatusCode::SeeOther && // RedirectMethod
        response.StatusCode() != HttpStatusCode::TemporaryRedirect && // RedirectKeepVerb
        response.StatusCode() != HttpStatusCode::PermanentRedirect) {
      break;
    }

    // A redirect status without a Location header is a final response; return it as is.
    auto location = response.Headers().Location();
    if (!location) {
      break;
    }

    redirectCount++;
    if (redirectCount > coMaxRedirects) {
      throw winrt::hresult_error{HRESULT_FROM_WIN32(ERROR_HTTP_REDIRECT_FAILED), L"Too many redirects"};
    }

    // Call event source's OnRedirecting before modifying request parameters.
    if (coEventSrc && !coEventSrc->OnRedirecting(coRequest, response)) {
      break;
    }

    auto redirectUri = Uri{location.AbsoluteUri()};
    if (!redirectUri) {
      break;
    }

    if (redirectUri.SchemeName() != L"http" && redirectUri.SchemeName() != L"https") {
      break;
    }

    // Do not "downgrade" from HTTPS to HTTP
    if (coRequest.RequestUri().SchemeName() == L"https" && redirectUri.SchemeName() == L"http") {
      break;
    }

    // See https://fetch.spec.whatwg.org/#http-redirect-fetch
    // 301/302 change POST to GET; 303 changes any method other than GET/HEAD to GET.
    // 307/308 (and other redirect statuses) keep the method and body.
    auto status = response.StatusCode();
    if (((status == HttpStatusCode::MovedPermanently || status == HttpStatusCode::Found) &&
         IsMethod(method, L"POST")) ||
        (status == HttpStatusCode::SeeOther && !IsMethod(method, L"GET") && !IsMethod(method, L"HEAD"))) {
      method = HttpMethod::Get();
      removeBody = true;
    }

    if (!IsSameOrigin(originalUri, redirectUri)) {
      removeCredentials = true;
    }

    if (auto requestFactory = coRequestFactory.lock()) {
      coRequest = co_await requestFactory->CreateRequest(
          HttpMethod{method}, Uri{redirectUri}, coRequest.Properties(), /*includeContent*/ !removeBody);

      if (!coRequest) {
        throw winrt::hresult_error{E_INVALIDARG, L"Invalid request handle"};
      }
    } else {
      coRequest.Method(method);
      coRequest.RequestUri(redirectUri);
    }

    if (removeBody) {
      coRequest.Content(nullptr);
    }

    if (removeCredentials) {
      RemoveHeader(coRequest, L"Authorization");
      RemoveHeader(coRequest, L"Cookie");
      RemoveHeader(coRequest, L"Cookie2");
    }
  } while (coAllowAutoRedirect);

  response.RequestMessage(coRequest);

  co_return response;
}

#pragma endregion IHttpFilter

#pragma endregion RedirectHttpFilter

} // namespace Microsoft::React::Networking
