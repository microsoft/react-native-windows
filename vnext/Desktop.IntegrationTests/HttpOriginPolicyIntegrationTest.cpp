// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include <CppUnitTest.h>

#include <Networking/IHttpResource.h>
#include <Networking/OriginPolicy.h>
#include <RuntimeOptions.h>
#include <Test/HttpServer.h>

// Standard Library
#include <future>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace http = boost::beast::http;

using Microsoft::React::Networking::IHttpResource;
using Microsoft::React::Networking::OriginPolicy;
using std::make_shared;
using std::promise;
using std::string;

namespace {
constexpr char s_serverHost[]{"http://localhost"};
constexpr char s_crossOriginUrl[]{"http://example.rnw"};
}//anonymous

// clang-format off
namespace Microsoft::React::Test {

TEST_CLASS(HttpOriginPolicyIntegrationTest)
{
  struct ServerParams
  {
    uint16_t Port;
    string Url;
    EmptyResponse Preflight;
    StringResponse Response;

    ServerParams(
      uint16_t port) noexcept
      : Port{port}
      , Url{s_serverHost + string{":"} + std::to_string(port)}
    {
      Preflight.set(http::field::access_control_allow_methods, "GET, POST, DELETE, PATCH");

      Response.result(http::status::ok);
      Response.body() = "RESPONSE_CONTENT";
    }
  };

  struct ClientParams
  {
    promise<void> ContentPromise;
    string ErrorMessage;
    IHttpResource::Response Response;
    string ResponseContent;
    http::verb Method;
    IHttpResource::Headers RequestHeaders;
    bool WithCredentials{false};

    ClientParams(http::verb method, IHttpResource::Headers&& headers)
      : Method{ method }
      , RequestHeaders{ std::move(headers) }
    {
    }
  };

  void TestOriginPolicy(ServerParams& serverArgs, ClientParams& clientArgs, bool shouldSucceed)
  {
    auto server = make_shared<HttpServer>(serverArgs.Port);
    server->Callbacks().OnOptions = [/*origin, */&serverArgs](const DynamicRequest& request) -> ResponseWrapper
    {
      return { std::move(serverArgs.Preflight) };
    };

    auto reqHandler = [&serverArgs](const DynamicRequest& request) -> ResponseWrapper
    {
      return { std::move(serverArgs.Response) };
    };

    switch (clientArgs.Method)
    {
      case http::verb::get:
        server->Callbacks().OnGet = reqHandler;
        break;

      case http::verb::post:
        server->Callbacks().OnPost = reqHandler;
        break;

      case http::verb::patch:
        server->Callbacks().OnPatch = reqHandler;
        break;

      case http::verb::trace:
        server->Callbacks().OnTrace = reqHandler;
        break;

      case http::verb::connect:
        server->Callbacks().OnConnect = reqHandler;
        break;

      case http::verb::options:
      default:
	    Assert::Fail(L"Unsupported request method");
    }

    server->Start();

    auto resource = IHttpResource::Make();
    resource->SetOnResponse([&clientArgs](int64_t, IHttpResource::Response&& res)
    {
        clientArgs.Response = std::move(res);
    });
    resource->SetOnData([&clientArgs](int64_t, string&& content)
    {
      clientArgs.ResponseContent = std::move(content);
      clientArgs.ContentPromise.set_value();
    });
    resource->SetOnError([&clientArgs](int64_t, string&& message)
    {
      clientArgs.ErrorMessage = std::move(message);
      clientArgs.ContentPromise.set_value();
    });

    resource->SendRequest(
      string{http::to_string(clientArgs.Method).data()},
      string{serverArgs.Url},
      std::move(clientArgs.RequestHeaders),
      { IHttpResource::BodyData::Type::String, "REQUEST_CONTENT" },
      "text",
      false,                      /*useIncrementalUpdates*/
      1000,                       /*timeout*/
      clientArgs.WithCredentials, /*withCredentials*/
      [](int64_t) {}              /*reactCallback*/
    );

    clientArgs.ContentPromise.get_future().wait();
    server->Stop();

    if (shouldSucceed)
    {
      Assert::AreEqual({}, clientArgs.ErrorMessage);
      Assert::AreEqual(static_cast<int>(http::status::ok), static_cast<int>(clientArgs.Response.StatusCode));
      Assert::AreEqual({"RESPONSE_CONTENT"}, clientArgs.ResponseContent);
    }
    else
    {
      Assert::AreNotEqual({}, clientArgs.ErrorMessage);
    }
  }

  TEST_METHOD_CLEANUP(MethodCleanup)
  {
    SetRuntimeOptionInt("Http.OriginPolicy", static_cast<int32_t>(OriginPolicy::None));
    SetRuntimeOptionString("Http.GlobalOrigin", nullptr);
  }

  TEST_METHOD(FullCorsPreflightSucceeds)
  {
    ServerParams serverArgs(5555);
    serverArgs.Preflight.set(http::field::access_control_allow_headers, "ValidHeader");
    serverArgs.Preflight.set(http::field::access_control_allow_origin, s_crossOriginUrl);

    ClientParams clientArgs(http::verb::get, {{"ValidHeader", "AnyValue"}});

    SetRuntimeOptionString("Http.GlobalOrigin", s_crossOriginUrl);
    SetRuntimeOptionInt("Http.OriginPolicy", static_cast<int32_t>(OriginPolicy::CrossOriginResourceSharing));
    TestOriginPolicy(serverArgs, clientArgs, true /*shouldSucceed*/);
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

    constexpr uint16_t port{ 5556 };
    constexpr char url[]{ "http://localhost:5556" };

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

      return { std::move(response) };
    };
    server->Callbacks().OnTrace = [](const DynamicRequest& request) -> ResponseWrapper
    {
      StringResponse response;
      response.result(http::status::ok);
      response.body() = "GET_CONTENT";

      return { std::move(response) };
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
      [](int64_t) {} /*callback*/
    );

    getDataPromise.get_future().wait();
    server->Stop();

    Assert::AreEqual({}, error);
    Assert::AreEqual(200, static_cast<int>(getResponse.StatusCode));
    Assert::AreEqual({ "GET_CONTENT" }, getContent);
  }// NoCorsForbiddenMethodSucceeds

