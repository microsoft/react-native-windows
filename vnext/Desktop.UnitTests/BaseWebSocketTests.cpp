// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include <BeastWebSocketResource.h>
#include <CppUnitTest.h>
#include <future>

using namespace boost::beast;
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

using boost::system::error_code;
using Microsoft::React::Beast::Test::TestWebSocketResource;
using std::future;
using std::make_unique;
using std::promise;
using std::string;

namespace {

constexpr char testUrl[] = "ws://localhost";

} // namespace

namespace Microsoft::React::Test {

using CloseCode = IWebSocketResource::CloseCode;
using Error = IWebSocketResource::Error;

// We turn clang format off here because it does not work with some of the
// test macros.
// clang-format off

TEST_CLASS(BaseWebSocketTest){
  BEGIN_TEST_CLASS_ATTRIBUTE()
  TEST_CLASS_ATTRIBUTE(L"Ignore", L"true")
  END_TEST_CLASS_ATTRIBUTE()

  TEST_METHOD(CreateAndSetHandlers){
    auto ws = make_unique<TestWebSocketResource>(Url(testUrl));

    Assert::IsFalse(nullptr == ws);
    ws->SetOnConnect([]() {});
    ws->SetOnPing([]() {});
    ws->SetOnSend([](size_t length) {});
    ws->SetOnMessage([](size_t length, const string &buffer, bool isBinary) {});
    ws->SetOnClose([](CloseCode, const string &) {});
    ws->SetOnError([](const Error &error) {});
  }

  TEST_METHOD(ConnectSucceeds) {
    string errorMessage;
    bool connected = false;
    auto ws = make_unique<TestWebSocketResource>(Url(testUrl));
    ws->SetOnError([&errorMessage](Error err) { errorMessage = err.Message; });
    ws->SetOnConnect([&connected]() { connected = true; });

    ws->Connect(testUrl, {}, {});
    ws->Close(CloseCode::Normal, {});

    Assert::AreEqual({}, errorMessage);
    Assert::IsTrue(connected);
  }

  TEST_METHOD(ConnectFails) {
    string errorMessage;
    bool connected = false;
    auto ws = make_unique<TestWebSocketResource>(Url(testUrl));
    ws->SetOnError([&errorMessage](Error err) { errorMessage = err.Message; });
    ws->SetOnConnect([&connected]() { connected = true; });
    ws->SetConnectResult([]() -> error_code {
      return make_error_code(errc::state_not_recoverable);
    });

    ws->Connect(testUrl, {}, {});
    ws->Close(CloseCode::Normal, {});

    Assert::AreNotEqual({}, errorMessage);
    Assert::IsFalse(connected);
  }

  BEGIN_TEST_METHOD_ATTRIBUTE(HandshakeFails)
  TEST_IGNORE()
  END_TEST_METHOD_ATTRIBUTE()
  TEST_METHOD(HandshakeFails) {
    string errorMessage;
    bool connected = false;
    auto ws = make_unique<TestWebSocketResource>(Url(testUrl));
    ws->SetOnError([&errorMessage](Error err) { errorMessage = err.Message; });
    ws->SetOnConnect([&connected]() { connected = true; });
    ws->SetHandshakeResult([](string, string) -> error_code {
      return make_error_code(errc::state_not_recoverable);
    });

    ws->Connect(testUrl, {}, {});
    ws->Close(CloseCode::Normal, {});

    Assert::AreNotEqual({}, errorMessage);
    Assert::IsFalse(connected);
  }

  BEGIN_TEST_METHOD_ATTRIBUTE(CloseSucceeds)
  TEST_IGNORE()
  END_TEST_METHOD_ATTRIBUTE()
  TEST_METHOD(CloseSucceeds) {
    string errorMessage;
    promise<void> connected;
    bool closed = false;
    auto ws = make_unique<TestWebSocketResource>(Url(testUrl));
    ws->SetOnError([&errorMessage](Error err) { errorMessage = err.Message; });
    ws->SetOnConnect([&connected]() { connected.set_value(); });
    ws->SetOnClose(
        [&closed](CloseCode code, const string &message) { closed = true; });

    ws->SetCloseResult(
        []() -> error_code { return make_error_code(errc::success); });

    ws->Connect(testUrl, {}, {});
    connected.get_future().wait();

    ws->Close(CloseCode::Normal, "Normal");

    Assert::AreNotEqual({}, errorMessage);
    Assert::IsTrue(closed);
  }
};

// clange-format on

} // namespace Microsoft::React::Test
