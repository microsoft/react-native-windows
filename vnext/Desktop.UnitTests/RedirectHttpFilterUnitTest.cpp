// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#undef WINRT_LEAN_AND_MEAN // TODO:Remove

#include <CppUnitTest.h>

#include <Networking/RedirectHttpFilter.h>
#include "WinRTNetworkingMocks.h"

//// Boost Library
//#include <boost/beast/http.hpp>
//
//// Standard Library
//#include <future>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

using std::make_shared;
//using std::promise;//TODO: keep?
using std::string;

using winrt::Windows::Web::Http::Filters::IHttpBaseProtocolFilter;
using winrt::Windows::Web::Http::Filters::IHttpFilter;

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

  TEST_METHOD(ManualRedirectSucceeds) {

    auto mockFilter1 = winrt::make<MockHttpBaseFilter>();
    auto mockFilter2 = winrt::make<MockHttpBaseFilter>();
    //TODO: Setup mocks.

    //auto filter = winrt::make<Networking::RedirectHttpFilter>(std::move(mockFilter1), std::move(mockFilter2));
    auto filter = winrt::make<Networking::RedirectHttpFilter>(
        /*winrt::make<MockHttpBaseFilter>(), winrt::make<MockHttpBaseFilter>()*/);
  }
};

} // namespace
