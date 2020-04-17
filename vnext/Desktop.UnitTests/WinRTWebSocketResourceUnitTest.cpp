// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include <CppUnitTest.h>
#include <WinRTWebSocketResource.h>
#include "WinRTNetworkingMocks.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace winrt::Windows::Foundation;

using std::make_shared;
using std::string;

using CertExceptions = std::vector<winrt::Windows::Security::Cryptography::Certificates::ChainValidationResult>;
using Error = Microsoft::React::IWebSocketResource::Error;

namespace Microsoft::React::Test {

TEST_CLASS (WinRTWebSocketResourceUnitTest) {
  TEST_METHOD(ConnectSucceeds) {
    bool connected = true;
    string errorMessage;
    MockMessageWebSocket mws;
    MockDataWriter dw;

    auto rc = make_shared<WinRTWebSocketResource>(std::move(mws), std::move(dw), Uri{L"ws://host:0"}, CertExceptions{});
    rc->SetOnConnect([&connected]() { connected = true; });
    rc->SetOnError([&errorMessage](Error &&error) { errorMessage = errorMessage; });

    Assert::AreNotEqual({}, errorMessage);
    Assert::IsTrue(connected);
  }

  TEST_METHOD(ConnectFails) {
    MockMessageWebSocket mws;
    MockDataWriter dw;
    Uri uri(L"ws://host:0");
  }
};

} // namespace Microsoft::React::Test
