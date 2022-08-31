// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "RedirectHttpFilter.h"

#include "WinRTTypes.h"

// Windows API
#include <winrt/Windows.Web.Http.Headers.h>

using winrt::Windows::Web::Http::HttpMethod;
using winrt::Windows::Web::Http::HttpRequestMessage;
using winrt::Windows::Web::Http::HttpResponseMessage;
using winrt::Windows::Web::Http::HttpStatusCode;
using winrt::Windows::Web::Http::Filters::IHttpBaseProtocolFilter;
using winrt::Windows::Web::Http::Filters::IHttpFilter;

namespace Microsoft::React::Networking {

#pragma region RedirectHttpFilter

RedirectHttpFilter::RedirectHttpFilter(IHttpFilter &&innerFilter, IHttpFilter &&innerFilterWithNoCredentials) noexcept
    : m_innerFilter{std::move(innerFilter)},
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

RedirectHttpFilter::RedirectHttpFilter() noexcept
    : RedirectHttpFilter(
          winrt::Windows::Web::Http::Filters::HttpBaseProtocolFilter{},
          winrt::Windows::Web::Http::Filters::HttpBaseProtocolFilter{}) {}

void RedirectHttpFilter::SetRequestFactory(std::weak_ptr<IWinRTHttpRequestFactory> factory) noexcept
{
  m_requestFactory = factory;
}

//#pragma region IHttpBaseProtocolFilter
//
//bool RedirectHttpFilter::AllowAutoRedirect() const
//{
//  return m_allowAutoRedirect;
//}
//
//void RedirectHttpFilter::AllowAutoRedirect(bool value) {
//  m_allowAutoRedirect = value;
//}
//
//#pragma endregion IHttpBaseProtocolFilter

#pragma region IHttpFilter

ResponseOperation RedirectHttpFilter::SendRequestAsync(HttpRequestMessage const& request)
{
  auto redirectCount = 0;
  HttpMethod method{nullptr};
  bool skipContentIfPresent = false;//TODO: Delte? Do we need manual HTTP content creation?
  HttpResponseMessage response{nullptr};

  //TODO: Ensure request is not null

  auto coRequest = request;

  method = coRequest.Method();

  //TODO: Enable autoredirect flag and turn into do-while???
  while (true) {
    //TODO: If cancellable, cancel/throw.

    if (response) {
      response = nullptr;
    }

    //TODO: Convert request?

    // Send subsequent requests through the filter that doesn't have the credentials included in the first request
    response = co_await (redirectCount > 0 ? m_innerFilterWithNoCredentials : m_innerFilter).SendRequestAsync(coRequest);

    //TODO: Convert response?

    // TODO: Process cookies.

    // Stop redirecting when a non-redirect status is responded.
    if (response.StatusCode() != HttpStatusCode::MultipleChoices &&
        response.StatusCode() != HttpStatusCode::MovedPermanently &&
        response.StatusCode() != HttpStatusCode::Found &&             // Redirect
        response.StatusCode() != HttpStatusCode::SeeOther &&          // RedirectMethod
        response.StatusCode() != HttpStatusCode::TemporaryRedirect && // RedirectKeepVerb
        response.StatusCode() != HttpStatusCode::PermanentRedirect) {
      break;
    }

    redirectCount++;
    if (redirectCount > 3 /*TODO: max redirections*/) {
      break;
    }

    if (auto requestFactory = m_requestFactory.lock()) {
      coRequest = co_await requestFactory->CreateRequest(
        coRequest.Method(), coRequest.RequestUri(), coRequest.Properties().Lookup(L"RequestArgs"));

      if (!coRequest) {
        break; // TODO: Throw?
      }
    }

    auto redirectUri = response.Headers().Location();
    if (!redirectUri) {
      break;
    }

    //TODO: Ensure absolute URI

    if (redirectUri.SchemeName() != L"http" && redirectUri.SchemeName() != L"https") {
      break;
    }

    // Do not "downgrade" from HTTPS to HTTP
    if (coRequest.RequestUri().SchemeName() == L"https" && redirectUri.SchemeName() == L"http") {
      break;
    }

    //TODO: Quote source
    // Follow HTTP RFC 7231 rules. In general, 3xx responses
    // except for 307 and 308 will keep verb except POST becomes GET.
    // 307 and 308 responses have all verbs stay the same.
    // https://tools.ietf.org/html/rfc7231#section-6.4
    if (response.StatusCode() != HttpStatusCode::TemporaryRedirect &&
        response.StatusCode() != HttpStatusCode::PermanentRedirect &&
        method != HttpMethod::Post()) {
      method = HttpMethod::Get();
      skipContentIfPresent = true;
    }

    coRequest.RequestUri(redirectUri);
  } // while(true)

  response.RequestMessage(coRequest);

  co_return response;
}

#pragma endregion IHttpFilter

#pragma endregion RedirectHttpFilter

}//namespace
