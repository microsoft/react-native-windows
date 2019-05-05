// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include <CppUnitTest.h>

#include <WebSocket.h>

using namespace boost::beast;
using namespace facebook::react;
using namespace Microsoft::React::Test;
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

using boost::system::error_code;
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
    string errorMessage;
    bool connected = false;
    auto ws = make_unique<TestWebSocket>(Url("ws://localhost:80"));
    ws->SetOnError([&errorMessage](IWebSocket::Error err)
    {
      errorMessage = err.Message;
    });
    ws->SetOnConnect([&connected]()
    {
      connected = true;
    });
    ws->SetConnectResult([]() -> error_code
    {
      return make_error_code(errc::success);
    });

    ws->Connect({}, {});
    ws->Close(IWebSocket::CloseCode::Normal, {});

    Assert::AreEqual({}, errorMessage);
    Assert::IsTrue(connected);
  }

  TEST_METHOD(ConnectFails)
  {
    string errorMessage;
    bool connected = false;
    auto ws = make_unique<TestWebSocket>(Url("ws://localhost:80"));
    ws->SetOnError([&errorMessage](IWebSocket::Error err)
    {
      errorMessage = err.Message;
    });
    ws->SetOnConnect([&connected]()
    {
      connected = true;
    });
    ws->SetConnectResult([]() -> error_code
    {
      return make_error_code(errc::state_not_recoverable);
    });

    ws->Connect({}, {});
    ws->Close(IWebSocket::CloseCode::Normal, {});

    Assert::AreNotEqual({}, errorMessage);
    Assert::IsFalse(connected);
  }
};
