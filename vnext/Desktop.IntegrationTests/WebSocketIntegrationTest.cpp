// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include <CppUnitTest.h>
#include <IWebSocket.h>
#include <WebSocketServer.h>
#include "unicode.h"

#include <condition_variable>
#include <future>
#include <mutex>

using namespace Microsoft::React;
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

using std::chrono::milliseconds;
using std::condition_variable;
using std::make_shared;
using std::unique_lock;
using std::lock_guard;
using std::promise;
using std::string;

TEST_CLASS(WebSocketIntegrationTest)
{
  TEST_METHOD(ConnectCloseInProcServer)
  {
    bool connected = false;
    auto server = make_shared<Test::WebSocketServer>(5556);
    server->SetOnConnection([&connected]()
    {
      connected = true;
    });
    server->Start();

    auto ws = IWebSocket::Make("ws://localhost:5556");
    ws->Connect();
    ws->Close(IWebSocket::CloseCode::Normal, "Closing");

    server->Stop();

    Assert::IsTrue(connected);
  }

  TEST_METHOD(SendReceiveInProcServer)
  {
    auto server = make_shared<Test::WebSocketServer>(5556);
    server->SetMessageFactory([](string&& message)
    {
      return message + "_suffix";
    });
    server->Start();

    promise<string> response;
    auto result = response.get_future();
    auto ws = IWebSocket::Make("ws://localhost:5556");
    ws->SetOnMessage([&response](size_t, const string& message)
    {
      response.set_value(message);
    });

    ws->Connect();
    ws->Send("prefix");
    result.wait();
    ws->Close(IWebSocket::CloseCode::Normal, "Closing");

    server->Stop();

    Assert::AreEqual({ "prefix_suffix" }, result.get());
  }

  TEST_METHOD(ConnectClose)
  {
    auto ws = IWebSocket::Make("ws://localhost:5555/");
    Assert::IsFalse(nullptr == ws);
    bool connected = false;
    string message;
    ws->SetOnConnect([&connected]()
    {
      connected = true;
    });

    ws->Connect();
    ws->Close(IWebSocket::CloseCode::Normal, "Closing");

    Assert::IsTrue(connected);
  }

  TEST_METHOD(ConnectNoClose)
  {
    bool connected = false;

    // IWebSocket scope. Ensures object is closed implicitly by destructor.
    {
      auto ws = IWebSocket::Make("ws://localhost:5555/");
      ws->SetOnConnect([&connected]()
      {
        connected = true;
      });

      ws->Connect();
    }

    Assert::IsTrue(connected);
  }

  TEST_METHOD(PingClose)
  {
    auto ws = IWebSocket::Make("ws://localhost:5555");
    promise<bool> pingPromise;
    ws->SetOnPing([&pingPromise]()
    {
      pingPromise.set_value(true);
    });
    string errorString;
    ws->SetOnError([&errorString](IWebSocket::Error err)
    {
      errorString = err.Message;
    });

    ws->Connect();
    ws->Ping();
    auto pingFuture = pingPromise.get_future();
    pingFuture.wait();
    bool pinged = pingFuture.get();

    ws->Close(IWebSocket::CloseCode::Normal, "Closing after reading");

    Assert::IsTrue(pinged);
    Assert::AreEqual({}, errorString);
  }

  TEST_METHOD(SendReceiveNoClose)
  {
    auto ws = IWebSocket::Make("ws://localhost:5555/");
    promise<string> response;
    ws->SetOnMessage([&response](size_t size, const string& message)
    {
      // Ignore greeting message.
      if (message == "hello")
        return;

      response.set_value(message);
    });
    string errorMessage;
    ws->SetOnError([&errorMessage](IWebSocket::Error err)
    {
      errorMessage = err.Message;
    });

    ws->Connect();
    ws->Send("suffixme");

    // Block until respone is received. Fail in case of a remote endpoint failure.
    auto future = response.get_future();
    future.wait();
    string result = future.get();

    Assert::AreEqual({}, errorMessage);
    Assert::AreEqual(string("suffixme_response"), result);
  }

  // Emulate promise/future functionality.
  // Fails when connecting to stock package bundler.
  BEGIN_TEST_METHOD_ATTRIBUTE(WaitForBundlerResponseNoClose)
    TEST_IGNORE()
  END_TEST_METHOD_ATTRIBUTE()
  TEST_METHOD(WaitForBundlerResponseNoClose)
  {
    string url = "ws://localhost:8081/debugger-proxy?role=client";
    //string url = "ws://localhost:5555/";
    auto ws = IWebSocket::Make(url);
    string json = "{\"inject\":{},\"id\":1,\"method\":\"executeApplicationScript\",\"url\":\"http://localhost:8081/IntegrationTests/IntegrationTestsAppWin.bundle?platform=ios&dev=true\"}";
    //string json = "{}";
    std::mutex mutex;
    condition_variable condition;
    bool read = false;
    bool connected = false;
    bool wrote = false;
    ws->SetOnConnect([&connected]()
    {
      connected = true;
    });
    ws->SetOnSend([&wrote](size_t size)
    {
      wrote = true;
    });
    ws->SetOnMessage([&mutex, &condition, &read](size_t size, const string& message)
    {
      lock_guard<std::mutex> guard(mutex);
      read = true;
      condition.notify_all();
    });

    ws->Connect();
    ws->Send(json);

    unique_lock<std::mutex> lock(mutex);
    condition.wait(lock);

    Assert::IsTrue(connected);
    Assert::IsTrue(wrote);
    Assert::IsTrue(read);
  }

  TEST_METHOD(SendReceiveClose)
  {
    auto ws = IWebSocket::Make("ws://localhost:5555/");
    promise<size_t> sentSizePromise;
    ws->SetOnSend([&sentSizePromise](size_t size)
    {
      sentSizePromise.set_value(size);
    });
    promise<string> receivedPromise;
    ws->SetOnMessage([&receivedPromise](size_t size, const string& message)
    {
      // Ignore greeting message
      if (message == "hello")
        return;

      receivedPromise.set_value(message);
    });
    string errorMessage;
    ws->SetOnError([&errorMessage](IWebSocket::Error err)
    {
      errorMessage = err.Message;
    });

    string sent = "suffixme";
    ws->Connect();
    ws->Send(sent);

    // Block until respone is received. Fail in case of a remote endpoint failure.
    auto sentSizeFuture = sentSizePromise.get_future();
    sentSizeFuture.wait();
    auto sentSize = sentSizeFuture.get();
    auto receivedFuture = receivedPromise.get_future();
    receivedFuture.wait();
    string received = receivedFuture.get();
    Assert::AreEqual({}, errorMessage);

    ws->Close(IWebSocket::CloseCode::Normal, "Closing after reading");

    Assert::AreEqual({}, errorMessage);
    Assert::AreEqual(sent.length(), sentSize);
    Assert::AreEqual(string("suffixme_response"), received);
  }

  TEST_METHOD(SendReceiveLargeMessage)
  {
    auto ws = IWebSocket::Make("ws://localhost:5555/");
    promise<string> response;
    ws->SetOnMessage([&response](size_t size, const string& message)
    {
      // Ignore greeting message
      if (message == "hello")
        return;

      response.set_value(message);
    });
    ws->SetOnError([](IWebSocket::Error err)
    {
      auto message = facebook::react::unicode::utf8ToUtf16(err.Message);
      Assert::Fail(message.c_str());
    });

    ws->Connect();

    char digits[] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };
    #define LEN 4096 + 4096 * 2 + 1
    char chars[LEN + 1];
    for (int i = 0; i < LEN; i++)
    {
      chars[i] = digits[i % 16];
    }
    chars[LEN] = '\0';
    string large(chars);
    ws->Send(large);

    // Block until respone is received. Fail in case of a remote endpoint failure.
    auto future = response.get_future();
    future.wait();
    string result = future.get();

    ws->Close(IWebSocket::CloseCode::Normal, "Closing after reading");

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
  TEST_METHOD(AdditionalHeaders)
  {
    auto ws = IWebSocket::Make("ws://localhost:5555/");
    promise<string> response;
    ws->SetOnMessage([&response](size_t size, const string& message)
    {
      response.set_value(message);
    });

    ws->Connect({}, {{ L"Cookie", "JSESSIONID=AD9A320CC4034641997FF903F1D10906" }});
    ws->Send("");

    auto future = response.get_future();
    future.wait();
    string result = future.get();

    Assert::AreEqual(string("JSESSIONID=AD9A320CC4034641997FF903F1D10906"), result);

    ws->Close(IWebSocket::CloseCode::Normal, "No reason");
  }

  ///
  // Run this test against a valid WebSocket server runing on SSL.
  // See sample below.
  ///
  /*
const WebSocket = require('ws');
const fs = require('fs');
const https = require('https');

const httpsServer = https.createServer({
  key:  fs.readFileSync('key.pem'),
  cert: fs.readFileSync('cert.pem')
});
const server = new WebSocket.Server({
  server:httpsServer
});

server.on('connection', (ws) => {
  ws.on('message', (message) => {
    console.log('Received message:', message);
    if (message === 'exit') {
      console.log('WebSocket integration test server exit');
      process.exit(0);
    }
    console.log('Cookie:', ws.upgradeReq.headers.cookie);
    ws.send(message + '_response');
  });

  ws.send('hello');
});

httpsServer.listen(443);
  */
  BEGIN_TEST_METHOD_ATTRIBUTE(SendAndReceiveSsl)
    TEST_IGNORE()
  END_TEST_METHOD_ATTRIBUTE()
  TEST_METHOD(SendAndReceiveSsl)
  {
    auto ws = IWebSocket::Make("wss://localhost/");
    string message;
    ws->SetOnMessage([&message](size_t size, const string& messageIn)
    {
      message = messageIn;
    });

    ws->Connect();
    ws->Send("suffixme");
    ws->Close(IWebSocket::CloseCode::Normal, "Closing after reading");

    Assert::AreEqual(string("hello"), message);
  }

  BEGIN_TEST_METHOD_ATTRIBUTE(SendBinary)
    //TEST_IGNORE()
  END_TEST_METHOD_ATTRIBUTE()
  TEST_METHOD(SendBinary)
  {
    auto ws = IWebSocket::Make("ws://localhost:5555/");

    std::vector<string> messages
    {
      // Empty
      "",        // []

      // Text
      "MQ==",    // "1"
      "MTI=",    // "12"
      "MTIz",    // "123"
      "MTIzNA==",// "1230"

      // Binary
      "AQ==",    // [ 01 ]
      "AQI=",    // [ 01 02 ]
      "AQID",    // [ 01 02 03 ]
      "AQIDAA==" // [ 00, 01, 02, 03, 00 ]
    };

    string errorMessage;
    promise<string> responsePromise;
    ws->SetOnMessage([&responsePromise](size_t size, const string& messageIn)
    {
      // Ignore on-connect greeting.
      if (messageIn == "hello")
        return;

      responsePromise.set_value(messageIn);
    });
    ws->SetOnError([&errorMessage](IWebSocket::Error error)
    {
      errorMessage = error.Message;
    });

    ws->Connect();

    for (size_t i = 0; i < messages.size(); i++)
    {
      ws->SendBinary(messages[i]);
      auto future = responsePromise.get_future();
      future.wait();
      string response = future.get();
      responsePromise = promise<string>(); //Reset promise.

      // Expect same message.
      Assert::AreEqual(messages[i], response);
    }

    ws->Close(IWebSocket::CloseCode::Normal, "Closing after reading");

    Assert::AreEqual({}, errorMessage);
  }

  TEST_METHOD(SendConsecutive)
  {
    auto ws = IWebSocket::Make("ws://localhost:5555/");
    promise<string> response;
    const int writes = 10;
    int count = 0;
    ws->SetOnMessage([&response, &count, writes](size_t size, const string& message)
    {
      // Ignore greeting message.
      if (message == "hello")
        return;

      if (++count < writes)
        return;

      response.set_value(message);
    });
    string errorMessage;
    ws->SetOnError([&errorMessage](IWebSocket::Error err)
    {
      errorMessage = err.Message;
    });

    ws->Connect();

    // Consecutive immediate writes should be enqueued.
    // The WebSocket implementation can't handle multiple write operations concurrently.
    for (int i = 0; i < writes; i++)
      ws->Send("suffixme");

    // Block until respone is received. Fail in case of a remote endpoint failure.
    auto future = response.get_future();
    future.wait();
    string result = future.get();

    ws->Close(IWebSocket::CloseCode::Normal, "Closing");
    Assert::AreEqual({}, errorMessage);
    Assert::AreEqual(string("suffixme_response"), result);
  }
};
