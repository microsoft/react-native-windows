// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

// Windows API
#include <winrt/Windows.Web.Http.Filters.h>
#include <winrt/Windows.Web.Http.h>

namespace Microsoft::React::Networking {

class RedirectHttpFilter
  : public winrt::implements<RedirectHttpFilter, winrt::Windows::Web::Http::Filters::IHttpFilter>
{
  //TODO: Quote
  //  We need two different WinRT filters because we need to remove credentials during redirection requests
  //  and WinRT doesn't allow changing the filter properties after the first request.
  winrt::Windows::Web::Http::Filters::IHttpFilter m_innerFilter;
  winrt::Windows::Web::Http::Filters::IHttpFilter m_innerFilterWithNoCredentials;

 public:
  RedirectHttpFilter(
      winrt::Windows::Web::Http::Filters::IHttpFilter &&innerFilter,
      winrt::Windows::Web::Http::Filters::IHttpFilter &&innerFilterWithNoCredentials);//TODO: noexcept?

  RedirectHttpFilter();//TODO: noexcept?

#pragma region IHttpFilter

  winrt::Windows::Foundation::IAsyncOperationWithProgress<
      winrt::Windows::Web::Http::HttpResponseMessage,
      winrt::Windows::Web::Http::HttpProgress>
  SendRequestAsync(winrt::Windows::Web::Http::HttpRequestMessage const &request);


#pragma endregion IHttpFilter
};

} //namespace
