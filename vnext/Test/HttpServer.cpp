// clang-format off
#include "HttpServer.h"

#include <boost/beast/core/multi_buffer.hpp>
#include <boost/beast/version.hpp>
#include <boost/asio/dispatch.hpp>
#include <boost/asio/strand.hpp>

// Include to prevent 'incomplete type' errors.
#include <boost/utility/in_place_factory.hpp>
#include <boost/utility/typed_in_place_factory.hpp>

using namespace boost::asio::ip;

namespace http = boost::beast::http;

using boost::asio::io_context;
using boost::asio::make_strand;
using boost::beast::bind_front_handler;
using boost::system::error_code;
using std::function;
using std::make_shared;
using std::shared_ptr;
using std::string;

namespace Microsoft::React::Test
{

#pragma region Utility functions

boost::beast::multi_buffer CreateStringResponseBody(string&& content)
{
  auto result = boost::beast::multi_buffer();
  auto n = boost::asio::buffer_copy(result.prepare(content.size()), boost::asio::buffer(content));
  result.commit(n);

  return result;
}

#pragma endregion // Utility functions

#pragma region HttpSession

HttpSession::HttpSession(tcp::socket &&socket, HttpCallbacks &callbacks,
  boost::asio::strand<boost::asio::io_context::executor_type>& readStrand)
  : m_stream{ std::move(socket) }
  , m_callbacks{ callbacks }
  , m_readStrand{ readStrand }
{
}

HttpSession::~HttpSession() {}

void HttpSession::Read()
{
  // Clear request.
  m_request = {};

  http::async_read(
    m_stream,
    m_buffer,
    m_request,
    bind_front_handler(&HttpSession::OnRead, shared_from_this())
  );
}

void HttpSession::OnRead(error_code ec, size_t /*transferred*/)
{
  if (http::error::end_of_stream == ec)
    return Close();

  if (ec)
  {
    // ISS:2735328 - Implement failure propagation mechanism.
    return;
  }

  Respond(); // ISS:2735328 - Handle request.
}

void HttpSession::Respond()
{
  switch (m_request.method())
  {
    case http::verb::get:
      m_response = make_shared<DynamicResponse>(m_callbacks.OnGet(m_request));

      http::async_write(
        m_stream,
        *m_response,
        bind_front_handler(
          &HttpSession::OnWrite,
          shared_from_this(),
          m_response->need_eof() // close
        )
      );

      break;

    case http::verb::options:
      if (m_callbacks.OnOptions)
      {
        m_response = make_shared<DynamicResponse>(m_callbacks.OnOptions(m_request));
      }
      else
      {
        m_response = make_shared<DynamicResponse>(http::status::accepted, m_request.version());
        m_response->set(
            http::field::access_control_request_headers,
            "Access-Control-Allow-Headers, Content-type, Custom-Header, Header-expose-allowed");
        m_response->set(http::field::access_control_allow_methods, "GET, POST, DELETE");
        m_response->set(http::field::access_control_expose_headers, "Header-expose-allowed");
        m_response->result(http::status::ok);
      }

      http::async_write(
        m_stream,
        *m_response,
        bind_front_handler(
          &HttpSession::OnWrite,
          shared_from_this(),
          m_response->need_eof() // close
        )
      );

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

void HttpSession::OnWrite(bool /*close*/, error_code ec, size_t /*transferred*/)
{
  if (ec)
  {
    m_response = nullptr;
    return;
  }

  if (m_callbacks.OnResponseSent)
  {
	  m_callbacks.OnResponseSent();
  }

  // TODO: Re-enable when concurrent sessions are implemented.
  // If response indicates "Connection: close"
  // if (close)
  //  return Close();

  // Clear response
  m_response = nullptr;

  // ISS:2735328: Re-enable for subsequent dispatching.
  Read();
}

void HttpSession::Close()
{
  m_stream.socket().shutdown(tcp::socket::shutdown_send);
}

void HttpSession::Start()
{
  // Ensure thread-safety.
  boost::asio::dispatch(m_stream.get_executor(), bind_front_handler(&HttpSession::Read, shared_from_this()));
}

#pragma endregion // HttpSession

#pragma region HttpServer

HttpServer::HttpServer(string &&address, uint16_t port)
  : m_strand{make_strand(m_context)}
  , m_readStrand{make_strand(m_context)}
  , m_acceptor{m_strand}
  //, m_sessions{}
{
  auto endpoint = tcp::endpoint{make_address(std::move(address)), port};
  error_code ec;
  m_acceptor.open(endpoint.protocol(), ec);
  if (ec)
  {
    // ISS:2735328 - Implement failure propagation mechanism
    return;
  }

  m_acceptor.set_option(boost::asio::socket_base::reuse_address(true), ec);
  if (ec)
  {
    // ISS:2735328 - Implement failure propagation mechanism
    return;
  }

  m_acceptor.bind(endpoint, ec);
  if (ec)
  {
    // ISS:2735328 - Implement failure propagation mechanism
    return;
  }

  m_acceptor.listen(boost::asio::socket_base::max_listen_connections, ec);
  if (ec)
  {
    // ISS:2735328 - Implement failure propagation mechanism
    return;
  }
}

HttpServer::~HttpServer() {}

void HttpServer::Accept()
{
  if (!m_acceptor.is_open())
    return;

  m_acceptor.async_accept(
    //make_strand(m_context),
    m_strand,
    bind_front_handler(
      &HttpServer::OnAccept,
      shared_from_this()
    )
  );
}

void HttpServer::OnAccept(error_code ec, tcp::socket socket)
{
  if (ec)
  {
    // ISS:2735328 - Implement failure propagation mechanism
    return;
  }
  else
  {
    //auto session = make_shared<HttpSession>(std::move(socket), m_callbacks);
    //m_sessions.push_back(session);
    //session->Start();

    make_shared<HttpSession>(std::move(socket), m_callbacks, m_readStrand)->Start();
  }

  // ISS:2735328: Uncomment after implementing multiple context threading.
  // Accept next connection.
  // Accept();
}

void HttpServer::Start()
{
  Accept();

  for (int i = 0; i < 1; i++)
  {
    m_contextThreads.emplace_back([self = shared_from_this()]()
    {
    // See
    // https://www.boost.org/doc/libs/1_76_0/doc/html/boost_asio/reference/io_context/run/overload1.html
    // The run() function blocks until all work has finished and there are no
    // more handlers to be dispatched, or until the io_context has been stopped.
      self->m_context.run();
    });
  }

  for (auto& t : m_contextThreads)
  {
    t.detach();//TODO: join instead (later)?
  }
}

void HttpServer::Stop()
{
  if (m_acceptor.is_open())
    m_acceptor.close();
}

void HttpServer::Abort()
{
  if (m_context.stopped())
    return;

  m_context.stop();

  Stop();
}

void HttpServer::SetOnResponseSent(function<void()> &&handler) noexcept
{
  m_callbacks.OnResponseSent = std::move(handler);
}

void HttpServer::SetOnGet(
  function<DynamicResponse(const DynamicRequest &)> &&handler) noexcept
{
  m_callbacks.OnGet = std::move(handler);
}

void HttpServer::SetOnOptions(
  function<DynamicResponse(const DynamicRequest &)> &&handler) noexcept
{
  m_callbacks.OnOptions = std::move(handler);
}

#pragma endregion HttpServer

} // namespace Microsoft::React::Test
