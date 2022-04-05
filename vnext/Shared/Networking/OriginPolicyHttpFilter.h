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

  // NOTE: Assumes static origin through owning client/resource/module/(React) instance's lifetime.
  static winrt::Windows::Foundation::Uri s_origin;

  struct AccessControlValues
  {
    winrt::hstring AllowedOrigin;
    winrt::hstring AllowedCredentials;
    std::set<std::wstring> AllowedHeaders;
    std::set<std::wstring> AllowedMethods;
    std::set<std::wstring> ExposedHeaders;
    size_t MaxAge;
  };

  winrt::Windows::Web::Http::Filters::IHttpFilter m_innerFilter;
  OriginPolicy m_originPolicy;

 public:
  static void SetStaticOrigin(const char *url);

  static bool IsSameOrigin(
      winrt::Windows::Foundation::Uri const &u1,
      winrt::Windows::Foundation::Uri const &u2) noexcept;

  static winrt::Windows::Foundation::Uri GetOrigin(winrt::Windows::Foundation::Uri const &uri) noexcept;

  static bool IsSimpleCorsRequest(winrt::Windows::Web::Http::HttpRequestMessage const &request) noexcept;

  static bool AreSafeRequestHeaders(
      winrt::Windows::Web::Http::Headers::HttpRequestHeaderCollection const &headers) noexcept;

  static std::set<const wchar_t *> CorsUnsafeNotForbiddenRequestHeaderNames(
      winrt::Windows::Web::Http::Headers::HttpRequestHeaderCollection const &headers) noexcept;

  static AccessControlValues ExtractAccessControlValues(
      winrt::Windows::Foundation::Collections::IMap<winrt::hstring, winrt::hstring> const &headers);

  static bool IsCorsSafelistedRequestHeader(winrt::hstring const &name, winrt::hstring const &value) noexcept;

  static bool IsCorsUnsafeRequestHeaderByte(wchar_t c) noexcept;

  OriginPolicyHttpFilter(OriginPolicy originPolicy, winrt::Windows::Web::Http::Filters::IHttpFilter &&innerFilter);

  OriginPolicyHttpFilter(OriginPolicy originPolicy);

  void ValidateRequest(winrt::Windows::Web::Http::HttpRequestMessage const &request);

  void ValidatePreflightResponse(
      winrt::Windows::Web::Http::HttpRequestMessage const &request,
      winrt::Windows::Web::Http::HttpResponseMessage const &response) const;

  void ValidateResponse(winrt::Windows::Web::Http::HttpResponseMessage const &response) const;

  void ValidateAllowOrigin(
      winrt::hstring const &allowedOrigin,
      winrt::hstring const &allowCredentials,
      winrt::Windows::Foundation::IInspectable const &iArgs) const;

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
