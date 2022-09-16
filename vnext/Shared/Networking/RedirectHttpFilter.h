// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include <winrt/base.h>
#include "IRedirectEventSource.h"
#include "IWinRTHttpRequestFactory.h"

// Windows API
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Security.Credentials.h>
#include <winrt/Windows.Security.Cryptography.Certificates.h>
#include <winrt/Windows.Web.Http.Filters.h>
#include <winrt/Windows.Web.Http.h>

namespace Microsoft::React::Networking {

class RedirectHttpFilter : public winrt::implements<
                               RedirectHttpFilter,
                               winrt::Windows::Web::Http::Filters::IHttpFilter,
                               winrt::Windows::Web::Http::Filters::IHttpBaseProtocolFilter> {
  // See
  // https://github.com/dotnet/corefx/pull/22702/files#diff-53f0c1940c6bec8054a95caac33680306aa6ab13ac48c9a8c9df013d3bc29d15R30
  //  We need two different WinRT filters because we need to remove credentials during redirection requests
  //  and WinRT doesn't allow changing the filter properties after the first request.
  winrt::Windows::Web::Http::Filters::IHttpFilter m_innerFilter;
  winrt::Windows::Web::Http::Filters::IHttpFilter m_innerFilterWithNoCredentials;

  std::weak_ptr<IWinRTHttpRequestFactory> m_requestFactory;
  winrt::com_ptr<Microsoft::React::Networking::IRedirectEventSource> m_redirEventSrc;
  bool m_allowAutoRedirect{true};
  size_t m_maximumRedirects;

 public:
  RedirectHttpFilter(
      size_t maxRedirects,
      winrt::Windows::Web::Http::Filters::IHttpFilter &&innerFilter,
      winrt::Windows::Web::Http::Filters::IHttpFilter &&innerFilterWithNoCredentials) noexcept;

  RedirectHttpFilter(
      winrt::Windows::Web::Http::Filters::IHttpFilter &&innerFilter,
      winrt::Windows::Web::Http::Filters::IHttpFilter &&innerFilterWithNoCredentials) noexcept;

  RedirectHttpFilter() noexcept;

  void SetRequestFactory(std::weak_ptr<IWinRTHttpRequestFactory> factory) noexcept;

  void SetRedirectSource(winrt::com_ptr<Microsoft::React::Networking::IRedirectEventSource> const &eventSrc) noexcept;

#pragma region IHttpFilter

  winrt::Windows::Foundation::IAsyncOperationWithProgress<
      winrt::Windows::Web::Http::HttpResponseMessage,
      winrt::Windows::Web::Http::HttpProgress>
  SendRequestAsync(winrt::Windows::Web::Http::HttpRequestMessage const &request);

#pragma endregion IHttpFilter

#pragma region IHttpBaseProtocolFilter

  bool AllowAutoRedirect() const;
  void AllowAutoRedirect(bool value);

  bool AllowUI() const;
  void AllowUI(bool value) const;

  bool AutomaticDecompression() const;
  void AutomaticDecompression(bool value) const;

  winrt::Windows::Web::Http::Filters::HttpCacheControl CacheControl() const;

  winrt::Windows::Web::Http::HttpCookieManager CookieManager() const;

  winrt::Windows::Security::Cryptography::Certificates::Certificate ClientCertificate() const;
  void ClientCertificate(winrt::Windows::Security::Cryptography::Certificates::Certificate const &value) const;

  winrt::Windows::Foundation::Collections::IVector<
      winrt::Windows::Security::Cryptography::Certificates::ChainValidationResult>
  IgnorableServerCertificateErrors() const;

  uint32_t MaxConnectionsPerServer() const;
  void MaxConnectionsPerServer(uint32_t value) const;

  winrt::Windows::Security::Credentials::PasswordCredential ProxyCredential() const;
  void ProxyCredential(winrt::Windows::Security::Credentials::PasswordCredential const &value) const;

  winrt::Windows::Security::Credentials::PasswordCredential ServerCredential() const;
  void ServerCredential(winrt::Windows::Security::Credentials::PasswordCredential const &value) const;

  bool UseProxy() const;
  void UseProxy(bool value) const;

#pragma endregion IHttpBaseProtocolFilter
};

} // namespace Microsoft::React::Networking
