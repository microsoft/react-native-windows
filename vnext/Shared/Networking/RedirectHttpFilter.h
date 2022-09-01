// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include "IWinRTHttpRequestFactory.h"

#include <winrt/base.h>

// Windows API
#include <winrt/Windows.Web.Http.Filters.h>
#include <winrt/Windows.Web.Http.h>

namespace Microsoft::React::Networking {

class RedirectHttpFilter : public winrt::implements<
          RedirectHttpFilter,
          winrt::Windows::Web::Http::Filters::IHttpFilter,
          winrt::Windows::Web::Http::Filters::IHttpBaseProtocolFilter> {
  //TODO: Quote
  //  We need two different WinRT filters because we need to remove credentials during redirection requests
  //  and WinRT doesn't allow changing the filter properties after the first request.
  winrt::Windows::Web::Http::Filters::IHttpFilter m_innerFilter;
  winrt::Windows::Web::Http::Filters::IHttpFilter m_innerFilterWithNoCredentials;

  std::weak_ptr<IWinRTHttpRequestFactory> m_requestFactory;
  bool m_allowAutoRedirect{true};

 public:
  RedirectHttpFilter(
      winrt::Windows::Web::Http::Filters::IHttpFilter &&innerFilter,
      winrt::Windows::Web::Http::Filters::IHttpFilter &&innerFilterWithNoCredentials) noexcept;

  RedirectHttpFilter() noexcept;

  void SetRequestFactory(std::weak_ptr<IWinRTHttpRequestFactory> factory) noexcept;

#pragma region IHttpFilter

  winrt::Windows::Foundation::IAsyncOperationWithProgress<
      winrt::Windows::Web::Http::HttpResponseMessage,
      winrt::Windows::Web::Http::HttpProgress>
  SendRequestAsync(winrt::Windows::Web::Http::HttpRequestMessage const &request);

#pragma endregion IHttpFilter

#pragma region IHttpBaseProtocolFilter

  bool AllowAutoRedirect() const;
  void AllowAutoRedirect(bool value);

  bool AllowUI() const {return false;}
  void AllowUI(bool value) const {}

  bool AutomaticDecompression() const{return false;}
  void AutomaticDecompression(bool value) const {}

  winrt::Windows::Web::Http::Filters::HttpCacheControl CacheControl() const {return nullptr;}

  winrt::Windows::Web::Http::HttpCookieManager CookieManager() const {return nullptr;}

  winrt::Windows::Security::Cryptography::Certificates::Certificate ClientCertificate() const {return nullptr;}
  void ClientCertificate(winrt::Windows::Security::Cryptography::Certificates::Certificate const &value) const {}

  winrt::Windows::Foundation::Collections::IVector<
      winrt::Windows::Security::Cryptography::Certificates::ChainValidationResult>
  IgnorableServerCertificateErrors() const { return nullptr;}

  uint32_t MaxConnectionsPerServer() const { return 0; }
  void MaxConnectionsPerServer(uint32_t value) const {}

  winrt::Windows::Security::Credentials::PasswordCredential ProxyCredential() const { return nullptr;}
  void ProxyCredential(winrt::Windows::Security::Credentials::PasswordCredential const &value) const {}

  winrt::Windows::Security::Credentials::PasswordCredential ServerCredential() const { return nullptr;}
  void ServerCredential(winrt::Windows::Security::Credentials::PasswordCredential const &value) const {}

  bool UseProxy() const { return false;}
  void UseProxy(bool value) const {}

#pragma endregion IHttpBaseProtocolFilter

};

} //namespace
