// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include <CppUnitTest.h>

#include <Networking/IHttpResource.h>
#include <Networking/OriginPolicy.h>
#include <Test/HttpServer.h>
#include <RuntimeOptions.h>

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
//using Test::DynamicRequest;
//using Test::DynamicResponse;
//using Test::HttpServer;
//using Test::ResponseWrapper;

// clang-format off
namespace Microsoft::React::Test {

TEST_CLASS(HttpOriginPolicyIntegrationTest)
{
  TEST_METHOD_CLEANUP(Cleanup)
  {
    SetRuntimeOptionInt("Http.OriginPolicy", static_cast<int32_t>(OriginPolicy::None));
  }

  TEST_METHOD(FullCorsPreflightSucceeds)
  {
    SetRuntimeOptionInt("Http.OriginPolicy", static_cast<int32_t>(OriginPolicy::CrossOriginResourceSharing));

    constexpr size_t port{5555};

    string error;
    string getContent;
    IHttpResource::Response getResponse;
    promise<void> getDataPromise;

    auto server = make_shared<HttpServer>(port);
    server->Callbacks().OnOptions = [](const DynamicRequest& request) -> ResponseWrapper
    {
      EmptyResponse response;
      response.result(http::status::accepted);

      response.set(http::field::access_control_allow_credentials, "false");
      response.set(http::field::access_control_allow_headers, "ValidHeader");
      response.set(http::field::access_control_allow_methods, "GET, POST, DELETE, PATCH");
      response.set(http::field::access_control_allow_origin, "http://localhost:5555");

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

    auto resource = IHttpResource::Make();
    resource->SetOnResponse([](int64_t, IHttpResource::Response&& res)
    {
      int x = 99;
    });
    resource->SetOnData([&getDataPromise](int64_t, string&& content)
    {
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
      "http://localhost:5555",
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
  }
};

}//namespace
// clang-format on
