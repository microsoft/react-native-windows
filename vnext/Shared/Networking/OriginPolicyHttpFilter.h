// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include "OriginPolicy.h"

// Windows API
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Web.Http.Filters.h>
#include <winrt/Windows.Web.Http.h>

// Standard Library
#include <set>

namespace Microsoft::React::Networking {

class OriginPolicyHttpFilter
    : public winrt::implements<OriginPolicyHttpFilter, winrt::Windows::Web::Http::Filters::IHttpFilter> {
  static std::set<const wchar_t *> s_forbiddenMethods;
  static std::set<const wchar_t *> s_simpleCorsMethods;
  static std::set<const wchar_t *> s_simpleCorsRequestHeaderNames;
  static std::set<const wchar_t *> s_simpleCorsResponseHeaderNames;
  static std::set<const wchar_t *> s_simpleCorsContentTypeValues;
  static std::set<const wchar_t *> s_corsForbiddenRequestHeaderNames;
  static std::set<const wchar_t *> s_corsForbiddenRequestHeaderNamePrefixes;

  winrt::Windows::Web::Http::Filters::IHttpFilter m_innerFilter;
  OriginPolicy m_originPolicy;
  // TODO: Keep?? May overlap through requests
  winrt::Windows::Foundation::Uri m_origin;

 public:
  static bool IsSameOrigin(
      winrt::Windows::Foundation::Uri const &u1,
      winrt::Windows::Foundation::Uri const &u2) noexcept;

  static winrt::Windows::Foundation::Uri GetOrigin(winrt::Windows::Foundation::Uri const &uri) noexcept;

  static bool IsSimpleCorsRequest(winrt::Windows::Web::Http::HttpRequestMessage const &request) noexcept;

  static bool AreSafeRequestHeaders(
      winrt::Windows::Web::Http::Headers::HttpRequestHeaderCollection const &headers) noexcept;

  OriginPolicyHttpFilter(OriginPolicy originPolicy, winrt::Windows::Web::Http::Filters::IHttpFilter &&innerFilter);

  OriginPolicyHttpFilter(OriginPolicy originPolicy);

  void ValidateRequest(winrt::Windows::Web::Http::HttpRequestMessage const &request);

  void ValidatePreflightResponse(
      winrt::Windows::Web::Http::HttpRequestMessage const &request,
      winrt::Windows::Web::Http::HttpResponseMessage const &response) const;

  void ValidateAllowOrigin(winrt::hstring const &origin, winrt::hstring const &allowCredentials, winrt::Windows::Foundation::IInspectable const& iArgs) const;

  winrt::Windows::Foundation::IAsyncOperationWithProgress<
      winrt::Windows::Web::Http::HttpResponseMessage,
      winrt::Windows::Web::Http::HttpProgress>
  SendPreflightAsync(winrt::Windows::Web::Http::HttpRequestMessage const &request) const;

#pragma region IHttpFilter

  winrt::Windows::Foundation::IAsyncOperationWithProgress<
      winrt::Windows::Web::Http::HttpResponseMessage,
      winrt::Windows::Web::Http::HttpProgress>
  SendRequestAsync(winrt::Windows::Web::Http::HttpRequestMessage const &request);

#pragma endregion IHttpFilter
};

} // namespace Microsoft::React::Networking
