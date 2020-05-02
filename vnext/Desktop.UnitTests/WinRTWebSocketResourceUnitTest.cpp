// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include <CppUnitTest.h>
#include <WinRTWebSocketResource.h>
#include <winrt/Windows.Foundation.h> // WinRT fundamentals, i.e. coroutines impl.
#include "WinRTNetworkingMocks.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace winrt::Windows::Foundation;
using namespace winrt::Windows::Networking::Sockets;

using std::make_shared;
using std::string;
using winrt::event_token;
using winrt::param::hstring;

using CertExceptions = std::vector<winrt::Windows::Security::Cryptography::Certificates::ChainValidationResult>;
using CloseCode = Microsoft::React::IWebSocketResource::CloseCode;
using Error = Microsoft::React::IWebSocketResource::Error;

namespace {

IAsyncAction DoNothingAsync() {
  co_return;
}

IAsyncAction ThrowAsync() {
  throw winrt::hresult_error(winrt::hresult(E_FAIL), L"Expected Failure");
}

} // namespace

namespace Microsoft::React::Test {

TEST_CLASS (WinRTWebSocketResourceUnitTest) {
  TEST_METHOD(ConnectSucceeds) {
    bool connected = true;
    string errorMessage;
    auto imws{winrt::make<MockMessageWebSocket>()};

    // Set up mocks
    auto mws{imws.as<MockMessageWebSocket>()};
    // TODO: Mock Control()
    mws->Mocks.ConnectAsync = [](const Uri &) -> IAsyncAction { return DoNothingAsync(); };
    mws->Mocks.MessageReceivedToken =
        [](TypedEventHandler<MessageWebSocket, MessageWebSocketMessageReceivedEventArgs> const &) -> event_token {
      return event_token{};
    };
    mws->Mocks.Close = [](uint16_t, const hstring &) {};

    // Test APIs
    auto rc =
        make_shared<WinRTWebSocketResource>(std::move(imws), MockDataWriter{}, Uri{L"ws://host:0"}, CertExceptions{});
    rc->SetOnConnect([&connected]() { connected = true; });
    rc->SetOnError([&errorMessage](Error &&error) { errorMessage = error.Message; });

    rc->Connect({}, {});
    rc->Close(CloseCode::Normal, {});

    Assert::AreEqual({}, errorMessage);
    Assert::IsTrue(connected);
  }

  // TODO: Re-enable. Fails in x86|Release.
  //      Hangs due to exception not being caught within WinRTWebSocketResource::PerformConnect.
  BEGIN_TEST_METHOD_ATTRIBUTE(ConnectFails)
  TEST_IGNORE()
  END_TEST_METHOD_ATTRIBUTE()
  TEST_METHOD(ConnectFails) {
    bool connected = false;
    string errorMessage;
    auto imws{winrt::make<MockMessageWebSocket>()};

    // Set up mocks
    auto mws{imws.as<MockMessageWebSocket>()};
    mws->Mocks.ConnectAsync = [](const Uri &) -> IAsyncAction { return ThrowAsync(); };
    mws->Mocks.MessageReceivedToken =
        [](TypedEventHandler<MessageWebSocket, MessageWebSocketMessageReceivedEventArgs> const &) -> event_token {
      return event_token{};
    };
    mws->Mocks.Close = [](uint16_t, const hstring &) {};

    // Test APIs
    auto rc =
        make_shared<WinRTWebSocketResource>(std::move(imws), MockDataWriter{}, Uri{L"ws://host:0"}, CertExceptions{});
    rc->SetOnConnect([&connected]() { connected = true; });
    rc->SetOnError([&errorMessage](Error &&error) { errorMessage = error.Message; });

    rc->Connect({}, {});
    rc->Close(CloseCode::Normal, {});

    Assert::AreNotEqual({}, errorMessage);
    Assert::IsFalse(connected);
  }
};

} // namespace Microsoft::React::Test
