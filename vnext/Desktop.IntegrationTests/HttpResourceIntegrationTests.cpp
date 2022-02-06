// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

//C4996: 'gethostbyaddr': Use getnameinfo() or GetNameInfoW() instead
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <boost/beast/http.hpp>
#include <Test/HttpServer.h>
#include <CppUnitTest.h>
#include <IHttpResource.h>

// Standard Library
#include <future>

using namespace Microsoft::React;
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace http = boost::beast::http;

using std::promise;
using std::string;
using std::vector;

TEST_CLASS (HttpResourceIntegrationTest) {

  TEST_METHOD(RequestGetSucceeds) {
    promise<void> getPromise;
    int statusCode = 0;

    // HTTP call scope
    {
      auto server = std::make_shared<Test::HttpServer>("127.0.0.1", static_cast<uint16_t>(5555));
      server->SetOnGet([](const http::request<http::string_body> &request) -> http::response<http::dynamic_body> {
        http::response<http::dynamic_body> response;
        response.result(http::status::ok);

        return response;
      });
      server->Start();

      auto resource = IHttpResource::Make();
      resource->SetOnResponse([&getPromise, &statusCode](int64_t, IHttpResource::Response response) {
        statusCode = static_cast<int>(response.StatusCode);
        getPromise.set_value();
      });
      resource->SendRequest(
          "GET",
          "http://localhost:5555",
          {} /*header*/,
          {} /*bodyData*/,
          "text",
          false,
          1000 /*timeout*/,
          false /*withCredentials*/,
          [](int64_t) {});

      server->Stop();
    }
    // Synchronize response.
    getPromise.get_future().wait();

    Assert::AreEqual(200, statusCode);
  }

  TEST_METHOD(RequestGetFails) {
    string error;
    promise<void> promise;

    auto resource = IHttpResource::Make();
    resource->SetOnError([&error, &promise](int64_t, string &&message) {
      error = message;
      promise.set_value();
    });

    resource->SendRequest(
      "GET",
      "http://nonexistinghost",
      {},
      {},
      "text",
      false,
      1000,
      false,
      [](int64_t) {}
    );

    promise.get_future().wait();

    Logger::WriteMessage(error.c_str());
    Assert::AreNotEqual(string{}, error);
  }
};
