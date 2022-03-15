// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

// C4996: 'gethostbyaddr': Use getnameinfo() or GetNameInfoW() instead
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <CppUnitTest.h>
#include <IHttpResource.h>
#include <Test/HttpServer.h>
#include <boost/beast/http.hpp>
#include <unicode.h>

// Standard Library
#include <future>

#define FALCO 1

#include <boost/asio/dispatch.hpp>
#include <boost/asio/strand.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/config.hpp>
#include <algorithm>
#include <cstdlib>
#include <functional>
#include <iostream>
#include <memory>
#include <string>
#include <thread>
#include <vector>

namespace falco {

namespace beast = boost::beast; // from <boost/beast.hpp>
namespace http = beast::http; // from <boost/beast/http.hpp>
namespace net = boost::asio; // from <boost/asio.hpp>
using tcp = boost::asio::ip::tcp; // from <boost/asio/ip/tcp.hpp>

// This function produces an HTTP response for the given
// request. The type of the response object depends on the
// contents of the request, so the interface requires the
// caller to pass a generic lambda for receiving the response.
template </*class Body, class Allocator, */class Send>
void handle_request(
    Microsoft::React::Test::DynamicRequest &&req,
    Send &&send) {

  Microsoft::React::Test::DynamicResponse res;
  res.result(http::status::ok);
  res.body() = Microsoft::React::Test::CreateStringResponseBody("some resposne content");

  return send(std::move(res));
}

//------------------------------------------------------------------------------

// Report a failure
void fail(beast::error_code ec, char const *what) {
  std::cerr << what << ": " << ec.message() << "\n";
}

// Handles an HTTP server connection
class session : public std::enable_shared_from_this<session> {
  // This is the C++11 equivalent of a generic lambda.
  // The function object is used to send an HTTP message.
  struct send_lambda {
    session &self_;

    explicit send_lambda(session &self) : self_(self) {}

    //template <bool isRequest, class Body, class Fields>
    void operator()(Microsoft::React::Test::DynamicResponse &&msg) const {
      // The lifetime of the message has to extend
      // for the duration of the async operation so
      // we use a shared_ptr to manage it.
      auto sp = std::make_shared<Microsoft::React::Test::DynamicResponse>(std::move(msg));

      // Store a type-erased version of the shared
      // pointer in the class to keep it alive.
      self_.m_response = sp;

      // Write the response
      http::async_write(
          self_.m_stream, *sp, beast::bind_front_handler(&session::on_write, self_.shared_from_this(), sp->need_eof()));
    }
  };

  std::function<void(Microsoft::React::Test::DynamicResponse&&)> m_sendLambda;

  beast::tcp_stream m_stream;
  beast::flat_buffer m_buffer;
  Microsoft::React::Test::DynamicRequest m_request;
  std::shared_ptr<Microsoft::React::Test::DynamicResponse> m_response;
  send_lambda m_lambda;
  Microsoft::React::Test::HttpCallbacks &m_callbacks;

 public:
  // Take ownership of the stream
  session(tcp::socket &&socket, Microsoft::React::Test::HttpCallbacks &callbacks)
    : m_stream(std::move(socket))
    , m_lambda(*this)
    , m_callbacks{ callbacks }
  {
  }

  // Start the asynchronous operation
  void run() {
    m_sendLambda = [self = shared_from_this()](Microsoft::React::Test::DynamicResponse&& res) {
      auto sp = std::make_shared<Microsoft::React::Test::DynamicResponse>(std::move(res));
      self->m_response = sp;

      http::async_write(
        self->m_stream,
        *sp,
        beast::bind_front_handler(&session::on_write, self->shared_from_this(), sp->need_eof())
      );
    };

    // We need to be executing within a strand to perform async operations
    // on the I/O objects in this session. Although not strictly necessary
    // for single-threaded contexts, this example code is written to be
    // thread-safe by default.
    net::dispatch(m_stream.get_executor(), beast::bind_front_handler(&session::do_read, shared_from_this()));
  }

  void do_read() {
    // Make the request empty before reading,
    // otherwise the operation behavior is undefined.
    m_request = {};

    // Set the timeout.
    m_stream.expires_after(std::chrono::seconds(30));

    // Read a request
    http::async_read(m_stream, m_buffer, m_request, beast::bind_front_handler(&session::on_read, shared_from_this()));
  }

  void Respond() {
    switch (m_request.method()) {
      case http::verb::get:
        m_sendLambda(m_callbacks.OnGet(m_request));
        break;

      case http::verb::options:
        m_sendLambda(m_callbacks.OnOptions(m_request));
        break;

      default:
        throw;
    }
  }

  void on_read(beast::error_code ec, std::size_t bytes_transferred) {
    boost::ignore_unused(bytes_transferred);

    // This means they closed the connection
    if (ec == http::error::end_of_stream)
      return do_close();

    if (ec)
      return fail(ec, "read");

    // Send the response
    Respond();
    // handle_request(std::move(req_), lambda_);
  }

