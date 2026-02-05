// clang-format off

// C4996: 'gethostbyaddr': Use getnameinfo() or GetNameInfoW() instead
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include "HttpServer.h"

#include <boost/beast/core/multi_buffer.hpp>
#include <boost/beast/version.hpp>
#include <boost/asio/dispatch.hpp>

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
using std::static_pointer_cast;
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

#pragma region ResponseWrapper

ResponseWrapper::ResponseWrapper(DynamicResponse&& response)
  : m_response{ make_shared<DynamicResponse>(std::move(response)) }
  , m_type{ ResponseType::Dynamic }
{
}

ResponseWrapper::ResponseWrapper(EmptyResponse&& response)
  : m_response{ make_shared<EmptyResponse>(std::move(response)) }
  , m_type{ ResponseType::Empty }
{
}

ResponseWrapper::ResponseWrapper(FileResponse&& response)
  : m_response{ make_shared<FileResponse>(std::move(response)) }
  , m_type{ ResponseType::File }
{
}

ResponseWrapper::ResponseWrapper(StringResponse&& response)
  : m_response{ make_shared<StringResponse>(std::move(response)) }
  , m_type{ ResponseType::String }
{
}

ResponseWrapper::ResponseWrapper(DynamicResponse const& response)
  : m_response{ make_shared<DynamicResponse>(response) }
  , m_type{ ResponseType::Dynamic }
{
}

ResponseWrapper::ResponseWrapper(EmptyResponse const& response)
  : m_response{ make_shared<EmptyResponse>(response) }
  , m_type{ ResponseType::Empty }
{
}

ResponseWrapper::ResponseWrapper(StringResponse const& response)
  : m_response{ make_shared<StringResponse>(response) }
  , m_type{ ResponseType::String }
{
}

shared_ptr<void> ResponseWrapper::Response()
{
  return m_response;
}

ResponseWrapper::ResponseType ResponseWrapper::Type()
{
  return m_type;
}

#pragma endregion ResponseWrapper

#pragma region HttpSession

HttpSession::HttpSession(tcp::socket &&socket, HttpCallbacks &callbacks, io_context& context)
  : m_stream{ std::move(socket) }
  , m_callbacks{ callbacks }
{
}

HttpSession::~HttpSession() {}

void HttpSession::Start()
{
  m_sendLambda = [self = shared_from_this()](ResponseWrapper&& wrapper)
  {
    auto dr = wrapper.Response();
    auto type = wrapper.Type();
    self->m_response = make_shared<ResponseWrapper>(std::move(wrapper));

    // Ugh!
    switch (type)
    {
    case ResponseWrapper::ResponseType::Empty:
      http::async_write(
        self->m_stream,
        *static_pointer_cast<EmptyResponse>(dr),
        bind_front_handler(
          &HttpSession::OnWrite,
          self->shared_from_this(),
          static_pointer_cast<EmptyResponse>(dr)->need_eof()
        )
      );
      break;

    case ResponseWrapper::ResponseType::Dynamic:
      http::async_write(
        self->m_stream,
        *static_pointer_cast<DynamicResponse>(dr),
        bind_front_handler(
          &HttpSession::OnWrite,
          self->shared_from_this(),
          static_pointer_cast<DynamicResponse>(dr)->need_eof()
        )
      );
      break;

    case ResponseWrapper::ResponseType::File:
      http::async_write(
        self->m_stream,
        *static_pointer_cast<FileResponse>(dr),
        bind_front_handler(
          &HttpSession::OnWrite,
          self->shared_from_this(),
          static_pointer_cast<FileResponse>(dr)->need_eof()
        )
      );
      break;

    case ResponseWrapper::ResponseType::String:
      http::async_write(
        self->m_stream,
        *static_pointer_cast<StringResponse>(dr),
        bind_front_handler(
          &HttpSession::OnWrite,
          self->shared_from_this(),
          static_pointer_cast<StringResponse>(dr)->need_eof()
        )
      );
      break;

    default:
      throw;
	}
  };

  // Ensure thread-safety.
  boost::asio::dispatch(m_stream.get_executor(), bind_front_handler(&HttpSession::Read, shared_from_this()));
}

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

  Respond();
}

