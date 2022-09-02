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

  TEST_METHOD(QueryInterfacesSucceeds)
  {
    auto filter = winrt::make<Networking::RedirectHttpFilter>();

    auto iFilter = filter.try_as<IHttpFilter>();
    Assert::IsFalse(iFilter == nullptr);
    auto baseFilter = filter.try_as<IHttpBaseProtocolFilter>();
    Assert::IsFalse(baseFilter == nullptr);
  }

  winrt::Windows::Foundation::IAsyncAction SendIt(HttpClient const& client, HttpRequestMessage const& request)
  {
    auto response = co_await client.SendRequestAsync(request);
    auto code = response.StatusCode();
    int x = 99;
  }
  TEST_METHOD(ManualRedirectSucceeds) {
    winrt::uninit_apartment(); // Why does this work?

    auto mockFilter1 = winrt::make<MockHttpBaseFilter>();
    auto mockFilter2 = winrt::make<MockHttpBaseFilter>();

    auto redirectCount = 0;
    auto url1 = L"http://initialhost";
    auto url2 = L"http://redirecthost";
    auto mockPtr1{mockFilter1.as<MockHttpBaseFilter>()};
    mockPtr1->Mocks.SendRequestAsync = [&redirectCount](HttpRequestMessage const &request) -> Networking::ResponseOperation {
      HttpResponseMessage response{};

      if (request.RequestUri().Host() == L"initialhost") {
        response.StatusCode(HttpStatusCode::MovedPermanently);
        response.Content(HttpStringContent{L""});
      }
      else if (request.RequestUri().Host() == L"redirecthost") {
        response.StatusCode(HttpStatusCode::Ok);
        response.Content(HttpStringContent{L"Response Content"});
      }

      co_return response;
    };

    auto filter = winrt::make<Networking::RedirectHttpFilter>(std::move(mockFilter1), std::move(mockFilter2));
    filter.try_as<IHttpBaseProtocolFilter>().AllowAutoRedirect(false);

    auto client = HttpClient{filter};
    auto request = HttpRequestMessage{HttpMethod::Get(), Uri{url1}};

    auto sendOp = client.SendRequestAsync(request);
    sendOp.get();

    auto response = sendOp.GetResults();

    Assert::AreEqual(HttpStatusCode::MovedPermanently, response.StatusCode());
    auto x = response.Headers().Location();
    //TODO: Re-send request.
  }
};

} // namespace
