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
    IHttpFilter &&innerFilterWithNoCredentials) noexcept
    : m_maximumRedirects{maxRedirects},
      m_innerFilter{std::move(innerFilter)},
      m_innerFilterWithNoCredentials{std::move(innerFilterWithNoCredentials)} {
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

RedirectHttpFilter::RedirectHttpFilter(IHttpFilter &&innerFilter, IHttpFilter &&innerFilterWithNoCredentials) noexcept
    : RedirectHttpFilter(DefaultMaxRedirects, std::move(innerFilter), std::move(innerFilterWithNoCredentials)) {}

RedirectHttpFilter::RedirectHttpFilter() noexcept
    : RedirectHttpFilter(
          winrt::Windows::Web::Http::Filters::HttpBaseProtocolFilter{},
          winrt::Windows::Web::Http::Filters::HttpBaseProtocolFilter{}) {}

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

  do {
    // Set User-Agent
    // See https://fetch.spec.whatwg.org/#http-network-or-cache-fetch
    auto userAgent = GetRuntimeOptionString("Http.UserAgent");
    if (userAgent.size() > 0) {
      coRequest.Headers().Append(L"User-Agent", winrt::to_hstring(userAgent));
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

    redirectCount++;
    if (redirectCount > coMaxRedirects) {
      throw winrt::hresult_error{HRESULT_FROM_WIN32(ERROR_HTTP_REDIRECT_FAILED), L"Too many redirects"};
    }

    // Call event source's OnRedirecting before modifying request parameters.
    if (coEventSrc && !coEventSrc->OnRedirecting(coRequest, response)) {
      break;
    }

    if (auto requestFactory = coRequestFactory.lock()) {
      coRequest =
          co_await requestFactory->CreateRequest(HttpMethod{method}, coRequest.RequestUri(), coRequest.Properties());

      if (!coRequest) {
        throw winrt::hresult_error{E_INVALIDARG, L"Invalid request handle"};
      }
    }

    auto redirectUri = Uri{response.Headers().Location().AbsoluteUri()};
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

    /// See https://github.com/dotnet/corefx/blob/v3.1.28/src/System.Net.Http/src/uap/System/Net/HttpClientHandler.cs
    // Follow HTTP RFC 7231 rules. In general, 3xx responses
    // except for 307 and 308 will keep verb except POST becomes GET.
    // 307 and 308 responses have all verbs stay the same.
    // https://tools.ietf.org/html/rfc7231#section-6.4
    if (response.StatusCode() != HttpStatusCode::TemporaryRedirect &&
        response.StatusCode() != HttpStatusCode::PermanentRedirect && method != HttpMethod::Post()) {
      method = HttpMethod::Get();
    }

    coRequest.RequestUri(redirectUri);
  } while (coAllowAutoRedirect);

  response.RequestMessage(coRequest);

  co_return response;
}

#pragma endregion IHttpFilter

#pragma endregion RedirectHttpFilter

} // namespace Microsoft::React::Networking
