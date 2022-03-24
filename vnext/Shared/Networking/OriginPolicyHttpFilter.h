// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include "OriginPolicy.h"

// Windows API
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Web.Http.h>
#include <winrt/Windows.Web.Http.Filters.h>

// Standard Library
#include <set>

namespace Microsoft::React::Networking {

class OriginPolicyHttpFilter : public winrt::implements<OriginPolicyHttpFilter, winrt::Windows::Web::Http::Filters::IHttpFilter> {

  static std::set<const char *> s_forbiddenMethods;

  static std::set<const char *> s_simpleCorsRequestHeaderNames;

  static std::set<const char *> s_simpleCorsResponseHeaderNames;

  static std::set<const char *> s_simpleCorsContentTypeValues;

  static std::set<const char *> s_corsForbiddenRequestHeaderNames;

  static std::set<const char *> s_corsForbiddenRequestHeaderNamePrefixes;

  winrt::Windows::Web::Http::Filters::IHttpFilter m_innerFilter;
  OriginPolicy m_originPolicy;
  //winrt::Windows::Foundation::Uri m_origin;

 public:
  OriginPolicyHttpFilter(
      OriginPolicy originPolicy,
      winrt::Windows::Foundation::Uri origin,
      winrt::Windows::Web::Http::Filters::IHttpFilter &&innerFilter);

  bool ValidateRequest(winrt::Windows::Foundation::Uri &url);

  winrt::Windows::Foundation::IAsyncOperationWithProgress<
      winrt::Windows::Web::Http::HttpResponseMessage,
      winrt::Windows::Web::Http::HttpProgress>
  SendPreflightAsync(winrt::Windows::Web::Http::HttpRequestMessage const &request) const;

  winrt::Windows::Foundation::IAsyncOperationWithProgress<
      winrt::Windows::Web::Http::HttpResponseMessage,
      winrt::Windows::Web::Http::HttpProgress>
  SendRequestAsync(winrt::Windows::Web::Http::HttpRequestMessage const &request) const;
};

}//namespace