  void on_write(bool close, beast::error_code ec, std::size_t bytes_transferred) {
    boost::ignore_unused(bytes_transferred);

    if (ec)
      return fail(ec, "write");

    if (close) {
      // This means we should close the connection, usually because
      // the response indicated the "Connection: close" semantic.
      return do_close();
    }

    // We're done with the response so delete it
    m_response = nullptr;

    // Read another request
    do_read();
  }

  void do_close() {
    // Send a TCP shutdown
    beast::error_code ec;
    m_stream.socket().shutdown(tcp::socket::shutdown_send, ec);

    // At this point the connection is closed gracefully
  }
};

//------------------------------------------------------------------------------

// Accepts incoming connections and launches the sessions
class listener : public std::enable_shared_from_this<listener> {
  net::io_context &ioc_;
  tcp::acceptor acceptor_;
  Microsoft::React::Test::HttpCallbacks &m_callbacks;

 public:
  listener(net::io_context &ioc, tcp::endpoint endpoint, Microsoft::React::Test::HttpCallbacks& callbacks)
      : ioc_(ioc)
      , acceptor_(net::make_strand(ioc))
      , m_callbacks{callbacks}
  {
    beast::error_code ec;

    // Open the acceptor
    acceptor_.open(endpoint.protocol(), ec);
    if (ec) {
      fail(ec, "open");
      return;
    }

    // Allow address reuse
    acceptor_.set_option(net::socket_base::reuse_address(true), ec);
    if (ec) {
      fail(ec, "set_option");
      return;
    }

    // Bind to the server address
    acceptor_.bind(endpoint, ec);
    if (ec) {
      fail(ec, "bind");
      return;
    }

    // Start listening for connections
    acceptor_.listen(net::socket_base::max_listen_connections, ec);
    if (ec) {
      fail(ec, "listen");
      return;
    }
  }

  // Start accepting incoming connections
  void run() {
    do_accept();
  }

 private:
  void do_accept() {
    // The new connection gets its own strand
    acceptor_.async_accept(net::make_strand(ioc_), beast::bind_front_handler(&listener::on_accept, shared_from_this()));
  }

  void on_accept(beast::error_code ec, tcp::socket socket) {
    if (ec) {
      fail(ec, "accept");
    } else {
      // Create the session and run it
      std::make_shared<session>(std::move(socket), m_callbacks)->run();
    }

    // Accept another connection
    do_accept();
  }
};

namespace net = boost::asio;

class HttpServer : public std::enable_shared_from_this<HttpServer> {
  net::io_context m_context;
  std::vector<std::thread> m_threads;
  size_t m_numThreads;
  Microsoft::React::Test::HttpCallbacks m_callbacks;
  net::ip::tcp::endpoint m_endpoint;

 public:
  HttpServer(const char *url, size_t port, size_t numThreads = 1)
  : m_numThreads{numThreads},
    m_context{static_cast<int>(m_numThreads)}
  , m_endpoint{net::ip::make_address(url), static_cast<unsigned short>(port)}
  {}

  HttpServer(size_t port, size_t numThreads = 1) : HttpServer("0.0.0.0", port, numThreads) {}

  ~HttpServer() {}

  void Start() {
    std::make_shared<listener>(
      m_context,
      m_endpoint,
      m_callbacks
    )->run();

    m_threads.reserve(m_numThreads);
    for (auto i = m_numThreads; i > 0; --i) {
      m_threads.emplace_back([self = shared_from_this()]() {
        self->m_context.run();
      });
    }
  }

  void Stop() {
    m_context.stop();

    for (auto &t : m_threads) {
      if (t.joinable())
        t.join();
    }
  }

  Microsoft::React::Test::HttpCallbacks& Callbacks() {
    return m_callbacks;
  }
};

} // falco

using namespace Microsoft::React;
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace http = boost::beast::http;

using std::promise;
using std::string;
using std::vector;
using Test::DynamicRequest;
using Test::DynamicResponse;

