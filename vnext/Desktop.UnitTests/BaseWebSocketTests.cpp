// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include <CppUnitTest.h>
#include <WebSocket.h>
#include <future>

using namespace boost::beast;
using namespace facebook::react;
using namespace Microsoft::React::Test;
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

using boost::system::error_code;
using std::future;
using std::make_unique;
using std::promise;
using std::string;

namespace Microsoft::React::Test {

using CloseCode = IWebSocket::CloseCode;
using Error = IWebSocket::Error;

// We turn clang format off here because it does not work with some of the
// test macros.
// clang-format off

TEST_CLASS(BaseWebSocketTest)
{
  // Add any needed class-level attributes here.
  // See https://docs.microsoft.com/en-us/visualstudio/test/microsoft-visualstudio-testtools-cppunittestframework-api-reference?view=vs-2019#test_class_attributes
  BEGIN_TEST_CLASS_ATTRIBUTE()
  END_TEST_CLASS_ATTRIBUTE()

  TEST_METHOD(CreateAndSetHandlers)
  {
    auto ws = make_unique<TestWebSocket>(Url("ws://localhost"));

    Assert::IsFalse(nullptr == ws);
    ws->SetOnConnect([]() {});
    ws->SetOnPing([]() {});
    ws->SetOnSend([](size_t length) {});
    ws->SetOnMessage([](size_t length, const string &buffer) {});
    ws->SetOnClose([](CloseCode, const string &) {});
    ws->SetOnError([](const Error &error) {});
  }

  TEST_METHOD(ConnectSucceeds)
  {
    string errorMessage;
    bool connected = false;
    auto ws = make_unique<TestWebSocket>(Url("ws://localhost"));
    ws->SetOnError([&errorMessage](Error err) { errorMessage = err.Message; });
    ws->SetOnConnect([&connected]() { connected = true; });

    ws->Connect({}, {});
    ws->Close(CloseCode::Normal, {});

    Assert::AreEqual({}, errorMessage);
    Assert::IsTrue(connected);
  }

  TEST_METHOD(ConnectFails)
  {
    string errorMessage;
    bool connected = false;
    auto ws = make_unique<TestWebSocket>(Url("ws://localhost"));
    ws->SetOnError([&errorMessage](Error err) { errorMessage = err.Message; });
    ws->SetOnConnect([&connected]() { connected = true; });
    ws->SetConnectResult([]() -> error_code {
      return make_error_code(errc::state_not_recoverable);
    });

    ws->Connect({}, {});
    ws->Close(CloseCode::Normal, {});

    Assert::AreNotEqual({}, errorMessage);
    Assert::IsFalse(connected);
  }

  BEGIN_TEST_METHOD_ATTRIBUTE(HandshakeFails)
  TEST_IGNORE()
  END_TEST_METHOD_ATTRIBUTE()
  TEST_METHOD(HandshakeFails)
  {
    string errorMessage;
    bool connected = false;
    auto ws = make_unique<TestWebSocket>(Url("ws://localhost"));
    ws->SetOnError([&errorMessage](Error err) { errorMessage = err.Message; });
    ws->SetOnConnect([&connected]() { connected = true; });
    ws->SetHandshakeResult([](string, string) -> error_code {
      return make_error_code(errc::state_not_recoverable);
    });

    ws->Connect({}, {});
    ws->Close(CloseCode::Normal, {});

    Assert::AreNotEqual({}, errorMessage);
    Assert::IsFalse(connected);
  }

  BEGIN_TEST_METHOD_ATTRIBUTE(CloseSucceeds)
  TEST_IGNORE()
  END_TEST_METHOD_ATTRIBUTE()
  TEST_METHOD(CloseSucceeds)
  {
    string errorMessage;
    promise<void> connected;
    bool closed = false;
    auto ws = make_unique<TestWebSocket>(Url("ws://localhost"));
    ws->SetOnError([&errorMessage](Error err) { errorMessage = err.Message; });
    ws->SetOnConnect([&connected]() { connected.set_value(); });
    ws->SetOnClose(
        [&closed](CloseCode code, const string &message) { closed = true; });

    ws->SetCloseResult(
        []() -> error_code { return make_error_code(errc::success); });

    ws->Connect({}, {});
    connected.get_future().wait();

    ws->Close(CloseCode::Normal, "Normal");

    Assert::AreNotEqual({}, errorMessage);
    Assert::IsTrue(closed);
  }
};

// clange-format on

} // namespace Microsoft::React::Test