void HttpSession::Respond()
{
  switch (m_request.method())
  {
    case http::verb::get:
      m_sendLambda(m_callbacks.OnGet(m_request));
      break;

    case http::verb::options:
      if (!m_callbacks.OnOptions)
      {
        // Default OPTIONS handler
        m_callbacks.OnOptions = [](const DynamicRequest& request) -> DynamicResponse {
          DynamicResponse response{http::status::accepted, request.version()};
          response.set(
            http::field::access_control_request_headers,
            "Access-Control-Allow-Headers, Content-type, Custom-Header, Header-expose-allowed");
          response.set(http::field::access_control_allow_methods, "GET, POST, DELETE");
          response.set(http::field::access_control_expose_headers, "Header-expose-allowed");
          response.result(http::status::ok);

          return { std::move(response) };
        };
      }
      m_sendLambda(m_callbacks.OnOptions(m_request));
      break;

    case http::verb::post:
      break;
    case http::verb::put:
      break;
    case http::verb::delete_:
      this->Close();
      break;
    case http::verb::patch:
      m_sendLambda(m_callbacks.OnPatch(m_request));
      break;

    case http::verb::connect:
      m_sendLambda(m_callbacks.OnConnect(m_request));
      break;

    case http::verb::trace:
      m_sendLambda(m_callbacks.OnTrace(m_request));
      break;

    default:
      // ISS:2735328 - Implement failure propagation mechanism
      throw;
  }
}

void HttpSession::OnWrite(bool close, error_code ec, size_t /*transferred*/)
{
  if (ec)
  {
    return;
  }

  if (m_callbacks.OnResponseSent)
  {
	  m_callbacks.OnResponseSent();
  }

   if (close)
    return Close();

  // Clear response
  m_response = nullptr;

  Read();
}

void HttpSession::Close()
{
  error_code ec;
  m_stream.socket().shutdown(tcp::socket::shutdown_send, ec);
}

#pragma endregion // HttpSession

#pragma region HttpServer

HttpServer::HttpServer(string &&address, uint16_t port, int concurrency)
  : m_ioThreadCount{ static_cast<size_t>(concurrency) }
  , m_ioContext{concurrency}
  , m_acceptor{make_strand(m_ioContext)}
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

HttpServer::HttpServer(uint16_t port, int concurrency)
  : HttpServer("0.0.0.0", port, concurrency)
{
}

HttpServer::~HttpServer() {}

void HttpServer::Accept()
{
  if (!m_acceptor.is_open())
    return;

  m_acceptor.async_accept(
    make_strand(m_ioContext),
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
    make_shared<HttpSession>(std::move(socket), m_callbacks, m_ioContext)->Start();
  }

  Accept();
}

void HttpServer::Start()
{
  Accept();

  m_ioThreads.reserve(m_ioThreadCount);
  for (size_t i = 0; i < m_ioThreadCount; i++)
  {
    m_ioThreads.emplace_back([self = shared_from_this()]()
    {
    // See
    // https://www.boost.org/doc/libs/1_76_0/doc/html/boost_asio/reference/io_context/run/overload1.html
    // The run() function blocks until all work has finished and there are no
    // more handlers to be dispatched, or until the io_context has been stopped.
      self->m_ioContext.run();
    });
  }
}

void HttpServer::Stop()
{
  m_ioContext.stop();

  for (auto& t : m_ioThreads)
    if (t.joinable())
      t.join();
}

void HttpServer::Abort()
{
  if (m_ioContext.stopped())
    return;

  Stop();
}

HttpCallbacks& HttpServer::Callbacks()
{
  return m_callbacks;
}

#pragma endregion HttpServer

} // namespace Microsoft::React::Test
