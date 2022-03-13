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

// Return a reasonable mime type based on the extension of a file.
beast::string_view mime_type(beast::string_view path) {
  using beast::iequals;
  auto const ext = [&path] {
    auto const pos = path.rfind(".");
    if (pos == beast::string_view::npos)
      return beast::string_view{};
    return path.substr(pos);
  }();
  if (iequals(ext, ".htm"))
    return "text/html";
  if (iequals(ext, ".html"))
    return "text/html";
  if (iequals(ext, ".php"))
    return "text/html";
  if (iequals(ext, ".css"))
    return "text/css";
  if (iequals(ext, ".txt"))
    return "text/plain";
  if (iequals(ext, ".js"))
    return "application/javascript";
  if (iequals(ext, ".json"))
    return "application/json";
  if (iequals(ext, ".xml"))
    return "application/xml";
  if (iequals(ext, ".swf"))
    return "application/x-shockwave-flash";
  if (iequals(ext, ".flv"))
    return "video/x-flv";
  if (iequals(ext, ".png"))
    return "image/png";
  if (iequals(ext, ".jpe"))
    return "image/jpeg";
  if (iequals(ext, ".jpeg"))
    return "image/jpeg";
  if (iequals(ext, ".jpg"))
    return "image/jpeg";
  if (iequals(ext, ".gif"))
    return "image/gif";
  if (iequals(ext, ".bmp"))
    return "image/bmp";
  if (iequals(ext, ".ico"))
    return "image/vnd.microsoft.icon";
  if (iequals(ext, ".tiff"))
    return "image/tiff";
  if (iequals(ext, ".tif"))
    return "image/tiff";
  if (iequals(ext, ".svg"))
    return "image/svg+xml";
  if (iequals(ext, ".svgz"))
    return "image/svg+xml";
  return "application/text";
}

// Append an HTTP rel-path to a local filesystem path.
// The returned path is normalized for the platform.
std::string path_cat(beast::string_view base, beast::string_view path) {
  if (base.empty())
    return std::string(path);
  std::string result(base);
#ifdef BOOST_MSVC
  char constexpr path_separator = '\\';
  if (result.back() == path_separator)
    result.resize(result.size() - 1);
  result.append(path.data(), path.size());
  for (auto &c : result)
    if (c == '/')
      c = path_separator;
#else
  char constexpr path_separator = '/';
  if (result.back() == path_separator)
    result.resize(result.size() - 1);
  result.append(path.data(), path.size());
#endif
  return result;
}

