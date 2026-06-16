// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include <CppUnitTest.h>

#include <CppRuntimeOptions.h>
#include <Networking/IRedirectEventSource.h>
#include <Networking/OriginPolicy.h>
#include <Networking/OriginPolicyHttpFilter.h>
#include <Networking/RedirectHttpFilter.h>
#include <Networking/WinRTTypes.h>
#include "WinRTNetworkingMocks.h"

// Boost Library
#include <boost/algorithm/string.hpp>

// Windows API
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Web.Http.Filters.h>
#include <winrt/Windows.Web.Http.Headers.h>
#include <winrt/Windows.Web.Http.h>

// Standard Library
#include <map>
#include <string>
#include <utility>
#include <vector>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace winrt::Windows::Web::Http;

using Microsoft::React::Networking::OriginPolicy;
using Microsoft::React::Networking::OriginPolicyHttpFilter;
using Microsoft::React::Networking::RedirectHttpFilter;
using Microsoft::React::Networking::RequestArgs;
using Microsoft::React::Networking::ResponseOperation;
using std::string;
using std::wstring;
using winrt::Windows::Foundation::Uri;
using winrt::Windows::Web::Http::Filters::IHttpFilter;
using winrt::Windows::Web::Http::Headers::HttpMediaTypeHeaderValue;

namespace {
// Clearly fake endpoint host names. These never reach the network (all requests are served by a
// mock filter), so they intentionally avoid "localhost" to prevent confusion with a real service.
constexpr wchar_t s_mockServerHost[]{L"mockserver.rnw"}; // Primary endpoint.
constexpr wchar_t s_redirectServerHost[]{L"redirserver.rnw"}; // Secondary endpoint (redirect target).

constexpr char s_crossOriginUrl[]{"http://example.rnw"}; // Narrow form, for runtime options.
constexpr wchar_t s_crossOriginUrlW[]{L"http://example.rnw"}; // Wide form, for header values.

// Common header field names (mirror the boost::beast::http::field values used by the original integration test).
constexpr wchar_t s_accessControlAllowCredentials[]{L"Access-Control-Allow-Credentials"};
constexpr wchar_t s_accessControlAllowHeaders[]{L"Access-Control-Allow-Headers"};
constexpr wchar_t s_accessControlAllowMethods[]{L"Access-Control-Allow-Methods"};
constexpr wchar_t s_accessControlAllowOrigin[]{L"Access-Control-Allow-Origin"};
constexpr wchar_t s_accessControlRequestHeaders[]{L"Access-Control-Request-Headers"};
constexpr wchar_t s_location[]{L"Location"};
constexpr wchar_t s_server[]{L"Server"};
} // namespace

