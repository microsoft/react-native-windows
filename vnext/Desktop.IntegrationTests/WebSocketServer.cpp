#include "WebSocketServer.h"

#include <boost/asio/bind_executor.hpp>

using namespace boost::asio;

using boost::system::error_code;
using std::function;
using std::string;

namespace Microsoft {
namespace React {
namespace Test {

#pragma region WebSocketSession

WebSocketSession::WebSocketSession(ip::tcp::socket socket, WebSocketServiceCallbacks& callbacks)
  : m_stream{ std::move(socket) }
  , m_strand{ m_stream.get_executor() }
  , m_callbacks{ callbacks }
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

  if (m_callbacks.OnConnection)
    m_callbacks.OnConnection();

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
    return;//TODO: fail instead

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

WebSocketServer::WebSocketServer(int port)
  : WebSocketServer(static_cast<uint16_t>(port))
{
}

void WebSocketServer::Start()
{
  if (!m_acceptor.is_open())
    return;

  Accept();

  m_contextThread = std::thread([self = shared_from_this()]()
  {
    self->m_context.run();
  });
}

void WebSocketServer::Accept()
{
  m_acceptor.async_accept(m_socket, std::bind(&WebSocketServer::OnAccept, shared_from_this(), std::placeholders::_1));
}

void WebSocketServer::Stop()
{
  if (m_acceptor.is_open())
    m_acceptor.close();

  m_contextThread.join();
}

void WebSocketServer::OnAccept(error_code ec)
{
  if (ec)
  {
    //TODO: fail
  }
  else
  {
    std::make_shared<WebSocketSession>(std::move(m_socket), m_callbacks)->Start();
  }

  //TODO: Accept again.
  //Accept();
}

void WebSocketServer::SetOnConnection(function<void()>&& func)
{
  m_callbacks.OnConnection = std::move(func);
}

void WebSocketServer::SetOnMessage(function<void(string)>&& func)
{
  m_callbacks.OnMessage = std::move(func);
}

void WebSocketServer::SetMessageFactory(function<string(bool)>&& func)
{
  m_callbacks.MessageFactory = std::move(func);
}

void WebSocketServer::SetOnError(function<void(IWebSocket::Error&&)>&& func)
{
  m_callbacks.OnError = std::move(func);
}

#pragma endregion // WebSocketServer

} } } // Microsoft::React::Test
