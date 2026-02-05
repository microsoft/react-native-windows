// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

// C4996: 'gethostbyaddr': Use getnameinfo() or GetNameInfoW() instead
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <CppUnitTest.h>

#include <Networking/IHttpResource.h>
#include <Networking/OriginPolicy.h>
#include <RuntimeOptions.h>
#include <unicode.h>

// Boost Library
#include <boost/beast/http.hpp>

// Standard Library
#include <future>

#include "HttpServer.h"

using namespace Microsoft::React;
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace http = boost::beast::http;

using Networking::IHttpResource;
using Networking::OriginPolicy;
using std::make_shared;
using std::promise;
using std::string;
using std::vector;
using Test::DynamicRequest;
using Test::DynamicResponse;
using Test::EmptyResponse;
using Test::HttpServer;
using Test::ResponseWrapper;

namespace Microsoft::React::Test {

TEST_CLASS (HttpResourceIntegrationTest) {
  static uint16_t s_port;

  TEST_METHOD_CLEANUP(MethodCleanup) {
    // Clear any runtime options that may be used by tests in this class.
    MicrosoftReactSetRuntimeOptionString("Http.UserAgent", nullptr);

    // Bug in test HTTP server does not correctly release TCP port between test methods.
    // Using a different por per test for now.
    s_port++;
  }
  TEST_METHOD(RequestGetSucceeds) {
    string url = "http://localhost:" + std::to_string(s_port);

    promise<void> resPromise;
    string error;
    int statusCode = 0;

    auto server = make_shared<HttpServer>(s_port);
    server->Callbacks().OnGet = [&resPromise](const DynamicRequest &request) -> ResponseWrapper {
      DynamicResponse response;
      response.result(http::status::ok);
      response.body() = Test::CreateStringResponseBody("some response content");

      return {std::move(response)};
    };
    server->Start();

    auto resource = IHttpResource::Make();
    resource->SetOnResponse([&resPromise, &statusCode](int64_t, IHttpResource::Response response) {
      statusCode = static_cast<int>(response.StatusCode);
    });
    resource->SetOnData([&resPromise](int64_t, string &&content) { resPromise.set_value(); });
    resource->SetOnError([&resPromise, &error, &server](int64_t, string &&message, bool) {
      error = std::move(message);
      resPromise.set_value();

      server->Stop();
    });
    resource->SendRequest(
        "GET",
        std::move(url),
        0, /*requestId*/
        {}, /*header*/
        {}, /*data*/
        "text",
        false,
        0 /*timeout*/,
        false /*withCredentials*/,
        [](int64_t) {});

    // Synchronize response.
    resPromise.get_future().wait();
    server->Stop();

    Assert::AreEqual({}, error);
    Assert::AreEqual(200, statusCode);
  }

  TEST_METHOD(RequestGetHeadersSucceeds) {
    string url = "http://localhost:" + std::to_string(s_port);

    promise<void> rcPromise;
    string error;
    IHttpResource::Response response;

    auto server = make_shared<HttpServer>(s_port);
    server->Callbacks().OnGet = [](const DynamicRequest &request) -> ResponseWrapper {
      DynamicResponse response;
      response.result(http::status::ok);

      // Response header
      response.set(http::field::server, "Microsoft::React::Test::HttpServer");
      // Response content header
      response.set(http::field::content_length, "0");
      // Response arbitrary header
      response.set("ResponseHeaderName1", "ResponseHeaderValue1");

      return {std::move(response)};
    };
    server->Start();

    auto resource = IHttpResource::Make();
    resource->SetOnResponse([&rcPromise, &response](int64_t, IHttpResource::Response callbackResponse) {
      response = callbackResponse;
      rcPromise.set_value();
    });
    resource->SetOnError([&rcPromise, &error, &server](int64_t, string &&message, bool) {
      error = std::move(message);
      rcPromise.set_value();

      server->Abort();
    });

    //clang-format off
    resource->SendRequest(
        "GET",
        std::move(url),
        0, /*requestId*/
        {
            {"Content-Type", "application/json"},
            {"Content-Encoding", "ASCII"},
            {"name3", "value3"},
            {"name4", "value4"},
        },
        {}, /*data*/
        "text",
        false,
        0 /*timeout*/,
        false /*withCredentials*/,
        [](int64_t) {});
    //clang-format on

    rcPromise.get_future().wait();
    server->Stop();

    Assert::AreEqual({}, error, L"Error encountered");
    for (auto header : response.Headers) {
      if (header.first == "Server") {
        Assert::AreEqual({"Microsoft::React::Test::HttpServer"}, header.second, L"Wrong header");
      } else if (header.first == "Content-Length") {
        Assert::AreEqual({"0"}, header.second, L"Wrong header");
      } else if (header.first == "ResponseHeaderName1") {
        Assert::AreEqual({"ResponseHeaderValue1"}, header.second, L"Wrong header");
      } else {
        string message = "Unexpected header: [" + header.first + "]=[" + header.second + "]";
        Assert::Fail(Microsoft::Common::Unicode::Utf8ToUtf16(message).c_str());
      }
    }
  }

  TEST_METHOD(RequestGetExplicitUserAgentSucceeds) {
    string url = "https://api.github.com/rate_limit";

    promise<void> rcPromise;
    string error;
    IHttpResource::Response response;

    auto resource = IHttpResource::Make();
    resource->SetOnResponse([&rcPromise, &response](int64_t, IHttpResource::Response callbackResponse) {
      response = callbackResponse;
      rcPromise.set_value();
    });
    resource->SetOnError([&rcPromise, &error](int64_t, string &&message, bool) {
      error = std::move(message);
      rcPromise.set_value();
    });

    //clang-format off
    resource->SendRequest(
        "GET",
        std::move(url),
        0, /*requestId*/
        {{"User-Agent", "React Native Windows"}},
        {}, /*data*/
        "text", /*responseType*/
        false,
        0 /*timeout*/,
        false /*withCredentials*/,
        [](int64_t) {});
    //clang-format on

    rcPromise.get_future().wait();

    Assert::AreEqual({}, error, L"Error encountered");
    Assert::AreEqual(static_cast<int64_t>(200), response.StatusCode);
  }

  TEST_METHOD(RequestGetImplicitUserAgentSucceeds) {
    string url = "https://api.github.com/rate_limit";

    promise<void> rcPromise;
    string error;
    IHttpResource::Response response;

    MicrosoftReactSetRuntimeOptionString("Http.UserAgent", "React Native Windows");

    auto resource = IHttpResource::Make();
    resource->SetOnResponse([&rcPromise, &response](int64_t, IHttpResource::Response callbackResponse) {
      response = callbackResponse;
      rcPromise.set_value();
    });
    resource->SetOnError([&rcPromise, &error](int64_t, string &&message, bool) {
      error = std::move(message);
      rcPromise.set_value();
    });

    //clang-format off
    resource->SendRequest(
        "GET",
        std::move(url),
        0, /*requestId*/
        {}, /*headers*/
        {}, /*data*/
        "text", /*responseType*/
        false,
        0 /*timeout*/,
        false /*withCredentials*/,
        [](int64_t) {});
    //clang-format on

    rcPromise.get_future().wait();

    Assert::AreEqual({}, error, L"Error encountered");
    Assert::AreEqual(static_cast<int64_t>(200), response.StatusCode);
  }

  TEST_METHOD(RequestGetMissingUserAgentFails) {
    // string url = "http://localhost:" + std::to_string(s_port);
    string url = "https://api.github.com/rate_limit";

    promise<void> rcPromise;
    string error;
    IHttpResource::Response response;

    auto resource = IHttpResource::Make();
    resource->SetOnResponse([&rcPromise, &response](int64_t, IHttpResource::Response callbackResponse) {
      response = callbackResponse;
      rcPromise.set_value();
    });
    resource->SetOnError([&rcPromise, &error](int64_t, string &&message, bool) {
      error = std::move(message);
      rcPromise.set_value();
    });

    //clang-format off
    resource->SendRequest(
        "GET",
        std::move(url),
        0, /*requestId*/
        {}, /*headers*/
        {}, /*data*/
        "text", /*responseType*/
        false,
        0 /*timeout*/,
        false /*withCredentials*/,
        [](int64_t) {});
    //clang-format on

    rcPromise.get_future().wait();

    Assert::AreEqual({}, error, L"Error encountered");
    Assert::AreEqual(static_cast<int64_t>(403), response.StatusCode);
  }

  TEST_METHOD(RequestGetFails) {
    string error;
    promise<void> promise;

    auto resource = IHttpResource::Make();
    resource->SetOnError([&error, &promise](int64_t, string &&message, bool) {
      error = message;
      promise.set_value();
    });

    resource->SendRequest("GET", "http://nonexistinghost", 0, {}, {}, "text", false, 0, false, [](int64_t) {});

    promise.get_future().wait();

    Logger::WriteMessage(error.c_str());
    Assert::AreNotEqual(string{}, error);
  }

  TEST_METHOD(RequestOptionsSucceeds) {
    string url = "http://localhost:" + std::to_string(s_port);

    promise<void> getResponsePromise;
    promise<void> getDataPromise;
    promise<void> optionsPromise;
    string error;
    IHttpResource::Response getResponse;
    IHttpResource::Response optionsResponse;
    string content;

    auto server = make_shared<HttpServer>(s_port);
    server->Callbacks().OnOptions = [](const DynamicRequest &request) -> ResponseWrapper {
      EmptyResponse response;
      response.result(http::status::partial_content);
      response.set("PreflightName", "PreflightValue");

      return {std::move(response)};
    };
    server->Callbacks().OnGet = [](const DynamicRequest &request) -> ResponseWrapper {
      DynamicResponse response;
      response.result(http::status::ok);
      response.body() = Test::CreateStringResponseBody("Response Body");

      return {std::move(response)};
    };
    server->Start();

    auto resource = IHttpResource::Make();
    resource->SetOnResponse([&getResponse, &getResponsePromise, &optionsResponse, &optionsPromise](
                                int64_t, IHttpResource::Response callbackResponse) {
      if (callbackResponse.StatusCode == static_cast<int64_t>(http::status::ok)) {
        getResponse = callbackResponse;
        getResponsePromise.set_value();
      } else if (callbackResponse.StatusCode == static_cast<int64_t>(http::status::partial_content)) {
        optionsResponse = callbackResponse;
        optionsPromise.set_value();
      }
    });
    resource->SetOnData([&getDataPromise, &content](int64_t, string &&responseData) {
      content = std::move(responseData);

      if (!content.empty())
        getDataPromise.set_value();
    });
    resource->SetOnError(
        [&optionsPromise, &getResponsePromise, &getDataPromise, &error, &server](int64_t, string &&message, bool) {
          error = std::move(message);

          optionsPromise.set_value();
          getResponsePromise.set_value();
          getDataPromise.set_value();

          server->Stop();
        });

    //clang-format off
    resource->SendRequest(
        "OPTIONS",
        string{url},
        0, /*requestId*/
        {}, /*headers*/
        {}, /*data*/
        "text",
        false,
        1000 /*timeout*/,
        false /*withCredentials*/,
        [](int64_t) {});
    resource->SendRequest(
        "GET",
        std::move(url),
        0, /*requestId*/
        {}, /*headers*/
        {}, /*data*/
        "text",
        false,
        0 /*timeout*/,
        false /*withCredentials*/,
        [](int64_t) {});
    //clang-format on

    optionsPromise.get_future().wait();
    getResponsePromise.get_future().wait();
    getDataPromise.get_future().wait();
    server->Stop();

    Assert::AreEqual({}, error, L"Error encountered");
    Assert::AreEqual(static_cast<size_t>(1), optionsResponse.Headers.size());
    for (auto header : optionsResponse.Headers) {
      if (header.first == "PreflightName") {
        Assert::AreEqual({"PreflightValue"}, header.second, L"Wrong header");
      } else {
        string message = "Unexpected header: [" + header.first + "]=[" + header.second + "]";
        Assert::Fail(Microsoft::Common::Unicode::Utf8ToUtf16(message).c_str());
      }
    }
    Assert::AreEqual({"Response Body"}, content);
  }

  TEST_METHOD(SimpleRedirectGetSucceeds) {
    auto port1 = s_port;
    auto port2 = ++s_port;
    string url = "http://localhost:" + std::to_string(port1);

    promise<void> responsePromise;
    promise<void> contentPromise;
    IHttpResource::Response responseResult;
    string content;
    string error;

    auto server1 = make_shared<HttpServer>(port1);
    server1->Callbacks().OnGet = [port2](const DynamicRequest &request) -> ResponseWrapper {
      DynamicResponse response;
      response.result(http::status::moved_permanently);
      response.set(http::field::location, {"http://localhost:" + std::to_string(port2)});

      return {std::move(response)};
    };
    auto server2 = make_shared<HttpServer>(port2);
    server2->Callbacks().OnGet = [](const DynamicRequest &request) -> ResponseWrapper {
      DynamicResponse response;
      response.result(http::status::ok);
      response.body() = Test::CreateStringResponseBody("Redirect Content");

      return {std::move(response)};
    };

    server1->Start();
    server2->Start();

    auto resource = IHttpResource::Make();
    resource->SetOnResponse([&responseResult, &responsePromise](int64_t, IHttpResource::Response response) {
      if (response.StatusCode == static_cast<int64_t>(http::status::ok)) {
        responseResult = response;
        responsePromise.set_value();
      }
    });
    resource->SetOnData([&contentPromise, &content](int64_t, string &&responseData) {
      content = std::move(responseData);

      if (!content.empty())
        contentPromise.set_value();
    });
    resource->SetOnError([&responsePromise, &contentPromise, &error, &server1](int64_t, string &&message, bool) {
      error = std::move(message);

      responsePromise.set_value();
      contentPromise.set_value();
    });

    //clang-format off
    resource->SendRequest(
        "GET",
        std::move(url),
        0, /*requestId*/
        {}, /*headers*/
        {}, /*data*/
        "text",
        false, /*useIncrementalUpdates*/
        0 /*timeout*/,
        false /*withCredentials*/,
        [](int64_t) {});
    //clang-format on

    responsePromise.get_future().wait();
    contentPromise.get_future().wait();

    server2->Stop();
    server1->Stop();

    Assert::AreEqual({}, error, L"Error encountered");
    Assert::AreEqual(static_cast<int64_t>(200), responseResult.StatusCode);
    Assert::AreEqual({"Redirect Content"}, content);
  }

  TEST_METHOD(SimpleRedirectPatchSucceeds) {
    auto port1 = s_port;
    auto port2 = ++s_port;
    string url = "http://localhost:" + std::to_string(port1);

    promise<void> responsePromise;
    promise<void> contentPromise;
    IHttpResource::Response responseResult;
    string content;
    string error;

    auto server1 = make_shared<HttpServer>(port1);
    server1->Callbacks().OnPatch = [port2](const DynamicRequest &request) -> ResponseWrapper {
      DynamicResponse response;
      response.result(http::status::moved_permanently);
      response.set(http::field::location, {"http://localhost:" + std::to_string(port2)});

      return {std::move(response)};
    };
    auto server2 = make_shared<HttpServer>(port2);
    server2->Callbacks().OnPatch = [](const DynamicRequest &request) -> ResponseWrapper {
      DynamicResponse response;
      response.result(http::status::ok);
      response.body() = Test::CreateStringResponseBody("Redirect Content");

      return {std::move(response)};
    };

    server1->Start();
    server2->Start();

    auto resource = IHttpResource::Make();
    resource->SetOnResponse([&responseResult, &responsePromise](int64_t, IHttpResource::Response response) {
      if (response.StatusCode == static_cast<int64_t>(http::status::ok)) {
        responseResult = response;
        responsePromise.set_value();
      }
    });
    resource->SetOnData([&contentPromise, &content](int64_t, string &&responseData) {
      content = std::move(responseData);

      if (!content.empty())
        contentPromise.set_value();
    });
    resource->SetOnError([&responsePromise, &contentPromise, &error, &server1](int64_t, string &&message, bool) {
      error = std::move(message);

      responsePromise.set_value();
      contentPromise.set_value();
    });

    //clang-format off
    resource->SendRequest(
        "PATCH",
        std::move(url),
        0, /*requestId*/
        {}, /*headers*/
        {}, /*data*/
        "text",
        false, /*useIncrementalUpdates*/
        0 /*timeout*/,
        false /*withCredentials*/,
        [](int64_t) {});
    //clang-format on

    responsePromise.get_future().wait();
    contentPromise.get_future().wait();

    server2->Stop();
    server1->Stop();

    Assert::AreEqual({}, error, L"Error encountered");
    Assert::AreEqual(static_cast<int64_t>(200), responseResult.StatusCode);
    Assert::AreEqual({"Redirect Content"}, content);
  }

  TEST_METHOD(TimeoutSucceeds) {
    auto port = s_port;
    string url = "http://localhost:" + std::to_string(port);

    promise<void> getPromise;
    string error;
    int statusCode = 0;
    bool timeoutError = false;

    auto server = std::make_shared<HttpServer>(s_port);
    server->Callbacks().OnGet = [](const DynamicRequest &) -> ResponseWrapper {
      DynamicResponse response;
      response.result(http::status::ok);

      // Hold response to test client timeout
      promise<void> timer;
      timer.get_future().wait_for(std::chrono::milliseconds(2000));

      return {std::move(response)};
    };
    server->Start();

    auto resource = IHttpResource::Make();
    resource->SetOnResponse([&getPromise, &statusCode](int64_t, IHttpResource::Response response) {
      statusCode = static_cast<int>(response.StatusCode);
      getPromise.set_value();
    });
    resource->SetOnError([&getPromise, &error, &timeoutError](int64_t, string &&errorMessage, bool isTimeout) {
      error = std::move(errorMessage);
      timeoutError = isTimeout;
      getPromise.set_value();
    });
    resource->SendRequest(
        "GET",
        std::move(url),
        0, /*requestId*/
        {}, /*headers*/
        {}, /*data*/
        "text", /*responseType*/
        false, /*useIncrementalUpdates*/
        6000, /*timeout*/
        false, /*withCredentials*/
        [](int64_t) {} /*callback*/);

    getPromise.get_future().wait();
    server->Stop();

    Assert::AreEqual({}, error);
    Assert::IsFalse(timeoutError);
    Assert::AreEqual(200, statusCode);
  }

  TEST_METHOD(TimeoutFails) {
    auto port = s_port;
    string url = "http://localhost:" + std::to_string(port);

    promise<void> getPromise;
    string error;
    int statusCode = 0;
    bool timeoutError = false;

    auto server = std::make_shared<HttpServer>(s_port);
    server->Callbacks().OnGet = [](const DynamicRequest &) -> ResponseWrapper {
      DynamicResponse response;
      response.result(http::status::ok);

      // Hold response to test client timeout
      promise<void> timer;
      timer.get_future().wait_for(std::chrono::milliseconds(4000));

      return {std::move(response)};
    };
    server->Start();

    auto resource = IHttpResource::Make();
    resource->SetOnResponse([&getPromise, &statusCode](int64_t, IHttpResource::Response response) {
      statusCode = static_cast<int>(response.StatusCode);
      getPromise.set_value();
    });
    resource->SetOnError([&getPromise, &error, &timeoutError](int64_t, string &&errorMessage, bool isTimeout) {
      error = std::move(errorMessage);
      timeoutError = isTimeout;
      getPromise.set_value();
    });
    resource->SendRequest(
        "GET",
        std::move(url),
        0, /*requestId*/
        {}, /*headers*/
        {}, /*data*/
        "text", /*responseType*/
        false, /*useIncrementalUpdates*/
        2000, /*timeout*/
        false, /*withCredentials*/
        [](int64_t) {} /*callback*/);

    getPromise.get_future().wait();
    server->Stop();

    Assert::IsTrue(timeoutError);
    Assert::AreEqual({"[0x800705b4] This operation returned because the timeout period expired."}, error);
    Assert::AreEqual(0, statusCode);
  }

  TEST_METHOD(GetDisableThenReenableCacheSucceeds) {
    string url = "http://localhost:" + std::to_string(s_port);

    std::vector<promise<void>> promises;
    promises.push_back(promise<void>());
    promises.push_back(promise<void>());
    promises.push_back(promise<void>());
    promises.push_back(promise<void>());
    promises.push_back(promise<void>());
    promises.push_back(promise<void>());

    string error;
    int statusCode = 0;
    int requestCount = 0;
    int body = 1;
    string result;

    auto server = make_shared<HttpServer>(s_port);
    server->Callbacks().OnGet = [&requestCount, &body](const DynamicRequest &request) -> ResponseWrapper {
      DynamicResponse response;
      response.result(http::status::ok);
      // Re-enable cache after the third request.
      // Subsequent responses should be fetched from the client cache.
      if (requestCount < 3)
        response.insert(http::field::cache_control, "no-cache");
      response.body() = Test::CreateStringResponseBody(std::to_string(body++));

      return {std::move(response)};
    };
    server->Start();

    auto resource = IHttpResource::Make();
    resource->SetOnResponse([&statusCode, &promises](int64_t, IHttpResource::Response response) {
      statusCode = static_cast<int>(response.StatusCode);
    });
    resource->SetOnData([&result, &requestCount, &promises](int64_t, string &&content) {
      result += std::move(content);

      promises[requestCount++].set_value();
    });
    resource->SetOnError([&error, &promises](int64_t, string &&message, bool) {
      error = std::move(message);
      for (auto &p : promises)
        p.set_value();
    });

    for (auto &p : promises) {
      resource->SendRequest(
          "GET",
          string{url},
          0, /*requestId*/
          {}, /*headers*/
          {}, /*data*/
          "text",
          false, /*incremental*/
          0 /*timeout*/,
          false /*withCredentials*/,
          [](int64_t) {});

      // Synchronize response.
      p.get_future().wait();
    }
    server->Stop();

    Assert::AreEqual({}, error);
    Assert::AreEqual(200, statusCode);
    Assert::AreEqual({"123444"}, result);
  }
};

/*static*/ uint16_t HttpResourceIntegrationTest::s_port = 4444;

} // namespace Microsoft::React::Test
