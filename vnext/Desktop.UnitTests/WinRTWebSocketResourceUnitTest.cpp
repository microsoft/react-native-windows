// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include <CppUnitTest.h>
#include <WinRTWebSocketResource.h>
#include "WinRTNetworkingMocks.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace winrt::Windows::Foundation;
using namespace winrt::Windows::Networking::Sockets;

using std::make_shared;
using std::string;
using winrt::event_token;
using winrt::param::hstring;

using CertExceptions = std::vector<winrt::Windows::Security::Cryptography::Certificates::ChainValidationResult>;
using Error = Microsoft::React::IWebSocketResource::Error;

namespace Microsoft::React::Test {

TEST_CLASS (WinRTWebSocketResourceUnitTest) {
  TEST_METHOD(ConnectSucceedsCloseSucceeds) {
    bool connected = true;
    string errorMessage;
    auto imws{winrt::make<MockMessageWebSocket>().as<IMessageWebSocket>()};

    auto mws{imws.as<MockMessageWebSocket>()};
    MessageWebSocket m;
    mws->Mocks.Control = [&m]() -> MessageWebSocketControl { return m.Control(); };
    mws->Mocks.ConnectAsync = [](const Uri &) -> IAsyncAction { return IAsyncAction(); };
    mws->Mocks.MessageReceivedToken =
        [](TypedEventHandler<MessageWebSocket, MessageWebSocketMessageReceivedEventArgs> const &) -> event_token {
      return event_token{};
    };
    mws->Mocks.Close = [](uint16_t, const hstring &) {};

    auto rc =
        make_shared<WinRTWebSocketResource>(std::move(imws), MockDataWriter{}, Uri{L"ws://host:0"}, CertExceptions{});

    rc->SetOnConnect([&connected]() { connected = true; });
    rc->SetOnError([&errorMessage](Error &&error) { errorMessage = errorMessage; });

    // Exercise resource methods
    rc->Connect({}, {});

    Assert::AreNotEqual({}, errorMessage);
    Assert::IsTrue(connected);
  }

  TEST_METHOD(ConnectFails) {
    auto mws{winrt::make<MockMessageWebSocket>().as<IMessageWebSocket>()};
    MockDataWriter dw;
    Uri uri(L"ws://host:0");
  }
};

} // namespace Microsoft::React::Test
