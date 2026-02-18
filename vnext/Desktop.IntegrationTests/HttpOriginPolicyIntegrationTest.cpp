// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include <CppUnitTest.h>

#include <CppRuntimeOptions.h>
#include <Networking/IHttpResource.h>
#include <Networking/OriginPolicy.h>

// Standard Library
#include <future>

#include "HttpServer.h"

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
} // namespace

// clang-format off
namespace Microsoft::React::Test {

TEST_CLASS(HttpOriginPolicyIntegrationTest)
{
  static constexpr bool s_shouldSucceed{true};
  static constexpr bool s_shouldFail{false};

  static uint16_t s_port;

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

      Response.result(http::status::unknown);
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

  std::shared_ptr<HttpServer> CreateServer(ServerParams& serverArgs, ClientParams& clientArgs) noexcept
  {
    auto server = make_shared<HttpServer>(serverArgs.Port);
    server->Callbacks().OnOptions = [&serverArgs](const DynamicRequest& request) -> ResponseWrapper
    {
      return { std::move(serverArgs.Preflight) };
    };

    auto reqHandler = [&serverArgs](const DynamicRequest& request) -> ResponseWrapper
    {
      // Don't use move constructor in case of multiple requests
      return { serverArgs.Response };
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

    return server;
  }

  void TestOriginPolicyWithRedirect(ServerParams& server1Args, ServerParams& server2Args, ClientParams& clientArgs, bool shouldSucceed)
  {
    auto server1 = CreateServer(server1Args, clientArgs);
    auto server2 = CreateServer(server2Args, clientArgs);

    server1->Start();
    server2->Start();

    auto resource = IHttpResource::Make();
    resource->SetOnResponse([&clientArgs](int64_t, IHttpResource::Response&& response)
    {
      clientArgs.Response = std::move(response);
    });
    resource->SetOnData([&clientArgs](int64_t, string&& content)
    {
        clientArgs.ResponseContent = std::move(content);
        clientArgs.ContentPromise.set_value();
    });
    resource->SetOnError([&clientArgs](int64_t, string&& message, bool)
    {
      clientArgs.ErrorMessage = std::move(message);
      clientArgs.ContentPromise.set_value();
    });

    resource->SendRequest(
      string{http::to_string(clientArgs.Method).data()},
      string{server1Args.Url},
      0,                          /*requestId*/
      std::move(clientArgs.RequestHeaders),
      { { "string", "" } },       /*data*/
      "text",
      false,                      /*useIncrementalUpdates*/
      0,                          /*timeout*/
      clientArgs.WithCredentials, /*withCredentials*/
      [](int64_t){}               /*reactCallback*/
    );

    clientArgs.ContentPromise.get_future().wait();

    server2->Stop();
    server1->Stop();

    if (shouldSucceed)
    {
      Assert::AreEqual({}, clientArgs.ErrorMessage);
      //TODO: chose server?
      // We assume 2-server tests will always redirect so the final response will come from server 2.
      Assert::AreEqual(server2Args.Response.result_int(), static_cast<unsigned int>(clientArgs.Response.StatusCode));
      Assert::AreEqual({"RESPONSE_CONTENT"}, clientArgs.ResponseContent);
    }
    else
    {
      Assert::AreNotEqual({}, clientArgs.ErrorMessage);
    }
  }

  void TestOriginPolicy(ServerParams& serverArgs, ClientParams& clientArgs, bool shouldSucceed)
  {
    auto server = CreateServer(serverArgs, clientArgs);

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
    resource->SetOnError([&clientArgs](int64_t, string&& message, bool)
    {
      clientArgs.ErrorMessage = std::move(message);
      clientArgs.ContentPromise.set_value();
    });

    resource->SendRequest(
      string{http::to_string(clientArgs.Method).data()},
      string{serverArgs.Url},
      0,                          /*requestId*/
      std::move(clientArgs.RequestHeaders),
      { { "string", "" } },       /*data*/
      "text",
      false,                      /*useIncrementalUpdates*/
      0,                          /*timeout*/
      clientArgs.WithCredentials, /*withCredentials*/
      [](int64_t) {}              /*reactCallback*/
    );

    clientArgs.ContentPromise.get_future().wait();
    server->Stop();

    if (shouldSucceed)
    {
      Assert::AreEqual({}, clientArgs.ErrorMessage);
      Assert::AreEqual(serverArgs.Response.result_int(), static_cast<unsigned int>(clientArgs.Response.StatusCode));
      Assert::AreEqual({"RESPONSE_CONTENT"}, clientArgs.ResponseContent);
    }
    else
    {
      Assert::AreNotEqual({}, clientArgs.ErrorMessage);
    }
  }

  TEST_METHOD_CLEANUP(MethodCleanup)
  {
    // Clear any runtime options that may be used by tests in this class.
    SetRuntimeOptionInt("Http.OriginPolicy", static_cast<int32_t>(OriginPolicy::None));
    SetRuntimeOptionString("Http.GlobalOrigin", {});
    SetRuntimeOptionBool("Http.OmitCredentials", false);

    // Bug in HttpServer does not correctly release TCP port between test methods.
    // Using a different por per test for now.
    s_port++;
  }

  BEGIN_TEST_METHOD_ATTRIBUTE(NoCorsForbiddenMethodSucceeds)
    // CONNECT, TRACE, and TRACK methods not supported by Windows.Web.Http
    // https://docs.microsoft.com/en-us/uwp/api/windows.web.http.httpmethod?view=winrt-22621#properties
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
    resource->SetOnError([&server, &error, &getDataPromise](int64_t, string&& message, bool)
      {
        error = std::move(message);
        getDataPromise.set_value();
      });

    resource->SendRequest(
      "TRACE",
      url,
      0,                          /*requestId*/
      {
        {"ValidHeader", "AnyValue"}
      },
      { { "string", "" } },       /*data*/
      "text",
      false                       /*useIncrementalUpdates*/,
      0                           /*timeout*/,
      false                       /*withCredentials*/,
      [](int64_t) {}              /*callback*/
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
    ServerParams serverArgs(s_port);
    serverArgs.Preflight.set(http::field::access_control_allow_origin, serverArgs.Url);

    ClientParams clientArgs(http::verb::connect, {{"Content-Type", "text/plain"}});

    SetRuntimeOptionString("Http.GlobalOrigin", s_crossOriginUrl);
    SetRuntimeOptionInt("Http.OriginPolicy", static_cast<int32_t>(OriginPolicy::SimpleCrossOriginResourceSharing));

    TestOriginPolicy(serverArgs, clientArgs, s_shouldFail);
  }// SimpleCorsForbiddenMethodFails

  //NoCors_ForbiddenMethodConnect_Failed

  BEGIN_TEST_METHOD_ATTRIBUTE(NoCorsCrossOriginFetchRequestSucceeds)
  END_TEST_METHOD_ATTRIBUTE()
  TEST_METHOD(NoCorsCrossOriginFetchRequestSucceeds)
  {
    SetRuntimeOptionString("Http.GlobalOrigin", s_crossOriginUrl);
    SetRuntimeOptionInt("Http.OriginPolicy", static_cast<int32_t>(OriginPolicy::None));

    ServerParams serverArgs(s_port);
    serverArgs.Response.result(http::status::ok);

    ClientParams clientArgs(http::verb::get, {{ "Content-Type", "text/plain" }});

    TestOriginPolicy(serverArgs, clientArgs, true /*shouldSucceed*/);
  }// NoCorsCrossOriginFetchRequestSucceeds

  BEGIN_TEST_METHOD_ATTRIBUTE(NoCorsCrossOriginPatchSucceeds)
  END_TEST_METHOD_ATTRIBUTE()
  TEST_METHOD(NoCorsCrossOriginPatchSucceeds)
  {
    SetRuntimeOptionString("Http.GlobalOrigin", s_crossOriginUrl);
    SetRuntimeOptionInt("Http.OriginPolicy", static_cast<int32_t>(OriginPolicy::None));

    ServerParams serverArgs(s_port);
    serverArgs.Response.result(http::status::ok);

    ClientParams clientArgs(http::verb::patch, {{ "Content-Type", "text/plain" }});

    TestOriginPolicy(serverArgs, clientArgs, true /*shouldSucceed*/);
  }// NoCorsCrossOriginPatchSucceeds

  // Simple-Cors — Prevents the method from being anything other than HEAD, GET or POST,
  // and the headers from being anything other than simple headers (CORS safe listed headers).
  // If any ServiceWorkers intercept these requests, they may not add or override any headers except for those that are simple headers.
  // In addition, JavaScript may not access any properties of the resulting Response.
  // This ensures that ServiceWorkers do not affect the semantics of the Web and prevents security and privacy issues arising from leaking data across domains.
  BEGIN_TEST_METHOD_ATTRIBUTE(SimpleCorsSameOriginSucceeds)
  END_TEST_METHOD_ATTRIBUTE()
  TEST_METHOD(SimpleCorsSameOriginSucceeds)
  {
    ServerParams serverArgs(s_port);
    serverArgs.Response.result(http::status::ok);

    ClientParams clientArgs(http::verb::patch, {{ "Content-Type", "text/plain" }});

    SetRuntimeOptionString("Http.GlobalOrigin", serverArgs.Url.c_str());
    SetRuntimeOptionInt("Http.OriginPolicy", static_cast<int32_t>(OriginPolicy::SimpleCrossOriginResourceSharing));

    TestOriginPolicy(serverArgs, clientArgs, true /*shouldSucceed*/);
  }// SimpleCorsSameOriginSucceeds

  BEGIN_TEST_METHOD_ATTRIBUTE(SimpleCorsCrossOriginFetchFails)
  END_TEST_METHOD_ATTRIBUTE()
  TEST_METHOD(SimpleCorsCrossOriginFetchFails)
  {
    ServerParams serverArgs(s_port);

    ClientParams clientArgs(http::verb::get, {{ "Content-Type", "text/html" }});  // text/html is a non-simple value

    SetRuntimeOptionString("Http.GlobalOrigin", s_crossOriginUrl);
    SetRuntimeOptionInt("Http.OriginPolicy", static_cast<int32_t>(OriginPolicy::SimpleCrossOriginResourceSharing));

    TestOriginPolicy(serverArgs, clientArgs, s_shouldFail);
  }// SimpleCorsCrossOriginFetchFails

  BEGIN_TEST_METHOD_ATTRIBUTE(FullCorsSameOriginRequestSucceeds)
  END_TEST_METHOD_ATTRIBUTE()
  TEST_METHOD(FullCorsSameOriginRequestSucceeds)
  {
    ServerParams serverArgs(s_port);
    serverArgs.Response.result(http::status::ok);

    ClientParams clientArgs(http::verb::get, {{ "Content-Type", "text/plain" }});  // text/plain is a non-simple header

    SetRuntimeOptionString("Http.GlobalOrigin", serverArgs.Url.c_str());
    SetRuntimeOptionInt("Http.OriginPolicy", static_cast<int32_t>(OriginPolicy::CrossOriginResourceSharing));

    TestOriginPolicy(serverArgs, clientArgs, true /*shouldSucceed*/);
  }// FullCorsSameOriginRequestSucceeds

  BEGIN_TEST_METHOD_ATTRIBUTE(FullCorsCrossOriginAllowOriginWildcardSucceeds)
  END_TEST_METHOD_ATTRIBUTE()
  TEST_METHOD(FullCorsCrossOriginAllowOriginWildcardSucceeds)
  {
    ServerParams serverArgs(s_port);
    serverArgs.Preflight.set(http::field::access_control_allow_headers,     "Content-Type");
    serverArgs.Preflight.set(http::field::access_control_allow_origin,      "*");
    serverArgs.Preflight.set(http::field::access_control_request_headers,   "Content-Type");
    serverArgs.Response.result(http::status::accepted);
    serverArgs.Response.set(http::field::access_control_allow_origin,       "*");
    serverArgs.Response.set(http::field::access_control_allow_credentials,  "true");

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
    ServerParams serverArgs(s_port);
    serverArgs.Preflight.set(http::field::access_control_allow_headers,     "Content-Type");
    serverArgs.Preflight.set(http::field::access_control_allow_origin,      s_crossOriginUrl);
    serverArgs.Preflight.set(http::field::access_control_request_headers,   "Content-Type");
    serverArgs.Response.result(http::status::accepted);
    serverArgs.Response.set(http::field::access_control_allow_origin,       s_crossOriginUrl);
    serverArgs.Response.set(http::field::access_control_allow_credentials,  "true");

    ClientParams clientArgs(http::verb::get, {{ "Content-Type", "text/plain" }});  // text/plain is a non-simple header

    SetRuntimeOptionString("Http.GlobalOrigin", s_crossOriginUrl);
    SetRuntimeOptionInt("Http.OriginPolicy", static_cast<int32_t>(OriginPolicy::CrossOriginResourceSharing));

    TestOriginPolicy(serverArgs, clientArgs, true /*shouldSucceed*/);
  }// FullCorsCrossOriginMatchingOriginSucceeds

  BEGIN_TEST_METHOD_ATTRIBUTE(FullCorsCrossOriginWithCredentialsFails)
  END_TEST_METHOD_ATTRIBUTE()
  TEST_METHOD(FullCorsCrossOriginWithCredentialsFails)
  {
    ServerParams serverArgs(s_port);
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

    TestOriginPolicy(serverArgs, clientArgs, s_shouldFail);
  }// FullCorsCrossOriginWithCredentialsFails


  BEGIN_TEST_METHOD_ATTRIBUTE(FullCorsCrossOriginMissingCorsHeadersFails)
  END_TEST_METHOD_ATTRIBUTE()
  TEST_METHOD(FullCorsCrossOriginMissingCorsHeadersFails)
  {
    ServerParams serverArgs(s_port);
    serverArgs.Preflight.erase(http::field::access_control_allow_methods);
    serverArgs.Preflight.result(http::status::not_implemented);

    ClientParams clientArgs(http::verb::get, {{ "Content-Type", "application/text" }}); // application/text is a non-simple header

    SetRuntimeOptionString("Http.GlobalOrigin", s_crossOriginUrl);
    SetRuntimeOptionInt("Http.OriginPolicy", static_cast<int32_t>(OriginPolicy::CrossOriginResourceSharing));

    TestOriginPolicy(serverArgs, clientArgs, s_shouldFail);
  }// FullCorsCrossOriginMissingCorsHeadersFails

  BEGIN_TEST_METHOD_ATTRIBUTE(FullCorsCrossOriginMismatchedCorsHeaderFails)
  END_TEST_METHOD_ATTRIBUTE()
  TEST_METHOD(FullCorsCrossOriginMismatchedCorsHeaderFails)
  {
    ServerParams serverArgs(s_port);
    serverArgs.Preflight.set(http::field::access_control_request_headers,   "Content-Type");
    serverArgs.Preflight.set(http::field::access_control_allow_headers,     "Content-Type");
    serverArgs.Preflight.set(http::field::access_control_allow_origin,      s_crossOriginUrl);
    serverArgs.Response.result(http::status::accepted);
    serverArgs.Response.set(http::field::access_control_allow_origin,       "http://other.example.rnw");

    ClientParams clientArgs(http::verb::get, {{ "Content-Type", "application/text" }}); // application/text is a non-simple header

    SetRuntimeOptionString("Http.GlobalOrigin", s_crossOriginUrl);
    SetRuntimeOptionInt("Http.OriginPolicy", static_cast<int32_t>(OriginPolicy::CrossOriginResourceSharing));

    TestOriginPolicy(serverArgs, clientArgs, s_shouldFail);
  }// FullCorsCrossOriginMismatchedCorsHeaderFails

  // https://developer.mozilla.org/en-US/docs/Web/HTTP/CORS/Errors/CORSExternalRedirectNotAllowed
  BEGIN_TEST_METHOD_ATTRIBUTE(FullCorsCrossOriginCheckFailsOnPreflightRedirectFails)
  END_TEST_METHOD_ATTRIBUTE()
  TEST_METHOD(FullCorsCrossOriginCheckFailsOnPreflightRedirectFails)
  {
    ServerParams serverArgs(s_port);
    serverArgs.Preflight.set(http::field::access_control_allow_origin,      s_crossOriginUrl);
    serverArgs.Preflight.set(http::field::location,                         "http://any-host.extension");
    serverArgs.Preflight.result(http::status::moved_permanently);

    ClientParams clientArgs(http::verb::get, {{ "Content-Type", "application/text" }});

    SetRuntimeOptionString("Http.GlobalOrigin", s_crossOriginUrl);
    SetRuntimeOptionInt("Http.OriginPolicy", static_cast<int32_t>(OriginPolicy::CrossOriginResourceSharing));

    TestOriginPolicy(serverArgs, clientArgs, s_shouldFail);
  }// FullCorsCrossOriginCheckFailsOnPreflightRedirectFails

  BEGIN_TEST_METHOD_ATTRIBUTE(FullCorsCorsCheckFailsOnResponseRedirectFails)
  END_TEST_METHOD_ATTRIBUTE()
  TEST_METHOD(FullCorsCorsCheckFailsOnResponseRedirectFails)
  {
    ServerParams serverArgs(s_port);

    // server1 allowed origin header includes http://example.com
    serverArgs.Preflight.set(http::field::access_control_request_headers,   "Content-Type");
    serverArgs.Preflight.set(http::field::access_control_allow_headers,     "Content-Type");
    serverArgs.Preflight.set(http::field::access_control_allow_origin,      s_crossOriginUrl);

    // This is a CORS request to server1, but server1 redirects the request to server2
    serverArgs.Response.result(http::status::moved_permanently);
    serverArgs.Response.set(http::field::access_control_allow_origin,       s_crossOriginUrl);
    // Redir server's URL
    serverArgs.Response.set(http::field::location,                          "http://localhost:6666");
    serverArgs.Response.set(http::field::server,                            "BaseServer");

    // Server2 does not set Access-Control-Allow-Origin for GET requests
    ServerParams redirServerArgs(++s_port);
    redirServerArgs.Response.result(http::status::accepted);
    redirServerArgs.Response.set(http::field::server,                    "RedirectServer");

    ClientParams clientArgs(http::verb::get, {{ "Content-Type", "application/text" }});

    SetRuntimeOptionString("Http.GlobalOrigin", s_crossOriginUrl);
    SetRuntimeOptionInt("Http.OriginPolicy", static_cast<int32_t>(OriginPolicy::CrossOriginResourceSharing));

    TestOriginPolicyWithRedirect(serverArgs, redirServerArgs, clientArgs, s_shouldFail);
  }// FullCorsCorsCheckFailsOnResponseRedirectFails


  BEGIN_TEST_METHOD_ATTRIBUTE(FullCorsSameOriginToSameOriginRedirectSucceeds)
  END_TEST_METHOD_ATTRIBUTE()
  TEST_METHOD(FullCorsSameOriginToSameOriginRedirectSucceeds)
  {
    ServerParams serverArgs(s_port);
    serverArgs.Preflight.set(http::field::location, serverArgs.Url);
    serverArgs.Response.result(http::status::accepted);

    ClientParams clientArgs(http::verb::get, {{ "Content-Type", "application/text" }});

    SetRuntimeOptionString("Http.GlobalOrigin", serverArgs.Url.c_str());
    SetRuntimeOptionInt("Http.OriginPolicy", static_cast<int32_t>(OriginPolicy::CrossOriginResourceSharing));

    TestOriginPolicy(serverArgs, clientArgs, s_shouldSucceed);
  } // FullCorsSameOriginToSameOriginRedirectSucceeds

  BEGIN_TEST_METHOD_ATTRIBUTE(FullCorsSameOriginToCrossOriginRedirectSucceeds)
  END_TEST_METHOD_ATTRIBUTE()
  TEST_METHOD(FullCorsSameOriginToCrossOriginRedirectSucceeds)
  {
    ServerParams serverArgs(s_port);
    ServerParams redirServerArgs(++s_port);

    serverArgs.Preflight.set(http::field::access_control_allow_origin,      serverArgs.Url);
    serverArgs.Response.result(http::status::moved_permanently);
    serverArgs.Response.set(http::field::location,                          redirServerArgs.Url);

    redirServerArgs.Response.result(http::status::accepted);
    redirServerArgs.Response.set(http::field::access_control_allow_origin,  serverArgs.Url);

    ClientParams clientArgs(http::verb::get, {{ "Content-Type", "application/text" }});

    SetRuntimeOptionString("Http.GlobalOrigin", serverArgs.Url.c_str());
    SetRuntimeOptionInt("Http.OriginPolicy", static_cast<int32_t>(OriginPolicy::CrossOriginResourceSharing));

    TestOriginPolicyWithRedirect(serverArgs, redirServerArgs, clientArgs, s_shouldSucceed);
  } // FullCorsSameOriginToCrossOriginRedirectSucceeds

  //TODO: Seems to redirect to exact same resource. Implement second resource in same server.
  // Redirects a cross origin request to cross origin request on the same server
  BEGIN_TEST_METHOD_ATTRIBUTE(FullCorsCrossOriginToCrossOriginRedirectSucceeds)
    TEST_IGNORE()
  END_TEST_METHOD_ATTRIBUTE()
  TEST_METHOD(FullCorsCrossOriginToCrossOriginRedirectSucceeds)
  {
    ServerParams serverArgs(s_port);
    //ServerParams redirServerArgs(++s_port);

    serverArgs.Preflight.set(http::field::access_control_allow_origin,      serverArgs.Url);
    serverArgs.Preflight.set(http::field::access_control_request_headers,   "Content-Type");
    serverArgs.Preflight.set(http::field::access_control_allow_headers,     "Content-Type");
    serverArgs.Preflight.set(http::field::access_control_allow_origin,      s_crossOriginUrl);
    serverArgs.Response.result(http::status::moved_permanently);
    serverArgs.Response.set(http::field::location,                          serverArgs.Url);
    serverArgs.Response.set(http::field::access_control_allow_origin,       s_crossOriginUrl);

    //redirServerArgs.Response.result(http::status::accepted);
    //redirServerArgs.Response.set(http::field::access_control_allow_origin,  serverArgs.Url);

    ClientParams clientArgs(http::verb::get, {{ "Content-Type", "application/text" }});

    SetRuntimeOptionString("Http.GlobalOrigin", s_crossOriginUrl);
    SetRuntimeOptionInt("Http.OriginPolicy",    static_cast<int32_t>(OriginPolicy::CrossOriginResourceSharing));

    TestOriginPolicy(serverArgs, /*redirServerArgs, */clientArgs, s_shouldSucceed);
  } // FullCorsCrossOriginToCrossOriginRedirectSucceeds

  // The initial request gets redirected back to the original origin,
  // but it will lack the Access-Control-Allow-Origin header.
  BEGIN_TEST_METHOD_ATTRIBUTE(FullCorsCrossOriginToOriginalOriginRedirectFails)
  END_TEST_METHOD_ATTRIBUTE()
  TEST_METHOD(FullCorsCrossOriginToOriginalOriginRedirectFails)
  {
    ServerParams serverArgs(s_port);
    ServerParams redirServerArgs(++s_port);

    serverArgs.Response.result(http::status::moved_permanently);
    serverArgs.Response.set(http::field::location,                          redirServerArgs.Url);
    serverArgs.Response.set(http::field::access_control_allow_origin,       redirServerArgs.Url);

    ClientParams clientArgs(http::verb::get, {{ "Content-Type", "text/plain" }});

    SetRuntimeOptionString("Http.GlobalOrigin", redirServerArgs.Url.c_str());
    SetRuntimeOptionInt("Http.OriginPolicy",    static_cast<int32_t>(OriginPolicy::CrossOriginResourceSharing));

    TestOriginPolicyWithRedirect(serverArgs, redirServerArgs, clientArgs, s_shouldFail);
  } // FullCorsCrossOriginToOriginalOriginRedirectFails

  // Redirects cross origin request to server1 to cross origin request to server2
  BEGIN_TEST_METHOD_ATTRIBUTE(FullCorsCrossOriginToAnotherCrossOriginRedirectSucceeds)
  END_TEST_METHOD_ATTRIBUTE()
  TEST_METHOD(FullCorsCrossOriginToAnotherCrossOriginRedirectSucceeds)
  {
    ServerParams serverArgs(s_port);
    ServerParams redirServerArgs(++s_port);

    serverArgs.Response.result(http::status::moved_permanently);
    serverArgs.Response.set(http::field::location,                          redirServerArgs.Url);
    serverArgs.Response.set(http::field::access_control_allow_origin,       s_crossOriginUrl);

    redirServerArgs.Response.result(http::status::accepted);
    redirServerArgs.Response.set(http::field::access_control_allow_origin,  "*");

    ClientParams clientArgs(http::verb::get, {{ "Content-Type", "text/plain" }});

    SetRuntimeOptionString("Http.GlobalOrigin", s_crossOriginUrl);
    SetRuntimeOptionInt("Http.OriginPolicy",    static_cast<int32_t>(OriginPolicy::CrossOriginResourceSharing));

    TestOriginPolicyWithRedirect(serverArgs, redirServerArgs, clientArgs, s_shouldSucceed);
  } // FullCorsCrossOriginToAnotherCrossOriginRedirectSucceeds

  BEGIN_TEST_METHOD_ATTRIBUTE(FullCorsCrossOriginToAnotherCrossOriginRedirectWithPreflightSucceeds)
  END_TEST_METHOD_ATTRIBUTE()
  TEST_METHOD(FullCorsCrossOriginToAnotherCrossOriginRedirectWithPreflightSucceeds)
  {
    ServerParams serverArgs(s_port);
    ServerParams redirServerArgs(++s_port);

    serverArgs.Preflight.set(http::field::access_control_request_headers,   "Content-Type");
    serverArgs.Preflight.set(http::field::access_control_allow_headers,     "Content-Type");
    serverArgs.Preflight.set(http::field::access_control_allow_origin,      s_crossOriginUrl);
    // server1 redirects the GET request to server2
    serverArgs.Response.result(http::status::moved_permanently);
    serverArgs.Response.set(http::field::location,                          redirServerArgs.Url);
    serverArgs.Response.set(http::field::access_control_allow_origin,       s_crossOriginUrl);

    // Since redirect tainted the origin, the server has to allow all origins for CORS to succeed
    redirServerArgs.Response.result(http::status::accepted);
    redirServerArgs.Response.set(http::field::access_control_allow_origin,  "*");

    // PATCH is not a simple method, so preflight is required for server1
    ClientParams clientArgs(http::verb::patch, {{ "Content-Type", "text/plain" }});

    SetRuntimeOptionString("Http.GlobalOrigin", s_crossOriginUrl);
    SetRuntimeOptionInt("Http.OriginPolicy",    static_cast<int32_t>(OriginPolicy::CrossOriginResourceSharing));

    TestOriginPolicyWithRedirect(serverArgs, redirServerArgs, clientArgs, s_shouldSucceed);
  } // FullCorsCrossOriginToAnotherCrossOriginRedirectWithPreflightSucceeds

  BEGIN_TEST_METHOD_ATTRIBUTE(FullCorsCrossOriginToAnotherCrossOriginRedirectWithPreflightFails)
  END_TEST_METHOD_ATTRIBUTE()
  TEST_METHOD(FullCorsCrossOriginToAnotherCrossOriginRedirectWithPreflightFails)
  {
    ServerParams serverArgs(s_port);
    ServerParams redirServerArgs(++s_port);

    serverArgs.Preflight.set(http::field::access_control_request_headers,   "Content-Type");
    serverArgs.Preflight.set(http::field::access_control_allow_headers,     "Content-Type");
    serverArgs.Preflight.set(http::field::access_control_allow_origin,      s_crossOriginUrl);
    // server1 redirects the GET request to server2
    serverArgs.Response.result(http::status::moved_permanently);
    serverArgs.Response.set(http::field::location,                          redirServerArgs.Url);
    serverArgs.Response.set(http::field::access_control_allow_origin,       s_crossOriginUrl);

    // Since redirect tainted the origin, the server does not know what origin to allow through a single value.
    // Even if server successfully guessed the single value, it will still fail on the client side.
    redirServerArgs.Response.result(http::status::accepted);
    redirServerArgs.Response.set(http::field::access_control_allow_origin,  s_crossOriginUrl);

    // PATCH is not a simple method, so preflight is required for server1
    ClientParams clientArgs(http::verb::patch, {{ "Content-Type", "text/plain" }});

    SetRuntimeOptionString("Http.GlobalOrigin", s_crossOriginUrl);
    SetRuntimeOptionInt("Http.OriginPolicy",    static_cast<int32_t>(OriginPolicy::CrossOriginResourceSharing));

    TestOriginPolicyWithRedirect(serverArgs, redirServerArgs, clientArgs, s_shouldFail);
  } // FullCorsCrossOriginToAnotherCrossOriginRedirectWithPreflightFails

  BEGIN_TEST_METHOD_ATTRIBUTE(FullCors304ForSimpleGetFails)
  END_TEST_METHOD_ATTRIBUTE()
  TEST_METHOD(FullCors304ForSimpleGetFails)
  {
    ServerParams serverArgs(s_port);
    serverArgs.Response.result(http::status::not_modified);

    // PATCH is not a simple method, so preflight is required for server1
    ClientParams clientArgs(http::verb::get, {{ "Content-Type", "text/plain" }});

    SetRuntimeOptionString("Http.GlobalOrigin", s_crossOriginUrl);
    SetRuntimeOptionInt("Http.OriginPolicy",    static_cast<int32_t>(OriginPolicy::CrossOriginResourceSharing));

    TestOriginPolicy(serverArgs, clientArgs, s_shouldFail);
  } // FullCors304ForSimpleGetFails

  TEST_METHOD(OfficeDev_OfficeJS_4144)
  {
    SetRuntimeOptionString("Http.GlobalOrigin", "http://orig.in");
    SetRuntimeOptionInt("Http.OriginPolicy", static_cast<int32_t>(OriginPolicy::CrossOriginResourceSharing));

    ClientParams clientArgs(http::verb::get, {} /*headers*/);
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
    resource->SetOnError([&clientArgs](int64_t, string&& message, bool)
    {
      clientArgs.ErrorMessage = std::move(message);
      clientArgs.ContentPromise.set_value();
    });

    resource->SendRequest(
      string { http::to_string(clientArgs.Method).data() },
      string { "http://localhost:5555/officedev/office-js/issues/4144"},
      0,                          /*requestId*/
      std::move(clientArgs.RequestHeaders),
      { { "string", "" } },       /*data*/
      "text",
      false,                      /*useIncrementalUpdates*/
      0,                          /*timeout*/
      clientArgs.WithCredentials, /*withCredentials*/
      [](int64_t) {}              /*reactCallback*/
    );

    clientArgs.ContentPromise.get_future().wait();

    Assert::AreEqual({}, clientArgs.ErrorMessage);
  }

  TEST_METHOD(FullCorsPreflightSucceeds)
  {
    ServerParams serverArgs(s_port);
    serverArgs.Preflight.set(http::field::access_control_request_headers, "ArbitraryHeader");
    serverArgs.Preflight.set(http::field::access_control_allow_headers,   "ArbitraryHeader");
    serverArgs.Preflight.set(http::field::access_control_allow_origin,    s_crossOriginUrl);
    serverArgs.Response.result(http::status::ok);
    serverArgs.Response.set(http::field::access_control_allow_origin,     s_crossOriginUrl);

    ClientParams clientArgs(http::verb::get, { {"Content-Type", "text/plain"}, {"ArbitraryHeader", "AnyValue"} });

    SetRuntimeOptionString("Http.GlobalOrigin", s_crossOriginUrl);
    SetRuntimeOptionInt("Http.OriginPolicy", static_cast<int32_t>(OriginPolicy::CrossOriginResourceSharing));
    TestOriginPolicy(serverArgs, clientArgs, s_shouldSucceed);
  }// FullCorsPreflightSucceeds

  // The current implementation omits withCredentials flag from request and always sets it to false
  // Configure the responses for CORS request
  BEGIN_TEST_METHOD_ATTRIBUTE(FullCorsCrossOriginWithCredentialsSucceeds)
  END_TEST_METHOD_ATTRIBUTE()
  TEST_METHOD(FullCorsCrossOriginWithCredentialsSucceeds)
  {
    ServerParams serverArgs(s_port);
    serverArgs.Preflight.set(http::field::access_control_request_headers,   "Content-Type");
    serverArgs.Preflight.set(http::field::access_control_allow_headers,     "Content-Type");
    serverArgs.Preflight.set(http::field::access_control_allow_origin,      s_crossOriginUrl);
    serverArgs.Preflight.set(http::field::access_control_allow_credentials, "true");
    serverArgs.Response.result(http::status::accepted);
    serverArgs.Response.set(http::field::access_control_allow_origin,       s_crossOriginUrl);

    ClientParams clientArgs(http::verb::get, {{ "Content-Type", "application/text" }}); // application/text is a non-simple header
    clientArgs.WithCredentials = true;

    SetRuntimeOptionString("Http.GlobalOrigin", s_crossOriginUrl);
    SetRuntimeOptionInt("Http.OriginPolicy", static_cast<int32_t>(OriginPolicy::CrossOriginResourceSharing));

    TestOriginPolicy(serverArgs, clientArgs, s_shouldSucceed);
  }// FullCorsCrossOriginWithCredentialsSucceeds

  // "Host" is one of the forbidden headers for fetch
  BEGIN_TEST_METHOD_ATTRIBUTE(FullCorsRequestWithHostHeaderFails)
    // "Host" is not an accepted request header in WinRT.
    TEST_IGNORE()
  END_TEST_METHOD_ATTRIBUTE()
  TEST_METHOD(FullCorsRequestWithHostHeaderFails)
  {
    ServerParams serverArgs(s_port);
    serverArgs.Preflight.set(http::field::access_control_request_headers,   "Content-Type");
    serverArgs.Preflight.set(http::field::access_control_allow_headers,     "Content-Type");
    serverArgs.Response.result(http::status::accepted);

    ClientParams clientArgs(http::verb::get, {{ "Content-Type", "application/text" }, { "Host", "http://sub.example.rnw" }});

    SetRuntimeOptionString("Http.GlobalOrigin", serverArgs.Url.c_str());
    SetRuntimeOptionInt("Http.OriginPolicy", static_cast<int32_t>(OriginPolicy::CrossOriginResourceSharing));

    TestOriginPolicy(serverArgs, clientArgs, s_shouldFail);

    Assert::Fail(L"FIX!!! Passes for the wrong reason. Error: 0x80070057 : 'Invalid HTTP headers.'");
  }// FullCorsRequestWithHostHeaderFails

  BEGIN_TEST_METHOD_ATTRIBUTE(RequestWithProxyAuthorizationHeaderFails)
  END_TEST_METHOD_ATTRIBUTE()
  TEST_METHOD(RequestWithProxyAuthorizationHeaderFails)
  {
    ServerParams serverArgs(s_port);
    serverArgs.Preflight.set(http::field::access_control_request_headers,   "Content-Type");
    serverArgs.Preflight.set(http::field::access_control_allow_headers,     "Content-Type");
    serverArgs.Response.result(http::status::accepted);

    ClientParams clientArgs(http::verb::get, {{ "Content-Type", "application/text" }, { "Proxy-Authorization", "Basic Zm9vOmJhcg==" }});

    SetRuntimeOptionString("Http.GlobalOrigin", serverArgs.Url.c_str());
    SetRuntimeOptionInt("Http.OriginPolicy", static_cast<int32_t>(OriginPolicy::CrossOriginResourceSharing));

    TestOriginPolicy(serverArgs, clientArgs, s_shouldFail);
  }// RequestWithProxyAuthorizationHeaderFails
};

uint16_t HttpOriginPolicyIntegrationTest::s_port = 7777;

}//namespace Microsoft::React::Test
// clang-format on
