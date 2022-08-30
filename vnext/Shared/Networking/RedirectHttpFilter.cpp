// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "RedirectHttpFilter.h"

#include "WinRTTypes.h"

using winrt::Windows::Web::Http::HttpRequestMessage;
using winrt::Windows::Web::Http::Filters::IHttpFilter;

namespace Microsoft::React::Networking {

#pragma region RedirectHttpFilter

RedirectHttpFilter::RedirectHttpFilter(IHttpFilter &&innerFilter, IHttpFilter &&innerFilterWithNoCredentials)
    : m_innerFilter{std::move(innerFilter)}, m_innerFilterWithNoCredentials{std::move(innerFilterWithNoCredentials)} {}

RedirectHttpFilter::RedirectHttpFilter()
    : m_innerFilter{winrt::Windows::Web::Http::Filters::HttpBaseProtocolFilter{}},
      m_innerFilterWithNoCredentials{winrt::Windows::Web::Http::Filters::HttpBaseProtocolFilter{}} {}

#pragma region IHttpFilter

ResponseOperation RedirectHttpFilter::SendRequestAsync(HttpRequestMessage const& request)
{
  return nullptr;
}

#pragma endregion IHttpFilter

#pragma endregion RedirectHttpFilter

}//namespace
