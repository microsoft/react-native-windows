// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "OriginPolicyHttpFilter.h"
#include "WinRTHttpResource.h" //RequestArgs

#include <RuntimeOptions.h>

// Boost Library
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast/try_lexical_convert.hpp>

// Windows API
#include <winrt/Windows.Web.Http.Headers.h>

// Standard Library
#include <regex>

using std::set;
using std::wstring;

using winrt::hresult_error;
using winrt::hstring;
using winrt::to_hstring;
using winrt::Windows::Foundation::IInspectable;
using winrt::Windows::Foundation::Uri;
using winrt::Windows::Web::Http::HttpMethod;
using winrt::Windows::Web::Http::HttpRequestMessage;
using winrt::Windows::Web::Http::HttpResponseMessage;
using winrt::Windows::Web::Http::Filters::IHttpFilter;
using winrt::Windows::Web::Http::Headers::HttpMediaTypeHeaderValue;
using winrt::Windows::Web::Http::Headers::HttpRequestHeaderCollection;

// TODO: Remove. Redundant.
typedef winrt::Windows::Foundation::
    IAsyncOperationWithProgress<winrt::Windows::Web::Http::HttpResponseMessage, winrt::Windows::Web::Http::HttpProgress>
        ResponseType;

namespace Microsoft::React::Networking {

#pragma region OriginPolicyHttpFilter

// https://fetch.spec.whatwg.org/#forbidden-method
/*static*/ set<const wchar_t *> OriginPolicyHttpFilter::s_forbiddenMethods = {L"CONNECT", L"TRACE", L"TRACK"};

/*static*/ set<const wchar_t *> OriginPolicyHttpFilter::s_simpleCorsMethods = {L"GET", L"HEAD", L"POST"};

/*static*/ set<const wchar_t *> OriginPolicyHttpFilter::s_simpleCorsRequestHeaderNames = {
    L"Accept",
    L"Accept-Language",
    L"Content-Language",
    L"Content-Type",
    L"DPR",
    L"Downlink",
    L"Save-Data",
    L"Viewport-Width",
    L"Width"};

/*static*/ set<const wchar_t *> OriginPolicyHttpFilter::s_simpleCorsResponseHeaderNames =
    {L"Cache-Control", L"Content-Language", L"Content-Type", L"Expires", L"Last-Modified", L"Pragma"};

/*static*/ set<const wchar_t *> OriginPolicyHttpFilter::s_simpleCorsContentTypeValues = {
    L"application/x-www-form-urlencoded",
    L"multipart/form-data",
    L"text/plain"};

// https://fetch.spec.whatwg.org/#forbidden-header-name
// Chromium still bans "User-Agent" due to https://crbug.com/571722 //TODO: Remove?
/*static*/ set<const wchar_t *> OriginPolicyHttpFilter::s_corsForbiddenRequestHeaderNames = {
    L"Accept-Charset",
    L"Accept-Encoding",
    L"Access-Control-Request-Headers",
    L"Access-Control-Request-Method",
    L"Connection",
    L"Content-Length",
    L"Cookie",
    L"Cookie2",
    L"Date",
    L"DNT",
    L"Expect",
    L"Host",
    L"Keep-Alive",
    L"Origin",
    L"Referer",
    L"TE",
    L"Trailer",
    L"Transfer-Encoding",
    L"Upgrade",
    L"Via"};

/*static*/ set<const wchar_t *> OriginPolicyHttpFilter::s_corsForbiddenRequestHeaderNamePrefixes = {L"Proxy-", L"Sec-"};

/*static*/ bool OriginPolicyHttpFilter::IsSameOrigin(Uri const &u1, Uri const &u2) noexcept {
  return u1.SchemeName() == u2.SchemeName() && u1.Host() == u2.Host() && u1.Port() == u2.Port();
}

/*static*/ bool OriginPolicyHttpFilter::IsSimpleCorsRequest(HttpRequestMessage const &request) noexcept {
  // Ensure header is in Simple CORS white list
  for (const auto &header : request.Headers()) {
    if (s_simpleCorsRequestHeaderNames.find(header.Key().c_str()) == s_simpleCorsRequestHeaderNames.cend())
      return false;

    // Ensure Content-Type value is in Simple CORS white list, if present
    if (header.Key() == L"Content-Type") {
      if (s_simpleCorsContentTypeValues.find(header.Value().c_str()) != s_simpleCorsContentTypeValues.cend())
        return false;
    }
  }

  // Ensure method is in Simple CORS white list
  return s_simpleCorsMethods.find(request.Method().ToString().c_str()) != s_simpleCorsMethods.cend();
}

/*static*/ Uri OriginPolicyHttpFilter::GetOrigin(Uri const &uri) noexcept {
  return Uri{uri.SchemeName() + L"://" + uri.Host() + L":" + to_hstring(uri.Port())};
}

/*static*/ bool OriginPolicyHttpFilter::AreSafeRequestHeaders(
    winrt::Windows::Web::Http::Headers::HttpRequestHeaderCollection const &headers) noexcept {
  for (const auto &header : headers) {
    if (s_corsForbiddenRequestHeaderNames.find(header.Key().c_str()) != s_corsForbiddenRequestHeaderNames.cend())
      return false;

    for (const auto &prefix : s_corsForbiddenRequestHeaderNamePrefixes) {
      // TODO: If prefix matches header name, RETURN FALSE
    }
  }

  return true;
}

// See https://fetch.spec.whatwg.org/#cors-safelisted-request-header
// "A CORS-safelisted header is a header whose name is either one of 'Accept', 'Accept-Language', and
// 'Content-Language', or whose name is 'Content-Type' and value is one of 'application/x-www-form-urlencoded',
// 'multipart/form-data', and 'text/plain'
/*static*/ bool OriginPolicyHttpFilter::IsCorsSafelistedRequestHeader(
    hstring const &name,
    hstring const &value) noexcept {
  // 1. If value's length is greater than 128, then return false.
  if (value.size() > 128)
    return false;

  // 2. Byte-lowercase name and switch on the result:
  static const wchar_t *const safeHeaderNames[] = {
      // The following four headers are from the CORS spec
      L"accept",
      L"accept-language",
      L"content-language",
      L"content-type",

      // https://developer.mozilla.org/en-US/docs/Web/HTTP/Headers/Save-Data
      L"save-data",

      // https://w3c.github.io/device-memory/#sec-device-memory-client-hint-header
      L"device-memory",
      L"dpr",
      L"width",
      L"viewport-width",

      // https://tools.ietf.org/html/draft-west-lang-client-hint
      L"sec-ch-lang",

      // https://tools.ietf.org/html/draft-west-ua-client-hints
      L"sec-ch-ua",
      L"sec-ch-ua-platform",
      L"sec-ch-ua-arch",
      L"sec-ch-ua-model",
      L"sec-ch-ua-mobile",
      L"sec-ch-ua-full-version",
      L"sec-ch-ua-platform-version",
  };

  auto nameLower = boost::to_lower_copy(wstring{name.c_str()});
  if (std::find(std::cbegin(safeHeaderNames), std::cend(safeHeaderNames), nameLower) == std::end(safeHeaderNames))
    return false;

  double doubleHolder;
  if (nameLower == L"device-memory" || nameLower == L"dpr")
    return boost::conversion::try_lexical_convert<double, wstring>(value.c_str(), doubleHolder);

  int intHolder;
  if (nameLower == L"width" || nameLower == L"viewport-width")
    return boost::conversion::try_lexical_convert<int, wstring>(value.c_str(), intHolder);

  auto valueLower = boost::to_lower_copy(wstring{value.c_str()});
  if (nameLower == L"save-data")
    return valueLower == L"on";

  if (nameLower == L"accept")
    return !std::any_of(valueLower.cbegin(), valueLower.cend(), IsCorsUnsafeRequestHeaderByte);

  if (nameLower == L"accept-language" || nameLower == L"content-language") {
    return std::all_of(valueLower.cbegin(), valueLower.cend(), [](wchar_t c) noexcept {
      return (0x30 <= c && c <= 0x39) || // 0-9
          (0x41 <= c && c <= 0x5A) || // A-Z
          (0x61 <= c && c <= 0x7A) || // a-z
          c == 0x20 || c == 0x2A || c == 0x2C || c == 0x2D || c == 0x2E || c == 0x3B || c == 0x3D; // *,-.;=
    });
  }

  if (nameLower == L"content-type") {
    if (std::any_of(valueLower.cbegin(), valueLower.cend(), IsCorsUnsafeRequestHeaderByte))
      return false;

    // https://mimesniff.spec.whatwg.org/#parse-a-mime-type
    HttpMediaTypeHeaderValue mediaType{nullptr};
    if (HttpMediaTypeHeaderValue::TryParse(valueLower, mediaType))
      return mediaType.ToString() == L"application/x-www-form-urlencoded" ||
          mediaType.ToString() == L"multipart/form-data" || mediaType.ToString() == L"text/plain";

    return false;
  }

  return true;
}

// https://fetch.spec.whatwg.org/#cors-unsafe-request-header-byte
/*static*/ bool OriginPolicyHttpFilter::IsCorsUnsafeRequestHeaderByte(wchar_t c) noexcept {
  // const auto u = static_cast<uint8_t>(c);
  return (c < 0x20 && c != 0x09) || c == 0x22 || c == 0x28 || c == 0x29 || c == 0x3a || c == 0x3c || c == 0x3e ||
      c == 0x3f || c == 0x40 || c == 0x5b || c == 0x5c || c == 0x5d || c == 0x7b || c == 0x7d || c == 0x7f;
}

/*static*/ set<const wchar_t *> OriginPolicyHttpFilter::CorsUnsafeNotForbiddenRequestHeaderNames(
    HttpRequestHeaderCollection const &headers) noexcept {
  constexpr size_t maxSafelistValueSize = 1024;
  size_t safelistValueSize = 0;
  std::vector<const wchar_t *> potentiallyUnsafeNames;
  set<const wchar_t *> result;
  for (const auto &header : headers) {
    const auto headerName = header.Key().c_str();

    // If header is not safe
    if (boost::istarts_with(headerName, L"Proxy-") || boost::istarts_with(headerName, L"Sec-") ||
        s_corsForbiddenRequestHeaderNames.find(headerName) != s_corsForbiddenRequestHeaderNames.cend())
      continue;

    if (!IsCorsSafelistedRequestHeader(header.Key(), header.Value())) {
      result.emplace(header.Key().c_str());
    } else {
      potentiallyUnsafeNames.emplace_back(std::wstring_view{header.Key()}.data());
      safelistValueSize += header.Value().size();
    }
  }

  if (safelistValueSize > maxSafelistValueSize)
    result.insert(potentiallyUnsafeNames.begin(), potentiallyUnsafeNames.end());

  return result;
}

OriginPolicyHttpFilter::OriginPolicyHttpFilter(OriginPolicy originPolicy, Uri &&origin, IHttpFilter &&innerFilter)
    : m_originPolicy{originPolicy}, m_origin{std::move(origin)}, m_innerFilter{std::move(innerFilter)} {}

OriginPolicyHttpFilter::OriginPolicyHttpFilter(OriginPolicy originPolicy, Uri &&origin)
    : OriginPolicyHttpFilter(
          originPolicy,
          std::move(origin),
          winrt::Windows::Web::Http::Filters::HttpBaseProtocolFilter{}) {}

void OriginPolicyHttpFilter::ValidateRequest(HttpRequestMessage const &request) {
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

  switch (m_originPolicy) {
    case OriginPolicy::None:
      return;

    case OriginPolicy::SameOrigin:
      if (!IsSameOrigin(m_origin, request.RequestUri()))
        throw hresult_error{E_INVALIDARG, L"SOP (same-origin policy) is enforced.\\n"};
      break;

    case OriginPolicy::SimpleCrossOriginResourceSharing:
      // Check for disallowed mixed content
      if (GetRuntimeOptionBool("Http.BlockMixedContentSimpleCors") &&
          m_origin.SchemeName() != request.RequestUri().SchemeName())
        throw hresult_error{E_INVALIDARG, L"The origin and request URLs must have the same scheme"};

      if (IsSameOrigin(m_origin, request.RequestUri()))
        // Same origin. Therefore, skip Cross-Origin handling.
        m_originPolicy = OriginPolicy::SameOrigin;
      else if (!IsSimpleCorsRequest(request))
        // TODO: isRedirect?
        throw hresult_error{
            E_INVALIDARG,
            L"The request does not meet the requirements for Same-Origin policy or Simple Cross-Origin resource sharing"};
      break;

    case OriginPolicy::CrossOriginResourceSharing:
      // TODO: Rewrite
      // https://developer.mozilla.org/en-US/docs/Web/HTTP/CORS#simple_requests
      // Refer to CorsURLLoaderFactory::IsValidRequest in chrome\src\services\network\cors\cors_url_loader_factory.cc.
      // Forbidden headers should be blocked regardless of origins.

      // It is correct to check for forbidden header first.
      // It is verified in Edge when I try to set the "host" header to a XHR. The browser rejected it as unsafe.
      // In fact, this check probably should apply to all networking security policy.
      // https://fetch.spec.whatwg.org/#forbidden-header-name

      // TODO: Make message static private
      if (m_origin.SchemeName() != request.RequestUri().SchemeName())
        throw hresult_error{E_INVALIDARG, L"The origin and request URLs must have the same scheme"};

      if (!AreSafeRequestHeaders(request.Headers()))
        throw hresult_error{E_INVALIDARG, L"Request header not allowed in cross-origin resource sharing"};

      if (s_forbiddenMethods.find(request.Method().ToString().c_str()) != s_forbiddenMethods.cend())
        throw hresult_error{E_INVALIDARG, L"Request method not allowed in cross-origin resource sharing"};

      // TODO: overwrite member OP, or set/return validated OP?
      if (IsSameOrigin(m_origin, request.RequestUri()))
        m_originPolicy = OriginPolicy::SameOrigin;
      else if (IsSimpleCorsRequest(request))
        m_originPolicy = OriginPolicy::SimpleCrossOriginResourceSharing;
      else
        m_originPolicy = OriginPolicy::CrossOriginResourceSharing;

      break;

    default:
      throw hresult_error{
          E_INVALIDARG, L"Invalid OriginPolicy type: " + to_hstring(static_cast<size_t>(m_originPolicy))};
  }
}

// See https://fetch.spec.whatwg.org/#cors-check
void OriginPolicyHttpFilter::ValidateAllowOrigin(
    hstring const &origin,
    hstring const &allowCredentials,
    IInspectable const &iArgs) const {
  if (origin.size() == 0)
    throw hresult_error{E_INVALIDARG, L"No valid origin in response"};

  // 4.10.1-2 - null allow origin
  if (L"null" == origin)
    throw hresult_error{
        E_INVALIDARG,
        L"The Access-Control-Allow-Origin header has a value of [null] which differs from the supplied origin.\\n"};

  bool withCredentials = winrt::get_self<RequestArgs, IInspectable>(iArgs)->WithCredentials;
  // 4.10.3 - valid wild card allow origin
  if (!withCredentials && L"*" == origin)
    return;

  // We assume the source (request) origin is not "*", "null", or empty string. Valid URI is expected.
  // 4.10.4 - Mismatched allow origin
  if (!IsSameOrigin(m_origin, Uri{origin}))
    throw hresult_error{
        E_INVALIDARG,
        L"The Access-Control-Allow-Origin header has a value of [" + origin +
            L"] which differs from the supplied origin.\\n"};

  // 4.10.5
  if (!withCredentials)
    return;

  // 4.10.6-8
  // https://fetch.spec.whatwg.org/#http-access-control-allow-credentials
  // This check should be case sensitive.
  // See also https://fetch.spec.whatwg.org/#http-new-header-syntax
  if (L"true" != allowCredentials)
    throw hresult_error{
        E_INVALIDARG,
        L"Access-Control-Allow-Credentials value must be \"true\" when the response includes credentials.\\n"};
};

void OriginPolicyHttpFilter::ValidatePreflightResponse(
    HttpRequestMessage const &request,
    HttpResponseMessage const &response) const {
  using std::wregex;
  using std::wsregex_token_iterator;

  // https://tools.ietf.org/html/rfc2616#section-4.2
  wregex rgx{L"\\s*,\\s*"};
  auto ciStrCmp = [](const wstring &a, const wstring &b) { return _wcsicmp(a.c_str(), b.c_str()) < 0; };

  set<wstring, decltype(ciStrCmp)> allowedHeaders{ciStrCmp};
  set<wstring, decltype(ciStrCmp)> allowedMethods{ciStrCmp};
  set<wstring, decltype(ciStrCmp)> exposedHeaders{ciStrCmp};
  hstring allowedOrigin;
  hstring allowedCredentials;
  size_t maxAge;

  // Extract header values
  for (const auto &header : response.Headers()) {
    if (header.Key() == L"Access-Control-Allow-Headers") {
      auto value = wstring{header.Value().c_str()};

      // TODO: Avoid redundant comparison.
      auto parsed = set<wstring, decltype(ciStrCmp)>{
          wsregex_token_iterator{value.cbegin(), value.cend(), rgx, -1}, wsregex_token_iterator{}, ciStrCmp};
      allowedHeaders.insert(parsed.cbegin(), parsed.cend());
    } else if (header.Key() == L"Access-Control-Allow-Methods") {
      auto value = wstring{header.Value().c_str()};

      // TODO: Avoid redundant comparison.
      auto parsed = set<wstring, decltype(ciStrCmp)>{
          wsregex_token_iterator{value.cbegin(), value.cend(), rgx, -1}, wsregex_token_iterator{}, ciStrCmp};
      allowedMethods.insert(parsed.cbegin(), parsed.cend());
    } else if (header.Key() == L"Access-Control-Allow-Origin")
      allowedOrigin = header.Value();
    else if (header.Key() == L"Access-Control-Expose-Headers") {
      auto value = wstring{header.Value().c_str()};

      // TODO: Avoid redundant comparison.
      auto parsed = set<wstring, decltype(ciStrCmp)>{
          wsregex_token_iterator{value.cbegin(), value.cend(), rgx, -1}, wsregex_token_iterator{}, ciStrCmp};
      exposedHeaders.insert(parsed.cbegin(), parsed.cend());
    } else if (header.Key() == L"Access-Control-Allow-Credentials")
      allowedCredentials = header.Value();
    else if (header.Key() == L"Access-Control-Max-Age")
      maxAge = _wtoi(header.Value().c_str());
  }

  auto iRequestArgs = request.Properties().Lookup(L"RequestArgs");
  // Check if the origin is allowed in conjuction with the withCredentials flag
  // CORS preflight should always exclude credentials although the subsequent CORS request may include credentials.
  // if (!CheckAccessStatic(allowedOrigin, allowCredentials, GetOrigin(), withCredentials, /*out*/ errorText))
  ValidateAllowOrigin(allowedOrigin, allowedCredentials, iRequestArgs);

  // Check if the request method is allowed
  // if (!IsCrossOriginRequestMethodAllowed(requestMethod, allowedMethodsList, withCredentials, /*out*/ errorText))
  bool withCredentials = winrt::get_self<RequestArgs, IInspectable>(iRequestArgs)->WithCredentials;
  bool requestMethodAllowed = false;
  for (const auto &method : allowedMethods) {
    if (L"*" == method) {
      if (!withCredentials) {
        requestMethodAllowed = true;
        break;
      }
    } else if (boost::iequals(method, request.Method().ToString())) {
      requestMethodAllowed = true;
      break;
    }
  }
  if (!requestMethodAllowed)
    throw hresult_error{
        E_INVALIDARG,
        L"Method [" + request.Method().ToString() +
            L"] is not allowed by Access-Control-Allow-Methods in preflight response.\\n"};

  // Check if request headers are allowed
  // if (!IsCrossOriginRequestHeadersAllowed(requestHeaders, allowedHeadersList, withCredentials, /*out*/ errorText))
  // See https://fetch.spec.whatwg.org/#cors-preflight-fetch, section 4.8.7.6-7
  // Check if the header should be allowed through wildcard, if the request does not have credentials.
  bool requestHeadersAllowed = false;
  if (!withCredentials && allowedHeaders.find(L"*") != allowedHeaders.cend()) {
    // "Authorization" header cannot be allowed through wildcard alone.
    // "Authorization" is the only member of https://fetch.spec.whatwg.org/#cors-non-wildcard-request-header-name.
    if (request.Headers().HasKey(L"Authorization") && allowedHeaders.find(L"Authorization") == allowedHeaders.cend())
      throw hresult_error{
          E_INVALIDARG,
          L"Request header field [Authorization] is not allowed by Access-Control-Allow-Headers in preflight response.\\n"};

    requestHeadersAllowed = true;
  }
  if (!requestHeadersAllowed) {
    // Forbidden headers are excluded from the JavaScript layer.
    // User agents may use these headers internally.
    // TODO: CorsUnsafeNotForbiddenRequestHeaderNames(requestHeaders);
    const set unsafeNotForbidenHeaderNames = CorsUnsafeNotForbiddenRequestHeaderNames(request.Headers());
    for (const auto name : unsafeNotForbidenHeaderNames) {
      if (allowedHeaders.find(name) == allowedHeaders.cend())
        throw hresult_error{
            E_INVALIDARG,
            L"Request header field [" + to_hstring(name) +
                L"]is not allowed by Access-Control-Allow-Headers in preflight response.\\n"};
    }
  }

  // #9770 - insert into preflight cache
}

ResponseType OriginPolicyHttpFilter::SendPreflightAsync(HttpRequestMessage const &request) const { // TODO: const& ??
  // TODO: Inject user agent?

  auto coRequest = request;

  HttpRequestMessage preflightRequest;

  // Section 4.8.2 https://fetch.spec.whatwg.org/#cors-preflight-fetch
  preflightRequest.Method(HttpMethod::Options());
  preflightRequest.RequestUri(m_origin);
  preflightRequest.Headers().Insert(L"Accept", L"*/*");
  preflightRequest.Headers().Insert(L"Access-Control-Request-Method", coRequest.Method().ToString());

  auto headerNames = wstring{};
  auto headerItr = coRequest.Headers().begin();
  if (headerItr != coRequest.Headers().end()) {
    headerNames += (*headerItr).Key();

    while (++headerItr != coRequest.Headers().end())
      headerNames += L", " + (*headerItr).Key();
  }
  if (coRequest.Content()) {
    headerItr = coRequest.Content().Headers().begin();
    if (headerItr != coRequest.Content().Headers().end()) {
      headerNames += (*headerItr).Key();

      while (++headerItr != coRequest.Content().Headers().end())
        headerNames += L", " + (*headerItr).Key();
    }
  }

  preflightRequest.Headers().Insert(L"Access-Control-Request-Headers", headerNames);
  preflightRequest.Headers().Insert(L"Origin", m_origin.AbsoluteCanonicalUri());
  preflightRequest.Headers().Insert(L"Sec-Fetch-Mode", L"CORS");

  co_return {co_await m_innerFilter.SendRequestAsync(preflightRequest)};
}

#pragma region IHttpFilter

ResponseType OriginPolicyHttpFilter::SendRequestAsync(HttpRequestMessage const &request) {
  auto coRequest = request;

  // Allow only HTTP or HTTPS schemes
  if (GetRuntimeOptionBool("Http.StrictScheme") && coRequest.RequestUri().SchemeName() != L"https" &&
      coRequest.RequestUri().SchemeName() != L"http")
    throw hresult_error{E_INVALIDARG, L"Invalid URL scheme: [" + m_origin.SchemeName() + L"]"};

  // Ensure absolute URL
  coRequest.RequestUri(Uri{coRequest.RequestUri().AbsoluteCanonicalUri()});

  // If fetch is in CORS mode, ValidateSecurityOnRequest() determines if it is a simple request by inspecting origin,
  // header, and method
  ValidateRequest(coRequest);

  if (m_originPolicy == OriginPolicy::SimpleCrossOriginResourceSharing ||
      m_originPolicy == OriginPolicy::CrossOriginResourceSharing) {
    if (coRequest.RequestUri().UserName().size() > 0 || coRequest.RequestUri().Password().size() > 0) {
      coRequest.RequestUri(Uri{coRequest.RequestUri().DisplayUri()});
    }
  }

  try {
    // #9770 - Validate preflight cache
    if (m_originPolicy == OriginPolicy::CrossOriginResourceSharing) {
      auto preflightResponse = co_await SendPreflightAsync(coRequest);

      // See 10.7.4 of https://fetch.spec.whatwg.org/#http-network-or-cache-fetch
      ValidatePreflightResponse(coRequest, preflightResponse);
    }
  } catch (const std::exception &e) {
    throw hresult_error{E_FAIL, to_hstring(e.what())};
  } catch (...) {
    throw hresult_error{E_FAIL, L"Unspecified error processing Origin Policy request"};
  }

  co_return {co_await m_innerFilter.SendRequestAsync(coRequest)};
}

#pragma endregion IHttpFilter

#pragma endregion OriginPolicyHttpFilter

} // namespace Microsoft::React::Networking
