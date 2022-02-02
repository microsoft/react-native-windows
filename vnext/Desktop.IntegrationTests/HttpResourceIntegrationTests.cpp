// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include <CppUnitTest.h>
#include <IHttpResource.h>

using namespace Microsoft::React;
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

using std::string;
using std::vector;

TEST_CLASS (HttpResourceIntegrationTest) {
  TEST_METHOD(MakeIsNotNull) {
    auto rc = IHttpResource::Make();
    Assert::IsFalse(nullptr == rc);
  }

  // This test always fails because the requested resource does not exist.
  // TEST_METHOD(RequestGetSucceeds) {
  void Disable_RequestGetSucceeds() {
    auto rc = IHttpResource::Make();
    bool sent = false;
    bool received = false;
    string error;
    rc->SetOnRequest([&sent]() { sent = true; });
    rc->SetOnResponse([&received](const string &message) { received = true; });
    rc->SetOnError([&error](const string &message) { error = message; });

    rc->SendRequest("GET", "http://localhost:8081/debugger-ui", {}, {}, "text", false, 1000, [](int64_t) {});

    Assert::IsTrue(sent);
    Assert::IsTrue(received);
    Assert::AreEqual(string(), error);
  }

  TEST_METHOD(RequestGetFails) {
    auto rc = IHttpResource::Make();
    string error;
    rc->SetOnError([&error](const string &message) { error = message; });

    rc->SendRequest("GET", "http://nonexistinghost", {}, {}, "text", false, 1000, [](int64_t) {});

    Assert::AreEqual(string("No such host is known"), error);
  }
};
