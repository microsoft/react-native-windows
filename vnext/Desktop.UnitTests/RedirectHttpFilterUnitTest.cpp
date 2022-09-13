// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include <CppUnitTest.h>

#include <Networking/RedirectHttpFilter.h>
#include <Networking/WinRTTypes.h>
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

    auto filter = winrt::make<RedirectHttpFilter>(maxRedirects, std::move(mockFilter1), std::move(mockFilter2));
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
};

} // namespace Microsoft::React::Test
