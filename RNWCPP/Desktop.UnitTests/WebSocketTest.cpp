// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include <CppUnitTest.h>

#include <WebSocket.h>

using namespace boost::beast;
using namespace facebook::react;
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

using std::string;

TEST_CLASS(WebSocketTest)
{
  TEST_METHOD(WebSocketTest_CreateAndSetHandlers)
  {
    auto ws = std::make_shared<WebSocket<boost::asio::ip::tcp>>(Url("ws://validurl:0/"));

    Assert::IsFalse(nullptr == ws);
    ws->SetOnConnect([](){});
    ws->SetOnPing([](){});
    ws->SetOnSend([](size_t length){});
    ws->SetOnMessage([](std::size_t length, const string& buffer){});
    ws->SetOnClose([](IWebSocket::CloseCode, const string&){});
    ws->SetOnError([](const IWebSocket::Error& error){});
  }

  // TODO: Implement Protocol and Socket mocks.
  void ConnectAndClose()
    //TEST_METHOD(WebSocketTest_ConnectAndClose)
  {
    auto ws = std::make_shared<WebSocket<boost::asio::ip::tcp>>(Url("ws://localhost:5555/"));
    bool connected = false;
    ws->SetOnConnect([&connected]()
    {
      connected = true;
    });

    ws->Connect({}, {});
    ws->Close(IWebSocket::CloseCode::Normal, "BECAUSE!");

    Assert::IsTrue(connected);
  }

  // TODO: Implement Protocol and Socket mocks.
  void SendAndReceive()
    //TEST_METHOD(WebSocketTest_SendAndReceive)
  {
    auto ws = std::make_shared<WebSocket<boost::asio::ip::tcp>>(Url("ws://localhost:5555/"));
    ws->SetOnMessage([](size_t size, const string& message)
    {
      //EXPECT_EQ("uppercaseme_response", ss.str());//TODO: Check test server. Sending back "hello".
      Assert::AreEqual(string("hello"), message);//TODO: Check test server. Sending back "hello".
    });

    ws->Connect({}, {});
    ws->Send("uppercaseme");
    ws->Close(IWebSocket::CloseCode::Normal, "");
  }
};