  BEGIN_TEST_METHOD_ATTRIBUTE(SimpleCorsForbiddenMethodFails)
  END_TEST_METHOD_ATTRIBUTE()
  TEST_METHOD(SimpleCorsForbiddenMethodFails)
  {
    ServerParams serverArgs(5557);
    serverArgs.Preflight.set(http::field::access_control_allow_origin, serverArgs.Url);

    ClientParams clientArgs(http::verb::connect, {{"Content-Type", "text/plain"}});

    SetRuntimeOptionString("Http.GlobalOrigin", s_crossOriginUrl);
    SetRuntimeOptionInt("Http.OriginPolicy", static_cast<int32_t>(OriginPolicy::SimpleCrossOriginResourceSharing));
    TestOriginPolicy(serverArgs, clientArgs, false /*shouldSucceed*/);
  }// SimpleCorsForbiddenMethodFails

  //NoCors_ForbiddenMethodConnect_Failed

  BEGIN_TEST_METHOD_ATTRIBUTE(NoCorsCrossOriginFetchRequestSucceeds)
  END_TEST_METHOD_ATTRIBUTE()
  TEST_METHOD(NoCorsCrossOriginFetchRequestSucceeds)
  {
    SetRuntimeOptionString("Http.GlobalOrigin", s_crossOriginUrl);
    SetRuntimeOptionInt("Http.OriginPolicy", static_cast<int32_t>(OriginPolicy::None));

    ServerParams serverArgs(5558);

    ClientParams clientArgs(http::verb::get, {{ "Content-Type", "text/plain" }});

    TestOriginPolicy(serverArgs, clientArgs, true /*shouldSucceed*/);
  }// NoCorsCrossOriginFetchRequestSucceeds

  //NoCors_CrossOriginFetchRequestWithTimeout_Succeeded //TODO: Implement timeout

