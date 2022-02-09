// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

// C4996: 'gethostbyaddr': Use getnameinfo() or GetNameInfoW() instead
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <CppUnitTest.h>
#include <IHttpResource.h>
#include <Test/HttpServer.h>
#include <boost/beast/http.hpp>

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

  // TODO: Hangs! Likely headers make server not send response.
  TEST_METHOD(RequestGetHeadersSucceeds) {
    promise<void> promise;
    string error;
    int statusCode = 0;

    // HTTP call scope
    {
      auto server = std::make_shared<Test::HttpServer>("127.0.0.1", static_cast<uint16_t>(5555));
      server->SetOnGet([](const http::request<http::string_body> &request) -> http::response<http::dynamic_body> {
        http::response<http::dynamic_body> response;
        response.result(http::status::ok);
        response.set(http::field::content_type, "text/html");
        response.set(http::field::content_encoding, "utf-8");
        response.set(http::field::content_length, "0");

        return response;
      });
      server->Start();

      auto resource = IHttpResource::Make();
      resource->SetOnResponse([&promise, &statusCode](int64_t, IHttpResource::Response response) {
        statusCode = static_cast<int>(response.StatusCode);
        for (auto& header : response.Headers) {
          auto &k = header.first;
          auto &v = header.second;

          continue;
        }    

        promise.set_value();
      });
      resource->SetOnError([&promise, &error, &server](int64_t, string &&message) {
        error = std::move(message);
        promise.set_value();

        server->Stop(true /*abort*/);
      });
      //clang-format off
      resource->SendRequest(
          "GET",
          "http://localhost:5555",
          {
            { "Content-Type",     "application/json"  },
            { "Content-Encoding", "ASCII"             },
            { "A",     "V"  },
          },
          {} /*bodyData*/,
          "text",
          false,
          1000 /*timeout*/,
          false /*withCredentials*/,
          [](int64_t) {});
      //clang-format on
      server->Stop();
    }

    promise.get_future().wait();

    Assert::AreEqual({}, error);
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

    resource->SendRequest("GET", "http://nonexistinghost", {}, {}, "text", false, 1000, false, [](int64_t) {});

    promise.get_future().wait();

    Logger::WriteMessage(error.c_str());
    Assert::AreNotEqual(string{}, error);
  }
};
