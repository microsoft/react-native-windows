// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include <CppUnitTest.h>

#include <Networking/IHttpResource.h>
#include <Networking/OriginPolicy.h>
#include <RuntimeOptions.h>
#include <unicode.h>

// Standard Library
#include <chrono>
#include <future>

using namespace Microsoft::React;
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

using Networking::IHttpResource;
using Networking::OriginPolicy;
using std::promise;
using std::string;
using std::vector;

namespace Microsoft::React::Test {

namespace {

const string kHttpResourceBaseUrl{"http://localhost:5555/rnw/http"};

string MakeHttpResourceUrl(const char *path) {
  return kHttpResourceBaseUrl + path;
}

string MakeCacheUrl(const string &requestId) {
  return kHttpResourceBaseUrl + "/cache/" + requestId;
}

} // namespace

TEST_CLASS (HttpResourceIntegrationTest) {
  static uint16_t s_requestId;

  TEST_METHOD_CLEANUP(MethodCleanup) {
    // Clear any runtime options that may be used by tests in this class.
    MicrosoftReactSetRuntimeOptionString("Http.UserAgent", nullptr);

    s_requestId++;
  }
  TEST_METHOD(RequestGetSucceeds) {
    string url = MakeHttpResourceUrl("/get");

    promise<void> resPromise;
    string error;
    int statusCode = 0;

    auto resource = IHttpResource::Make();
    resource->SetOnResponse([&resPromise, &statusCode](int64_t, IHttpResource::Response response) {
      statusCode = static_cast<int>(response.StatusCode);
    });
    resource->SetOnData([&resPromise](int64_t, string &&content) { resPromise.set_value(); });
    resource->SetOnError([&resPromise, &error](int64_t, string &&message, bool) {
      error = std::move(message);
      resPromise.set_value();
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

    Assert::AreEqual({}, error);
    Assert::AreEqual(200, statusCode);
  }

  TEST_METHOD(RequestGetHeadersSucceeds) {
    string url = MakeHttpResourceUrl("/headers");

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

    Assert::AreEqual({}, error, L"Error encountered");
    bool foundServerHeader = false;
    bool foundContentLengthHeader = false;
    bool foundResponseHeader = false;
    for (auto header : response.Headers) {
      if (header.first == "Server") {
        Assert::IsTrue(header.second.find("Microsoft.React.Test.Website") != string::npos, L"Wrong header");
        foundServerHeader = true;
      } else if (header.first == "Content-Length") {
        Assert::AreEqual({"0"}, header.second, L"Wrong header");
        foundContentLengthHeader = true;
      } else if (header.first == "ResponseHeaderName1") {
        Assert::AreEqual({"ResponseHeaderValue1"}, header.second, L"Wrong header");
        foundResponseHeader = true;
      }
    }
    Assert::IsTrue(foundServerHeader, L"Missing Server header");
    Assert::IsTrue(foundContentLengthHeader, L"Missing Content-Length header");
    Assert::IsTrue(foundResponseHeader, L"Missing custom response header");
  }

  TEST_METHOD(RequestGetExplicitUserAgentSucceeds) {
    string url = MakeHttpResourceUrl("/useragent");

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
    string url = MakeHttpResourceUrl("/useragent");

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
    string url = MakeHttpResourceUrl("/useragent");

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
    string url = MakeHttpResourceUrl("/options");

    promise<void> getResponsePromise;
    promise<void> getDataPromise;
    promise<void> optionsPromise;
    string error;
    IHttpResource::Response getResponse;
    IHttpResource::Response optionsResponse;
    string content;

    auto resource = IHttpResource::Make();
    resource->SetOnResponse([&getResponse, &getResponsePromise, &optionsResponse, &optionsPromise](
                                int64_t, IHttpResource::Response callbackResponse) {
      if (callbackResponse.StatusCode == 200) {
        getResponse = callbackResponse;
        getResponsePromise.set_value();
      } else if (callbackResponse.StatusCode == 206) {
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
        [&optionsPromise, &getResponsePromise, &getDataPromise, &error](int64_t, string &&message, bool) {
          error = std::move(message);

          optionsPromise.set_value();
          getResponsePromise.set_value();
          getDataPromise.set_value();
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

    Assert::AreEqual({}, error, L"Error encountered");
    bool foundPreflightHeader = false;
    for (auto header : optionsResponse.Headers) {
      if (header.first == "PreflightName") {
        Assert::AreEqual({"PreflightValue"}, header.second, L"Wrong header");
        foundPreflightHeader = true;
      }
    }
    Assert::IsTrue(foundPreflightHeader, L"Missing PreflightName header");
    Assert::AreEqual({"Response Body"}, content);
  }

  TEST_METHOD(SimpleRedirectGetSucceeds) {
    string url = MakeHttpResourceUrl("/redirect/get/start");

    promise<void> responsePromise;
    promise<void> contentPromise;
    IHttpResource::Response responseResult;
    string content;
    string error;

    auto resource = IHttpResource::Make();
    resource->SetOnResponse([&responseResult, &responsePromise](int64_t, IHttpResource::Response response) {
      if (response.StatusCode == 200) {
        responseResult = response;
        responsePromise.set_value();
      }
    });
    resource->SetOnData([&contentPromise, &content](int64_t, string &&responseData) {
      content = std::move(responseData);

      if (!content.empty())
        contentPromise.set_value();
    });
    resource->SetOnError([&responsePromise, &contentPromise, &error](int64_t, string &&message, bool) {
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

    Assert::AreEqual({}, error, L"Error encountered");
    Assert::AreEqual(static_cast<int64_t>(200), responseResult.StatusCode);
    Assert::AreEqual({"Redirect Content"}, content);
  }

  TEST_METHOD(SimpleRedirectPatchSucceeds) {
    string url = MakeHttpResourceUrl("/redirect/patch/start");

    promise<void> responsePromise;
    promise<void> contentPromise;
    IHttpResource::Response responseResult;
    string content;
    string error;

    auto resource = IHttpResource::Make();
    resource->SetOnResponse([&responseResult, &responsePromise](int64_t, IHttpResource::Response response) {
      if (response.StatusCode == 200) {
        responseResult = response;
        responsePromise.set_value();
      }
    });
    resource->SetOnData([&contentPromise, &content](int64_t, string &&responseData) {
      content = std::move(responseData);

      if (!content.empty())
        contentPromise.set_value();
    });
    resource->SetOnError([&responsePromise, &contentPromise, &error](int64_t, string &&message, bool) {
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

    Assert::AreEqual({}, error, L"Error encountered");
    Assert::AreEqual(static_cast<int64_t>(200), responseResult.StatusCode);
    Assert::AreEqual({"Redirect Content"}, content);
  }

  TEST_METHOD(TimeoutSucceeds) {
    string url = MakeHttpResourceUrl("/delay/2000");

    promise<void> getPromise;
    string error;
    int statusCode = 0;
    bool timeoutError = false;

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

    Assert::AreEqual({}, error);
    Assert::IsFalse(timeoutError);
    Assert::AreEqual(200, statusCode);
  }

  TEST_METHOD(TimeoutFails) {
    string url = MakeHttpResourceUrl("/delay/4000");

    promise<void> getPromise;
    string error;
    int statusCode = 0;
    bool timeoutError = false;

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

    Assert::IsTrue(timeoutError);
    Assert::AreEqual({"[0x800705b4] This operation returned because the timeout period expired."}, error);
    Assert::AreEqual(0, statusCode);
  }

  TEST_METHOD(GetDisableThenReenableCacheSucceeds) {
    const auto uniqueId =
        std::to_string(s_requestId) + "-" + std::to_string(std::chrono::steady_clock::now().time_since_epoch().count());
    string url = MakeCacheUrl(uniqueId);

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
    string result;

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

    Assert::AreEqual({}, error);
    Assert::AreEqual(200, statusCode);
    Assert::AreEqual({"123444"}, result);
  }
};

/*static*/ uint16_t HttpResourceIntegrationTest::s_requestId = 1;

} // namespace Microsoft::React::Test
