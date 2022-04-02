// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include <CppUnitTest.h>

#include <Networking/IHttpResource.h>
#include <Networking/OriginPolicy.h>
#include <RuntimeOptions.h>
#include <Test/HttpServer.h>

// Boost Library
#include <boost/beast/http.hpp>

// Standard Library
#include <future>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace http = boost::beast::http;

using Microsoft::React::Networking::IHttpResource;
using Microsoft::React::Networking::OriginPolicy;
using std::make_shared;
using std::promise;
using std::string;
// using Test::DynamicRequest;
// using Test::DynamicResponse;
// using Test::HttpServer;
// using Test::ResponseWrapper;

// clang-format off
namespace Microsoft::React::Test {

TEST_CLASS(HttpOriginPolicyIntegrationTest)
{
  TEST_METHOD_CLEANUP(MethodCleanup)
  {
    SetRuntimeOptionInt("Http.OriginPolicy", static_cast<int32_t>(OriginPolicy::None));
  }

  TEST_METHOD(FullCorsPreflightSucceeds)
  {
    SetRuntimeOptionInt("Http.OriginPolicy", static_cast<int32_t>(OriginPolicy::CrossOriginResourceSharing));

    constexpr uint16_t port{5555};
    constexpr char url[]{"http://localhost:5555"};

    string error;
    string getContent;
    IHttpResource::Response getResponse;
    promise<void> getDataPromise;

    auto server = make_shared<HttpServer>(port);
    server->Callbacks().OnOptions = [&url](const DynamicRequest& request) -> ResponseWrapper
    {
      EmptyResponse response;
      response.result(http::status::accepted);

      response.set(http::field::access_control_allow_credentials, "false");
      response.set(http::field::access_control_allow_headers, "ValidHeader");
      response.set(http::field::access_control_allow_methods, "GET, POST, DELETE, PATCH");
      response.set(http::field::access_control_allow_origin, url);

      return {std::move(response)};
    };
    server->Callbacks().OnGet = [](const DynamicRequest& request) -> ResponseWrapper
    {
      StringResponse response;
      response.result(http::status::ok);
      response.body() = "GET_CONTENT";

      return {std::move(response)};
    };
    server->Start();

    auto resource = IHttpResource::Make(url);
    resource->SetOnResponse([&getResponse](int64_t, IHttpResource::Response&& res)
    {
      getResponse = std::move(res);
    });
    resource->SetOnData([&getDataPromise, &getContent](int64_t, string&& content)
    {
      getContent = std::move(content);
      getDataPromise.set_value();
    });
    resource->SetOnError([&server, &error, &getDataPromise](int64_t, string&& message)
    {
      error = std::move(message);
      getDataPromise.set_value();

      server->Stop();
    });

    resource->SendRequest(
      "GET",
      url,
      {
        {"ValidHeader", "AnyValue"}
      },
      {} /*bodyData*/,
      "text",
      false /*useIncrementalUpdates*/,
      1000 /*timeout*/,
      false /*withCredentials*/,
      [](int64_t){} /*callback*/
    );

    getDataPromise.get_future().wait();
    server->Stop();

    Assert::AreEqual({}, error);
    Assert::AreEqual(200, static_cast<int>(getResponse.StatusCode));
    Assert::AreEqual({"GET_CONTENT"}, getContent);
  }// FullCorsPreflightSucceeds

  //NoCors_InvalidMethod_Failed

  BEGIN_TEST_METHOD_ATTRIBUTE(NoCorsForbiddenMethodSucceeds)
    // CONNECT, TRACE, and TRACK methods not supported by Windows.Web.Http
    // https://docs.microsoft.com/en-us/uwp/api/windows.web.http.httpmethod?view=winrt-19041#properties
    TEST_IGNORE()
  END_TEST_METHOD_ATTRIBUTE()
  TEST_METHOD(NoCorsForbiddenMethodSucceeds)
  {
    SetRuntimeOptionInt("Http.OriginPolicy", static_cast<int32_t>(OriginPolicy::None));

    constexpr uint16_t port{5556};
    constexpr char url[]{"http://localhost:5556"};

    string error;
    string getContent;
    IHttpResource::Response getResponse;
    promise<void> getDataPromise;

    auto server = make_shared<HttpServer>(port);
    server->Callbacks().OnOptions = [&url](const DynamicRequest& request) -> ResponseWrapper
    {
      EmptyResponse response;
      response.result(http::status::accepted);

      response.set(http::field::access_control_allow_credentials, "false");
      response.set(http::field::access_control_allow_headers, "ValidHeader");
      response.set(http::field::access_control_allow_methods, "GET, POST, DELETE, PATCH");
      response.set(http::field::access_control_allow_origin, url);

      return {std::move(response)};
    };
    server->Callbacks().OnTrace = [](const DynamicRequest& request) -> ResponseWrapper
    {
      StringResponse response;
      response.result(http::status::ok);
      response.body() = "GET_CONTENT";

      return {std::move(response)};
    };
    server->Start();

    auto resource = IHttpResource::Make();
    resource->SetOnResponse([&getResponse](int64_t, IHttpResource::Response&& res)
    {
      getResponse = std::move(res);
    });
    resource->SetOnData([&getDataPromise, &getContent](int64_t, string&& content)
    {
      getContent = std::move(content);
      getDataPromise.set_value();
    });
    resource->SetOnError([&server, &error, &getDataPromise](int64_t, string&& message)
    {
      error = std::move(message);
      getDataPromise.set_value();
    });

    resource->SendRequest(
      "TRACE",
      url,
      {
        {"ValidHeader", "AnyValue"}
      },
      {} /*bodyData*/,
      "text",
      false /*useIncrementalUpdates*/,
      1000 /*timeout*/,
      false /*withCredentials*/,
      [](int64_t){} /*callback*/
    );

    getDataPromise.get_future().wait();
    server->Stop();

    Assert::AreEqual({}, error);
    Assert::AreEqual(200, static_cast<int>(getResponse.StatusCode));
    Assert::AreEqual({"GET_CONTENT"}, getContent);
  }// NoCorsForbiddenMethodSucceeds

  BEGIN_TEST_METHOD_ATTRIBUTE(SimpleCorsForbiddenMethodFails)
  END_TEST_METHOD_ATTRIBUTE()
  TEST_METHOD(SimpleCorsForbiddenMethodFails)
  {
    SetRuntimeOptionInt("Http.OriginPolicy", static_cast<int32_t>(OriginPolicy::SimpleCrossOriginResourceSharing));

    constexpr uint16_t port{5557};
    constexpr char url[]{"http://localhost:5557"};

    string error;
    string getContent;
    IHttpResource::Response getResponse;
    promise<void> getDataPromise;

    auto server = make_shared<HttpServer>(port);
    server->Callbacks().OnOptions = [&url](const DynamicRequest& request) -> ResponseWrapper
    {
      EmptyResponse response;
      response.result(http::status::accepted);

      response.set(http::field::access_control_allow_credentials, "false");
      response.set(http::field::access_control_allow_methods, "GET, POST, DELETE, PATCH");
      response.set(http::field::access_control_allow_origin, url);

      return {std::move(response)};
    };
    server->Callbacks().OnConnect = [](const DynamicRequest& request) -> ResponseWrapper
    {
      StringResponse response;
      response.result(http::status::ok);
      response.body() = "GET_CONTENT";

      return {std::move(response)};
    };
    server->Start();

    auto resource = IHttpResource::Make("http://example.com");
    resource->SetOnResponse([&getResponse](int64_t, IHttpResource::Response&& res)
    {
      getResponse = std::move(res);
    });
    resource->SetOnData([&getDataPromise, &getContent](int64_t, string&& content)
    {
      getContent = std::move(content);
      getDataPromise.set_value();
    });
    resource->SetOnError([&server, &error, &getDataPromise](int64_t, string&& message)
    {
      error = std::move(message);
      getDataPromise.set_value();
    });

    resource->SendRequest(
      "CONNECT",
      url,
      {
        {"Content-Type", "text/plain"}
      },
      {} /*bodyData*/,
      "text",
      false /*useIncrementalUpdates*/,
      1000 /*timeout*/,
      false /*withCredentials*/,
      [](int64_t){} /*callback*/
    );

    getDataPromise.get_future().wait();
    server->Stop();

    Assert::AreNotEqual({}, error);
  }// SimpleCorsForbiddenMethodFails

  //NoCors_ForbiddenMethodConnect_Failed

  BEGIN_TEST_METHOD_ATTRIBUTE(NoCorsCrossOriginFetchRequestSucceeds)
  END_TEST_METHOD_ATTRIBUTE()
  TEST_METHOD(NoCorsCrossOriginFetchRequestSucceeds)
  {
    SetRuntimeOptionInt("Http.OriginPolicy", static_cast<int32_t>(OriginPolicy::None));

    constexpr uint16_t port{5558};
    constexpr char url[]{"http://localhost:5558"};

    string error;
    string getContent;
    IHttpResource::Response getResponse;
    promise<void> getDataPromise;

    auto server = make_shared<HttpServer>(port);
    server->Callbacks().OnOptions = [&url](const DynamicRequest& request) -> ResponseWrapper
    {
      EmptyResponse response;
      response.result(http::status::accepted);

      response.set(http::field::access_control_allow_credentials, "false");
      response.set(http::field::access_control_allow_methods, "GET, POST, DELETE, PATCH");
      response.set(http::field::access_control_allow_origin, url);

      return {std::move(response)};
    };
    server->Callbacks().OnGet = [](const DynamicRequest& request) -> ResponseWrapper
    {
      StringResponse response;
      response.result(http::status::ok);
      response.body() = "GET_CONTENT";

      return {std::move(response)};
    };
    server->Start();

    auto resource = IHttpResource::Make("http://example.com");
    resource->SetOnResponse([&getResponse](int64_t, IHttpResource::Response&& res)
    {
      getResponse = std::move(res);
    });
    resource->SetOnData([&getDataPromise, &getContent](int64_t, string&& content)
    {
      getContent = std::move(content);
      getDataPromise.set_value();
    });
    resource->SetOnError([&server, &error, &getDataPromise](int64_t, string&& message)
    {
      error = std::move(message);
      getDataPromise.set_value();
    });

    resource->SendRequest(
      "GET",
      url,
      {
        { "Content-Type", "text/plain" },
      },
      {} /*bodyData*/,
      "text",
      false /*useIncrementalUpdates*/,
      1000 /*timeout*/,
      false /*withCredentials*/,
      [](int64_t){} /*callback*/
    );

    getDataPromise.get_future().wait();
    server->Stop();

    Assert::AreEqual({}, error);
    Assert::AreEqual(200, static_cast<int>(getResponse.StatusCode));
    Assert::AreEqual({"GET_CONTENT"}, getContent);
  }// NoCorsCrossOriginFetchRequestSucceeds

  //NoCors_CrossOriginFetchRequestWithTimeout_Succeeded //TODO: Implement timeout

  BEGIN_TEST_METHOD_ATTRIBUTE(NoCorsCrossOriginPatchSucceededs)
  END_TEST_METHOD_ATTRIBUTE()
  TEST_METHOD(NoCorsCrossOriginPatchSucceededs)
  {
    SetRuntimeOptionInt("Http.OriginPolicy", static_cast<int32_t>(OriginPolicy::None));

    constexpr uint16_t port{5559};
    constexpr char url[]{"http://localhost:5559"};

    string error;
    string getContent;
    IHttpResource::Response getResponse;
    promise<void> getDataPromise;

    auto server = make_shared<HttpServer>(port);
    server->Callbacks().OnOptions = [&url](const DynamicRequest& request) -> ResponseWrapper
    {
      EmptyResponse response;
      response.result(http::status::accepted);

      response.set(http::field::access_control_allow_credentials, "false");
      response.set(http::field::access_control_allow_methods, "GET, POST, DELETE, PATCH");
      response.set(http::field::access_control_allow_origin, url);

      return {std::move(response)};
    };
    server->Callbacks().OnPatch = [](const DynamicRequest& request) -> ResponseWrapper
    {
      StringResponse response;
      response.result(http::status::ok);
      response.body() = "SERVER_CONTENT";

      return {std::move(response)};
    };
    server->Start();

    auto resource = IHttpResource::Make("http://example.com");
    resource->SetOnResponse([&getResponse](int64_t, IHttpResource::Response&& res)
    {
      getResponse = std::move(res);
    });
    resource->SetOnData([&getDataPromise, &getContent](int64_t, string&& content)
    {
      getContent = std::move(content);
      getDataPromise.set_value();
    });
    resource->SetOnError([&server, &error, &getDataPromise](int64_t, string&& message)
    {
      error = std::move(message);
      getDataPromise.set_value();
    });

    resource->SendRequest(
      "PATCH",
      url,
      {
        { "Content-Type", "text/plain" },
      },
      {} /*bodyData*/,
      "text",
      false /*useIncrementalUpdates*/,
      1000 /*timeout*/,
      false /*withCredentials*/,
      [](int64_t){} /*callback*/
    );

    getDataPromise.get_future().wait();
    server->Stop();

    Assert::AreEqual({}, error);
    Assert::AreEqual(200, static_cast<int>(getResponse.StatusCode));
    Assert::AreEqual({"SERVER_CONTENT"}, getContent);
  }// NoCorsCrossOriginPatchSucceededs

  // Simple-Cors — Prevents the method from being anything other than HEAD, GET or POST,
  // and the headers from being anything other than simple headers (CORS safe listed headers).
  // If any ServiceWorkers intercept these requests, they may not add or override any headers except for those that are simple headers.
  // In addition, JavaScript may not access any properties of the resulting Response.
  // This ensures that ServiceWorkers do not affect the semantics of the Web and prevents security and privacy issues arising from leaking data across domains.
  BEGIN_TEST_METHOD_ATTRIBUTE(SimpleCorsSameOriginSucceededs)
  END_TEST_METHOD_ATTRIBUTE()
  TEST_METHOD(SimpleCorsSameOriginSucceededs)
  {
    SetRuntimeOptionInt("Http.OriginPolicy", static_cast<int32_t>(OriginPolicy::SimpleCrossOriginResourceSharing));

    constexpr uint16_t port{5560};
    constexpr char url[]{"http://localhost:5560"};

    string error;
    string getContent;
    IHttpResource::Response getResponse;
    promise<void> getDataPromise;

    auto server = make_shared<HttpServer>(port);
    server->Callbacks().OnOptions = [&url](const DynamicRequest& request) -> ResponseWrapper
    {
      EmptyResponse response;
      response.result(http::status::accepted);

      response.set(http::field::access_control_allow_credentials, "false");
      response.set(http::field::access_control_allow_methods, "GET, POST, DELETE, PATCH");

      return {std::move(response)};
    };
    server->Callbacks().OnGet = [](const DynamicRequest& request) -> ResponseWrapper
    {
      StringResponse response;
      response.result(http::status::ok);
      response.body() = "SERVER_CONTENT";

      return {std::move(response)};
    };
    server->Start();

    auto resource = IHttpResource::Make(url);
    resource->SetOnResponse([&getResponse](int64_t, IHttpResource::Response&& res)
    {
      getResponse = std::move(res);
    });
    resource->SetOnData([&getDataPromise, &getContent](int64_t, string&& content)
    {
      getContent = std::move(content);
      getDataPromise.set_value();
    });
    resource->SetOnError([&server, &error, &getDataPromise](int64_t, string&& message)
    {
      error = std::move(message);
      getDataPromise.set_value();
    });

    resource->SendRequest(
      "GET",
      url,
      {
        { "Content-Type", "text/plain" },
      },
      {} /*bodyData*/,
      "text",
      false /*useIncrementalUpdates*/,
      1000 /*timeout*/,
      false /*withCredentials*/,
      [](int64_t){} /*callback*/
    );

    getDataPromise.get_future().wait();
    server->Stop();

    Assert::AreEqual({}, error);
    Assert::AreEqual(200, static_cast<int>(getResponse.StatusCode));
    Assert::AreEqual({"SERVER_CONTENT"}, getContent);
  }// SimpleCorsSameOriginSucceededs

  //TODO!
  BEGIN_TEST_METHOD_ATTRIBUTE(SimpleCorsCrossOriginFetchFails)
  END_TEST_METHOD_ATTRIBUTE()
  TEST_METHOD(SimpleCorsCrossOriginFetchFails)
  {
    SetRuntimeOptionInt("Http.OriginPolicy", static_cast<int32_t>(OriginPolicy::SimpleCrossOriginResourceSharing));

    constexpr uint16_t port{5561};
    constexpr char url[]{"http://localhost:5561"};

    string error;
    string getContent;
    IHttpResource::Response getResponse;
    promise<void> getDataPromise;

    auto server = make_shared<HttpServer>(port);
    server->Callbacks().OnOptions = [&url](const DynamicRequest& request) -> ResponseWrapper
    {
      EmptyResponse response;
      response.result(http::status::accepted);

      response.set(http::field::access_control_allow_credentials, "false");
      response.set(http::field::access_control_allow_methods, "GET, POST, DELETE, PATCH");

      return {std::move(response)};
    };
    server->Callbacks().OnGet = [](const DynamicRequest& request) -> ResponseWrapper
    {
      StringResponse response;
      response.result(http::status::ok);
      response.body() = "SERVER_CONTENT";

      return {std::move(response)};
    };
    server->Start();

    auto resource = IHttpResource::Make("http://example.com");
    resource->SetOnResponse([&getResponse](int64_t, IHttpResource::Response&& res)
    {
      getResponse = std::move(res);
    });
    resource->SetOnData([&getDataPromise, &getContent](int64_t, string&& content)
    {
      getContent = std::move(content);
      getDataPromise.set_value();
    });
    resource->SetOnError([&server, &error, &getDataPromise](int64_t, string&& message)
    {
      error = std::move(message);
      getDataPromise.set_value();
    });

    resource->SendRequest(
      "GET",
      url,
      {
        { "Content-Type", "text/html" }, // text/html is a non-simple value
      },
      {} /*bodyData*/,
      "text",
      false /*useIncrementalUpdates*/,
      1000 /*timeout*/,
      false /*withCredentials*/,
      [](int64_t){} /*callback*/
    );

    getDataPromise.get_future().wait();
    server->Stop();

    Assert::AreNotEqual({}, error);
  }// SimpleCorsCrossOriginFetchFails

};

}//namespace
// clang-format on
