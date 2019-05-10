#include "WebSocketServer.h"

#include <boost/asio/bind_executor.hpp>

using namespace boost::asio;

using boost::system::error_code;

namespace Microsoft {
namespace React {
namespace Test {

#pragma region WebSocketSession

WebSocketSession::WebSocketSession(io_context& context)
  : m_context{ context }
  , m_stream{ context }
  , m_strand{ m_stream.get_executor() }
{
}

WebSocketSession::WebSocketSession(io_context& context, ip::tcp::socket socket)
  : m_context{ context }
  , m_stream{ std::move(socket) }
  , m_strand{ m_stream.get_executor() }
{
}

void WebSocketSession::Start()
{
  m_stream.async_accept(bind_executor(m_strand, std::bind(&WebSocketSession::OnAccept, shared_from_this(), std::placeholders::_1)));
}

void WebSocketSession::OnAccept(error_code ec)
{
  if (ec)
    return;//TODO: fail

  Read();
}

void WebSocketSession::Read()
{
  m_stream.async_read(m_buffer, bind_executor(m_strand, std::bind(
    &WebSocketSession::OnRead,
    shared_from_this(),
    std::placeholders::_1,
    std::placeholders::_2
  )));
}

void WebSocketSession::OnRead(error_code ec, size_t /*transferred*/)
{
  if (boost::beast::websocket::error::closed == ec)
    return;

  if (ec)
    ;//TODO: fail

  m_stream.text(m_stream.got_text());
  m_stream.async_write(m_buffer.data(), bind_executor(m_strand, std::bind(
    &WebSocketSession::OnWrite,
    shared_from_this(),
    std::placeholders::_1,
    std::placeholders::_2
  )));
}

void WebSocketSession::OnWrite(error_code ec, size_t /*transferred*/)
{
  if (ec)
    return; //TODO: fail

  m_buffer.consume(m_buffer.size());

  Read();
}

#pragma endregion // WebSocketSession

#pragma region WebSocketServer

WebSocketServer::WebSocketServer(uint16_t port)
  : m_acceptor{ m_context }
  , m_socket{ m_context }
{
  ip::tcp::endpoint ep{ip::make_address("0.0.0.0"), port };
  error_code ec;

  m_acceptor.open(ep.protocol(), ec);
  if (ec)
  {
    return; //
  }

  m_acceptor.set_option(socket_base::reuse_address(true), ec);
  if (ec)
  {
    return; //
  }

  m_acceptor.bind(ep, ec);
  if (ec)
  {
    return; //
  }

  m_acceptor.listen(socket_base::max_listen_connections, ec);
  if (ec)
  {
    return; //
  }
} // constructor(uint16_t)

void WebSocketServer::Start()
{
  if (!m_acceptor.is_open())
    return;

  Accept();
}

void WebSocketServer::Accept()
{
  m_acceptor.async_accept(m_socket, std::bind(&WebSocketServer::OnAccept, shared_from_this(), std::placeholders::_1));
}

void WebSocketServer::OnAccept(error_code ec)
{
  if (ec)
  {
    //TODO: fail
  }
  else
  {
    std::make_shared<WebSocketSession>(m_context, std::move(m_socket))->Start();
  }

  //TODO: Accept again.
  //Accept();
}

#pragma endregion // WebSocketServer

} } } // Microsoft::React::Test