// This function produces an HTTP response for the given
// request. The type of the response object depends on the
// contents of the request, so the interface requires the
// caller to pass a generic lambda for receiving the response.
template <class Body, class Allocator, class Send>
void handle_request(
    beast::string_view doc_root,
    http::request<Body, http::basic_fields<Allocator>> &&req,
    Send &&send) {
  // Returns a bad request response
  //auto const bad_request = [&req](beast::string_view why) {
  //  http::response<http::string_body> res{http::status::bad_request, req.version()};
  //  res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
  //  res.set(http::field::content_type, "text/html");
  //  res.keep_alive(req.keep_alive());
  //  res.body() = std::string(why);
  //  res.prepare_payload();
  //  return res;
  //};

  //// Returns a not found response
  //auto const not_found = [&req](beast::string_view target) {
  //  http::response<http::string_body> res{http::status::not_found, req.version()};
  //  res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
  //  res.set(http::field::content_type, "text/html");
  //  res.keep_alive(req.keep_alive());
  //  res.body() = "The resource '" + std::string(target) + "' was not found.";
  //  res.prepare_payload();
  //  return res;
  //};

  //// Returns a server error response
  //auto const server_error = [&req](beast::string_view what) {
  //  http::response<http::string_body> res{http::status::internal_server_error, req.version()};
  //  res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
  //  res.set(http::field::content_type, "text/html");
  //  res.keep_alive(req.keep_alive());
  //  res.body() = "An error occurred: '" + std::string(what) + "'";
  //  res.prepare_payload();
  //  return res;
  //};

  //// Make sure we can handle the method
  //if (req.method() != http::verb::get && req.method() != http::verb::head)
  //  return send(bad_request("Unknown HTTP-method"));

  //// Request path must be absolute and not contain "..".
  //if (req.target().empty() || req.target()[0] != '/' || req.target().find("..") != beast::string_view::npos)
  //  return send(bad_request("Illegal request-target"));

  //// Build the path to the requested file
  //std::string path = path_cat(doc_root, req.target());
  //if (req.target().back() == '/')
  //  path.append("index.html");

  //// Attempt to open the file
  //beast::error_code ec;
  //http::file_body::value_type body;
  //body.open(path.c_str(), beast::file_mode::scan, ec);

  //// Handle the case where the file doesn't exist
  //if (ec == beast::errc::no_such_file_or_directory)
  //  return send(not_found(req.target()));

  //// Handle an unknown error
  //if (ec)
  //  return send(server_error(ec.message()));

  //// Cache the size since we need it after the move
  //auto const size = body.size();

  //// Respond to HEAD request
  //if (req.method() == http::verb::head) {
  //  http::response<http::empty_body> res{http::status::ok, req.version()};
  //  res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
  //  res.set(http::field::content_type, mime_type(path));
  //  res.content_length(size);
  //  res.keep_alive(req.keep_alive());
  //  return send(std::move(res));
  //}

  //// Respond to GET request
  //http::response<http::file_body> res{
  //    std::piecewise_construct, std::make_tuple(std::move(body)), std::make_tuple(http::status::ok, req.version())};
  //res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
  //res.set(http::field::content_type, mime_type(path));
  //res.content_length(size);
  //res.keep_alive(req.keep_alive());
  //return send(std::move(res));

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

    template <bool isRequest, class Body, class Fields>
    void operator()(http::message<isRequest, Body, Fields> &&msg) const {
      // The lifetime of the message has to extend
      // for the duration of the async operation so
      // we use a shared_ptr to manage it.
      auto sp = std::make_shared<http::message<isRequest, Body, Fields>>(std::move(msg));

      // Store a type-erased version of the shared
      // pointer in the class to keep it alive.
      self_.res_ = sp;

      // Write the response
      http::async_write(
          self_.stream_, *sp, beast::bind_front_handler(&session::on_write, self_.shared_from_this(), sp->need_eof()));
    }
  };

  beast::tcp_stream stream_;
  beast::flat_buffer buffer_;
  std::shared_ptr<std::string const> doc_root_;
  http::request<http::string_body> req_;
  std::shared_ptr<void> res_;
  send_lambda lambda_;

 public:
  // Take ownership of the stream
  session(tcp::socket &&socket, std::shared_ptr<std::string const> const &doc_root)
      : stream_(std::move(socket)), doc_root_(doc_root), lambda_(*this) {}

  // Start the asynchronous operation
  void run() {
    // We need to be executing within a strand to perform async operations
    // on the I/O objects in this session. Although not strictly necessary
    // for single-threaded contexts, this example code is written to be
    // thread-safe by default.
    net::dispatch(stream_.get_executor(), beast::bind_front_handler(&session::do_read, shared_from_this()));
  }

  void do_read() {
    // Make the request empty before reading,
    // otherwise the operation behavior is undefined.
    req_ = {};

    // Set the timeout.
    stream_.expires_after(std::chrono::seconds(30));

    // Read a request
    http::async_read(stream_, buffer_, req_, beast::bind_front_handler(&session::on_read, shared_from_this()));
  }

  void on_read(beast::error_code ec, std::size_t bytes_transferred) {
    boost::ignore_unused(bytes_transferred);

    // This means they closed the connection
    if (ec == http::error::end_of_stream)
      return do_close();

    if (ec)
      return fail(ec, "read");

    // Send the response
    handle_request(*doc_root_, std::move(req_), lambda_);
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
    res_ = nullptr;

    // Read another request
    do_read();
  }

  void do_close() {
    // Send a TCP shutdown
    beast::error_code ec;
    stream_.socket().shutdown(tcp::socket::shutdown_send, ec);

    // At this point the connection is closed gracefully
  }
};

//------------------------------------------------------------------------------

// Accepts incoming connections and launches the sessions
class listener : public std::enable_shared_from_this<listener> {
  net::io_context &ioc_;
  tcp::acceptor acceptor_;
  std::shared_ptr<std::string const> doc_root_;

 public:
  listener(net::io_context &ioc, tcp::endpoint endpoint, std::shared_ptr<std::string const> const &doc_root)
      : ioc_(ioc), acceptor_(net::make_strand(ioc)), doc_root_(doc_root) {
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
      std::make_shared<session>(std::move(socket), doc_root_)->run();
    }

    // Accept another connection
    do_accept();
  }
};

//------------------------------------------------------------------------------

int mains(int argc, char *argv[]) {
  // Check command line arguments.
  if (argc != 5) {
    std::cerr << "Usage: http-server-async <address> <port> <doc_root> <threads>\n"
              << "Example:\n"
              << "    http-server-async 0.0.0.0 8080 . 1\n";
    return EXIT_FAILURE;
  }
  auto const address = net::ip::make_address(argv[1]);
  auto const port = static_cast<unsigned short>(std::atoi(argv[2]));
  auto const doc_root = std::make_shared<std::string>(argv[3]);
  auto const threads = std::max<int>(1, std::atoi(argv[4]));

  // The io_context is required for all I/O
  net::io_context ioc{threads};

  // Create and launch a listening port
  std::make_shared<listener>(ioc, tcp::endpoint{address, port}, doc_root)->run();

  // Run the I/O service on the requested number of threads
  std::vector<std::thread> v;
  v.reserve(threads - 1);
  for (auto i = threads - 1; i > 0; --i)
    v.emplace_back([&ioc] { ioc.run(); });
  ioc.run();

  return EXIT_SUCCESS;
}

}

