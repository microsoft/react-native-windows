// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "OriginPolicyHttpFilter.h"

using std::set;

using winrt::hresult_error;
using winrt::to_hstring;
using winrt::Windows::Foundation::Uri;
using winrt::Windows::Web::Http::HttpMethod;
using winrt::Windows::Web::Http::HttpRequestMessage;
using winrt::Windows::Web::Http::Filters::IHttpFilter;

// TODO: Remove. Redundant.
typedef winrt::Windows::Foundation::
    IAsyncOperationWithProgress<winrt::Windows::Web::Http::HttpResponseMessage, winrt::Windows::Web::Http::HttpProgress>
        ResponseType;

namespace Microsoft::React::Networking {

#pragma region OriginPolicyHttpFilter

// https://fetch.spec.whatwg.org/#forbidden-method
/*static*/ inline set<const char *> OriginPolicyHttpFilter::s_forbiddenMethods{"CONNECT", "TRACE", "TRACK"};

/*static*/ inline set<const char *> OriginPolicyHttpFilter::s_simpleCorsRequestHeaderNames{
    "Accept",
    "Accept-Language",
    "Content-Language",
    "Content-Type",
    "DPR",
    "Downlink",
    "Save-Data",
    "Viewport-Width",
    "Width"};

/*static*/ inline set<const char *> OriginPolicyHttpFilter::s_simpleCorsResponseHeaderNames{
    "Cache-Control",
    "Content-Language",
    "Content-Type",
    "Expires",
    "Last-Modified",
    "Pragma"};

/*static*/ inline set<const char *> OriginPolicyHttpFilter::s_simpleCorsContentTypeValues{
    "application/x-www-form-urlencoded",
    "multipart/form-data",
    "text/plain"};

// https://fetch.spec.whatwg.org/#forbidden-header-name
// Chromium still bans "User-Agent" due to https://crbug.com/571722 //TODO: Remove?
/*static*/ inline set<const char *> OriginPolicyHttpFilter::s_corsForbiddenRequestHeaderNames{
    "Accept-Charset",
    "Accept-Encoding",
    "Access-Control-Request-Headers",
    "Access-Control-Request-Method",
    "Connection",
    "Content-Length",
    "Cookie",
    "Cookie2",
    "Date",
    "DNT",
    "Expect",
    "Host",
    "Keep-Alive",
    "Origin",
    "Referer",
    "TE",
    "Trailer",
    "Transfer-Encoding",
    "Upgrade",
    "Via"};

/*static*/ inline set<const char *> OriginPolicyHttpFilter::s_corsForbiddenRequestHeaderNamePrefixes{"Proxy-", "Sec-"};

IHttpFilter m_innerFilter;
OriginPolicy m_originPolicy;
Uri m_origin;

OriginPolicyHttpFilter::OriginPolicyHttpFilter(OriginPolicy originPolicy, Uri origin, IHttpFilter &&innerFilter)
    : m_originPolicy{m_originPolicy}, m_origin{origin}, m_innerFilter{std::move(innerFilter)} {}

bool OriginPolicyHttpFilter::ValidateRequest(Uri &url) {
  // case CORS:
  // https://developer.mozilla.org/en-US/docs/Web/HTTP/CORS#simple_requests
  // Refer to CorsURLLoaderFactory::IsValidRequest in chrome\src\services\network\cors\cors_url_loader_factory.cc.
  // Forbidden headers should be blocked regardless of origins.
  // It is correct to check for forbidden header first.
  // It is verified in Edge when I try to set the "host" header to a XHR. The browser rejected it as unsafe.
  // In fact, this check probably should apply to all networking security policy.
  // https://fetch.spec.whatwg.org/#forbidden-header-name
  /// Is mixed content? (incoming URL vs origin URL)
  //// => false
  /// AreRequestHeadersSafe(headers)
  //// => false
  /// IsForbiddenMethod(method)
  //// => false
  /// IsSameOrigin(m_securitySettings.origin, destinationOrigin)
  //// => SOP; Reset the validated policy to SOP policy and no need to involve simple CORS which is for cross origins.
  /// IsSimpleCors(meth, headers)
  //// => SimpleCORS; Cross origins but meet Simple CORS conditions. No need to go through Preflight.
  /// else => CORS

  return true;
}

// Mso::React::HttpResource::SendPreflight
ResponseType OriginPolicyHttpFilter::SendPreflightAsync(HttpRequestMessage const &request) const { // TODO: const& ??

  co_return {};
}

#pragma region IHttpFilter

ResponseType OriginPolicyHttpFilter::SendRequestAsync(HttpRequestMessage const &request) const {
  // Ensure absolute URL

  // If fetch is in CORS mode, ValidateSecurityOnRequest() determines if it is a simple request by inspecting origin,
  // header, and method
  /// ValidateRequest()

  // const bool isSameOrigin =
  /// (validatedSecurityPolicy != NetworkingSecurityPolicy::SimpleCORS) &&
  /// (validatedSecurityPolicy != NetworkingSecurityPolicy::CORS);
  // !isSameOrigin => CrossOrigin => RemoveUserNamePasswordFromUrl || FAIL

  // If CORS && !inCache => Preflight! { cache() }
  //

  auto coRequest = request;
  // Prototype
  try {
    auto preflightRequest = HttpRequestMessage();
    preflightRequest.RequestUri(request.RequestUri());
    preflightRequest.Method(HttpMethod::Options());

    auto response = co_await m_innerFilter.SendRequestAsync(preflightRequest);
    auto preflightStatus = response.Headers().Lookup(L"Preflight");
    if (L"Approved" != preflightStatus)
      throw hresult_error{E_FAIL, L"Origin policy non-compliance"}; // TODO: Better HRESULT?

    for (auto header : response.Headers()) {
      coRequest.Headers().Insert(header.Key(), header.Value());
    }
  } catch (hresult_error const &e) {
    throw e;
  } catch (const std::exception &e) {
    throw hresult_error{E_FAIL, to_hstring(e.what())};
  } catch (...) {
    throw hresult_error{E_FAIL, L"Unspecified error processing Origin Policy request"};
  }

  co_return {co_await m_innerFilter.SendRequestAsync(coRequest)};
}

#pragma endregion IHttpFilter

#pragma endregion OriginPolicyHttpFilter


}// namespace