TEST_CLASS (HttpResourceIntegrationTest) {

  TEST_METHOD(RequestGetSucceeds) {
    promise<void> resPromise;
    string error;
    int statusCode = 0;

    auto server = std::make_shared<Test::HttpServer>(5556);
    server->Callbacks().OnGet = [&resPromise](const DynamicRequest &request)->DynamicResponse {
      DynamicResponse response;
      response.result(http::status::ok);
      response.body() = Test::CreateStringResponseBody("some response content");

      return response;
    };
    server->Start();

    auto resource = IHttpResource::Make();
    resource->SetOnResponse([&resPromise, &statusCode](int64_t, IHttpResource::Response response) {
      statusCode = static_cast<int>(response.StatusCode);
    });
    resource->SetOnData([&resPromise](int64_t, string &&content) {
      resPromise.set_value();
    });
    resource->SetOnError([&resPromise, &error, &server](int64_t, string &&message) {
      error = std::move(message);
      resPromise.set_value();

      server->Stop();
    });
    resource->SendRequest(
        "GET",
        "http://localhost:5556",
        {} /*header*/,
        {} /*bodyData*/,
        "text",
        false,
        1000 /*timeout*/,
        false /*withCredentials*/,
        [](int64_t) {});

    // Synchronize response.
    resPromise.get_future().wait();
    server->Stop();

    Assert::AreEqual({}, error);
    Assert::AreEqual(200, statusCode);
  }

  TEST_METHOD(RequestGetHeadersSucceeds) {
    promise<void> rcPromise;
    string error;
    IHttpResource::Response response;

    auto server = std::make_shared<Test::HttpServer>(5555);
    server->Callbacks().OnGet = [](const DynamicRequest &request)->DynamicResponse {
      DynamicResponse response;
      response.result(http::status::ok);

      // Response header
      response.set(http::field::server, "Microsoft::React::Test::HttpServer");
      // Response content header
      response.set(http::field::content_length, "0");
      // Response arbitrary header
      response.set("ResponseHeaderName1", "ResponseHeaderValue1");

      return response;
    };
    server->Start();

    auto resource = IHttpResource::Make();
    resource->SetOnResponse([&rcPromise, &response](int64_t, IHttpResource::Response callbackResponse) {
      response = callbackResponse;
      rcPromise.set_value();
    });
    resource->SetOnError([&rcPromise, &error, &server](int64_t, string &&message) {
      error = std::move(message);
      rcPromise.set_value();

      server->Abort();
    });

    //clang-format off
    resource->SendRequest(
        "GET",
        "http://localhost:5555",
        {
            {"Content-Type", "application/json"},
            {"Content-Encoding", "ASCII"},
            {"name3", "value3"},
            {"name4", "value4"},
        },
        {} /*bodyData*/,
        "text",
        false,
        1000 /*timeout*/,
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

  TEST_METHOD(RequestOptionsSucceeds) {
    promise<void> getResponsePromise;
    promise<void> getDataPromise;
    promise<void> optionsPromise;
    string error;
    IHttpResource::Response getResponse;
    IHttpResource::Response optionsResponse;
    string content;

#if 1
      auto server = std::make_shared<falco::HttpServer>(5555);
#else
      auto server = std::make_shared<Test::HttpServer>(5555);
#endif
    server->Callbacks().OnGet = [](const DynamicRequest &request) -> DynamicResponse {
      DynamicResponse response;
      response.result(http::status::ok);
      response.body() = Test::CreateStringResponseBody("Response Body");

      return response;
    };

    server->Callbacks().OnOptions = [](const DynamicRequest &request)->DynamicResponse {
      DynamicResponse response;
      response.result(http::status::partial_content);
      response.set("PreflightName", "PreflightValue");

      return response;
    };
    server->Start();

    auto resource = IHttpResource::Make();
    resource->SetOnResponse([&getResponse, &getResponsePromise, &optionsResponse, &optionsPromise](int64_t, IHttpResource::Response callbackResponse) {
      if (callbackResponse.StatusCode == static_cast<int64_t>(http::status::ok)) {
        getResponse = callbackResponse;
        getResponsePromise.set_value();
      }
      else if (callbackResponse.StatusCode == static_cast<int64_t>(http::status::partial_content)) {
        optionsResponse = callbackResponse;
        optionsPromise.set_value();
      }
    });
    resource->SetOnData([&getDataPromise, &content](int64_t, string &&responseData) {
      content = std::move(responseData);

      if (! content.empty())
        getDataPromise.set_value();
    });
    resource->SetOnError(
        [&optionsPromise, &getResponsePromise, &getDataPromise, &error, &server](int64_t, string &&message) {
      error = std::move(message);

      optionsPromise.set_value();
      getResponsePromise.set_value();
      getDataPromise.set_value();

      server->Stop();
    });

    //clang-format off
    resource->SendRequest(
        "OPTIONS",
        "http://localhost:5555",
        {} /*headers*/,
        {} /*bodyData*/,
        "text",
        false,
        1000 /*timeout*/,
        false /*withCredentials*/,
        [](int64_t) {});
    resource->SendRequest(
        "GET",
        "http://localhost:5555",
        {} /*headers*/,
        {} /*bodyData*/,
        "text",
        false,
        1000 /*timeout*/,
        false /*withCredentials*/,
        [](int64_t) {});
    //clang-format on

    optionsPromise.get_future().wait();
    getResponsePromise.get_future().wait();
    getDataPromise.get_future().wait();
    server->Stop();

    Assert::AreEqual({}, error, L"Error encountered");
    for (auto header : getResponse.Headers) {
      if (header.first == "PreflightName") {
        Assert::AreEqual({"PreflightValue"}, header.second, L"Wrong header");
      } else {
        string message = "Unexpected header: [" + header.first + "]=[" + header.second + "]";
        Assert::Fail(Microsoft::Common::Unicode::Utf8ToUtf16(message).c_str());
      }
    }
    Assert::AreEqual({"Response Body"}, content);
  }
};
