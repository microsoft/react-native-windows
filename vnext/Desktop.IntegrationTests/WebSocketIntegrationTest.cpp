// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

// cspell:disable

// clang-format off
#include <CppUnitTest.h>
#include <Networking/IWebSocketResource.h>
#include <Test/WebSocketServer.h>

// Standard Library
#include <future>

using namespace Microsoft::React;
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

using std::chrono::milliseconds;
using std::make_shared;
using std::once_flag;
using std::promise;
using std::string;
using std::vector;

using Networking::IWebSocketResource;
using CloseCode = IWebSocketResource::CloseCode;
using Error = IWebSocketResource::Error;

namespace {
void SetPromise(once_flag& flag, promise<void>& prom)
{
  std::call_once(flag, [&prom]()
  {
    prom.set_value();
  });
}

void SetPromise(once_flag& flag, promise<string>& prom, string value)
{
  std::call_once(flag, [&prom, &value]()
  {
    prom.set_value(value);
  });
}

DWORD testTid = 0;
DWORD lambTid = 0;
} // namespace <anonymous>

namespace Microsoft::React::Test {

TEST_CLASS (WebSocketIntegrationTest)
{
  static uint16_t s_port;

  void SendReceiveCloseBase(bool isSecure)
  {
    string scheme = "ws";
    string port = "5555";
    if (isSecure)
    {
      scheme += "s";
      port = "5543";
    }
    auto ws = IWebSocketResource::Make();
    promise<string> receivedPromise;
    ws->SetOnMessage([&receivedPromise](size_t size, const string& message, bool isBinary)
    {
      receivedPromise.set_value(message);
    });
    string clientError{};
    ws->SetOnError([&clientError, &receivedPromise](Error err)
    {
      clientError = err.Message;
      receivedPromise.set_value("");
    });

    string sent = "prefix";
    auto expectedSize = sent.size();
    ws->Connect(scheme + "://localhost:" + port + "/rnw/websockets/echosuffix");
    ws->Send(std::move(sent));

    // Block until response is received. Fail in case of a remote endpoint failure.
    auto receivedFuture = receivedPromise.get_future();
    receivedFuture.wait();
    string received = receivedFuture.get();
    Assert::AreEqual({}, clientError);

    ws->Close(CloseCode::Normal, "Closing after reading");

    Assert::AreEqual({}, clientError);
    Assert::AreEqual({"prefix_response"}, received);
  }

  TEST_METHOD_CLEANUP(MethodCleanup)
  {
    // Bug in WebSocketServer does not correctly release TCP port between test methods.
    // Using a different por per test for now.
    s_port++;
  }

  TEST_METHOD(ConnectClose)
  {
    auto ws = IWebSocketResource::Make();
    Assert::IsFalse(nullptr == ws);
    bool connected = false;
    bool closed = false;
    string errorMessage;
    promise<void> donePromise;
    once_flag doneFlag;
    ws->SetOnConnect([&connected]()
    {
      lambTid = GetCurrentThreadId();
      connected = true;
    });
    ws->SetOnClose([&closed, &doneFlag, &donePromise](CloseCode code, const string& reason)
    {
      lambTid = GetCurrentThreadId();
      closed = true;

      SetPromise(doneFlag, donePromise);
    });
    ws->SetOnError([&errorMessage, &doneFlag, &donePromise](Error&& e)
    {
      errorMessage = e.Message;

      SetPromise(doneFlag, donePromise);
    });

    ws->Connect("ws://localhost:5555");
    ws->Close(CloseCode::Normal, "Closing");

    testTid = GetCurrentThreadId();
    donePromise.get_future().wait();

    Assert::AreEqual({}, errorMessage);
    Assert::IsTrue(connected);
    Assert::IsTrue(closed);
  }

  TEST_METHOD(ConnectNoClose)
  {
    bool connected = false;
    bool closed = false;
    string errorMessage;
    once_flag doneFlag;
    promise<void> donePromise;

    // IWebSocketResource scope. Ensures object is closed implicitly by destructor.
    {
      auto ws = IWebSocketResource::Make();
      ws->SetOnConnect([&connected]()
      {
        connected = true;
      });
      ws->SetOnClose([&closed, &doneFlag, &donePromise](CloseCode code, const string& reason)
      {
        closed = true;

        SetPromise(doneFlag, donePromise);
      });
      ws->SetOnError([&errorMessage, &doneFlag, &donePromise](Error && error)
      {
        errorMessage = error.Message;

        SetPromise(doneFlag, donePromise);
      });

      ws->Connect("ws://localhost:5555");
      ws->Close();//TODO: Either remove or rename test.
    }
    donePromise.get_future().wait();

    Assert::AreEqual({}, errorMessage);
    Assert::IsTrue(connected);
    Assert::IsTrue(closed);
  }

  BEGIN_TEST_METHOD_ATTRIBUTE(PingClose)
  TEST_IGNORE()
  END_TEST_METHOD_ATTRIBUTE()
  TEST_METHOD(PingClose)
  {
    auto ws = IWebSocketResource::Make();
    promise<bool> pingPromise;
    once_flag doneFlag;
    promise<void> donePromise;
    bool pinged = false;
    ws->SetOnMessage([&pinged, &doneFlag, &donePromise](size_t size, const string &message, bool isBinary)
    {
      pinged = true;

      SetPromise(doneFlag, donePromise);
    });
    string errorString;
    ws->SetOnError([&errorString, &doneFlag, &donePromise](Error err)
    {
      errorString = err.Message;

      SetPromise(doneFlag, donePromise);
    });

    ws->Connect("ws://localhost:5555/rnw/websockets/pong");
    ws->Ping();
    donePromise.get_future().wait();
    ws->Close(CloseCode::Normal, "Closing after reading");

    Assert::AreEqual({}, errorString);
    Assert::IsTrue(pinged);
  }

  TEST_METHOD(SendReceiveClose)
  {
    SendReceiveCloseBase(/*isSecure*/ false);
  }

  TEST_METHOD(SendReceiveLargeMessage) {
    auto ws = IWebSocketResource::Make();
    once_flag responseFlag;
    promise<string> responsePromise;
    ws->SetOnMessage([&responseFlag, &responsePromise](size_t size, const string &message, bool isBinary) {
      SetPromise(responseFlag, responsePromise, message);
    });
    string errorMessage;
    ws->SetOnError([&errorMessage, &responseFlag, &responsePromise](Error err) {
      errorMessage = err.Message;
      SetPromise(responseFlag, responsePromise, "");
    });

    ws->Connect("ws://localhost:5555/rnw/websockets/echosuffix");

    char digits[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};
#define LEN 4096 + 4096 * 2 + 1
    char chars[LEN + 1];
    for (int i = 0; i < LEN; i++) {
      chars[i] = digits[i % 16];
    }
    chars[LEN] = '\0';
    ws->Send(string{chars});

    // Block until response is received. Fail in case of a remote endpoint failure.
    auto future = responsePromise.get_future();
    future.wait();
    string result = future.get();

    ws->Close(CloseCode::Normal, "Closing after reading");

    Assert::AreEqual({}, errorMessage);
    Assert::AreEqual(static_cast<size_t>(LEN + string("_response").length()), result.length());
  }

  /*
    Currently, this test requires a modified websocket test server.
    TODO: Write an in-place werver with this behavior:

   server.on('connection', (ws) => {
     ws.on('message', (message) => {
       for (var propName in ws.upgradeReq.headers) {
         console.log(`${propName}: [${ws.upgradeReq.headers[propName]}]`);
       }

       // Send the cookie back to the client.
       ws.send(ws.upgradeReq.headers.cookie);
     });
   });

    Test passes, otherwise.
   */
  BEGIN_TEST_METHOD_ATTRIBUTE(AdditionalHeaders)
  TEST_IGNORE()
  END_TEST_METHOD_ATTRIBUTE()
  TEST_METHOD(AdditionalHeaders) {
    string cookie;
    auto server = make_shared<Test::WebSocketServer>(s_port);
    server->SetOnHandshake([server](boost::beast::websocket::response_type &response) {
      auto cookie = string{response[boost::beast::http::field::cookie]};
      server->SetMessageFactory([cookie](string &&) { return cookie; });
    });
    auto ws = IWebSocketResource::Make();
    promise<string> response;
    ws->SetOnMessage([&response](size_t size, const string &message, bool isBinary) { response.set_value(message); });

    server->Start();
    ws->Connect("ws://localhost:" + std::to_string(s_port), {}, {{L"Cookie", "JSESSIONID=AD9A320CC4034641997FF903F1D10906"}});
    ws->Send("");

    auto future = response.get_future();
    future.wait();
    string result = future.get();

    Assert::AreEqual({"JSESSIONID=AD9A320CC4034641997FF903F1D10906"}, result);

    ws->Close(CloseCode::Normal, "No reason");
    server->Stop();
  }

  BEGIN_TEST_METHOD_ATTRIBUTE(SendReceiveSsl)
  END_TEST_METHOD_ATTRIBUTE()
  TEST_METHOD(SendReceiveSsl)
  {
    auto ws = IWebSocketResource::Make();
    promise<string> receivedPromise;
    ws->SetOnMessage([&receivedPromise](size_t size, const string& message, bool isBinary)
    {
      receivedPromise.set_value(message);
    });
    string clientError{};
    ws->SetOnError([&clientError, &receivedPromise](Error err)
    {
      clientError = err.Message;
      receivedPromise.set_value("");
    });

    string sent = "prefix";
    auto expectedSize = sent.size();
    ws->Connect("wss://localhost:5543/rnw/websockets/echosuffix");
    ws->Send(std::move(sent));

    // Block until response is received. Fail in case of a remote endpoint failure.
    auto receivedFuture = receivedPromise.get_future();
    receivedFuture.wait();
    string received = receivedFuture.get();
    Assert::AreEqual({}, clientError);

    ws->Close(CloseCode::Normal, "Closing after reading");

    Assert::AreEqual({}, clientError);
    Assert::AreEqual({"prefix_response"}, received);
  }

  BEGIN_TEST_METHOD_ATTRIBUTE(SendBinary)
  END_TEST_METHOD_ATTRIBUTE()
  TEST_METHOD(SendBinary) {

    auto server = make_shared<Test::WebSocketServer>(s_port);

    // The result should be the message "grown" by one element which value is the result's size.
    server->SetMessageFactory([](vector<uint8_t>&& message)
    {
      message.push_back(static_cast<uint8_t>(message.size() + 1));

      return message;
    });

    auto ws = IWebSocketResource::Make();

    std::vector<string> messages{
        "AQ==",     // [ 01 ]
        "AQI=",     // [ 01 02 ]
        "AQID",     // [ 01 02 03 ]
        "AQIDBA==", // [ 01, 02, 03, 04 ]
        "AQIDBAU="  // [ 01, 02, 03, 04, 05 ]
    };

    string errorMessage;
    promise<string> responsePromise;
    ws->SetOnMessage([&responsePromise](size_t size, const string &messageIn, bool isBinary)
    {
      responsePromise.set_value(messageIn);
    });
    ws->SetOnError([&errorMessage](Error error)
    {
      errorMessage = error.Message;
    });

    server->Start();
    ws->Connect("ws://localhost:" + std::to_string(s_port));

    // Send all but the last message.
    // Compare result with the next message in the sequence.
    for (size_t i = 0; i < messages.size() - 1; i++)
    {
      ws->SendBinary(string{messages[i]});
      auto future = responsePromise.get_future();
      future.wait();
      string response = future.get();
      responsePromise = promise<string>(); // Reset promise.

      // Expect the encoded value of the next element.
      Assert::AreEqual(messages[i+1], response);
    }

    ws->Close(CloseCode::Normal, "Closing after reading");
    server->Stop();

    Assert::AreEqual({}, errorMessage);
  }

  TEST_METHOD(SendConsecutive)
  {
    auto ws = IWebSocketResource::Make();

    string expected = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    string result(expected.size(), '0');
    size_t index = 0;
    once_flag doneFlag;
    promise<void> donePromise;
    ws->SetOnMessage([&result, &index, &doneFlag, &donePromise, count=expected.size()](size_t size, const string& message, bool isBinary)
    {
      result[index++] = message[0];

      if (index == count)
        SetPromise(doneFlag, donePromise);
    });
    string errorMessage;
    ws->SetOnError([&errorMessage, &doneFlag, &donePromise](Error err)
    {
      errorMessage = err.Message;

      SetPromise(doneFlag, donePromise);
    });

    ws->Connect("ws://localhost:5555/rnw/websockets/echo");

    // Consecutive immediate writes should be enqueued.
    // The WebSocket library (WinRT or Beast) can't handle multiple write operations
    // concurrently.
    for (size_t i = 0; i < expected.size(); i++)
    {
      ws->Send(string{expected[i]});
    }

    // Block until response is received. Fail in case of a remote endpoint failure.
    donePromise.get_future().wait();

    ws->Close(CloseCode::Normal, "Closing");

    Assert::AreEqual({}, errorMessage);
    Assert::AreEqual(expected, result);
  }

  BEGIN_TEST_METHOD_ATTRIBUTE(AbruptDisconnectFailsWithSpecificMessage)
  TEST_IGNORE() //TODO: Find a way to emulate abrupt disconnection using Test::WebSocketServer
  END_TEST_METHOD_ATTRIBUTE()
  TEST_METHOD(AbruptDisconnectFailsWithSpecificMessage)
  {
    promise<void> closedPromise;
    auto ws = IWebSocketResource::Make();
    IWebSocketResource::Error error{{}, IWebSocketResource::ErrorType::None};
    CloseCode closeCode;
    ws->SetOnClose([&closedPromise, &closeCode](CloseCode code, const string& reason)
    {
      closeCode = code;
      closedPromise.set_value();
    });
    ws->SetOnError([&closedPromise, &error](IWebSocketResource::Error&& wsError)
    {
      error = std::move(wsError);
    });

    ws->Connect("ws://localhost:5555");

    closedPromise.get_future().wait();

    //NOTE: This message is implementation-specific (WinRTWebSocketResource)
    Assert::AreEqual({"[0x80072EFE] Underlying TCP connection suddenly terminated"}, error.Message);
    Assert::AreEqual(static_cast<size_t>(IWebSocketResource::ErrorType::Connection), static_cast<size_t>(error.Type));
    Assert::AreEqual(static_cast<size_t>(CloseCode::BadPayload), static_cast<size_t>(closeCode));
  }
};

uint16_t WebSocketIntegrationTest::s_port = 6666;

} // namespace Microsoft::React::Test
