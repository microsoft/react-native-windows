// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include <CppUnitTest.h>

#include <WebSocket.h>

using namespace boost::beast;
using namespace facebook::react;
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

using facebook::react::test::TestWebSocket;
using std::make_unique;
using std::string;

TEST_CLASS(BaseWebSocketTest)
{
  TEST_METHOD(CreateAndSetHandlers)
  {
    auto ws = make_unique<TestWebSocket>(Url("ws://validurl:0"));

    Assert::IsFalse(nullptr == ws);
    ws->SetOnConnect([](){});
    ws->SetOnPing([](){});
    ws->SetOnSend([](size_t length){});
    ws->SetOnMessage([](size_t length, const string& buffer){});
    ws->SetOnClose([](IWebSocket::CloseCode, const string&){});
    ws->SetOnError([](const IWebSocket::Error& error){});
  }

  TEST_METHOD(ConnectSucceeds)
  {
    auto ws = make_unique<TestWebSocket>(Url("ws://validurl:0"));
    ws->Connect({}, {});
  }
};