namespace Microsoft::React::Test {

TEST_CLASS (OriginPolicyHttpFilterTest) {
  TEST_CLASS_INITIALIZE(Initialize) {
    winrt::uninit_apartment();
  }

  // TEMP tests to see if Uri has comparison capabilities
  TEST_METHOD(UrlsHaveSameOrigin) {
    // clang-format off
  Assert::IsTrue (OriginPolicyHttpFilter::IsSameOrigin(Uri{L"http://www.microsoft.com"     }, Uri{L"http://www.microsoft.com"       }));
  Assert::IsTrue (OriginPolicyHttpFilter::IsSameOrigin(Uri{L"http://www.microsoft.com"     }, Uri{L"http://www.microsoft.com/"      }));
  Assert::IsTrue (OriginPolicyHttpFilter::IsSameOrigin(Uri{L"http://www.microsoft.com"     }, Uri{L"http://www.microsoft.com//"     }));
  Assert::IsTrue (OriginPolicyHttpFilter::IsSameOrigin(Uri{L"http://www.microsoft.com:80"  }, Uri{L"http://www.microsoft.com:80"    }));
  Assert::IsTrue (OriginPolicyHttpFilter::IsSameOrigin(Uri{L"https://msw"                  }, Uri{L"https://MSW"                    }));
  Assert::IsTrue (OriginPolicyHttpFilter::IsSameOrigin(Uri{L"http://www.microsoft.com"     }, Uri{L"http://www.microsoft.com:80"    }));
  Assert::IsTrue (OriginPolicyHttpFilter::IsSameOrigin(Uri{L"https://www.microsoft.com"    }, Uri{L"https://www.microsoft.com:443"  }));
  Assert::IsFalse(OriginPolicyHttpFilter::IsSameOrigin(Uri{L"http://www.microsoft.com"     }, Uri{L"https://www.microsoft.com"      }));
  Assert::IsFalse(OriginPolicyHttpFilter::IsSameOrigin(Uri{L"http://www.microsoft.com"     }, Uri{L"http://www.microsoft.com:8080"  }));
  Assert::IsFalse(OriginPolicyHttpFilter::IsSameOrigin(Uri{L"http://www.microsoft.com"     }, Uri{L"http://www.microsoft.com:81"    }));
  Assert::IsFalse(OriginPolicyHttpFilter::IsSameOrigin(Uri{L"http://www.microsoft.com:80"  }, Uri{L"http://www.microsoft.com:81"    }));
  Assert::IsFalse(OriginPolicyHttpFilter::IsSameOrigin(Uri{L"file://testfolder"            }, Uri{L"http://testfolder"              }));
  Assert::IsFalse(OriginPolicyHttpFilter::IsSameOrigin(Uri{L"http://www.microsoft.com:443" }, Uri{L"https://www.microsoft.com:443"  }));
  Assert::IsFalse(OriginPolicyHttpFilter::IsSameOrigin(Uri{L"ftp://www.microsoft.com"      }, Uri{L"file://abc.com"                 }));
  Assert::IsFalse(OriginPolicyHttpFilter::IsSameOrigin(Uri{L"http://"                      }, Uri{L"http://www.microsoft.com"       }));
  // Assert::IsFalse(OriginPolicyHttpFilter::IsSameOrigin(Uri{L""                             }, Uri{L"http://www.microsoft.com:81"    }));
  // Assert::IsFalse(Uri{L"bar://abc.com"                }.Equals(Uri{L"abc.com"}));

  Assert::IsTrue (OriginPolicyHttpFilter::IsSameOrigin(Uri{L"http://www.microsoft.com"                }, Uri{L"HTTP://WWW.MICROSOFT.COM"}));
  Assert::IsTrue (OriginPolicyHttpFilter::IsSameOrigin(Uri{L"https://www.microsoft.com:801"           }, Uri{L"https://www.microsoft.com:801"}));
  Assert::IsTrue (OriginPolicyHttpFilter::IsSameOrigin(Uri{L"http://www.microsoft.com"                }, Uri{L"http://www.microsoft.com/app/test.html"}));
  Assert::IsTrue (OriginPolicyHttpFilter::IsSameOrigin(Uri{L"http://www.microsoft.com:801"            }, Uri{L"http://www.microsoft.com:801/app/test.html"}));
  //Assert::IsTrue (OriginPolicyHttpFilter::IsSameOrigin(Uri{L""                                        }, Uri{L"file://sharedfolder"}));
  Assert::IsTrue (OriginPolicyHttpFilter::IsSameOrigin(Uri{L"http://msw"                              }, Uri{L"http://msw/app/test.html"}));
  //Assert::IsTrue (OriginPolicyHttpFilter::IsSameOrigin(Uri{L""                                        }, Uri{L"xyz"}));
  //Assert::IsTrue (OriginPolicyHttpFilter::IsSameOrigin(Uri{L""                                        }, Uri{L"//bad"}));
  //Assert::IsTrue (OriginPolicyHttpFilter::IsSameOrigin(Uri{L""                                        }, Uri{L"x.&y.z"}));
  Assert::IsTrue (OriginPolicyHttpFilter::IsSameOrigin(Uri{L"http://www.microsoft.com:801"            }, Uri{L"http://www.microsoft.com:801\\test.html"}));
  Assert::IsTrue (OriginPolicyHttpFilter::IsSameOrigin(Uri{L"http://www.microsoft.com"                }, Uri{L"http://www.microsoft.com/%35test.html"}));
  Assert::IsTrue (OriginPolicyHttpFilter::IsSameOrigin(Uri{L"https://bing.com"                        }, Uri{L"https://foo:bar@bing.com/FakeResponse.ashx"}));
  Assert::IsTrue (OriginPolicyHttpFilter::IsSameOrigin(Uri{L"http://[2001:db8:1f70::999:de8:7648:6e8]"}, Uri{L"http://[2001:db8:1f70::999:de8:7648:6e8]/test.html"}));
  Assert::IsTrue (OriginPolicyHttpFilter::IsSameOrigin(Uri{L"https://\u65E5\u672C\u8A9E.com"          }, Uri{L"https://\u65E5\u672C\u8A9E.com/FakeResponse.ashx"}));
  Assert::IsTrue (OriginPolicyHttpFilter::IsSameOrigin(Uri{L"https://www.microsoft.com"               }, Uri{L"https://www.microsoft.com:443"}));
  Assert::IsFalse(OriginPolicyHttpFilter::IsSameOrigin(Uri{L"https://www.microsoft.com"               }, Uri{nullptr}));
  Assert::IsFalse(OriginPolicyHttpFilter::IsSameOrigin(Uri{nullptr                                    }, Uri{L"https://www.microsoft.com"}));
  Assert::IsFalse(OriginPolicyHttpFilter::IsSameOrigin(Uri{nullptr                                    }, Uri{nullptr}));
  Assert::IsFalse(OriginPolicyHttpFilter::IsSameOrigin(Uri{L"https://www.microsoft.com"               }, nullptr));
  Assert::IsFalse(OriginPolicyHttpFilter::IsSameOrigin(nullptr                                         , Uri{L"https://www.microsoft.com"}));
  Assert::IsFalse(OriginPolicyHttpFilter::IsSameOrigin(nullptr                                         , nullptr));
    // clang-format on
  }

  TEST_METHOD(ExcludeHttpOnlyCookies) {
    std::vector<std::map<std::wstring, std::wstring>> cases{
        {
            {L"Cache-Control", L"controlled"},
            {L"Set-Cookie", L"k=v; HttpOnly  ; k2=v2"},
        },

        {
            {L"k1", L"v1=w1; HttpOnly"},
            {L"Set-Cookie", L"id=a3fWa; Expires=Wed, 21 Oct 2020 07:28:00 GMT;  HttpOnly"},
            {L"k3", L"HttpOnly; v3=w3"},
        },

        {
            {L"k1", L"v1=w1; HttpOnly"},
            {L"Set-Cookie", L"id=a3fWa; Expires=Wed, 21 Oct 2020 07:28:00 GMT;  httponly"},
            {L"k3", L"HttpOnly; v3=w3"},
        },

        {
            {L"k1", L"v1=w1; HttpOnly"},
            {L"Set-Cookie", L"id=a3fWa; Expires=Wed, 21 Oct 2020 07:28:00 GMT; httpOnly;"},
            {L"k3", L"HttpOnly; v3=w3"},
        },

        {
            {L"k1", L"v1=w1; HttpOnly"},
            {L"Set-Cookie", L"HTTPONLY;id=a3fWa; Expires=Wed, 21 Oct 2020 07:28:00 GMT;"},
            {L"k3", L"HttpOnly; v3=w3"},
        },

        {
            {L"k1", L"v1=w1; HttpOnly"},
            {L"set-cookie", L"HttpOnly id=a3fWa; Expires=Wed, 21 Oct 2020 07:28:00 GMT;  HttpOnly"},
            {L"k3", L"HttpOnly; v3=w3"},
        },

        {
            {L"k1", L"v1=w1; HttpOnly"},
            {L"set-cookie2", L"HttpOnly id=a3fWa; Expires=Wed, 21 Oct 2020 07:28:00 GMT;  HttpOnly"},
            {L"k3", L"HttpOnly; v3=w3"},
        },
    };

    std::vector<std::map<std::wstring, std::wstring>> expected{
        {
            {L"Cache-Control", L"controlled"},
        },

        {
            {L"k1", L"v1=w1; HttpOnly"},
            {L"k3", L"HttpOnly; v3=w3"},
        },

        {
            {L"k1", L"v1=w1; HttpOnly"},
            {L"k3", L"HttpOnly; v3=w3"},
        },

        {
            {L"k1", L"v1=w1; HttpOnly"},
            {L"k3", L"HttpOnly; v3=w3"},
        },

        {
            {L"k1", L"v1=w1; HttpOnly"},
            {L"k3", L"HttpOnly; v3=w3"},
        },

        {
            {L"k1", L"v1=w1; HttpOnly"},
            {L"k3", L"HttpOnly; v3=w3"},
        },

        {
            {L"k1", L"v1=w1; HttpOnly"},
            {L"k3", L"HttpOnly; v3=w3"},
        },
    };

    for (size_t i = 0; i < cases.size(); ++i) {
      winrt::Windows::Web::Http::HttpResponseMessage response;
      for (const auto &header : cases[i]) {
        response.Headers().Insert(header.first, header.second);
      }

      winrt::Windows::Web::Http::HttpResponseMessage expectedResponse;
      auto expectedHeaders = expectedResponse.Headers();
      for (const auto &header : expected[i]) {
        expectedHeaders.Insert(header.first, header.second);
      }

      OriginPolicyHttpFilter::RemoveHttpOnlyCookiesFromResponseHeaders(response, false /*removeAll*/);

      // Set-Cookie field removed.
      Assert::AreEqual(expected[i].size(), static_cast<size_t>(response.Headers().Size()));
      bool equals = true;
      for (const auto &expectedHeader : expected[i]) {
        auto optHeader = response.Headers().TryLookup(expectedHeader.first);
        if (!optHeader.has_value() || optHeader.value() != expectedHeader.second) {
          equals = false;
          break;
        }
      }
      Assert::IsTrue(equals);
    }
  }

  TEST_METHOD(KeepNonHttpOnlyCookies) {
    std::vector<std::map<std::wstring, std::wstring>> cases{
        {
            {L"k1", L"v1"},
            {L"Set-Cookie", L"id=a3fWa; Expires=Wed, 21 Oct 2020 07:28:00 GMT HttpOnly;"},
            {L"k3", L"v3"},
        },

        {
            {L"k1", L"v1"},
            {L"Set-Cookie", L"HttpOnly=a3fWa; Expires=Wed, 21 Oct 2020 07:28:00 GMT ;"},
            {L"k3", L"v3"},
        },

        {
            {L"k1", L"v1"},
            {L"Set-Cookies", L"id=a3fWa; Expires=Wed, 21 Oct 2020 07:28:00 GMT;  HttpOnly"},
            {L"k3", L"v3"},
        },

        {
            {L"k1", L"v1"},
            {L"Set-Cookie", L"HttpOnly id=a3fWa; Expires=Wed, 21 Oct 2020 07:28:00 GMT"},
            {L"k3", L"v3"},
        },

        {
            {L"k1", L"v1"},
            {L"Set-Cookie2", L"HttpOnly id=a3fWa; Expires=Wed, 21 Oct 2020 07:28:00 GMT"},
            {L"k3", L"v3"},
        },
    };

    for (size_t i = 0; i < cases.size(); ++i) {
      winrt::Windows::Web::Http::HttpResponseMessage response;
      for (const auto &header : cases[i]) {
        response.Headers().Insert(header.first, header.second);
      }

      OriginPolicyHttpFilter::RemoveHttpOnlyCookiesFromResponseHeaders(response, false /*removeAll*/);

      // No fields removed.
      Assert::AreEqual(cases[i].size(), static_cast<size_t>(response.Headers().Size()));
    }
  }

  TEST_METHOD(RemoveAllCookies) {
    std::vector<std::map<std::wstring, std::wstring>> cases{
        {
            {L"k1", L"v1"},
            {L"Set-Cookie", L"id=a3fWa; Expires=Wed, 21 Oct 2020 07:28:00 GMT HttpOnly;"},
            {L"k3", L"v3"},
        },

        {
            {L"k1", L"v1"},
            {L"Set-Cookie", L"Expires=Wed, 21 Oct 2020 07:28:00 GMT ;"},
            {L"k3", L"v3"},
        },
    };

    for (size_t i = 0; i < cases.size(); ++i) {
      winrt::Windows::Web::Http::HttpResponseMessage response;
      for (const auto &header : cases[i]) {
        response.Headers().Insert(header.first, header.second);
      }

      OriginPolicyHttpFilter::RemoveHttpOnlyCookiesFromResponseHeaders(response, true /*removeAll*/);

      Assert::AreEqual(2, static_cast<int>(response.Headers().Size()));
    }
  }

  TEST_METHOD(ValidatePreflightResponseHeadersCaseMismatchSucceeds) {
    auto mockFilter = winrt::make<MockHttpBaseFilter>();
    mockFilter.as<MockHttpBaseFilter>()->Mocks.SendRequestAsync =
        [](HttpRequestMessage const &request) -> ResponseOperation {
      HttpResponseMessage response{};

      response.StatusCode(HttpStatusCode::Ok);
      response.Headers().Insert(L"Access-Control-Allow-Origin", L"*");

      // Return allowed headers as requested by client, in lower case.
      // This tests case-insensitive preflight validation.
      auto allowHeaders = boost::to_lower_copy(wstring{request.Headers().Lookup(L"Access-Control-Request-Headers")});
      response.Headers().Insert(L"Access-Control-Allow-Headers", std::move(allowHeaders));

      co_return response;
    };

    auto reqArgs = winrt::make<RequestArgs>();
    auto request = HttpRequestMessage(HttpMethod::Get(), Uri{L"http://somehost"});
    request.Properties().Insert(L"RequestArgs", reqArgs);
    request.Headers().TryAppendWithoutValidation(L"ChangeMyCase", L"Value");
    // Should implicitly set Content-Length and Content-Type
    request.Content(HttpStringContent{L"PreflightContent"});

    auto filter = winrt::make<OriginPolicyHttpFilter>("http://somehost", mockFilter);
    auto opFilter = filter.as<OriginPolicyHttpFilter>();

    try {
      auto sendOp = opFilter->SendPreflightAsync(request);
      sendOp.get();

      auto response = sendOp.GetResults();
      opFilter->ValidatePreflightResponse(request, response);

      Assert::IsTrue(boost::iequals(
          response.Headers().Lookup(L"Access-Control-Allow-Headers").c_str(),
          L"ChangeMyCase, Content-Length, Content-Type"));
    } catch (const winrt::hresult_error &e) {
      Assert::Fail(e.message().c_str());
    }
  }

  TEST_METHOD(ValidatePreflightResponseMainAndContentHeadersSucceeds) {
    auto mockFilter = winrt::make<MockHttpBaseFilter>();
    mockFilter.as<MockHttpBaseFilter>()->Mocks.SendRequestAsync =
        [](HttpRequestMessage const &request) -> ResponseOperation {
      HttpResponseMessage response{};

      response.StatusCode(HttpStatusCode::Ok);
      response.Headers().Insert(L"Access-Control-Allow-Origin", L"*");
      // Return allowed headers as requested by client
      response.Headers().Insert(
          L"Access-Control-Allow-Headers", request.Headers().Lookup(L"Access-Control-Request-Headers"));

      co_return response;
    };

    auto reqArgs = winrt::make<RequestArgs>();
    auto request = HttpRequestMessage(HttpMethod::Get(), Uri{L"http://somehost"});
    request.Properties().Insert(L"RequestArgs", reqArgs);
    request.Headers().TryAppendWithoutValidation(L"Authorization", L"Bearer abc");
    // Should implicitly set Content-Length and Content-Type
    request.Content(HttpStringContent{L"PreflightContent"});

    auto filter = winrt::make<OriginPolicyHttpFilter>("http://somehost", mockFilter);
    auto opFilter = filter.as<OriginPolicyHttpFilter>();

    try {
      auto sendOp = opFilter->SendPreflightAsync(request);
      sendOp.get();

      auto response = sendOp.GetResults();
      opFilter->ValidatePreflightResponse(request, response);

      Assert::AreEqual(
          L"Authorization, Content-Length, Content-Type",
          response.Headers().Lookup(L"Access-Control-Allow-Headers").c_str());
    } catch (const winrt::hresult_error &e) {
      Assert::Fail(e.message().c_str());
    }
  }

  TEST_METHOD(GetOriginRespectsDefaultPorts) {
    constexpr const wchar_t *urls[] = {
        L"http://site.ext",
        L"https://site.ext",
        L"http://site.ext:80",
        L"https://site.ext:443",
        L"http://site.ext:5555",
        L"https://site.ext:5555",
        L"http://site.ext:443",
        L"https://site.ext:80"};

    constexpr const wchar_t *expected[] = {
        L"http://site.ext",
        L"https://site.ext",
        L"http://site.ext",
        L"https://site.ext",
        L"http://site.ext:5555",
        L"https://site.ext:5555",
        L"http://site.ext:443",
        L"https://site.ext:80"};

    auto size = sizeof(urls) / sizeof(wchar_t *);
    for (size_t i = 0; i < size; ++i) {
      Assert::AreEqual(expected[i], OriginPolicyHttpFilter::GetOrigin(Uri{urls[i]}).c_str());
    }
  }

#pragma region Former Integration Tests
  // clang-format off

  //
  // The tests below were migrated from the HttpOriginPolicyIntegrationTest, which exercised the
  // origin policy against an in-process HTTP server (Microsoft::React::Test::HttpServer). Here the
  // network is replaced with a MockHttpBaseFilter terminating the same WinRT filter chain assembled
  // by IHttpResource::Make() (OriginPolicyHttpFilter -> RedirectHttpFilter -> inner filter).
  //

  /// <summary>
  /// Describes a mocked HTTP endpoint. A single mock filter routes requests to the matching endpoint
  /// by host and replies with the configured preflight (OPTIONS) or main response.
  /// </summary>
  struct ServerParams
  {
    wstring Host; // Host name used to route requests to this endpoint (e.g. L"mockserver.rnw").
    wstring WideUrl; // Wide URL form, used as Location / Access-Control-Allow-Origin header values.
    string Url; // Narrow URL form, used for runtime options and request URLs.

    HttpStatusCode PreflightStatus{HttpStatusCode::Ok};
    std::map<wstring, wstring> PreflightHeaders;

    HttpStatusCode ResponseStatus{HttpStatusCode::Ok};
    std::map<wstring, wstring> ResponseHeaders;
    wstring ResponseContent{L"RESPONSE_CONTENT"};

    ServerParams(wstring host)
      : Host{std::move(host)}
      , WideUrl{L"http://" + Host}
      , Url{winrt::to_string(WideUrl)}
    {
      PreflightHeaders[s_accessControlAllowMethods] = L"GET, POST, DELETE, PATCH";
    }
  };

  struct ClientParams
  {
    wstring Method;
    std::vector<std::pair<wstring, wstring>> Headers;
    bool WithCredentials{false};

    ClientParams(wstring&& method, std::vector<std::pair<wstring, wstring>>&& headers)
      : Method{std::move(method)}
      , Headers{std::move(headers)}
    {
    }
  };

  struct RequestResult
  {
    winrt::hstring Error;
    HttpStatusCode StatusCode{HttpStatusCode::None};
    winrt::hstring Content;
    bool HasResponse{false};
  };

  /// <summary>
  /// Builds the same WinRT HTTP filter chain that IHttpResource::Make() assembles
  /// (OriginPolicyHttpFilter -> RedirectHttpFilter -> inner filter), but terminates it with the
  /// supplied mock filter instead of the live network stack. Reads the same runtime options.
  /// </summary>
  HttpClient MakeMockedClient(IHttpFilter const& innerMock)
  {
    auto redirFilter = winrt::make<RedirectHttpFilter>(IHttpFilter{innerMock}, IHttpFilter{innerMock}, winrt::hstring{});

    if (static_cast<OriginPolicy>(GetRuntimeOptionInt("Http.OriginPolicy")) == OriginPolicy::None)
    {
      return HttpClient{redirFilter};
    }

    auto globalOrigin = GetRuntimeOptionString("Http.GlobalOrigin");
    auto opFilter = winrt::make<OriginPolicyHttpFilter>(std::move(globalOrigin), redirFilter);
    redirFilter.as<RedirectHttpFilter>()->SetRedirectSource(
        opFilter.as<Microsoft::React::Networking::IRedirectEventSource>());

    return HttpClient{opFilter};
  }

  /// <summary>
  /// Builds an HTTP request the way WinRTHttpResource::CreateRequest does for the integration test's
  /// inputs: empty string body, Content-Type attached to the request content, all other headers
  /// attached to the request itself, and a RequestArgs property the OriginPolicyHttpFilter relies on.
  /// </summary>
  HttpRequestMessage BuildRequest(ClientParams const& client, string const& url)
  {
    HttpRequestMessage request{HttpMethod{winrt::hstring{client.Method}}, Uri{winrt::to_hstring(url)}};

    auto reqArgs = winrt::make<RequestArgs>();
    reqArgs.as<RequestArgs>()->WithCredentials = client.WithCredentials;
    request.Properties().Insert(L"RequestArgs", reqArgs);

    HttpStringContent content{winrt::hstring{L""}};
    for (auto const& header : client.Headers)
    {
      if (_wcsicmp(header.first.c_str(), L"Content-Type") == 0)
      {
        content.Headers().ContentType(HttpMediaTypeHeaderValue::Parse(header.second));
      }
      else
      {
        // Use unvalidated append so that forbidden headers (e.g. Proxy-Authorization, Host) reach the
        // OriginPolicyHttpFilter, which is the component under test responsible for rejecting them.
        request.Headers().TryAppendWithoutValidation(header.first, header.second);
      }
    }
    request.Content(content);

    return request;
  }

  RequestResult RunRequest(std::vector<ServerParams*> const& servers, ClientParams const& client, string const& requestUrl)
  {
    auto mockFilter = winrt::make<MockHttpBaseFilter>();
    mockFilter.as<MockHttpBaseFilter>()->Mocks.SendRequestAsync =
        [&servers](HttpRequestMessage const& request) -> ResponseOperation
    {
      HttpResponseMessage response{};

      ServerParams const* match = nullptr;
      for (auto* server : servers)
      {
        if (request.RequestUri().Host() == server->Host)
        {
          match = server;
          break;
        }
      }

      if (!match)
      {
        response.StatusCode(HttpStatusCode::NotFound);
        response.Content(HttpStringContent{winrt::hstring{L""}});
        co_return response;
      }

      bool isPreflight = request.Method().ToString() == L"OPTIONS";
      response.StatusCode(isPreflight ? match->PreflightStatus : match->ResponseStatus);

      auto const& headers = isPreflight ? match->PreflightHeaders : match->ResponseHeaders;
      for (auto const& header : headers)
      {
        if (_wcsicmp(header.first.c_str(), s_location) == 0)
        {
          response.Headers().Location(Uri{header.second});
        }
        else
        {
          response.Headers().Insert(header.first, header.second);
        }
      }

      response.Content(HttpStringContent{isPreflight ? winrt::hstring{L""} : winrt::hstring{match->ResponseContent}});

      co_return response;
    };

    auto client_ = MakeMockedClient(mockFilter);

    RequestResult result;
    try
    {
      auto request = BuildRequest(client, requestUrl);
      auto sendOp = client_.SendRequestAsync(request);
      sendOp.get();

      auto response = sendOp.GetResults();
      result.HasResponse = static_cast<bool>(response);
      if (response)
      {
        result.StatusCode = response.StatusCode();
        if (response.Content())
        {
          auto contentOp = response.Content().ReadAsStringAsync();
          contentOp.get();
          result.Content = contentOp.GetResults();
        }
      }
    }
    catch (winrt::hresult_error const& e)
    {
      result.Error = e.message();
    }

    return result;
  }

  void AssertSucceeds(RequestResult const& result, HttpStatusCode expectedStatus)
  {
    Assert::AreEqual(L"", result.Error.c_str());
    Assert::AreEqual(static_cast<int>(expectedStatus), static_cast<int>(result.StatusCode));
    Assert::AreEqual(L"RESPONSE_CONTENT", result.Content.c_str());
  }

  void AssertFails(RequestResult const& result)
  {
    Assert::AreNotEqual(L"", result.Error.c_str());
  }

  void TestOriginPolicy(ServerParams& serverArgs, ClientParams& clientArgs, bool shouldSucceed)
  {
    auto result = RunRequest({&serverArgs}, clientArgs, serverArgs.Url);

    if (shouldSucceed)
      AssertSucceeds(result, serverArgs.ResponseStatus);
    else
      AssertFails(result);
  }

  void TestOriginPolicyWithRedirect(
      ServerParams& server1Args,
      ServerParams& server2Args,
      ClientParams& clientArgs,
      bool shouldSucceed)
  {
    // We assume 2-server tests will always redirect so the final response will come from server 2.
    auto result = RunRequest({&server1Args, &server2Args}, clientArgs, server1Args.Url);

    if (shouldSucceed)
      AssertSucceeds(result, server2Args.ResponseStatus);
    else
      AssertFails(result);
  }

  TEST_METHOD_CLEANUP(MethodCleanup)
  {
    // Clear any runtime options that may be used by tests in this class.
    SetRuntimeOptionInt("Http.OriginPolicy", static_cast<int32_t>(OriginPolicy::None));
    SetRuntimeOptionString("Http.GlobalOrigin", {});
    SetRuntimeOptionBool("Http.OmitCredentials", false);
  }

  BEGIN_TEST_METHOD_ATTRIBUTE(NoCorsForbiddenMethodSucceeds)
    // CONNECT, TRACE, and TRACK methods not supported by Windows.Web.Http
    // https://docs.microsoft.com/en-us/uwp/api/windows.web.http.httpmethod?view=winrt-22621#properties
    TEST_IGNORE()
  END_TEST_METHOD_ATTRIBUTE()
  TEST_METHOD(NoCorsForbiddenMethodSucceeds)
  {
    SetRuntimeOptionInt("Http.OriginPolicy", static_cast<int32_t>(OriginPolicy::None));

    ServerParams serverArgs(s_mockServerHost);
    serverArgs.ResponseStatus = HttpStatusCode::Ok;
    serverArgs.ResponseContent = L"GET_CONTENT";

    ClientParams clientArgs(L"TRACE", {{L"ValidHeader", L"AnyValue"}});

    auto result = RunRequest({&serverArgs}, clientArgs, serverArgs.Url);

    Assert::AreEqual(L"", result.Error.c_str());
    Assert::AreEqual(static_cast<int>(HttpStatusCode::Ok), static_cast<int>(result.StatusCode));
    Assert::AreEqual(L"GET_CONTENT", result.Content.c_str());
  } // NoCorsForbiddenMethodSucceeds

  BEGIN_TEST_METHOD_ATTRIBUTE(SimpleCorsForbiddenMethodFails)
  END_TEST_METHOD_ATTRIBUTE()
  TEST_METHOD(SimpleCorsForbiddenMethodFails)
  {
    ServerParams serverArgs(s_mockServerHost);
    serverArgs.PreflightHeaders[s_accessControlAllowOrigin] = serverArgs.WideUrl;

    ClientParams clientArgs(L"CONNECT", {{L"Content-Type", L"text/plain"}});

    SetRuntimeOptionString("Http.GlobalOrigin", s_crossOriginUrl);
    SetRuntimeOptionInt("Http.OriginPolicy", static_cast<int32_t>(OriginPolicy::SimpleCrossOriginResourceSharing));

    TestOriginPolicy(serverArgs, clientArgs, false /*shouldSucceed*/);
  } // SimpleCorsForbiddenMethodFails

  BEGIN_TEST_METHOD_ATTRIBUTE(NoCorsCrossOriginFetchRequestSucceeds)
  END_TEST_METHOD_ATTRIBUTE()
  TEST_METHOD(NoCorsCrossOriginFetchRequestSucceeds)
  {
    SetRuntimeOptionString("Http.GlobalOrigin", s_crossOriginUrl);
    SetRuntimeOptionInt("Http.OriginPolicy", static_cast<int32_t>(OriginPolicy::None));

    ServerParams serverArgs(s_mockServerHost);
    serverArgs.ResponseStatus = HttpStatusCode::Ok;

    ClientParams clientArgs(L"GET", {{L"Content-Type", L"text/plain"}});

    TestOriginPolicy(serverArgs, clientArgs, true /*shouldSucceed*/);
  } // NoCorsCrossOriginFetchRequestSucceeds

  BEGIN_TEST_METHOD_ATTRIBUTE(NoCorsCrossOriginPatchSucceeds)
  END_TEST_METHOD_ATTRIBUTE()
  TEST_METHOD(NoCorsCrossOriginPatchSucceeds)
  {
    SetRuntimeOptionString("Http.GlobalOrigin", s_crossOriginUrl);
    SetRuntimeOptionInt("Http.OriginPolicy", static_cast<int32_t>(OriginPolicy::None));

    ServerParams serverArgs(s_mockServerHost);
    serverArgs.ResponseStatus = HttpStatusCode::Ok;

    ClientParams clientArgs(L"PATCH", {{L"Content-Type", L"text/plain"}});

    TestOriginPolicy(serverArgs, clientArgs, true /*shouldSucceed*/);
  } // NoCorsCrossOriginPatchSucceeds

  // Simple-Cors - Prevents the method from being anything other than HEAD, GET or POST,
  // and the headers from being anything other than simple headers (CORS safe listed headers).
  BEGIN_TEST_METHOD_ATTRIBUTE(SimpleCorsSameOriginSucceeds)
  END_TEST_METHOD_ATTRIBUTE()
  TEST_METHOD(SimpleCorsSameOriginSucceeds)
  {
    ServerParams serverArgs(s_mockServerHost);
    serverArgs.ResponseStatus = HttpStatusCode::Ok;

    ClientParams clientArgs(L"PATCH", {{L"Content-Type", L"text/plain"}});

    SetRuntimeOptionString("Http.GlobalOrigin", serverArgs.Url.c_str());
    SetRuntimeOptionInt("Http.OriginPolicy", static_cast<int32_t>(OriginPolicy::SimpleCrossOriginResourceSharing));

    TestOriginPolicy(serverArgs, clientArgs, true /*shouldSucceed*/);
  } // SimpleCorsSameOriginSucceeds

  BEGIN_TEST_METHOD_ATTRIBUTE(SimpleCorsCrossOriginFetchFails)
  END_TEST_METHOD_ATTRIBUTE()
  TEST_METHOD(SimpleCorsCrossOriginFetchFails)
  {
    ServerParams serverArgs(s_mockServerHost);

    ClientParams clientArgs(L"GET", {{L"Content-Type", L"text/html"}}); // text/html is a non-simple value

    SetRuntimeOptionString("Http.GlobalOrigin", s_crossOriginUrl);
    SetRuntimeOptionInt("Http.OriginPolicy", static_cast<int32_t>(OriginPolicy::SimpleCrossOriginResourceSharing));

    TestOriginPolicy(serverArgs, clientArgs, false /*shouldSucceed*/);
  } // SimpleCorsCrossOriginFetchFails

  BEGIN_TEST_METHOD_ATTRIBUTE(FullCorsSameOriginRequestSucceeds)
  END_TEST_METHOD_ATTRIBUTE()
  TEST_METHOD(FullCorsSameOriginRequestSucceeds)
  {
    ServerParams serverArgs(s_mockServerHost);
    serverArgs.ResponseStatus = HttpStatusCode::Ok;

    ClientParams clientArgs(L"GET", {{L"Content-Type", L"text/plain"}});

    SetRuntimeOptionString("Http.GlobalOrigin", serverArgs.Url.c_str());
    SetRuntimeOptionInt("Http.OriginPolicy", static_cast<int32_t>(OriginPolicy::CrossOriginResourceSharing));

    TestOriginPolicy(serverArgs, clientArgs, true /*shouldSucceed*/);
  } // FullCorsSameOriginRequestSucceeds

  BEGIN_TEST_METHOD_ATTRIBUTE(FullCorsCrossOriginAllowOriginWildcardSucceeds)
  END_TEST_METHOD_ATTRIBUTE()
  TEST_METHOD(FullCorsCrossOriginAllowOriginWildcardSucceeds)
  {
    ServerParams serverArgs(s_mockServerHost);
    serverArgs.PreflightHeaders[s_accessControlAllowHeaders] = L"Content-Type";
    serverArgs.PreflightHeaders[s_accessControlAllowOrigin] = L"*";
    serverArgs.PreflightHeaders[s_accessControlRequestHeaders] = L"Content-Type";
    serverArgs.ResponseStatus = HttpStatusCode::Accepted;
    serverArgs.ResponseHeaders[s_accessControlAllowOrigin] = L"*";
    serverArgs.ResponseHeaders[s_accessControlAllowCredentials] = L"true";

    ClientParams clientArgs(L"GET", {{L"Content-Type", L"text/plain"}});

    SetRuntimeOptionString("Http.GlobalOrigin", s_crossOriginUrl);
    SetRuntimeOptionInt("Http.OriginPolicy", static_cast<int32_t>(OriginPolicy::CrossOriginResourceSharing));

    TestOriginPolicy(serverArgs, clientArgs, true /*shouldSucceed*/);
  } // FullCorsCrossOriginAllowOriginWildcardSucceeds

  // With CORS, the server can decide what origins are permitted to read information from the client.
  // Additionally, for non-simple requests, the client should preflight the request through HTTP OPTIONS.
  BEGIN_TEST_METHOD_ATTRIBUTE(FullCorsCrossOriginMatchingOriginSucceeds)
  END_TEST_METHOD_ATTRIBUTE()
  TEST_METHOD(FullCorsCrossOriginMatchingOriginSucceeds)
  {
    ServerParams serverArgs(s_mockServerHost);
    serverArgs.PreflightHeaders[s_accessControlAllowHeaders] = L"Content-Type";
    serverArgs.PreflightHeaders[s_accessControlAllowOrigin] = s_crossOriginUrlW;
    serverArgs.PreflightHeaders[s_accessControlRequestHeaders] = L"Content-Type";
    serverArgs.ResponseStatus = HttpStatusCode::Accepted;
    serverArgs.ResponseHeaders[s_accessControlAllowOrigin] = s_crossOriginUrlW;
    serverArgs.ResponseHeaders[s_accessControlAllowCredentials] = L"true";

    ClientParams clientArgs(L"GET", {{L"Content-Type", L"text/plain"}});

    SetRuntimeOptionString("Http.GlobalOrigin", s_crossOriginUrl);
    SetRuntimeOptionInt("Http.OriginPolicy", static_cast<int32_t>(OriginPolicy::CrossOriginResourceSharing));

    TestOriginPolicy(serverArgs, clientArgs, true /*shouldSucceed*/);
  } // FullCorsCrossOriginMatchingOriginSucceeds

  BEGIN_TEST_METHOD_ATTRIBUTE(FullCorsCrossOriginWithCredentialsFails)
  END_TEST_METHOD_ATTRIBUTE()
  TEST_METHOD(FullCorsCrossOriginWithCredentialsFails)
  {
    ServerParams serverArgs(s_mockServerHost);
    serverArgs.PreflightHeaders[s_accessControlRequestHeaders] = L"Content-Type";
    serverArgs.PreflightHeaders[s_accessControlAllowHeaders] = L"Content-Type";
    serverArgs.PreflightHeaders[s_accessControlAllowOrigin] = s_crossOriginUrlW;
    serverArgs.PreflightHeaders[s_accessControlAllowCredentials] = L"true";
    serverArgs.ResponseHeaders[s_accessControlAllowOrigin] = s_crossOriginUrlW;

    ClientParams clientArgs(L"GET", {{L"Content-Type", L"application/text"}}); // application/text is a non-simple header
    clientArgs.WithCredentials = true;

    SetRuntimeOptionString("Http.GlobalOrigin", s_crossOriginUrl);
    SetRuntimeOptionInt("Http.OriginPolicy", static_cast<int32_t>(OriginPolicy::CrossOriginResourceSharing));
    SetRuntimeOptionBool("Http.OmitCredentials", true);

    TestOriginPolicy(serverArgs, clientArgs, false /*shouldSucceed*/);
  } // FullCorsCrossOriginWithCredentialsFails

  BEGIN_TEST_METHOD_ATTRIBUTE(FullCorsCrossOriginMissingCorsHeadersFails)
  END_TEST_METHOD_ATTRIBUTE()
  TEST_METHOD(FullCorsCrossOriginMissingCorsHeadersFails)
  {
    ServerParams serverArgs(s_mockServerHost);
    serverArgs.PreflightHeaders.erase(s_accessControlAllowMethods);
    serverArgs.PreflightStatus = HttpStatusCode::NotImplemented;

    ClientParams clientArgs(L"GET", {{L"Content-Type", L"application/text"}}); // application/text is a non-simple header

    SetRuntimeOptionString("Http.GlobalOrigin", s_crossOriginUrl);
    SetRuntimeOptionInt("Http.OriginPolicy", static_cast<int32_t>(OriginPolicy::CrossOriginResourceSharing));

    TestOriginPolicy(serverArgs, clientArgs, false /*shouldSucceed*/);
  } // FullCorsCrossOriginMissingCorsHeadersFails

  BEGIN_TEST_METHOD_ATTRIBUTE(FullCorsCrossOriginMismatchedCorsHeaderFails)
  END_TEST_METHOD_ATTRIBUTE()
  TEST_METHOD(FullCorsCrossOriginMismatchedCorsHeaderFails)
  {
    ServerParams serverArgs(s_mockServerHost);
    serverArgs.PreflightHeaders[s_accessControlRequestHeaders] = L"Content-Type";
    serverArgs.PreflightHeaders[s_accessControlAllowHeaders] = L"Content-Type";
    serverArgs.PreflightHeaders[s_accessControlAllowOrigin] = s_crossOriginUrlW;
    serverArgs.ResponseStatus = HttpStatusCode::Accepted;
    serverArgs.ResponseHeaders[s_accessControlAllowOrigin] = L"http://other.example.rnw";

    ClientParams clientArgs(L"GET", {{L"Content-Type", L"application/text"}}); // application/text is a non-simple header

    SetRuntimeOptionString("Http.GlobalOrigin", s_crossOriginUrl);
    SetRuntimeOptionInt("Http.OriginPolicy", static_cast<int32_t>(OriginPolicy::CrossOriginResourceSharing));

    TestOriginPolicy(serverArgs, clientArgs, false /*shouldSucceed*/);
  } // FullCorsCrossOriginMismatchedCorsHeaderFails

  // https://developer.mozilla.org/en-US/docs/Web/HTTP/CORS/Errors/CORSExternalRedirectNotAllowed
  BEGIN_TEST_METHOD_ATTRIBUTE(FullCorsCrossOriginCheckFailsOnPreflightRedirectFails)
  END_TEST_METHOD_ATTRIBUTE()
  TEST_METHOD(FullCorsCrossOriginCheckFailsOnPreflightRedirectFails)
  {
    ServerParams serverArgs(s_mockServerHost);
    serverArgs.PreflightHeaders[s_accessControlAllowOrigin] = s_crossOriginUrlW;
    serverArgs.PreflightHeaders[s_location] = L"http://any-host.extension";
    serverArgs.PreflightStatus = HttpStatusCode::MovedPermanently;

    ClientParams clientArgs(L"GET", {{L"Content-Type", L"application/text"}});

    SetRuntimeOptionString("Http.GlobalOrigin", s_crossOriginUrl);
    SetRuntimeOptionInt("Http.OriginPolicy", static_cast<int32_t>(OriginPolicy::CrossOriginResourceSharing));

    TestOriginPolicy(serverArgs, clientArgs, false /*shouldSucceed*/);
  } // FullCorsCrossOriginCheckFailsOnPreflightRedirectFails

  BEGIN_TEST_METHOD_ATTRIBUTE(FullCorsCorsCheckFailsOnResponseRedirectFails)
  END_TEST_METHOD_ATTRIBUTE()
  TEST_METHOD(FullCorsCorsCheckFailsOnResponseRedirectFails)
  {
    ServerParams serverArgs(s_mockServerHost);
    ServerParams redirServerArgs(s_redirectServerHost);

    // server1 allowed origin header includes the cross origin
    serverArgs.PreflightHeaders[s_accessControlRequestHeaders] = L"Content-Type";
    serverArgs.PreflightHeaders[s_accessControlAllowHeaders] = L"Content-Type";
    serverArgs.PreflightHeaders[s_accessControlAllowOrigin] = s_crossOriginUrlW;

    // This is a CORS request to server1, but server1 redirects the request to server2
    serverArgs.ResponseStatus = HttpStatusCode::MovedPermanently;
    serverArgs.ResponseHeaders[s_accessControlAllowOrigin] = s_crossOriginUrlW;
    serverArgs.ResponseHeaders[s_location] = redirServerArgs.WideUrl;
    serverArgs.ResponseHeaders[s_server] = L"BaseServer";

    // Server2 does not set Access-Control-Allow-Origin for GET requests
    redirServerArgs.ResponseStatus = HttpStatusCode::Accepted;
    redirServerArgs.ResponseHeaders[s_server] = L"RedirectServer";

    ClientParams clientArgs(L"GET", {{L"Content-Type", L"application/text"}});

    SetRuntimeOptionString("Http.GlobalOrigin", s_crossOriginUrl);
    SetRuntimeOptionInt("Http.OriginPolicy", static_cast<int32_t>(OriginPolicy::CrossOriginResourceSharing));

    TestOriginPolicyWithRedirect(serverArgs, redirServerArgs, clientArgs, false /*shouldSucceed*/);
  } // FullCorsCorsCheckFailsOnResponseRedirectFails

  BEGIN_TEST_METHOD_ATTRIBUTE(FullCorsSameOriginToSameOriginRedirectSucceeds)
  END_TEST_METHOD_ATTRIBUTE()
  TEST_METHOD(FullCorsSameOriginToSameOriginRedirectSucceeds)
  {
    ServerParams serverArgs(s_mockServerHost);
    serverArgs.PreflightHeaders[s_location] = serverArgs.WideUrl;
    serverArgs.ResponseStatus = HttpStatusCode::Accepted;

    ClientParams clientArgs(L"GET", {{L"Content-Type", L"application/text"}});

    SetRuntimeOptionString("Http.GlobalOrigin", serverArgs.Url.c_str());
    SetRuntimeOptionInt("Http.OriginPolicy", static_cast<int32_t>(OriginPolicy::CrossOriginResourceSharing));

    TestOriginPolicy(serverArgs, clientArgs, true /*shouldSucceed*/);
  } // FullCorsSameOriginToSameOriginRedirectSucceeds

  BEGIN_TEST_METHOD_ATTRIBUTE(FullCorsSameOriginToCrossOriginRedirectSucceeds)
  END_TEST_METHOD_ATTRIBUTE()
  TEST_METHOD(FullCorsSameOriginToCrossOriginRedirectSucceeds)
  {
    ServerParams serverArgs(s_mockServerHost);
    ServerParams redirServerArgs(s_redirectServerHost);

    serverArgs.PreflightHeaders[s_accessControlAllowOrigin] = serverArgs.WideUrl;
    serverArgs.ResponseStatus = HttpStatusCode::MovedPermanently;
    serverArgs.ResponseHeaders[s_location] = redirServerArgs.WideUrl;

    redirServerArgs.ResponseStatus = HttpStatusCode::Accepted;
    redirServerArgs.ResponseHeaders[s_accessControlAllowOrigin] = serverArgs.WideUrl;

    ClientParams clientArgs(L"GET", {{L"Content-Type", L"application/text"}});

    SetRuntimeOptionString("Http.GlobalOrigin", serverArgs.Url.c_str());
    SetRuntimeOptionInt("Http.OriginPolicy", static_cast<int32_t>(OriginPolicy::CrossOriginResourceSharing));

    TestOriginPolicyWithRedirect(serverArgs, redirServerArgs, clientArgs, true /*shouldSucceed*/);
  } // FullCorsSameOriginToCrossOriginRedirectSucceeds

  // Redirects a cross origin request to a cross origin request on the same server.
  BEGIN_TEST_METHOD_ATTRIBUTE(FullCorsCrossOriginToCrossOriginRedirectSucceeds)
    // Seems to redirect to exact same resource. Implement second resource in same server.
    TEST_IGNORE()
  END_TEST_METHOD_ATTRIBUTE()
  TEST_METHOD(FullCorsCrossOriginToCrossOriginRedirectSucceeds)
  {
    ServerParams serverArgs(s_mockServerHost);

    serverArgs.PreflightHeaders[s_accessControlAllowOrigin] = serverArgs.WideUrl;
    serverArgs.PreflightHeaders[s_accessControlRequestHeaders] = L"Content-Type";
    serverArgs.PreflightHeaders[s_accessControlAllowHeaders] = L"Content-Type";
    serverArgs.PreflightHeaders[s_accessControlAllowOrigin] = s_crossOriginUrlW;
    serverArgs.ResponseStatus = HttpStatusCode::MovedPermanently;
    serverArgs.ResponseHeaders[s_location] = serverArgs.WideUrl;
    serverArgs.ResponseHeaders[s_accessControlAllowOrigin] = s_crossOriginUrlW;

    ClientParams clientArgs(L"GET", {{L"Content-Type", L"application/text"}});

    SetRuntimeOptionString("Http.GlobalOrigin", s_crossOriginUrl);
    SetRuntimeOptionInt("Http.OriginPolicy", static_cast<int32_t>(OriginPolicy::CrossOriginResourceSharing));

    TestOriginPolicy(serverArgs, clientArgs, true /*shouldSucceed*/);
  } // FullCorsCrossOriginToCrossOriginRedirectSucceeds

  // The initial request gets redirected back to the original origin,
  // but it will lack the Access-Control-Allow-Origin header.
  BEGIN_TEST_METHOD_ATTRIBUTE(FullCorsCrossOriginToOriginalOriginRedirectFails)
  END_TEST_METHOD_ATTRIBUTE()
  TEST_METHOD(FullCorsCrossOriginToOriginalOriginRedirectFails)
  {
    ServerParams serverArgs(s_mockServerHost);
    ServerParams redirServerArgs(s_redirectServerHost);

    serverArgs.ResponseStatus = HttpStatusCode::MovedPermanently;
    serverArgs.ResponseHeaders[s_location] = redirServerArgs.WideUrl;
    serverArgs.ResponseHeaders[s_accessControlAllowOrigin] = redirServerArgs.WideUrl;

    ClientParams clientArgs(L"GET", {{L"Content-Type", L"text/plain"}});

    SetRuntimeOptionString("Http.GlobalOrigin", redirServerArgs.Url.c_str());
    SetRuntimeOptionInt("Http.OriginPolicy", static_cast<int32_t>(OriginPolicy::CrossOriginResourceSharing));

    TestOriginPolicyWithRedirect(serverArgs, redirServerArgs, clientArgs, false /*shouldSucceed*/);
  } // FullCorsCrossOriginToOriginalOriginRedirectFails

  // Redirects a cross origin request to server1 to a cross origin request to server2.
  BEGIN_TEST_METHOD_ATTRIBUTE(FullCorsCrossOriginToAnotherCrossOriginRedirectSucceeds)
  END_TEST_METHOD_ATTRIBUTE()
  TEST_METHOD(FullCorsCrossOriginToAnotherCrossOriginRedirectSucceeds)
  {
    ServerParams serverArgs(s_mockServerHost);
    ServerParams redirServerArgs(s_redirectServerHost);

    serverArgs.ResponseStatus = HttpStatusCode::MovedPermanently;
    serverArgs.ResponseHeaders[s_location] = redirServerArgs.WideUrl;
    serverArgs.ResponseHeaders[s_accessControlAllowOrigin] = s_crossOriginUrlW;

    redirServerArgs.ResponseStatus = HttpStatusCode::Accepted;
    redirServerArgs.ResponseHeaders[s_accessControlAllowOrigin] = L"*";

    ClientParams clientArgs(L"GET", {{L"Content-Type", L"text/plain"}});

    SetRuntimeOptionString("Http.GlobalOrigin", s_crossOriginUrl);
    SetRuntimeOptionInt("Http.OriginPolicy", static_cast<int32_t>(OriginPolicy::CrossOriginResourceSharing));

    TestOriginPolicyWithRedirect(serverArgs, redirServerArgs, clientArgs, true /*shouldSucceed*/);
  } // FullCorsCrossOriginToAnotherCrossOriginRedirectSucceeds

  BEGIN_TEST_METHOD_ATTRIBUTE(FullCorsCrossOriginToAnotherCrossOriginRedirectWithPreflightSucceeds)
  END_TEST_METHOD_ATTRIBUTE()
  TEST_METHOD(FullCorsCrossOriginToAnotherCrossOriginRedirectWithPreflightSucceeds)
  {
    ServerParams serverArgs(s_mockServerHost);
    ServerParams redirServerArgs(s_redirectServerHost);

    serverArgs.PreflightHeaders[s_accessControlRequestHeaders] = L"Content-Type";
    serverArgs.PreflightHeaders[s_accessControlAllowHeaders] = L"Content-Type";
    serverArgs.PreflightHeaders[s_accessControlAllowOrigin] = s_crossOriginUrlW;
    // server1 redirects the request to server2
    serverArgs.ResponseStatus = HttpStatusCode::MovedPermanently;
    serverArgs.ResponseHeaders[s_location] = redirServerArgs.WideUrl;
    serverArgs.ResponseHeaders[s_accessControlAllowOrigin] = s_crossOriginUrlW;

    // Since redirect tainted the origin, the server has to allow all origins for CORS to succeed
    redirServerArgs.ResponseStatus = HttpStatusCode::Accepted;
    redirServerArgs.ResponseHeaders[s_accessControlAllowOrigin] = L"*";

    // PATCH is not a simple method, so preflight is required for server1
    ClientParams clientArgs(L"PATCH", {{L"Content-Type", L"text/plain"}});

    SetRuntimeOptionString("Http.GlobalOrigin", s_crossOriginUrl);
    SetRuntimeOptionInt("Http.OriginPolicy", static_cast<int32_t>(OriginPolicy::CrossOriginResourceSharing));

    TestOriginPolicyWithRedirect(serverArgs, redirServerArgs, clientArgs, true /*shouldSucceed*/);
  } // FullCorsCrossOriginToAnotherCrossOriginRedirectWithPreflightSucceeds

  BEGIN_TEST_METHOD_ATTRIBUTE(FullCorsCrossOriginToAnotherCrossOriginRedirectWithPreflightFails)
  END_TEST_METHOD_ATTRIBUTE()
  TEST_METHOD(FullCorsCrossOriginToAnotherCrossOriginRedirectWithPreflightFails)
  {
    ServerParams serverArgs(s_mockServerHost);
    ServerParams redirServerArgs(s_redirectServerHost);

    serverArgs.PreflightHeaders[s_accessControlRequestHeaders] = L"Content-Type";
    serverArgs.PreflightHeaders[s_accessControlAllowHeaders] = L"Content-Type";
    serverArgs.PreflightHeaders[s_accessControlAllowOrigin] = s_crossOriginUrlW;
    // server1 redirects the request to server2
    serverArgs.ResponseStatus = HttpStatusCode::MovedPermanently;
    serverArgs.ResponseHeaders[s_location] = redirServerArgs.WideUrl;
    serverArgs.ResponseHeaders[s_accessControlAllowOrigin] = s_crossOriginUrlW;

    // Since redirect tainted the origin, the server does not know what origin to allow through a single value.
    // Even if the server successfully guessed the single value, it will still fail on the client side.
    redirServerArgs.ResponseStatus = HttpStatusCode::Accepted;
    redirServerArgs.ResponseHeaders[s_accessControlAllowOrigin] = s_crossOriginUrlW;

    // PATCH is not a simple method, so preflight is required for server1
    ClientParams clientArgs(L"PATCH", {{L"Content-Type", L"text/plain"}});

    SetRuntimeOptionString("Http.GlobalOrigin", s_crossOriginUrl);
    SetRuntimeOptionInt("Http.OriginPolicy", static_cast<int32_t>(OriginPolicy::CrossOriginResourceSharing));

    TestOriginPolicyWithRedirect(serverArgs, redirServerArgs, clientArgs, false /*shouldSucceed*/);
  } // FullCorsCrossOriginToAnotherCrossOriginRedirectWithPreflightFails

  BEGIN_TEST_METHOD_ATTRIBUTE(FullCors304ForSimpleGetFails)
  END_TEST_METHOD_ATTRIBUTE()
  TEST_METHOD(FullCors304ForSimpleGetFails)
  {
    ServerParams serverArgs(s_mockServerHost);
    serverArgs.ResponseStatus = HttpStatusCode::NotModified;

    ClientParams clientArgs(L"GET", {{L"Content-Type", L"text/plain"}});

    SetRuntimeOptionString("Http.GlobalOrigin", s_crossOriginUrl);
    SetRuntimeOptionInt("Http.OriginPolicy", static_cast<int32_t>(OriginPolicy::CrossOriginResourceSharing));

    TestOriginPolicy(serverArgs, clientArgs, false /*shouldSucceed*/);
  } // FullCors304ForSimpleGetFails

  TEST_METHOD(OfficeDev_OfficeJS_4144)
  {
    SetRuntimeOptionString("Http.GlobalOrigin", s_crossOriginUrl);
    SetRuntimeOptionInt("Http.OriginPolicy", static_cast<int32_t>(OriginPolicy::CrossOriginResourceSharing));

    // Regression test for https://github.com/OfficeDev/office-js/issues/4144.
    // A header-less GET should be handled without raising an application error.
    ServerParams serverArgs(s_mockServerHost);
    serverArgs.PreflightHeaders[s_accessControlAllowOrigin] = L"*";
    serverArgs.ResponseHeaders[s_accessControlAllowOrigin] = L"*";

    ClientParams clientArgs(L"GET", {} /*headers*/);

    auto result = RunRequest({&serverArgs}, clientArgs, serverArgs.Url + "/officedev/office-js/issues/4144");

    Assert::AreEqual(L"", result.Error.c_str());
  } // OfficeDev_OfficeJS_4144

  TEST_METHOD(FullCorsPreflightSucceeds)
  {
    ServerParams serverArgs(s_mockServerHost);
    serverArgs.PreflightHeaders[s_accessControlRequestHeaders] = L"ArbitraryHeader";
    serverArgs.PreflightHeaders[s_accessControlAllowHeaders] = L"ArbitraryHeader";
    serverArgs.PreflightHeaders[s_accessControlAllowOrigin] = s_crossOriginUrlW;
    serverArgs.ResponseStatus = HttpStatusCode::Ok;
    serverArgs.ResponseHeaders[s_accessControlAllowOrigin] = s_crossOriginUrlW;

    ClientParams clientArgs(L"GET", {{L"Content-Type", L"text/plain"}, {L"ArbitraryHeader", L"AnyValue"}});

    SetRuntimeOptionString("Http.GlobalOrigin", s_crossOriginUrl);
    SetRuntimeOptionInt("Http.OriginPolicy", static_cast<int32_t>(OriginPolicy::CrossOriginResourceSharing));

    TestOriginPolicy(serverArgs, clientArgs, true /*shouldSucceed*/);
  } // FullCorsPreflightSucceeds

  // The current implementation omits the withCredentials flag from the request and always sets it to false.
  BEGIN_TEST_METHOD_ATTRIBUTE(FullCorsCrossOriginWithCredentialsSucceeds)
  END_TEST_METHOD_ATTRIBUTE()
  TEST_METHOD(FullCorsCrossOriginWithCredentialsSucceeds)
  {
    ServerParams serverArgs(s_mockServerHost);
    serverArgs.PreflightHeaders[s_accessControlRequestHeaders] = L"Content-Type";
    serverArgs.PreflightHeaders[s_accessControlAllowHeaders] = L"Content-Type";
    serverArgs.PreflightHeaders[s_accessControlAllowOrigin] = s_crossOriginUrlW;
    serverArgs.PreflightHeaders[s_accessControlAllowCredentials] = L"true";
    serverArgs.ResponseStatus = HttpStatusCode::Accepted;
    serverArgs.ResponseHeaders[s_accessControlAllowOrigin] = s_crossOriginUrlW;

    ClientParams clientArgs(L"GET", {{L"Content-Type", L"application/text"}}); // application/text is a non-simple header
    clientArgs.WithCredentials = true;

    SetRuntimeOptionString("Http.GlobalOrigin", s_crossOriginUrl);
    SetRuntimeOptionInt("Http.OriginPolicy", static_cast<int32_t>(OriginPolicy::CrossOriginResourceSharing));

    TestOriginPolicy(serverArgs, clientArgs, true /*shouldSucceed*/);
  } // FullCorsCrossOriginWithCredentialsSucceeds

  // "Host" is one of the forbidden headers for fetch
  BEGIN_TEST_METHOD_ATTRIBUTE(FullCorsRequestWithHostHeaderFails)
    // "Host" is not an accepted request header in WinRT.
    TEST_IGNORE()
  END_TEST_METHOD_ATTRIBUTE()
  TEST_METHOD(FullCorsRequestWithHostHeaderFails)
  {
    ServerParams serverArgs(s_mockServerHost);
    serverArgs.PreflightHeaders[s_accessControlRequestHeaders] = L"Content-Type";
    serverArgs.PreflightHeaders[s_accessControlAllowHeaders] = L"Content-Type";
    serverArgs.ResponseStatus = HttpStatusCode::Accepted;

    ClientParams clientArgs(L"GET", {{L"Content-Type", L"application/text"}, {L"Host", L"http://sub.example.rnw"}});

    SetRuntimeOptionString("Http.GlobalOrigin", serverArgs.Url.c_str());
    SetRuntimeOptionInt("Http.OriginPolicy", static_cast<int32_t>(OriginPolicy::CrossOriginResourceSharing));

    TestOriginPolicy(serverArgs, clientArgs, false /*shouldSucceed*/);

    Assert::Fail(L"FIX!!! Passes for the wrong reason. Error: 0x80070057 : 'Invalid HTTP headers.'");
  } // FullCorsRequestWithHostHeaderFails

  BEGIN_TEST_METHOD_ATTRIBUTE(RequestWithProxyAuthorizationHeaderFails)
  END_TEST_METHOD_ATTRIBUTE()
  TEST_METHOD(RequestWithProxyAuthorizationHeaderFails)
  {
    ServerParams serverArgs(s_mockServerHost);
    serverArgs.PreflightHeaders[s_accessControlRequestHeaders] = L"Content-Type";
    serverArgs.PreflightHeaders[s_accessControlAllowHeaders] = L"Content-Type";
    serverArgs.ResponseStatus = HttpStatusCode::Accepted;

    ClientParams clientArgs(
        L"GET", {{L"Content-Type", L"application/text"}, {L"Proxy-Authorization", L"Basic Zm9vOmJhcg=="}});

    SetRuntimeOptionString("Http.GlobalOrigin", serverArgs.Url.c_str());
    SetRuntimeOptionInt("Http.OriginPolicy", static_cast<int32_t>(OriginPolicy::CrossOriginResourceSharing));

    TestOriginPolicy(serverArgs, clientArgs, false /*shouldSucceed*/);
  } // RequestWithProxyAuthorizationHeaderFails

  // clang-format on
};
#pragma endregion Former Integration Tests

} // namespace Microsoft::React::Test
