// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "OriginPolicyHttpFilter.h"

// React Native Windows
#include <CppRuntimeOptions.h>
#include "WinRTTypes.h"

// Boost Library
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast/try_lexical_convert.hpp>

// Standard Library
#include <queue>
#include <regex>

using std::set;
using std::wstring;

using winrt::hresult_error;
using winrt::hstring;
using winrt::to_hstring;
using winrt::Windows::Foundation::IInspectable;
using winrt::Windows::Foundation::IPropertyValue;
using winrt::Windows::Foundation::Uri;
using winrt::Windows::Foundation::Collections::IMap;
using winrt::Windows::Web::Http::HttpMethod;
using winrt::Windows::Web::Http::HttpRequestMessage;
using winrt::Windows::Web::Http::HttpResponseMessage;
using winrt::Windows::Web::Http::Filters::IHttpBaseProtocolFilter;
using winrt::Windows::Web::Http::Filters::IHttpFilter;
using winrt::Windows::Web::Http::Headers::HttpMediaTypeHeaderValue;
using winrt::Windows::Web::Http::Headers::HttpRequestHeaderCollection;

namespace Microsoft::React::Networking {

#pragma region OriginPolicyHttpFilter

#pragma region ConstWcharComparer

bool OriginPolicyHttpFilter::ConstWcharComparer::operator()(const wchar_t *a, const wchar_t *b) const {
  return _wcsicmp(a, b) < 0;
}

#pragma endregion ConstWcharComparer

// https://fetch.spec.whatwg.org/#forbidden-method
/*static*/ set<const wchar_t *, OriginPolicyHttpFilter::ConstWcharComparer> OriginPolicyHttpFilter::s_forbiddenMethods =
    {L"CONNECT", L"TRACE", L"TRACK"};

/*static*/ set<const wchar_t *, OriginPolicyHttpFilter::ConstWcharComparer>
    OriginPolicyHttpFilter::s_simpleCorsMethods = {L"GET", L"HEAD", L"POST"};

/*static*/ set<const wchar_t *, OriginPolicyHttpFilter::ConstWcharComparer>
    OriginPolicyHttpFilter::s_simpleCorsRequestHeaderNames = {
        L"Accept",
        L"Accept-Language",
        L"Content-Language",
        L"Content-Type",
        L"DPR",
        L"Downlink",
        L"Save-Data",
        L"Viewport-Width",
        L"Width"};

/*static*/ set<const wchar_t *, OriginPolicyHttpFilter::ConstWcharComparer>
    OriginPolicyHttpFilter::s_simpleCorsResponseHeaderNames =
        {L"Cache-Control", L"Content-Language", L"Content-Type", L"Expires", L"Last-Modified", L"Pragma"};

/*static*/ set<const wchar_t *, OriginPolicyHttpFilter::ConstWcharComparer>
    OriginPolicyHttpFilter::s_simpleCorsContentTypeValues = {
        L"application/x-www-form-urlencoded",
        L"multipart/form-data",
        L"text/plain"};

// https://fetch.spec.whatwg.org/#forbidden-header-name
// Chromium still bans "User-Agent" due to https://crbug.com/571722
/*static*/ set<const wchar_t *, OriginPolicyHttpFilter::ConstWcharComparer>
    OriginPolicyHttpFilter::s_corsForbiddenRequestHeaderNames = {
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

/*static*/ set<const wchar_t *, OriginPolicyHttpFilter::ConstWcharComparer>
    OriginPolicyHttpFilter::s_cookieSettingResponseHeaders = {
        L"Set-Cookie",
        L"Set-Cookie2", // Deprecated by the spec, but probably still used
};

/*static*/ set<const wchar_t *, OriginPolicyHttpFilter::ConstWcharComparer>
    OriginPolicyHttpFilter::s_corsForbiddenRequestHeaderNamePrefixes = {L"Proxy-", L"Sec-"};

/*static*/ Uri OriginPolicyHttpFilter::s_origin{nullptr};

/*static*/ void OriginPolicyHttpFilter::SetStaticOrigin(std::string &&url) {
  if (!url.empty())
    s_origin = Uri{to_hstring(url)};
  else
    s_origin = nullptr;
}

/*static*/ bool OriginPolicyHttpFilter::IsSameOrigin(Uri const &u1, Uri const &u2) noexcept {
  return (u1 && u2) && u1.SchemeName() == u2.SchemeName() && u1.Host() == u2.Host() && u1.Port() == u2.Port();
}

/*static*/ bool OriginPolicyHttpFilter::IsSimpleCorsRequest(HttpRequestMessage const &request) noexcept {
  // Ensure header is in Simple CORS allowlist
  for (const auto &header : request.Headers()) {
    if (s_simpleCorsRequestHeaderNames.find(header.Key().c_str()) == s_simpleCorsRequestHeaderNames.cend())
      return false;

    // Ensure Content-Type value is in Simple CORS allowlist, if present
    if (boost::iequals(header.Key(), L"Content-Type")) {
      if (s_simpleCorsContentTypeValues.find(header.Value().c_str()) != s_simpleCorsContentTypeValues.cend())
        return false;
    }
  }

  // WinRT separates request headers from request content headers
  if (auto content = request.Content()) {
    for (const auto &header : content.Headers()) {
      // WinRT automatically appends non-allowlisted header Content-Length when Content-Type is set. Skip it.
      if (s_simpleCorsRequestHeaderNames.find(header.Key().c_str()) == s_simpleCorsRequestHeaderNames.cend() &&
          !boost::iequals(header.Key(), "Content-Length"))
        return false;

      // Ensure Content-Type value is in Simple CORS allowlist, if present
      if (boost::iequals(header.Key(), L"Content-Type")) {
        if (s_simpleCorsContentTypeValues.find(header.Value().c_str()) == s_simpleCorsContentTypeValues.cend())
          return false;
      }
    }
  }

  // Ensure method is in Simple CORS allowlist
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
      if (boost::istarts_with(header.Key(), prefix))
        return false;
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

/*static*/ OriginPolicyHttpFilter::AccessControlValues OriginPolicyHttpFilter::ExtractAccessControlValues(
    winrt::Windows::Foundation::Collections::IMap<hstring, hstring> const &headers) {
  using std::wregex;
  using std::wsregex_token_iterator;

  // https://tools.ietf.org/html/rfc2616#section-4.2
  wregex rgx{L"\\s*,\\s*"};
  AccessControlValues result;

  auto ciStrCmp = [](const wstring &a, const wstring &b) { return _wcsicmp(a.c_str(), b.c_str()) < 0; };
  set<wstring, decltype(ciStrCmp)> allowedHeaders{ciStrCmp};
  set<wstring, decltype(ciStrCmp)> allowedMethods{ciStrCmp};
  set<wstring, decltype(ciStrCmp)> exposedHeaders{ciStrCmp};

  for (const auto &header : headers) {
    if (boost::iequals(header.Key(), L"Access-Control-Allow-Headers")) {
      auto value = wstring{header.Value().c_str()};

      // TODO: Avoid redundant comparison.
      auto parsed = set<wstring, decltype(ciStrCmp)>{
          wsregex_token_iterator{value.cbegin(), value.cend(), rgx, -1}, wsregex_token_iterator{}, ciStrCmp};
      allowedHeaders.insert(parsed.cbegin(), parsed.cend());
      result.AllowedHeaders.insert(allowedHeaders.cbegin(), allowedHeaders.cend());
    } else if (boost::iequals(header.Key(), L"Access-Control-Allow-Methods")) {
      auto value = wstring{header.Value().c_str()};

      // TODO: Avoid redundant comparison.
      auto parsed = set<wstring, decltype(ciStrCmp)>{
          wsregex_token_iterator{value.cbegin(), value.cend(), rgx, -1}, wsregex_token_iterator{}, ciStrCmp};
      allowedMethods.insert(parsed.cbegin(), parsed.cend());
      result.AllowedMethods.insert(allowedMethods.cbegin(), allowedMethods.cend());
    } else if (boost::iequals(header.Key(), L"Access-Control-Allow-Origin")) {
      result.AllowedOrigin = header.Value();
    } else if (boost::iequals(header.Key(), L"Access-Control-Expose-Headers")) {
      auto value = wstring{header.Value().c_str()};

      // TODO: Avoid redundant comparison.
      auto parsed = set<wstring, decltype(ciStrCmp)>{
          wsregex_token_iterator{value.cbegin(), value.cend(), rgx, -1}, wsregex_token_iterator{}, ciStrCmp};
      exposedHeaders.insert(parsed.cbegin(), parsed.cend());
      result.ExposedHeaders.insert(exposedHeaders.cbegin(), exposedHeaders.cend());
    } else if (boost::iequals(header.Key(), L"Access-Control-Allow-Credentials")) {
      result.AllowedCredentials = header.Value();
    } else if (boost::iequals(header.Key(), L"Access-Control-Max-Age")) {
      result.MaxAge = _wtoi(header.Value().c_str());
    }
  }

  return result;
} // ExtractAccessControlValues

/*static*/ void OriginPolicyHttpFilter::RemoveHttpOnlyCookiesFromResponseHeaders(
    HttpResponseMessage const &response,
    bool removeAll) {
  // Example: "Set-Cookie", L"id=a3fWa; Expires=Wed, 21 Oct 2020 07:28:00 GMT;  HttpOnly"
  std::queue<hstring> httpOnlyCookies;
  for (const auto &header : response.Headers()) {
    if (s_cookieSettingResponseHeaders.find(header.Key().c_str()) == s_cookieSettingResponseHeaders.cend())
      continue;

    if (removeAll) {
      httpOnlyCookies.push(header.Key());
      continue;
    }

    // Anchors (^$) can't be part of bracket expressions ([]).
    // Create 3 matching groups: 1. Beginning of string 2. Between delimiters 3. End of string
    const std::wregex re(L"(^HttpOnly\\s*;)|(;\\s*HttpOnly\\s*;)|(;\\s*HttpOnly$)", std::regex_constants::icase);
    if (!std::regex_search(header.Value().c_str(), re))
      continue;

    // HttpOnly cookie detected. Removing.
    httpOnlyCookies.push(header.Key());
  } // const auto &header : response.Headers()

  while (!httpOnlyCookies.empty()) {
    response.Headers().Remove(httpOnlyCookies.front());
    httpOnlyCookies.pop();
  }
}

OriginPolicyHttpFilter::OriginPolicyHttpFilter(IHttpFilter const &innerFilter) : m_innerFilter{innerFilter} {}

OriginPolicyHttpFilter::OriginPolicyHttpFilter()
    : OriginPolicyHttpFilter(winrt::Windows::Web::Http::Filters::HttpBaseProtocolFilter{}) {}

OriginPolicy OriginPolicyHttpFilter::ValidateRequest(HttpRequestMessage const &request) {
  auto effectiveOriginPolicy =
      static_cast<OriginPolicy>(request.Properties().Lookup(L"OriginPolicy").as<IPropertyValue>().GetUInt64());
  switch (effectiveOriginPolicy) {
    case OriginPolicy::None:
      return effectiveOriginPolicy;

    case OriginPolicy::SameOrigin:
      if (!IsSameOrigin(s_origin, request.RequestUri()))
        throw hresult_error{E_INVALIDARG, L"SOP (same-origin policy) is enforced"};
      break;

    case OriginPolicy::SimpleCrossOriginResourceSharing:
      // Check for disallowed mixed content
      if (GetRuntimeOptionBool("Http.BlockMixedContentSimpleCors") &&
          s_origin.SchemeName() != request.RequestUri().SchemeName())
        throw hresult_error{E_INVALIDARG, L"The origin and request URLs must have the same scheme"};

      if (IsSameOrigin(s_origin, request.RequestUri()))
        // Same origin. Therefore, skip Cross-Origin handling.
        effectiveOriginPolicy = OriginPolicy::SameOrigin;
      else if (!IsSimpleCorsRequest(request))
        throw hresult_error{
            E_INVALIDARG,
            L"The request does not meet the requirements for Same-Origin policy or Simple Cross-Origin resource sharing"};
      break;

    case OriginPolicy::CrossOriginResourceSharing:
      // https://developer.mozilla.org/en-US/docs/Web/HTTP/CORS#simple_requests
      // Refer to CorsURLLoaderFactory::IsValidRequest in chrome\src\services\network\cors\cors_url_loader_factory.cc.
      // Forbidden headers should be blocked regardless of origins.
      // Example: On the Edge browser, an XHR request with the "Host" header set gets rejected as unsafe.
      // https://fetch.spec.whatwg.org/#forbidden-header-name

      if (s_origin.SchemeName() != request.RequestUri().SchemeName())
        throw hresult_error{E_INVALIDARG, L"The origin and request URLs must have the same scheme"};

      if (!AreSafeRequestHeaders(request.Headers()))
        throw hresult_error{E_INVALIDARG, L"Request header not allowed in cross-origin resource sharing"};

      if (s_forbiddenMethods.find(request.Method().ToString().c_str()) != s_forbiddenMethods.cend())
        throw hresult_error{E_INVALIDARG, L"Request method not allowed in cross-origin resource sharing"};

      if (IsSameOrigin(s_origin, request.RequestUri()))
        effectiveOriginPolicy = OriginPolicy::SameOrigin;
      else if (IsSimpleCorsRequest(request))
        effectiveOriginPolicy = OriginPolicy::SimpleCrossOriginResourceSharing;
      else
        effectiveOriginPolicy = OriginPolicy::CrossOriginResourceSharing;

      break;

    default:
      throw hresult_error{
          E_INVALIDARG, L"Invalid OriginPolicy type: " + to_hstring(static_cast<size_t>(effectiveOriginPolicy))};
  }

  return effectiveOriginPolicy;
}

// See https://fetch.spec.whatwg.org/#cors-check
void OriginPolicyHttpFilter::ValidateAllowOrigin(
    hstring const &allowedOrigin,
    hstring const &allowCredentials,
    IMap<hstring, IInspectable> props) const {
  // 4.10.1-2 - null allow origin
  if (L"null" == allowedOrigin)
    throw hresult_error{
        E_INVALIDARG,
        L"Response header Access-Control-Allow-Origin has a value of [null] which differs from the supplied origin"};

  bool withCredentials = props.Lookup(L"RequestArgs").as<RequestArgs>()->WithCredentials;
  // 4.10.3 - valid wild card allow origin
  if (!withCredentials && L"*" == allowedOrigin)
    return;

  // We assume the source (request) origin is not "*", "null", or empty string. Valid URI is expected
  // 4.10.4 - Mismatched allow origin
  auto taintedOriginProp = props.TryLookup(L"TaintedOrigin");
  auto taintedOrigin = taintedOriginProp && winrt::unbox_value<bool>(taintedOriginProp);
  auto origin = taintedOrigin ? nullptr : s_origin;
  if (allowedOrigin.empty() || !IsSameOrigin(origin, Uri{allowedOrigin})) {
    hstring errorMessage;
    if (allowedOrigin.empty())
      errorMessage = L"No valid origin in response";

    // See https://fetch.spec.whatwg.org/#http-access-control-allow-origin.
    else if (boost::contains(allowedOrigin, L" ,"))
      errorMessage = L"Response header Access-Control-Allow-Origin can not have multiple values";

    // A wildcard Access-Control-Allow-Origin can not be used if credentials are to be sent,
    // even with Access-Control-Allow-Credentials set to true
    // See https://fetch.spec.whatwg.org/#cors-protocol-and-credentials
    else if (L"*" == allowedOrigin)
      errorMessage =
          L"Response header Access-Control-Allow-Origin can not have a wildcard value when the request includes credentials";

    else
      errorMessage = L"The Access-Control-Allow-Origin header has a value of [" + allowedOrigin +
          L"] which differs from the supplied origin";

    throw hresult_error{E_INVALIDARG, errorMessage};
  }

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
        L"Access-Control-Allow-Credentials value must be \"true\" when the response includes credentials"};
};

void OriginPolicyHttpFilter::ValidatePreflightResponse(
    HttpRequestMessage const &request,
    HttpResponseMessage const &response) const {
  // https://developer.mozilla.org/en-US/docs/Web/HTTP/CORS/Errors/CORSExternalRedirectNotAllowed
  using winrt::Windows::Web::Http::HttpStatusCode;
  switch (response.StatusCode()) {
    case HttpStatusCode::MovedPermanently:
    case HttpStatusCode::TemporaryRedirect:
    case HttpStatusCode::PermanentRedirect:
      throw hresult_error{INET_E_REDIRECTING, L"Redirect is not allowed in a preflight request"};

    default:
      break;
  }

  auto controlValues = ExtractAccessControlValues(response.Headers());

  auto props = request.Properties();
  // Check if the origin is allowed in conjuction with the withCredentials flag
  // CORS preflight should always exclude credentials although the subsequent CORS request may include credentials.
  ValidateAllowOrigin(controlValues.AllowedOrigin, controlValues.AllowedCredentials, props);

  // See https://fetch.spec.whatwg.org/#cors-preflight-fetch, section 4.8.7.5
  // Check if the request method is allowed
  bool withCredentials = props.Lookup(L"RequestArgs").as<RequestArgs>()->WithCredentials;
  bool requestMethodAllowed = false;
  for (const auto &method : controlValues.AllowedMethods) {
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

  // Preflight should always allow simple CORS methods
  requestMethodAllowed |= s_simpleCorsMethods.find(request.Method().ToString().c_str()) != s_simpleCorsMethods.cend();

  if (!requestMethodAllowed)
    throw hresult_error{
        E_INVALIDARG,
        L"Method [" + request.Method().ToString() +
            L"] is not allowed by Access-Control-Allow-Methods in preflight response"};

  // Check if request headers are allowed
  // See https://fetch.spec.whatwg.org/#cors-preflight-fetch, section 4.8.7.6-7
  // Check if the header should be allowed through wildcard, if the request does not have credentials.
  bool requestHeadersAllowed = false;
  if (!withCredentials && controlValues.AllowedHeaders.find(L"*") != controlValues.AllowedHeaders.cend()) {
    // "Authorization" header cannot be allowed through wildcard alone.
    // "Authorization" is the only member of https://fetch.spec.whatwg.org/#cors-non-wildcard-request-header-name.
    if (request.Headers().HasKey(L"Authorization") &&
        controlValues.AllowedHeaders.find(L"Authorization") == controlValues.AllowedHeaders.cend())
      throw hresult_error{
          E_INVALIDARG,
          L"Request header field [Authorization] is not allowed by Access-Control-Allow-Headers in preflight response"};

    requestHeadersAllowed = true;
  }
  if (!requestHeadersAllowed) {
    // Forbidden headers are excluded from the JavaScript layer.
    // User agents may use these headers internally.
    const set unsafeNotForbidenHeaderNames = CorsUnsafeNotForbiddenRequestHeaderNames(request.Headers());
    for (const auto name : unsafeNotForbidenHeaderNames) {
      if (controlValues.AllowedHeaders.find(name) == controlValues.AllowedHeaders.cend())
        throw hresult_error{
            E_INVALIDARG,
            L"Request header field [" + to_hstring(name) +
                L"] is not allowed by Access-Control-Allow-Headers in preflight response"};
    }
  }

  // #9770 - insert into preflight cache
}

// See 10.7.4 of https://fetch.spec.whatwg.org/#http-network-or-cache-fetch
void OriginPolicyHttpFilter::ValidateResponse(HttpResponseMessage const &response, const OriginPolicy originPolicy)
    const {
  bool removeAllCookies = false;
  if (originPolicy == OriginPolicy::SimpleCrossOriginResourceSharing ||
      originPolicy == OriginPolicy::CrossOriginResourceSharing) {
    auto controlValues = ExtractAccessControlValues(response.Headers());
    auto props = response.RequestMessage().Properties();
    auto withCredentials = props.Lookup(L"RequestArgs").try_as<RequestArgs>()->WithCredentials;

    if (GetRuntimeOptionBool("Http.StrictOriginCheckSimpleCors") &&
        originPolicy == OriginPolicy::SimpleCrossOriginResourceSharing) {
      bool originAllowed = false;
      for (const auto &header : response.Headers()) {
        if (boost::iequals(header.Key(), L"Access-Control-Allow-Origin")) {
          originAllowed |= L"*" == header.Value() || s_origin == Uri{header.Value()};
        }
      }

      if (!originAllowed) {
        throw hresult_error{E_INVALIDARG, L"The server does not support CORS or the origin is not allowed"};
      }
    } else {
      ValidateAllowOrigin(controlValues.AllowedOrigin, controlValues.AllowedCredentials, props);
    }

    if (originPolicy == OriginPolicy::SimpleCrossOriginResourceSharing) {
      // Filter out response headers that are not in the Simple CORS allowlist
      std::queue<hstring> nonSimpleNames;
      for (const auto &header : response.Headers().GetView()) {
        if (s_simpleCorsResponseHeaderNames.find(header.Key().c_str()) == s_simpleCorsResponseHeaderNames.cend())
          nonSimpleNames.push(header.Key());
      }

      while (!nonSimpleNames.empty()) {
        response.Headers().Remove(nonSimpleNames.front());
        nonSimpleNames.pop();
      }
    } else {
      // Filter out response headers that are not simple headers and not in expose list

      // Keep simple headers and those found in the expose header list.
      if (withCredentials || controlValues.ExposedHeaders.find(L"*") == controlValues.ExposedHeaders.cend()) {
        std::queue<hstring> nonSimpleNonExposedHeaders;

        for (const auto &header : response.Headers().GetView()) {
          if (s_simpleCorsResponseHeaderNames.find(header.Key().c_str()) == s_simpleCorsResponseHeaderNames.cend() &&
              controlValues.ExposedHeaders.find(header.Key().c_str()) == controlValues.ExposedHeaders.cend()) {
            nonSimpleNonExposedHeaders.push(header.Key());
          }
        }

        while (!nonSimpleNonExposedHeaders.empty()) {
          response.Headers().Remove(nonSimpleNonExposedHeaders.front());
          nonSimpleNonExposedHeaders.pop();
        }
      }
    }

    // When withCredentials is false, request cannot include cookies. Also, cookies will be ignored in responses.
    removeAllCookies = !withCredentials && GetRuntimeOptionBool("Http.RemoveCookiesFromResponse");
  } // originPolicy == SimpleCrossOriginResourceSharing || CrossOriginResourceSharing

  // Don't expose HttpOnly cookies to JavaScript
  RemoveHttpOnlyCookiesFromResponseHeaders(response, removeAllCookies);
}

ResponseOperation OriginPolicyHttpFilter::SendPreflightAsync(HttpRequestMessage const &request) const {
  auto coRequest = request;

  HttpRequestMessage preflightRequest;

  // Section 4.8.2 https://fetch.spec.whatwg.org/#cors-preflight-fetch
  preflightRequest.Method(HttpMethod::Options());
  preflightRequest.RequestUri(coRequest.RequestUri());
  preflightRequest.Headers().Insert(L"Accept", L"*/*");
  preflightRequest.Headers().Insert(L"Access-Control-Request-Method", coRequest.Method().ToString());

  auto headerNames = wstring{};
  auto writeSeparator = false;
  for (const auto &header : coRequest.Headers()) {
    if (writeSeparator) {
      headerNames += L", ";
    } else {
      writeSeparator = true;
    }

    headerNames += header.Key();
  }

  if (coRequest.Content()) {
    for (const auto &header : coRequest.Content().Headers()) {
      if (writeSeparator) {
        headerNames += L", ";
      } else {
        writeSeparator = true;
      }

      headerNames += header.Key();
    }
  }

  preflightRequest.Headers().Insert(L"Access-Control-Request-Headers", headerNames);
  preflightRequest.Headers().Insert(L"Origin", s_origin.AbsoluteCanonicalUri());
  preflightRequest.Headers().Insert(L"Sec-Fetch-Mode", L"CORS");

  co_return {co_await m_innerFilter.SendRequestAsync(preflightRequest)};
}

#pragma region IRedirectEventSource

bool OriginPolicyHttpFilter::OnRedirecting(
    HttpRequestMessage const &request,
    HttpResponseMessage const &response) noexcept {
  // Consider the following scenario.
  // User signs in to http://a.com and visits a page that makes CORS request to http://b.com with origin=http://a.com.
  // Http://b.com reponds with a redirect to http://a.com. The browser follows the redirect to http://a.com with
  // origin=http://a.com. Since the origin matches the URL, the request is authorized at http://a.com, but it actually
  // allows http://b.com to bypass the CORS check at http://a.com since the redirected URL is from http://b.com.
  if (!IsSameOrigin(response.Headers().Location(), request.RequestUri()) &&
      !IsSameOrigin(s_origin, request.RequestUri())) {
    // By masking the origin field in the request header, we make it impossible for the server to set a single value for
    // the access-control-allow-origin header. It means, the only way to support redirect is that server allows access
    // from all sites through wildcard.
    request.Headers().Insert(L"Origin", L"null");

    auto props = request.Properties();
    // Look for 'RequestArgs' key to ensure we are redirecting the main request.
    if (auto iReqArgs = props.TryLookup(L"RequestArgs")) {
      props.Insert(L"TaintedOrigin", winrt::box_value(true));
    } else {
      // Abort redirection if the request is either preflight or extraneous.
      return false;
    }
  }

  return true;
}

#pragma endregion IRedirectEventSource

#pragma region IHttpFilter

ResponseOperation OriginPolicyHttpFilter::SendRequestAsync(HttpRequestMessage const &request) {
  auto coRequest = request;

  // Set initial origin policy to global runtime option.
  request.Properties().Insert(L"OriginPolicy", winrt::box_value(GetRuntimeOptionInt("Http.OriginPolicy")));

  // Allow only HTTP or HTTPS schemes
  if (GetRuntimeOptionBool("Http.StrictScheme") && coRequest.RequestUri().SchemeName() != L"https" &&
      coRequest.RequestUri().SchemeName() != L"http")
    throw hresult_error{E_INVALIDARG, L"Invalid URL scheme: [" + s_origin.SchemeName() + L"]"};

  if (!GetRuntimeOptionBool("Http.OmitCredentials")) {
    coRequest.Properties().Lookup(L"RequestArgs").as<RequestArgs>()->WithCredentials = false;
  }

  // Ensure absolute URL
  coRequest.RequestUri(Uri{coRequest.RequestUri().AbsoluteCanonicalUri()});

  auto originPolicy = ValidateRequest(coRequest);
  if (originPolicy == OriginPolicy::SimpleCrossOriginResourceSharing ||
      originPolicy == OriginPolicy::CrossOriginResourceSharing) {
    if (coRequest.RequestUri().UserName().size() > 0 || coRequest.RequestUri().Password().size() > 0) {
      coRequest.RequestUri(Uri{coRequest.RequestUri().DisplayUri()});
    }
  }

  try {
    // #9770 - Validate preflight cache
    if (originPolicy == OriginPolicy::CrossOriginResourceSharing) {
      // If inner filter can AllowRedirect, disable for preflight.
      winrt::impl::com_ref<IHttpBaseProtocolFilter> baseFilter;
      if (baseFilter = m_innerFilter.try_as<IHttpBaseProtocolFilter>()) {
        baseFilter.AllowAutoRedirect(false);
      }

      auto preflightResponse = co_await SendPreflightAsync(coRequest);

      if (baseFilter) {
        baseFilter.AllowAutoRedirect(true);
      }

      ValidatePreflightResponse(coRequest, preflightResponse);
    }

    if (originPolicy == OriginPolicy::SimpleCrossOriginResourceSharing ||
        originPolicy == OriginPolicy::CrossOriginResourceSharing) {
      coRequest.Headers().Insert(L"Origin", s_origin.AbsoluteCanonicalUri());
    }

    auto response = co_await m_innerFilter.SendRequestAsync(coRequest);

    ValidateResponse(response, originPolicy);

    co_return response;

  } catch (hresult_error const &e) {
    throw e;
  } catch (const std::exception &e) {
    throw hresult_error{E_FAIL, to_hstring(e.what())};
  } catch (...) {
    throw hresult_error{E_FAIL, L"Unspecified error processing Origin Policy request"};
  }
}

#pragma endregion IHttpFilter

#pragma endregion OriginPolicyHttpFilter

} // namespace Microsoft::React::Networking
