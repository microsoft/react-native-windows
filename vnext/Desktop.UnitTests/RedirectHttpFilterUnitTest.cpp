// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include <CppUnitTest.h>

#include <Networking/RedirectHttpFilter.h>
#include "../Shared/Networking/WinRTTypes.h"
#include "WinRTNetworkingMocks.h"

#include <winrt/Windows.Web.Http.Headers.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace winrt::Windows::Web::Http;

using std::make_shared;
//using std::promise;//TODO: keep?
using std::string;

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

  TEST_CLASS_INITIALIZE(Initialize)
  {
    winrt::uninit_apartment(); // Why does this work?
  }

  TEST_METHOD(QueryInterfacesSucceeds)
  {
    auto filter = winrt::make<Networking::RedirectHttpFilter>();

    auto iFilter = filter.try_as<IHttpFilter>();
    Assert::IsFalse(iFilter == nullptr);
    auto baseFilter = filter.try_as<IHttpBaseProtocolFilter>();
    Assert::IsFalse(baseFilter == nullptr);
  }

  TEST_METHOD(AutomaticRedirectSucceeds) {
    auto mockFilter1 = winrt::make<MockHttpBaseFilter>();
    auto mockFilter2 = winrt::make<MockHttpBaseFilter>();

    auto url1 = L"http://initialhost";
    auto url2 = L"http://redirecthost";
    mockFilter1.as<MockHttpBaseFilter>()->Mocks.SendRequestAsync =
        [&url2](HttpRequestMessage const &request) -> Networking::ResponseOperation {
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
        [](HttpRequestMessage const &request) -> Networking::ResponseOperation {
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


    auto filter = winrt::make<Networking::RedirectHttpFilter>(std::move(mockFilter1), std::move(mockFilter2));
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
    auto mockFilter1 = winrt::make<MockHttpBaseFilter>();
    auto mockFilter2 = winrt::make<MockHttpBaseFilter>();

    auto url1 = L"http://initialhost";
    auto url2 = L"http://redirecthost";
    auto mockPtr1{mockFilter1.as<MockHttpBaseFilter>()};
    mockFilter1.as<MockHttpBaseFilter>()->Mocks.SendRequestAsync =
        [&url2](HttpRequestMessage const &request) -> Networking::ResponseOperation {
      HttpResponseMessage response{};

      if (request.RequestUri().Host() == L"initialhost") {
        response.StatusCode(HttpStatusCode::MovedPermanently);
        response.Headers().Location(Uri{url2});
        response.Content(HttpStringContent{L""});
      }
      else if (request.RequestUri().Host() == L"redirecthost") {
        response.StatusCode(HttpStatusCode::Ok);
        response.Content(HttpStringContent{L"Response Content"});
      }

      co_return response;
    };

    auto filter = winrt::make<Networking::RedirectHttpFilter>(std::move(mockFilter1), std::move(mockFilter2));
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
};

} // namespace
