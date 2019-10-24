#include "HttpServer.h"

#include <boost/asio/bind_executor.hpp>
#include <boost/beast/version.hpp>

// Include to prevent 'incomplete type' errors.
#include <boost/utility/in_place_factory.hpp>
#include <boost/utility/typed_in_place_factory.hpp>

using namespace boost::asio::ip;

namespace http = boost::beast::http;

using boost::asio::bind_executor;
using boost::asio::io_context;
using boost::system::error_code;
using std::function;
using std::make_shared;
using std::shared_ptr;
using std::string;
using std::placeholders::_1;
using std::placeholders::_2;

namespace Microsoft::React::Test {

#pragma region Utility functions

boost::beast::multi_buffer CreateStringResponseBody(string &&content) {
  auto result = boost::beast::multi_buffer();
  auto n = boost::asio::buffer_copy(result.prepare(content.size()), boost::asio::buffer(content));
  result.commit(n);

  return result;
}

#pragma endregion // Utility functions

#pragma region HttpSession

HttpSession::HttpSession(tcp::socket &socket, HttpCallbacks &callbacks)
    : m_socket{socket}, m_strand{m_socket.get_executor()}, m_callbacks{callbacks} {}

HttpSession::~HttpSession() {}

void HttpSession::Read() {
  // Clear request.
  m_request = {};

  http::async_read(
      m_socket,
      m_buffer,
      m_request,
      bind_executor(
          m_strand,
          std::bind(
              &HttpSession::OnRead,
              shared_from_this(),
              _1, // error code
              _2 // transferred
              )));
}

void HttpSession::OnRead(error_code ec, size_t /*transferred*/) {
  if (http::error::end_of_stream == ec)
    return Close();

  if (ec) {
    // ISS:2735328 - Implement failure propagation mechanism.
    return;
  }

  Respond(); // ISS:2735328 - Handle request.
}

// disable __WARNING_IMPLICIT_CTOR
#pragma warning(push)
#pragma warning(disable : 25001)

void HttpSession::Respond() {
  switch (m_request.method()) {
    case http::verb::get:
      m_response = make_shared<http::response<http::dynamic_body>>(m_callbacks.OnGet(m_request));

      http::async_write(
          m_socket,
          *m_response,
          bind_executor(
              m_strand,
              std::bind(
                  &HttpSession::OnWrite,
                  shared_from_this(),
                  _1, // error code
                  _2, // transferred
                  m_response->need_eof() // close
                  )));

      break;

    case http::verb::options:
      m_response = make_shared<http::response<http::dynamic_body>>(http::status::accepted, m_request.version());
      m_response->set(
          http::field::access_control_request_headers,
          "Access-Control-Allow-Headers, Content-type, Custom-Header, Header-expose-allowed");
      m_response->set(http::field::access_control_allow_methods, "GET, POST, DELETE");
      m_response->set(http::field::access_control_expose_headers, "Header-expose-allowed");
      m_response->result(http::status::ok);

      http::async_write(
          m_socket,
          *m_response,
          bind_executor(
              m_strand,
              std::bind(
                  &HttpSession::OnWrite,
                  shared_from_this(),
                  _1, // error code
                  _2, // transferred
                  m_response->need_eof() // close
                  )));

      break;

    case http::verb::post:
      break;
    case http::verb::put:
      break;
    case http::verb::delete_:
      this->Close();
      break;
    default:
      // ISS:2735328 - Implement failure propagation mechanism
      break;
  }
}

void HttpSession::OnWrite(error_code ec, size_t /*transferred*/, bool /*close*/) {
  if (ec) {
    m_response = nullptr;
    return;
  }

  m_callbacks.OnResponseSent();

  // TODO: Re-enable when concurrent sessions are implemented.
  // If response indicates "Connection: close"
  // if (close)
  //  return Close();

  // Clear response
  m_response = nullptr;

  // ISS:2735328: Re-enable for subsequent dispatching.
  // Read();
}

void HttpSession::Close() {
  m_socket.shutdown(tcp::socket::shutdown_send);
}

void HttpSession::Start() {
  Read();
}

#pragma warning(pop)

#pragma endregion // HttpSession

#pragma region HttpServer

HttpServer::HttpServer(string &&address, uint16_t port) : m_acceptor{m_context}, m_socket{m_context}, m_sessions{} {
  auto endpoint = tcp::endpoint{make_address(std::move(address)), port};
  error_code ec;
  m_acceptor.open(endpoint.protocol(), ec);
  if (ec) {
    // ISS:2735328 - Implement failure propagation mechanism
    return;
  }

  m_acceptor.set_option(boost::asio::socket_base::reuse_address(true), ec);
  if (ec) {
    // ISS:2735328 - Implement failure propagation mechanism
    return;
  }

  m_acceptor.bind(endpoint, ec);
  if (ec) {
    // ISS:2735328 - Implement failure propagation mechanism
    return;
  }

  m_acceptor.listen(boost::asio::socket_base::max_listen_connections, ec);
  if (ec) {
    // ISS:2735328 - Implement failure propagation mechanism
    return;
  }
}

HttpServer::~HttpServer() {}

void HttpServer::Accept() {
  if (!m_acceptor.is_open())
    return;

  m_acceptor.async_accept(m_socket, std::bind(&HttpServer::OnAccept, shared_from_this(), _1));
}

void HttpServer::OnAccept(error_code ec) {
  if (ec) {
    // ISS:2735328 - Implement failure propagation mechanism
  } else {
    auto session = make_shared<HttpSession>(m_socket, m_callbacks);
    m_sessions.push_back(session);
    session->Start();
  }

  // Accept next connection.
  // Accept(); //ISS:2735328: Uncomment after implementing multiple context
  // threading.
}

void HttpServer::Start() {
  Accept();

  m_contextThread = std::thread([self = shared_from_this()]() {
    // See
    // https://www.boost.org/doc/libs/1_68_0/doc/html/boost_asio/reference/io_context/run/overload1.html
    // The run() function blocks until all work has finished and there are no
    // more handlers to be dispatched, or until the io_context has been stopped.
    self->m_context.run();
  });
}

void HttpServer::Stop() {
  m_contextThread.join();

  if (m_acceptor.is_open())
    m_acceptor.close();
}

void HttpServer::SetOnResponseSent(function<void()> &&handler) noexcept {
  m_callbacks.OnResponseSent = std::move(handler);
}

void HttpServer::SetOnGet(
    function<http::response<http::dynamic_body>(const http::request<http::string_body> &)> &&handler) noexcept {
  m_callbacks.OnGet = std::move(handler);
}

#pragma endregion HttpServer

} // namespace Microsoft::React::Test