using namespace Microsoft::React;
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace http = boost::beast::http;

using std::promise;
using std::string;
using std::vector;
using Test::DynamicRequest;
using Test::DynamicResponse;

TEST_CLASS (HttpResourceIntegrationTest) {

  TEST_METHOD(Vinime) {
    auto const address = boost::asio::ip::make_address("0.0.0.0");
    unsigned short const port = 5556;
    auto const docRoot = std::make_shared<string>("nel");
    int const threadCount = 1;

    boost::asio::io_context ioc{threadCount};

    std::make_shared<falco::listener>(ioc, boost::asio::ip::tcp::endpoint{address, port}, docRoot)->run();

    std::vector<std::thread> threads;
    threads.reserve(threadCount - 1);
    for (auto i = threadCount - 1; i > 0; --i) {
      threads.emplace_back([&ioc]() {
        ioc.run();
      });
    }
    ioc.run();
  }

  TEST_METHOD(RequestGetSucceeds) {
    promise<void> promise;
    string error;
    int statusCode = 0;

    auto server = std::make_shared<Test::HttpServer>("127.0.0.1", static_cast<uint16_t>(5556));
    server->SetOnGet([&promise](const DynamicRequest &request) -> DynamicResponse {
      DynamicResponse response;
      response.result(http::status::ok);
      response.body() = Test::CreateStringResponseBody("some resposne content");
      promise.set_value();

      return response;
    });
    server->Start();

    //auto resource = IHttpResource::Make();
    //resource->SetOnResponse([&promise, &statusCode](int64_t, IHttpResource::Response response) {
    //  statusCode = static_cast<int>(response.StatusCode);
    //  promise.set_value();
    //});
    //resource->SetOnError([&promise, &error, &server](int64_t, string &&message) {
    //  error = std::move(message);
    //  promise.set_value();

    //  server->Abort();
    //});
    //resource->SendRequest(
    //    "GET",
    //    "http://localhost:5556",
    //    {} /*header*/,
    //    {} /*bodyData*/,
    //    "text",
    //    false,
    //    1000 /*timeout*/,
    //    false /*withCredentials*/,
    //    [](int64_t) {});

      // Synchronize response.
    promise.get_future().wait();
    server->Stop();

    Assert::AreEqual({}, error);
    Assert::AreEqual(200, statusCode);
  }

  TEST_METHOD(RequestGetHeadersSucceeds) {
    promise<void> promise;
    string error;
    IHttpResource::Response response;

    // HTTP call scope
    //{
      auto server = std::make_shared<Test::HttpServer>("127.0.0.1", static_cast<uint16_t>(5555));
      server->SetOnGet([](const DynamicRequest &request) -> DynamicResponse {
        DynamicResponse response;
        response.result(http::status::ok);

        // Response header
        response.set(http::field::server, "Microsoft::React::Test::HttpServer");
        // Response content header
        response.set(http::field::content_length, "0");
        // Response arbitrary header
        response.set("ResponseHeaderName1", "ResponseHeaderValue1");

        return response;
      });
      server->Start();

      auto resource = IHttpResource::Make();
      resource->SetOnResponse([&promise, &response](int64_t, IHttpResource::Response callbackResponse) {
        response = callbackResponse;
        promise.set_value();
      });
      resource->SetOnError([&promise, &error, &server](int64_t, string &&message) {
        error = std::move(message);
        promise.set_value();

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

      server->Stop();
    //}

    promise.get_future().wait();

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
    promise<void> promise;
    string error;
    IHttpResource::Response response;
    string content;

    // HTTP call scope
    //{
      auto server = std::make_shared<Test::HttpServer>("127.0.0.1", static_cast<uint16_t>(5555));
      server->SetOnGet([](const DynamicRequest& request) -> DynamicResponse {
        DynamicResponse response;
        response.result(http::status::ok);
        response.body() = Test::CreateStringResponseBody("Response Body");

        return response;
      });
      server->SetOnOptions([](const DynamicRequest &request) -> DynamicResponse {
        DynamicResponse response;
        response.result(http::status::ok);
        response.set("PreflightName", "PreflightValue");

        return response;
      });
      server->Start();

      auto resource = IHttpResource::Make();
      resource->SetOnResponse([/*&promise, */&response](int64_t, IHttpResource::Response callbackResponse) {
        response = callbackResponse;
      });
      resource->SetOnData([&promise, &content](int64_t, string &&responseData) {
        content = std::move(responseData);
        promise.set_value();
      });
      resource->SetOnError([&promise, &error, &server](int64_t, string &&message) {
        error = std::move(message);
        promise.set_value();

        server->Abort();
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

      server->Stop();
    //}

    promise.get_future().wait();

    Assert::AreEqual({}, error, L"Error encountered");
    for (auto header : response.Headers) {
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
