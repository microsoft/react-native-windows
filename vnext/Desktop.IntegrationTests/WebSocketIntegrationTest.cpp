// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

// clang-format off
#include <CppUnitTest.h>
#include <IWebSocketResource.h>
#include <Test/WebSocketServer.h>

// Standard Library
#include <future>

using namespace Microsoft::React;
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

using std::chrono::milliseconds;
using std::make_shared;
using std::promise;
using std::string;
using std::vector;

using CloseCode = IWebSocketResource::CloseCode;
using Error = IWebSocketResource::Error;

TEST_CLASS (WebSocketIntegrationTest)
{
  void SendReceiveCloseBase(bool isSecure)
  {
    auto server = make_shared<Test::WebSocketServer>(5556, isSecure);
    server->SetMessageFactory([](string&& message)
    {
      return message + "_response";
    });
    string serverError;
    server->SetOnError([&serverError](Error&& err)
    {
      serverError = err.Message;
    });

    string scheme = "ws";
    if (isSecure)
      scheme += "s";
    auto ws = IWebSocketResource::Make(scheme + "://localhost:5556/");
    promise<size_t> sentSizePromise;
    ws->SetOnSend([&sentSizePromise](size_t size)
    {
      sentSizePromise.set_value(size);
    });
    promise<string> receivedPromise;
    ws->SetOnMessage([&receivedPromise](size_t size, const string& message, bool isBinary)
    {
      receivedPromise.set_value(message);
    });
    string clientError{};
    ws->SetOnError([&clientError, &sentSizePromise, &receivedPromise](Error err)
    {
      clientError = err.Message;
      sentSizePromise.set_value(0);
      receivedPromise.set_value("");
    });

    server->Start();
    string sent = "prefix";
    auto expectedSize = sent.size();
    ws->Connect();
    ws->Send(std::move(sent));

    // Block until response is received. Fail in case of a remote endpoint failure.
    auto sentSizeFuture = sentSizePromise.get_future();
    sentSizeFuture.wait();
    auto sentSize = sentSizeFuture.get();
    auto receivedFuture = receivedPromise.get_future();
    receivedFuture.wait();
    string received = receivedFuture.get();
    Assert::AreEqual({}, clientError);

    ws->Close(CloseCode::Normal, "Closing after reading");
    server->Stop();

    Assert::AreEqual({}, serverError);
    Assert::AreEqual({}, clientError);
    Assert::AreEqual(expectedSize, sentSize);
    Assert::AreEqual({"prefix_response"}, received);
  }

  TEST_METHOD(ConnectClose)
  {
    auto server = make_shared<Test::WebSocketServer>(5556);
    auto ws = IWebSocketResource::Make("ws://localhost:5556/");
    Assert::IsFalse(nullptr == ws);
    bool connected = false;
    bool closed = false;
    bool error = false;
    string errorMessage;
    ws->SetOnConnect([&connected]()
    {
      connected = true;
    });
    ws->SetOnClose([&closed](CloseCode code, const string& reason)
    {
      closed = true;
    });
    ws->SetOnError([&errorMessage](Error&& e)
    {
      errorMessage = e.Message;
    });

    server->Start();
    ws->Connect();
    ws->Close(CloseCode::Normal, "Closing");
    server->Stop();

    Assert::AreEqual({}, errorMessage);
    Assert::IsTrue(connected);
    Assert::IsTrue(closed);
  }

  TEST_METHOD(ConnectNoClose)
  {
    bool connected = false;
    bool closed = false;
    string errorMessage;
    auto server = make_shared<Test::WebSocketServer>(5556);
    server->Start();

    // IWebSocketResource scope. Ensures object is closed implicitly by destructor.
    {
      auto ws = IWebSocketResource::Make("ws://localhost:5556");
      ws->SetOnConnect([&connected]()
      {
        connected = true;
      });
      ws->SetOnClose([&closed](CloseCode code, const string& reason)
      {
        closed = true;
      });
      ws->SetOnError([&errorMessage](Error && error)
      {
        errorMessage = error.Message;
      });

      ws->Connect();
      ws->Close();//TODO: Either remove or rename test.
    }

    server->Stop();

    Assert::AreEqual({}, errorMessage);
    Assert::IsTrue(connected);
    Assert::IsTrue(closed);
  }

  BEGIN_TEST_METHOD_ATTRIBUTE(PingClose)
  END_TEST_METHOD_ATTRIBUTE()
  TEST_METHOD(PingClose)
  {
    auto server = make_shared<Test::WebSocketServer>(5556);
    server->Start();

    auto ws = IWebSocketResource::Make("ws://localhost:5556");
    promise<bool> pingPromise;
    ws->SetOnPing([&pingPromise]()
    {
      pingPromise.set_value(true);
    });
    string errorString;
    ws->SetOnError([&errorString](Error err)
    {
      errorString = err.Message;
    });

    ws->Connect();
    ws->Ping();
    auto pingFuture = pingPromise.get_future();
    pingFuture.wait();
    bool pinged = pingFuture.get();
    ws->Close(CloseCode::Normal, "Closing after reading");

    server->Stop();

    Assert::IsTrue(pinged);
    Assert::AreEqual({}, errorString);
  }

  TEST_METHOD(SendReceiveClose)
  {
    SendReceiveCloseBase(/*isSecure*/ false);
  }

  TEST_METHOD(SendReceiveLargeMessage) {
    auto server = make_shared<Test::WebSocketServer>(5556);
    server->SetMessageFactory([](string &&message) { return message + "_response"; });
    auto ws = IWebSocketResource::Make("ws://localhost:5556/");
    promise<string> response;
    ws->SetOnMessage([&response](size_t size, const string &message, bool isBinary) { response.set_value(message); });
    string errorMessage;
    ws->SetOnError([&errorMessage](Error err) { errorMessage = err.Message; });

    server->Start();
    ws->Connect();

    char digits[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};
#define LEN 4096 + 4096 * 2 + 1
    char chars[LEN + 1];
    for (int i = 0; i < LEN; i++) {
      chars[i] = digits[i % 16];
    }
    chars[LEN] = '\0';
    ws->Send(string{chars});

    // Block until response is received. Fail in case of a remote endpoint failure.
    auto future = response.get_future();
    future.wait();
    string result = future.get();

    ws->Close(CloseCode::Normal, "Closing after reading");
    server->Stop();

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
    auto server = make_shared<Test::WebSocketServer>(5556);
    server->SetOnHandshake([server](boost::beast::websocket::response_type &response) {
      auto cookie = response[boost::beast::http::field::cookie].to_string();
      server->SetMessageFactory([cookie](string &&) { return cookie; });
    });
    auto ws = IWebSocketResource::Make("ws://localhost:5556/");
    promise<string> response;
    ws->SetOnMessage([&response](size_t size, const string &message, bool isBinary) { response.set_value(message); });

    server->Start();
    ws->Connect({}, {{L"Cookie", "JSESSIONID=AD9A320CC4034641997FF903F1D10906"}});
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
    SendReceiveCloseBase(/*isSecure*/ true);
  }

  BEGIN_TEST_METHOD_ATTRIBUTE(SendBinary)
  END_TEST_METHOD_ATTRIBUTE()
  TEST_METHOD(SendBinary) {

    auto server = make_shared<Test::WebSocketServer>(5556);

    // The result should be the message "grown" by one element which value is the result's size.
    server->SetMessageFactory([](vector<uint8_t>&& message)
    {
      message.push_back(static_cast<uint8_t>(message.size() + 1));

      return message;
    });

    auto ws = IWebSocketResource::Make("ws://localhost:5556/");

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
    ws->Connect();

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
    auto server = make_shared<Test::WebSocketServer>(5556);
    server->SetMessageFactory([](string&& message)
    {
      return message;
    });
    auto ws = IWebSocketResource::Make("ws://localhost:5556/");

    string expected = "ABCDEFGHIJ";
    string result(expected.size(), '0');
    size_t index = 0;
    promise<void> responsesReceived;
    ws->SetOnMessage([&result, &index, &responsesReceived, count=expected.size()](size_t size, const string& message, bool isBinary)
    {
      result[index++] = message[0];

      if (index == count)
        responsesReceived.set_value();
    });
    string errorMessage;
    ws->SetOnError([&errorMessage](Error err)
    {
      errorMessage = err.Message;
    });

    server->Start();
    ws->Connect();

    // Consecutive immediate writes should be enqueued.
    // The WebSocket library (WinRT or Beast) can't handle multiple write operations
    // concurrently.
    for (size_t i = 0; i < expected.size(); i++)
    {
      ws->Send(string{expected[i]});
    }

    // Block until response is received. Fail in case of a remote endpoint failure.
    responsesReceived.get_future().wait();

    ws->Close(CloseCode::Normal, "Closing");
    server->Stop();

    Assert::AreEqual({}, errorMessage);
    Assert::AreEqual(expected, result);
  }
};