  BEGIN_TEST_METHOD_ATTRIBUTE(NoCorsCrossOriginPatchSucceededs)
  END_TEST_METHOD_ATTRIBUTE()
  TEST_METHOD(NoCorsCrossOriginPatchSucceededs)
  {
    SetRuntimeOptionString("Http.GlobalOrigin", s_crossOriginUrl);
    SetRuntimeOptionInt("Http.OriginPolicy", static_cast<int32_t>(OriginPolicy::None));

    ServerParams serverArgs(5559);

    ClientParams clientArgs(http::verb::patch, {{ "Content-Type", "text/plain" }});

    TestOriginPolicy(serverArgs, clientArgs, true /*shouldSucceed*/);
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
    ServerParams serverArgs(5560);

    ClientParams clientArgs(http::verb::patch, {{ "Content-Type", "text/plain" }});

    SetRuntimeOptionString("Http.GlobalOrigin", serverArgs.Url.c_str());
    SetRuntimeOptionInt("Http.OriginPolicy", static_cast<int32_t>(OriginPolicy::SimpleCrossOriginResourceSharing));
    TestOriginPolicy(serverArgs, clientArgs, true /*shouldSucceed*/);
  }// SimpleCorsSameOriginSucceededs

  BEGIN_TEST_METHOD_ATTRIBUTE(SimpleCorsCrossOriginFetchFails)
  END_TEST_METHOD_ATTRIBUTE()
  TEST_METHOD(SimpleCorsCrossOriginFetchFails)
  {
    ServerParams serverArgs(5561);

    ClientParams clientArgs(http::verb::get, {{ "Content-Type", "text/html" }});  // text/html is a non-simple value

    SetRuntimeOptionString("Http.GlobalOrigin", s_crossOriginUrl);
    SetRuntimeOptionInt("Http.OriginPolicy", static_cast<int32_t>(OriginPolicy::SimpleCrossOriginResourceSharing));
    TestOriginPolicy(serverArgs, clientArgs, false /*shouldSucceed*/);
  }// SimpleCorsCrossOriginFetchFails

  BEGIN_TEST_METHOD_ATTRIBUTE(FullCorsSameOriginRequestSucceeds)
  END_TEST_METHOD_ATTRIBUTE()
  TEST_METHOD(FullCorsSameOriginRequestSucceeds)
  {
    ServerParams serverArgs(5562);

    ClientParams clientArgs(http::verb::get, {{ "Content-Type", "text/plain" }});  // text/plain is a non-simple header

    SetRuntimeOptionString("Http.GlobalOrigin", serverArgs.Url.c_str());
    SetRuntimeOptionInt("Http.OriginPolicy", static_cast<int32_t>(OriginPolicy::CrossOriginResourceSharing));
    TestOriginPolicy(serverArgs, clientArgs, true /*shouldSucceed*/);
  }// FullCorsSameOriginRequestSucceeds

  BEGIN_TEST_METHOD_ATTRIBUTE(FullCorsCrossOriginAllowOriginWildcardSucceeds)
  END_TEST_METHOD_ATTRIBUTE()
  TEST_METHOD(FullCorsCrossOriginAllowOriginWildcardSucceeds)
  {
    ServerParams serverArgs(5563);
    serverArgs.Preflight.set(http::field::access_control_allow_headers,   "Content-Type");
    serverArgs.Preflight.set(http::field::access_control_allow_origin,    "*");
    serverArgs.Preflight.set(http::field::access_control_request_headers, "Content-Type");

    ClientParams clientArgs(http::verb::get, {{ "Content-Type", "text/plain" }});  // text/plain is a non-simple header

    SetRuntimeOptionString("Http.GlobalOrigin", s_crossOriginUrl);
    SetRuntimeOptionInt("Http.OriginPolicy", static_cast<int32_t>(OriginPolicy::CrossOriginResourceSharing));

    TestOriginPolicy(serverArgs, clientArgs, true /*shouldSucceed*/);
  }// FullCorsCrossOriginAllowOriginWildcardSucceeds

