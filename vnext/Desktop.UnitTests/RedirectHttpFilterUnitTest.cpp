// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include <CppUnitTest.h>

#include <Networking/RedirectHttpFilter.h>
#include <Networking/WinRTTypes.h>
#include "WinRTNetworkingMocks.h"

// Windows API
#include <Windows.h>
// Leaving a line so clang-format does not mess up include ordering
#include <WinInet.h>
#include <winrt/Windows.Web.Http.Headers.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace winrt::Windows::Web::Http;

using Microsoft::React::Networking::IWinRTHttpRequestFactory;
using Microsoft::React::Networking::RedirectHttpFilter;
using Microsoft::React::Networking::ResponseOperation;
using winrt::Windows::Foundation::Uri;
using winrt::Windows::Web::Http::Filters::IHttpBaseProtocolFilter;
using winrt::Windows::Web::Http::Filters::IHttpFilter;

namespace Microsoft::VisualStudio::CppUnitTestFramework {

template <>
std::wstring ToString<HttpStatusCode>(const HttpStatusCode &status) {
  return ToString(static_cast<int32_t>(status));
}

} // namespace Microsoft::VisualStudio::CppUnitTestFramework

namespace Microsoft::React::Test {

namespace {

// Mimics WinRTHttpResource::CreateRequest, which rebuilds every header and the body from the original request
// arguments on each redirect hop.
struct RebuildingRequestFactory : IWinRTHttpRequestFactory {
  winrt::Windows::Foundation::IAsyncOperation<HttpRequestMessage> CreateRequest(
      HttpMethod &&method,
      Uri &&uri,
      winrt::Windows::Foundation::Collections::IMap<winrt::hstring, winrt::Windows::Foundation::IInspectable> props,
      bool includeContent) noexcept override {
    IncludeContentCalls.push_back(includeContent);

    auto request = HttpRequestMessage{std::move(method), std::move(uri)};
    for (auto prop : props) {
      request.Properties().Insert(prop.Key(), prop.Value());
    }
    request.Headers().TryAppendWithoutValidation(L"Authorization", L"Bearer abc123");
    request.Headers().Append(L"Cookie", L"name=value");
    request.Headers().Append(L"X-Custom", L"custom");
    if (includeContent) {
      request.Content(HttpStringContent{L"body"});
    }

    co_return request;
  }

  std::vector<bool> IncludeContentCalls;
};

struct CountingRedirectEventSource : Microsoft::React::Networking::IRedirectEventSource {
  bool OnRedirecting(HttpRequestMessage const &, HttpResponseMessage const &) noexcept override {
    ++Calls;
    return true;
  }

