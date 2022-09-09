// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include <CppUnitTest.h>

#include <Networking/RedirectHttpFilter.h>
#include "../Shared/Networking/WinRTTypes.h"
#include "WinRTNetworkingMocks.h"

// Windows API
#include <WinInet.h>
#include <winrt/Windows.Web.Http.Headers.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace winrt::Windows::Web::Http;

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

TEST_CLASS (RedirectHttpFilterUnitTest) {
  TEST_CLASS_INITIALIZE(Initialize) {
    winrt::uninit_apartment(); // Why does this work?
  }

  TEST_METHOD(QueryInterfacesSucceeds) {
    auto filter = winrt::make<RedirectHttpFilter>();

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

    auto filter = winrt::make<RedirectHttpFilter>(std::move(mockFilter1), std::move(mockFilter2));
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

    auto filter = winrt::make<RedirectHttpFilter>(std::move(mockFilter1), std::move(mockFilter2));
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

  TEST_METHOD(MaxAllowedRedirectsSucceeds) {
    auto url1 = L"http://initialhost";
    auto url2 = L"http://redirecthost";
    auto mockFilter1 = winrt::make<MockHttpBaseFilter>();
    auto mockFilter2 = winrt::make<MockHttpBaseFilter>();

    mockFilter1.as<MockHttpBaseFilter>()->Mocks.SendRequestAsync =
        [&url2](HttpRequestMessage const &request) -> ResponseOperation {
      HttpResponseMessage response;

      auto uri2 = Uri{url2 + winrt::to_hstring(L"?redirCount=1")};
      response.Headers().Location(uri2);
      response.StatusCode(HttpStatusCode::MovedPermanently);
      response.Content(HttpStringContent{L""});

      co_return response;
    };
    mockFilter2.as<MockHttpBaseFilter>()->Mocks.SendRequestAsync =
        [&url2](HttpRequestMessage const &request) -> ResponseOperation {
      HttpResponseMessage response;

      auto query = request.RequestUri().QueryParsed().GetFirstValueByName(L"redirCount");
      auto redirCount = std::stoi(query.c_str());
      if (redirCount >= 3) {
        response.StatusCode(HttpStatusCode::Ok);
        response.Content(HttpStringContent{L"Response Content"});
      } else {
        redirCount++;
        response.Headers().Location(Uri{url2 + winrt::to_hstring(L"?redirCount=") + winrt::to_hstring(redirCount)});
        response.StatusCode(HttpStatusCode::MovedPermanently);
        response.Content(HttpStringContent{L"Redirecting: " + winrt::to_hstring(redirCount)});
      }

      co_return response;
    };

    auto filter = winrt::make<RedirectHttpFilter>(3, std::move(mockFilter1), std::move(mockFilter2));
    auto client = HttpClient{filter};
    auto request = HttpRequestMessage(HttpMethod::Get(), Uri{url1});
    auto sendOp = client.SendRequestAsync(request);
    sendOp.get();
    auto response = sendOp.GetResults();

    Assert::AreEqual(HttpStatusCode::Ok, response.StatusCode());

    auto contentOp = response.Content().ReadAsStringAsync();
    contentOp.get();
    auto content = contentOp.GetResults();
    Assert::AreEqual(L"Response Content", content.c_str());
  }

  TEST_METHOD(TooManyRedirectsFails) {
    constexpr size_t maxRedirects = 2;

    auto url1 = L"http://initialhost";
    auto url2 = L"http://redirecthost";
    auto mockFilter1 = winrt::make<MockHttpBaseFilter>();
    auto mockFilter2 = winrt::make<MockHttpBaseFilter>();

    mockFilter1.as<MockHttpBaseFilter>()->Mocks.SendRequestAsync =
        [&url2](HttpRequestMessage const &request) -> ResponseOperation {
      HttpResponseMessage response;

      auto uri2 = Uri{url2 + winrt::to_hstring(L"?redirCount=1")};
      response.Headers().Location(uri2);
      response.StatusCode(HttpStatusCode::MovedPermanently);
      response.Content(HttpStringContent{L""});

      co_return response;
    };
    mockFilter2.as<MockHttpBaseFilter>()->Mocks.SendRequestAsync =
        [&url2](HttpRequestMessage const &request) -> ResponseOperation {
      HttpResponseMessage response;

      auto query = request.RequestUri().QueryParsed().GetFirstValueByName(L"redirCount");
      auto redirCount = std::stoi(query.c_str());
      if (redirCount > maxRedirects) {
        response.StatusCode(HttpStatusCode::Ok);
        response.Content(HttpStringContent{L"Response Content"});
      } else {
        redirCount++;
        response.Headers().Location(Uri{url2 + winrt::to_hstring(L"?redirCount=") + winrt::to_hstring(redirCount)});
        response.StatusCode(HttpStatusCode::MovedPermanently);
        response.Content(HttpStringContent{L"Redirecting: " + winrt::to_hstring(redirCount)});
      }

      co_return response;
    };

    auto filter = winrt::make<RedirectHttpFilter>(maxRedirects, std::move(mockFilter1), std::move(mockFilter2));
    auto client = HttpClient{filter};
    auto request = HttpRequestMessage(HttpMethod::Get(), Uri{url1});
    ResponseOperation sendOp = nullptr;
    long code = 0;
    winrt::hstring errorMessage{};
    try {
      sendOp = client.SendRequestAsync(request);
      sendOp.get();
    } catch (const winrt::hresult_error &e) {
      code = e.code();
      errorMessage = e.message();
    }

    Assert::AreEqual(HRESULT_FROM_WIN32(ERROR_HTTP_REDIRECT_FAILED), code);
    Assert::AreEqual(L"Too many redirects", errorMessage.c_str());
  }
};

} // namespace Microsoft::React::Test