  // With CORS, Cross-Origin Resource Sharing, the server can decide what origins are permitted to read information from the client.
  // Additionally, for non-simple requests, client should preflight the request through the HTTP Options request, and only send the
  // actual request after the server has responded that the desired headers are supported.
  BEGIN_TEST_METHOD_ATTRIBUTE(FullCorsCrossOriginMatchingOriginSucceeds)
  END_TEST_METHOD_ATTRIBUTE()
  TEST_METHOD(FullCorsCrossOriginMatchingOriginSucceeds)
  {
    ServerParams serverArgs(5564);
    serverArgs.Preflight.set(http::field::access_control_allow_headers,   "Content-Type");
    serverArgs.Preflight.set(http::field::access_control_allow_origin,    s_crossOriginUrl);
    serverArgs.Preflight.set(http::field::access_control_request_headers, "Content-Type");

    ClientParams clientArgs(http::verb::get, {{ "Content-Type", "text/plain" }});  // text/plain is a non-simple header

    SetRuntimeOptionString("Http.GlobalOrigin", s_crossOriginUrl);
    SetRuntimeOptionInt("Http.OriginPolicy", static_cast<int32_t>(OriginPolicy::CrossOriginResourceSharing));

    TestOriginPolicy(serverArgs, clientArgs, true /*shouldSucceed*/);
  }// FullCorsCrossOriginMatchingOriginSucceeds

  BEGIN_TEST_METHOD_ATTRIBUTE(FullCorsCrossOriginWithCredentialsFails)
  END_TEST_METHOD_ATTRIBUTE()
  TEST_METHOD(FullCorsCrossOriginWithCredentialsFails)
  {
    ServerParams serverArgs(5565);
    serverArgs.Preflight.set(http::field::access_control_request_headers,   "Content-Type");
    serverArgs.Preflight.set(http::field::access_control_allow_headers,     "Content-Type");
    serverArgs.Preflight.set(http::field::access_control_allow_origin,      s_crossOriginUrl);
    serverArgs.Preflight.set(http::field::access_control_allow_credentials, "true");
    serverArgs.Response.set(http::field::access_control_allow_origin,       s_crossOriginUrl);

    ClientParams clientArgs(http::verb::get, {{ "Content-Type", "application/text" }}); // application/text is a non-simple header
    clientArgs.WithCredentials = true;

    SetRuntimeOptionString("Http.GlobalOrigin", s_crossOriginUrl);
    SetRuntimeOptionInt("Http.OriginPolicy", static_cast<int32_t>(OriginPolicy::CrossOriginResourceSharing));
    SetRuntimeOptionBool("Http.OmitCredentials", true);

    TestOriginPolicy(serverArgs, clientArgs, false /*shouldSucceed*/);
  }// FullCorsCrossOriginWithCredentialsFails

  BEGIN_TEST_METHOD_ATTRIBUTE(FullCorsCrossOriginWithCredentialsSucceeds)
  END_TEST_METHOD_ATTRIBUTE()
  TEST_METHOD(FullCorsCrossOriginWithCredentialsSucceeds)
  {
    ServerParams serverArgs(5566);
    serverArgs.Preflight.set(http::field::access_control_request_headers,   "Content-Type");
    serverArgs.Preflight.set(http::field::access_control_allow_headers,     "Content-Type");
    serverArgs.Preflight.set(http::field::access_control_allow_origin,      s_crossOriginUrl);
    serverArgs.Preflight.set(http::field::access_control_allow_credentials, "true");

    ClientParams clientArgs(http::verb::get, {{ "Content-Type", "application/text" }}); // application/text is a non-simple header
    clientArgs.WithCredentials = true;

    SetRuntimeOptionString("Http.GlobalOrigin", s_crossOriginUrl);
    SetRuntimeOptionInt("Http.OriginPolicy", static_cast<int32_t>(OriginPolicy::CrossOriginResourceSharing));

    TestOriginPolicy(serverArgs, clientArgs, true /*shouldSucceed*/);
  }// FullCorsCrossOriginWithCredentialsSucceeds
};

}//namespace
// clang-format on
