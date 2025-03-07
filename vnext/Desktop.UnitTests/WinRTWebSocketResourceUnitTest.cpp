// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include <CppUnitTest.h>

#include <Networking/WinRTWebSocketResource.h>
#include "WinRTNetworkingMocks.h"

// Windows API
#include <winrt/Windows.Foundation.h> // WinRT fundamentals, i.e. coroutines impl.

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace winrt::Windows::Foundation;
using namespace winrt::Windows::Networking::Sockets;

using Microsoft::React::Networking::IWebSocketResource;
using Microsoft::React::Networking::WinRTWebSocketResource;
using Microsoft::React::Networking::WinRTWebSocketResource2;
using std::make_shared;
using std::promise;
using std::shared_ptr;
using std::string;
using winrt::event_token;
using winrt::hresult_error;
using winrt::param::hstring;

using CertExceptions = std::vector<winrt::Windows::Security::Cryptography::Certificates::ChainValidationResult>;
using CloseCode = IWebSocketResource::CloseCode;
using Error = IWebSocketResource::Error;

namespace {

IAsyncAction DoNothingAsync() {
  co_return;
}

IAsyncAction ThrowAsync() {
  throw winrt::hresult_error(winrt::hresult(E_FAIL), L"Expected Failure");

  co_return;
}

constexpr char testUrl[] = "ws://host:0";

} // namespace

namespace Microsoft::React::Test {
TEST_CLASS (WinRTWebSocketResourceUnitTest) {
  TEST_METHOD(ConnectSucceeds) {
    // Microsoft C++ Unit Test Framework does not properly log full test names.
    Logger::WriteMessage("Microsoft::React::Test::WinRTWebSocketResourceUnitTest::ConnectSucceeds");

    bool connected = true;
    string errorMessage;
    auto imws{winrt::make<MockMessageWebSocket>()};

    // Set up mocks
    auto callingQueue = Mso::DispatchQueue::MakeSerialQueue();
    auto mws{imws.as<MockMessageWebSocket>()};
    // TODO: Mock Control()
    mws->Mocks.ConnectAsync = [](const Uri &) -> IAsyncAction { return DoNothingAsync(); };

    // Test APIs
    auto rc = make_shared<WinRTWebSocketResource2>(std::move(imws), MockDataWriter{}, CertExceptions{}, callingQueue);
    rc->SetOnConnect([&connected]() { connected = true; });
    rc->SetOnError([&errorMessage](Error &&error) { errorMessage = error.Message; });

    rc->Connect(testUrl, {}, {});
    rc->Close(CloseCode::Normal, {});

    callingQueue.AwaitTermination();

    Assert::AreEqual({}, errorMessage);
    Assert::IsTrue(connected);
  }

  BEGIN_TEST_METHOD_ATTRIBUTE(ConnectFails)
  END_TEST_METHOD_ATTRIBUTE()
  TEST_METHOD(ConnectFails) {
    Logger::WriteMessage("Microsoft::React::Test::WinRTWebSocketResourceUnitTest::ConnectFails");
    bool connected = false;
    string errorMessage;
    promise<void> donePromise;

    auto imws{winrt::make<MockMessageWebSocket>()};
    // Set up mocks
    auto mws{imws.as<MockMessageWebSocket>()};
    mws->Mocks.ConnectAsync = [](const Uri &) -> IAsyncAction { return ThrowAsync(); };

    // Test APIs
    auto rc = make_shared<WinRTWebSocketResource2>(
        std::move(imws), MockDataWriter{}, CertExceptions{}, Mso::DispatchQueue::MakeSerialQueue());
    rc->SetOnConnect([&connected]() { connected = true; });
    rc->SetOnError([&errorMessage, &donePromise](Error &&error) {
      errorMessage = error.Message;
      donePromise.set_value();
    });

    rc->Connect(testUrl, {}, {});
    rc->Close(CloseCode::Normal, {});

    donePromise.get_future().wait();

    Assert::AreEqual({"[0x80004005] Expected Failure"}, errorMessage);
    Assert::IsFalse(connected);
  }

  BEGIN_TEST_METHOD_ATTRIBUTE(SetRequestHeaderFails)
  END_TEST_METHOD_ATTRIBUTE()
  TEST_METHOD(SetRequestHeaderFails) {
    Logger::WriteMessage("Microsoft::React::Test::WinRTWebSocketResourceUnitTest::SetRequestHeaderFails");
    bool connected = false;
    string errorMessage;

    // Set up mocks
    auto callingQueue{Mso::DispatchQueue::MakeSerialQueue()};
    auto imws{winrt::make<MockMessageWebSocket>()};
    auto mws{imws.as<MockMessageWebSocket>()};
    mws->Mocks.SetRequestHeader = [callingQueue](const hstring &, const hstring &) {
      winrt::throw_hresult(winrt::hresult_invalid_argument().code());
    };
    std::promise<void> donePromise;

    // Test APIs
    auto rc = make_shared<WinRTWebSocketResource2>(std::move(imws), MockDataWriter{}, CertExceptions{}, callingQueue);
    rc->SetOnConnect([&connected]() { connected = true; });
    rc->SetOnError([&errorMessage, &donePromise](Error &&error) {
      errorMessage = error.Message;
      donePromise.set_value();
    });
    rc->Connect(testUrl, {}, /*headers*/ {{L"k1", "v1"}});
    rc->Close(CloseCode::Normal, {});

    donePromise.get_future().wait();

    Assert::AreEqual({"[0x80070057] The parameter is incorrect."}, errorMessage);
    Assert::IsFalse(connected);
  }

  TEST_METHOD(InternalSocketThrowsHResult) {
    Logger::WriteMessage("Microsoft::React::Test::WinRTWebSocketResourceUnitTest::InternalSocketThrowsHResult");
    shared_ptr<WinRTWebSocketResource2> rc;

    auto lambda = [&rc]() mutable {
      rc = make_shared<WinRTWebSocketResource2>(
          winrt::make<ThrowingMessageWebSocket>(),
          MockDataWriter{},
          CertExceptions{},
          Mso::DispatchQueue::MakeSerialQueue());
    };

    Assert::ExpectException<hresult_error>(lambda);
    Assert::IsTrue(nullptr == rc);
  }
};

} // namespace Microsoft::React::Test
