// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include <CppUnitTest.h>

#include <BeastWebSocketResource.h>

using namespace boost::beast;
using namespace Microsoft::React;
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

using std::string;

namespace Microsoft::React::Test {

// We turn clang format off here because it does not work with some of the
// test macros.
// clang-format off

TEST_CLASS(WebSocketTest) {
  TEST_METHOD(WebSocketTest_CreateAndSetHandlers) {
    auto ws = std::make_shared<Beast::WebSocketResource>(Url("ws://validurl:0/"));
    Assert::IsFalse(nullptr == ws);
    ws->SetOnConnect([]() {});
    ws->SetOnPing([]() {});
    ws->SetOnSend([](size_t length) {});
    ws->SetOnMessage([](std::size_t length, const string &buffer) {});
    ws->SetOnClose([](IWebSocketResource::CloseCode, const string &) {});
    ws->SetOnError([](const IWebSocketResource::Error &error) {});
  }

  // TODO: Implement Protocol and Socket mocks.
  void ConnectAndClose()
  // TEST_METHOD(WebSocketTest_ConnectAndClose)
  {
    auto ws = std::make_shared<Beast::WebSocketResource>(Url("ws://localhost:5555/"));
    bool connected = false;
    ws->SetOnConnect([&connected]() { connected = true; });

    ws->Connect({}, {});
    ws->Close(IWebSocketResource::CloseCode::Normal, "BECAUSE!");

    Assert::IsTrue(connected);
  }

  // TODO: Implement Protocol and Socket mocks.
  void SendAndReceive()
  // TEST_METHOD(WebSocketTest_SendAndReceive)
  {
    auto ws = std::make_shared<Beast::WebSocketResource>(Url("ws://localhost:5555/"));
    ws->SetOnMessage([](size_t size, const string &message) {
      // EXPECT_EQ("uppercaseme_response", ss.str());//TODO: Check test server.
      // Sending back "hello".
      Assert::AreEqual(
          string("hello"),
          message); // TODO: Check test server. Sending back "hello".
    });

    ws->Connect({}, {});
    ws->Send("uppercaseme");
    ws->Close(IWebSocketResource::CloseCode::Normal, "");
  }
};

// clang-format on

} // namespace Microsoft::React::Test
