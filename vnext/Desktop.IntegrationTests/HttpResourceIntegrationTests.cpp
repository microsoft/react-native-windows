// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

//C4996: 'gethostbyaddr': Use getnameinfo() or GetNameInfoW() instead
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <boost/beast/http.hpp>
#include <Test/HttpServer.h>
#include <CppUnitTest.h>
#include <IHttpResource.h>

using namespace Microsoft::React;
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace http = boost::beast::http;

using std::string;
using std::vector;

TEST_CLASS (HttpResourceIntegrationTest) {
  TEST_METHOD(MakeIsNotNull) {
    auto rc = IHttpResource::Make();
    Assert::IsFalse(nullptr == rc);
  }

  // This test always fails because the requested resource does not exist.
  TEST_METHOD(RequestGetSucceeds) {
    bool responded = false;
    auto server = std::make_shared<Test::HttpServer>("127.0.0.1", static_cast<uint16_t>(5555));
    server->SetOnRequest([&responded]()
    {
        responded = true;
    });
    //http::response<http::dynamic_body>(const http::request<http::string_body> &)
    server->SetOnGet([](const http::request<http::string_body>& request) -> http::response<http::dynamic_body>
    {
        http::response<http::dynamic_body> response;
        response.result(http::status::ok);

        return response;
    });
    server->Start();

    auto resource = IHttpResource::Make();
    resource->SetOnResponse([](int64_t, IHttpResource::Response response)
    {
        auto sc = response.StatusCode;
    });
    resource->SendRequest(
      "GET",
      "http://localhost:5555",
      {}/*header*/,
      {}/*bodyData*/,
      "responseType",
      false,
      1000/*timeout*/,
      false/*withCredentials*/,
      [](int64_t) {}
    );

    server->Stop();
  }

  TEST_METHOD(RequestGetFails) {
    auto rc = IHttpResource::Make();
    string error;
    rc->SetOnError([&error](int64_t, string &&message) { error = message; });

    rc->SendRequest("GET", "http://nonexistinghost", {}, {}, "text", false, 1000, false, [](int64_t) {});

    Assert::AreEqual(string("No such host is known"), error);
  }
};