  int Calls{0};
};

struct ObservedRequest {
  std::wstring Host;
  std::wstring Method;
  bool HasAuthorization{false};
  bool HasCookie{false};
  bool HasCustom{false};
  bool HasContent{false};
};

ObservedRequest Observe(HttpRequestMessage const &request) {
  ObservedRequest result;
  result.Host = request.RequestUri().Host().c_str();
  result.Method = request.Method().Method().c_str();
  result.HasAuthorization = request.Headers().HasKey(L"Authorization");
  result.HasCookie = request.Headers().HasKey(L"Cookie");
  result.HasCustom = request.Headers().HasKey(L"X-Custom");
  result.HasContent = request.Content() != nullptr;

  return result;
}

// Sends a request through a RedirectHttpFilter. Each entry of `hops` is {status, location} returned for the
// corresponding request; the request following the last hop gets 200 OK.
std::vector<ObservedRequest> RunRedirects(
    HttpMethod const &method,
    wchar_t const *initialUrl,
    std::vector<std::pair<HttpStatusCode, wchar_t const *>> const &hops,
    bool useFactory,
    std::vector<bool> *includeContentCalls = nullptr) {
  auto observed = std::make_shared<std::vector<ObservedRequest>>();
  auto handler = [observed, hops](HttpRequestMessage const &request) -> ResponseOperation {
    auto index = observed->size();
    observed->push_back(Observe(request));

    HttpResponseMessage response;
    if (index < hops.size()) {
      response.StatusCode(hops[index].first);
      response.Headers().Location(Uri{hops[index].second});
    } else {
      response.StatusCode(HttpStatusCode::Ok);
    }
    response.Content(HttpStringContent{L""});

    co_return response;
  };

  auto mockFilter1 = winrt::make<MockHttpBaseFilter>();
  auto mockFilter2 = winrt::make<MockHttpBaseFilter>();
  mockFilter1.as<MockHttpBaseFilter>()->Mocks.SendRequestAsync = handler;
  mockFilter2.as<MockHttpBaseFilter>()->Mocks.SendRequestAsync = handler;

  auto filter = winrt::make<RedirectHttpFilter>(std::move(mockFilter1), std::move(mockFilter2), winrt::hstring{});
  auto factory = std::make_shared<RebuildingRequestFactory>();
  if (useFactory) {
    filter.as<RedirectHttpFilter>()->SetRequestFactory(std::weak_ptr<IWinRTHttpRequestFactory>{factory});
  }

  auto request = HttpRequestMessage{method, Uri{initialUrl}};
  request.Headers().TryAppendWithoutValidation(L"Authorization", L"Bearer abc123");
  request.Headers().Append(L"Cookie", L"name=value");
  request.Headers().Append(L"X-Custom", L"custom");
  if (method.Method() != L"GET") {
    request.Content(HttpStringContent{L"body"});
  }

  auto response = filter.SendRequestAsync(request).get();
  Assert::AreEqual(HttpStatusCode::Ok, response.StatusCode());

  if (includeContentCalls) {
    *includeContentCalls = factory->IncludeContentCalls;
  }

  return *observed;
}

} // namespace

TEST_CLASS (RedirectHttpFilterUnitTest) {
  TEST_CLASS_INITIALIZE(Initialize) {
    winrt::uninit_apartment(); // Why does this work?
  }

  TEST_METHOD(QueryInterfacesSucceeds) {
    auto filter = winrt::make<RedirectHttpFilter>(winrt::hstring{});

    auto iFilter = filter.try_as<IHttpFilter>();
    Assert::IsFalse(iFilter == nullptr);
    auto baseFilter = filter.try_as<IHttpBaseProtocolFilter>();
    Assert::IsFalse(baseFilter == nullptr);
  }

  TEST_METHOD(AutomaticRedirectSucceeds) {
    auto url1 = L"http://initialhost";
    auto url2 = L"http://redirecthost";
    auto mockFilter1 = winrt::make<MockHttpBaseFilter>();
    auto mockFilter2 = winrt::make<MockHttpBaseFilter>();
    mockFilter1.as<MockHttpBaseFilter>()->Mocks.SendRequestAsync =
        [&url2](HttpRequestMessage const &request) -> ResponseOperation {
      HttpResponseMessage response{};

      if (request.RequestUri().Host() == L"initialhost") {
        response.StatusCode(HttpStatusCode::MovedPermanently);
        response.Headers().Location(Uri{url2});
        response.Content(HttpStringContent{L""});
      } else {
        response.StatusCode(HttpStatusCode::BadRequest);
        response.Content(HttpStringContent{L""});
      }

      co_return response;
    };
    mockFilter2.as<MockHttpBaseFilter>()->Mocks.SendRequestAsync =
        [](HttpRequestMessage const &request) -> ResponseOperation {
      HttpResponseMessage response;

      if (request.RequestUri().Host() == L"redirecthost") {
        response.StatusCode(HttpStatusCode::Ok);
        response.Content(HttpStringContent{L"Response Content"});
      } else {
        response.StatusCode(HttpStatusCode::BadRequest);
        response.Content(HttpStringContent{L""});
      }

      co_return response;
    };

    auto filter = winrt::make<RedirectHttpFilter>(std::move(mockFilter1), std::move(mockFilter2), winrt::hstring{});
    auto client = HttpClient{filter};
    auto request = HttpRequestMessage{HttpMethod::Get(), Uri{url1}};
    auto sendOp = client.SendRequestAsync(request);
    sendOp.get();
    auto response = sendOp.GetResults();

    Assert::AreEqual(HttpStatusCode::Ok, response.StatusCode());

    auto contentOp = response.Content().ReadAsStringAsync();
    contentOp.get();
    auto content = contentOp.GetResults();
    Assert::AreEqual(L"Response Content", content.c_str());
  }

  TEST_METHOD(ManualRedirectSucceeds) {
    auto url1 = L"http://initialhost";
    auto url2 = L"http://redirecthost";
    auto mockFilter1 = winrt::make<MockHttpBaseFilter>();
    auto mockFilter2 = winrt::make<MockHttpBaseFilter>();
    mockFilter1.as<MockHttpBaseFilter>()->Mocks.SendRequestAsync =
        [&url2](HttpRequestMessage const &request) -> ResponseOperation {
      HttpResponseMessage response{};

      if (request.RequestUri().Host() == L"initialhost") {
        response.StatusCode(HttpStatusCode::MovedPermanently);
        response.Headers().Location(Uri{url2});
        response.Content(HttpStringContent{L""});
      } else if (request.RequestUri().Host() == L"redirecthost") {
        response.StatusCode(HttpStatusCode::Ok);
        response.Content(HttpStringContent{L"Response Content"});
      }

      co_return response;
    };

    auto filter = winrt::make<RedirectHttpFilter>(std::move(mockFilter1), std::move(mockFilter2), winrt::hstring{});
    // Disable automatic redirect
    filter.try_as<IHttpBaseProtocolFilter>().AllowAutoRedirect(false);

    auto client = HttpClient{filter};
    auto request = HttpRequestMessage{HttpMethod::Get(), Uri{url1}};
    auto sendOp = client.SendRequestAsync(request);
    sendOp.get();
    auto response = sendOp.GetResults();

    Assert::AreEqual(HttpStatusCode::MovedPermanently, response.StatusCode());

    request = HttpRequestMessage{response.RequestMessage().Method(), response.Headers().Location()};
    sendOp = client.SendRequestAsync(request);
    sendOp.get();
    response = sendOp.GetResults();

    Assert::AreEqual(HttpStatusCode::Ok, response.StatusCode());

    auto contentOp = response.Content().ReadAsStringAsync();
    contentOp.get();
    auto content = contentOp.GetResults();
    Assert::AreEqual(L"Response Content", content.c_str());
  }

  void TestRedirectCount(size_t maxRedirects, size_t actualRedirects) {
    auto url1 = L"http://initialhost";
    auto url2 = L"http://redirecthost";
    auto mockFilter1 = winrt::make<MockHttpBaseFilter>();
    auto mockFilter2 = winrt::make<MockHttpBaseFilter>();

    size_t redirectCount = 0;
    mockFilter1.as<MockHttpBaseFilter>()->Mocks.SendRequestAsync =
        [&url2, &redirectCount](auto const &request) -> ResponseOperation {
      HttpResponseMessage response;

      response.Headers().Location(Uri{url2});
      response.StatusCode(HttpStatusCode::MovedPermanently);
      redirectCount++;

      co_return response;
    };
    mockFilter2.as<MockHttpBaseFilter>()->Mocks.SendRequestAsync =
        [&url2, &redirectCount, actualRedirects](auto const &request) -> ResponseOperation {
      HttpResponseMessage response;

      if (redirectCount >= actualRedirects) {
        response.StatusCode(HttpStatusCode::Ok);
        response.Content(HttpStringContent{L"Response Content"});
      } else {
        response.Headers().Location(Uri{url2});
        response.StatusCode(HttpStatusCode::MovedPermanently);
        redirectCount++;
      }

      co_return response;
    };

    auto filter =
        winrt::make<RedirectHttpFilter>(maxRedirects, std::move(mockFilter1), std::move(mockFilter2), winrt::hstring{});
    auto client = HttpClient{filter};
    auto request = HttpRequestMessage{HttpMethod::Get(), Uri{url1}};
    ResponseOperation sendOp = nullptr;
    long errorCode = 0;
    winrt::hstring errorMessage{};
    try {
      sendOp = client.SendRequestAsync(request);
      sendOp.get();
    } catch (const winrt::hresult_error &e) {
      errorCode = e.code();
      errorMessage = e.message();
    }

    if (maxRedirects >= actualRedirects) {
      // Should succeed
      auto response = sendOp.GetResults();

      Assert::AreEqual(0, (int)errorCode);
      Assert::AreEqual(L"", errorMessage.c_str());
      Assert::AreEqual(HttpStatusCode::Ok, response.StatusCode());

      auto contentOp = response.Content().ReadAsStringAsync();
      contentOp.get();
      auto content = contentOp.GetResults();
      Assert::AreEqual(L"Response Content", content.c_str());
    } else {
      // Should fail
      Assert::AreEqual(HRESULT_FROM_WIN32(ERROR_HTTP_REDIRECT_FAILED), errorCode);
      Assert::AreEqual(L"Too many redirects", errorMessage.c_str());
    }
  }

  TEST_METHOD(MaxAllowedRedirectsSucceeds) {
    TestRedirectCount(3, 3);
  }

  TEST_METHOD(TooManyRedirectsFails) {
    TestRedirectCount(2, 3);
  }

  void TestCrossOriginRemovesCredentials(bool useFactory) {
    // a -> b -> b -> a: Authorization/Cookie are dropped from the first cross-origin hop onward.
    auto requests = RunRedirects(
        HttpMethod::Get(),
        L"https://a.test/",
        {{HttpStatusCode::Found, L"https://b.test/1"},
         {HttpStatusCode::Found, L"https://b.test/2"},
         {HttpStatusCode::Found, L"https://a.test/3"}},
        useFactory);

    Assert::AreEqual(size_t{4}, requests.size());
    Assert::IsTrue(requests[0].HasAuthorization);
    Assert::IsTrue(requests[0].HasCookie);
    for (size_t i = 1; i < requests.size(); ++i) {
      Assert::IsFalse(requests[i].HasAuthorization);
      Assert::IsFalse(requests[i].HasCookie);
      Assert::IsTrue(requests[i].HasCustom);
    }
    Assert::AreEqual(L"b.test", requests[1].Host.c_str());
    Assert::AreEqual(L"b.test", requests[2].Host.c_str());
    Assert::AreEqual(L"a.test", requests[3].Host.c_str());
  }

  TEST_METHOD(CrossOriginRedirectRemovesCredentials) {
    TestCrossOriginRemovesCredentials(true);
  }

  TEST_METHOD(CrossOriginRedirectRemovesCredentialsWithoutFactory) {
    TestCrossOriginRemovesCredentials(false);
  }

  TEST_METHOD(CrossOriginPortOrSchemeRemovesCredentials) {
    auto requests =
        RunRedirects(HttpMethod::Get(), L"http://a.test/", {{HttpStatusCode::Found, L"http://a.test:8080/"}}, true);
    Assert::IsFalse(requests[1].HasAuthorization);

    requests = RunRedirects(HttpMethod::Get(), L"http://a.test/", {{HttpStatusCode::Found, L"https://a.test/"}}, true);
    Assert::IsFalse(requests[1].HasAuthorization);
  }

  TEST_METHOD(SameOriginRedirectKeepsCredentials) {
    auto requests = RunRedirects(
        HttpMethod::Get(),
        L"https://a.test/",
        {{HttpStatusCode::MovedPermanently, L"https://A.TEST/1"}, {HttpStatusCode::Found, L"https://a.test:443/2"}},
        true);

    Assert::AreEqual(size_t{3}, requests.size());
    for (auto &request : requests) {
      Assert::IsTrue(request.HasAuthorization);
      Assert::IsTrue(request.HasCookie);
    }
  }

  void TestMethod(
      HttpMethod const &method,
      HttpStatusCode status,
      wchar_t const *expectedMethod,
      bool expectedContent,
      bool useFactory) {
    auto requests = RunRedirects(method, L"https://a.test/", {{status, L"https://a.test/next"}}, useFactory);

    Assert::AreEqual(size_t{2}, requests.size());
    Assert::AreEqual(method.Method().c_str(), requests[0].Method.c_str());
    Assert::AreEqual(expectedMethod, requests[1].Method.c_str());
    Assert::AreEqual(expectedContent, requests[1].HasContent);
  }

  void TestMethodRules(bool useFactory) {
    TestMethod(HttpMethod::Post(), HttpStatusCode::MovedPermanently, L"GET", false, useFactory);
    TestMethod(HttpMethod::Post(), HttpStatusCode::Found, L"GET", false, useFactory);
    TestMethod(HttpMethod::Post(), HttpStatusCode::SeeOther, L"GET", false, useFactory);
    TestMethod(HttpMethod::Post(), HttpStatusCode::TemporaryRedirect, L"POST", true, useFactory);
    TestMethod(HttpMethod::Post(), HttpStatusCode::PermanentRedirect, L"POST", true, useFactory);
    TestMethod(HttpMethod::Put(), HttpStatusCode::Found, L"PUT", true, useFactory);
    TestMethod(HttpMethod::Put(), HttpStatusCode::SeeOther, L"GET", false, useFactory);
    TestMethod(HttpMethod::Put(), HttpStatusCode::TemporaryRedirect, L"PUT", true, useFactory);
    TestMethod(HttpMethod::Delete(), HttpStatusCode::MovedPermanently, L"DELETE", true, useFactory);
    TestMethod(HttpMethod::Head(), HttpStatusCode::SeeOther, L"HEAD", true, useFactory);
  }

  TEST_METHOD(RedirectMethodRulesFollowFetchSpec) {
    TestMethodRules(true);
  }

  TEST_METHOD(RedirectMethodRulesFollowFetchSpecWithoutFactory) {
    TestMethodRules(false);
  }

  TEST_METHOD(RedirectToGetStaysGetWithoutBody) {
    // POST -302-> GET, then 307 must keep GET and must not restore the body.
    auto requests = RunRedirects(
        HttpMethod::Post(),
        L"https://a.test/",
        {{HttpStatusCode::Found, L"https://a.test/1"}, {HttpStatusCode::TemporaryRedirect, L"https://a.test/2"}},
        true);

    Assert::AreEqual(size_t{3}, requests.size());
    Assert::AreEqual(L"GET", requests[2].Method.c_str());
    Assert::IsFalse(requests[2].HasContent);
  }

  TEST_METHOD(DroppedBodyIsNotRebuilt) {
    // POST -302-> GET -307-> GET: once the body is dropped, the factory is not asked to rebuild it.
    std::vector<bool> includeContentCalls;
    RunRedirects(
        HttpMethod::Post(),
        L"https://a.test/",
        {{HttpStatusCode::Found, L"https://a.test/1"}, {HttpStatusCode::TemporaryRedirect, L"https://a.test/2"}},
        true,
        &includeContentCalls);
    Assert::AreEqual(size_t{2}, includeContentCalls.size());
    Assert::IsFalse(includeContentCalls[0]);
    Assert::IsFalse(includeContentCalls[1]);

    // POST -307-> POST keeps the body.
    RunRedirects(
        HttpMethod::Post(),
        L"https://a.test/",
        {{HttpStatusCode::TemporaryRedirect, L"https://a.test/1"}},
        true,
        &includeContentCalls);
    Assert::AreEqual(size_t{1}, includeContentCalls.size());
    Assert::IsTrue(includeContentCalls[0]);
  }

  TEST_METHOD(RedirectStatusWithoutLocationIsReturned) {
    for (auto status : {HttpStatusCode::MultipleChoices, HttpStatusCode::Found, HttpStatusCode::PermanentRedirect}) {
      int sendCount = 0;
      auto handler = [&sendCount, status](HttpRequestMessage const &) -> ResponseOperation {
        ++sendCount;
        HttpResponseMessage response;
        response.StatusCode(status);
        response.Content(HttpStringContent{L"Final"});
        co_return response;
      };

      auto mockFilter1 = winrt::make<MockHttpBaseFilter>();
      auto mockFilter2 = winrt::make<MockHttpBaseFilter>();
      mockFilter1.as<MockHttpBaseFilter>()->Mocks.SendRequestAsync = handler;
      mockFilter2.as<MockHttpBaseFilter>()->Mocks.SendRequestAsync = handler;

      // With a limit of 0 redirects, any counted redirect would throw "Too many redirects".
      auto filter =
          winrt::make<RedirectHttpFilter>(size_t{0}, std::move(mockFilter1), std::move(mockFilter2), winrt::hstring{});
      auto eventSource = winrt::make_self<CountingRedirectEventSource>();
      winrt::com_ptr<Microsoft::React::Networking::IRedirectEventSource> eventSourceBase;
      eventSourceBase.copy_from(eventSource.get());
      filter.as<RedirectHttpFilter>()->SetRedirectSource(eventSourceBase);

      auto response = filter.SendRequestAsync(HttpRequestMessage{HttpMethod::Get(), Uri{L"https://a.test/"}}).get();

      Assert::AreEqual(status, response.StatusCode());
      Assert::AreEqual(1, sendCount);
      Assert::AreEqual(0, eventSource->Calls);
    }
  }
};

} // namespace Microsoft::React::Test
