#include "WebSocketServer.h"

#include <boost/asio/bind_executor.hpp>
#include <boost/beast/core/buffers_to_string.hpp>

using namespace boost::asio;

using boost::system::error_code;
using std::function;
using std::placeholders::_1;
using std::placeholders::_2;
using std::string;

namespace websocket = boost::beast::websocket;

namespace Microsoft {
namespace React {
namespace Test {

#pragma region BaseWebSocketSession

template<typename NextLayer>
BaseWebSocketSession<NextLayer>::BaseWebSocketSession(WebSocketServiceCallbacks& callbacks)
  : m_callbacks{ callbacks }
  , m_state{ State::Stopped }
{
}

template<typename NextLayer>
BaseWebSocketSession<NextLayer>::~BaseWebSocketSession()
{
}

template<typename NextLayer>
void BaseWebSocketSession<NextLayer>::Start()
{
  Accept();
}

template<typename NextLayer>
void BaseWebSocketSession<NextLayer>::Accept()
{
  m_stream->async_accept_ex(
    bind_executor(*m_strand, std::bind(
      &BaseWebSocketSession<NextLayer>::OnHandshake,
      this->shared_from_this(),
      _1 // response
    )),
    bind_executor(*m_strand, std::bind(
      &BaseWebSocketSession<NextLayer>::OnAccept,
      this->shared_from_this(),
      _1 // ec
    ))
  );
}

template<typename NextLayer>
void BaseWebSocketSession<NextLayer>::OnHandshake(websocket::response_type& response)
{
  if (m_callbacks.OnHandshake)
    m_callbacks.OnHandshake(response);
}

template<typename NextLayer>
void BaseWebSocketSession<NextLayer>::OnAccept(error_code ec)
{
  if (ec)
    return;//TODO: fail

  m_state = State::Started;

  if (m_callbacks.OnConnection)
    m_callbacks.OnConnection();

  Read();
}

template<typename NextLayer>
void BaseWebSocketSession<NextLayer>::Read()
{
  if (State::Stopped == m_state)
    return;

  m_stream->async_read(m_buffer, bind_executor(*m_strand, std::bind(
    &BaseWebSocketSession<NextLayer>::OnRead,
    this->shared_from_this(),
    _1, // ec
    _2  // transferred
  )));
}

template<typename NextLayer>
void BaseWebSocketSession<NextLayer>::OnRead(error_code ec, size_t /*transferred*/)
{
  if (websocket::error::closed == ec)
    return;

  if (ec)
    return;//TODO: fail instead

  if (!m_callbacks.MessageFactory)
  {
    m_buffer.consume(m_buffer.size());
    return Read();
  }

  m_message = m_callbacks.MessageFactory(buffers_to_string(m_buffer.data()));
  m_buffer.consume(m_buffer.size());

  m_stream->text(m_stream->got_text());
  m_stream->async_write(buffer(m_message), bind_executor(*m_strand, std::bind(
    &BaseWebSocketSession<NextLayer>::OnWrite,
    this->shared_from_this(),
    _1, // ec
    _2  // transferred
  )));
}

template<typename NextLayer>
void BaseWebSocketSession<NextLayer>::OnWrite(error_code ec, size_t /*transferred*/)
{
  if (ec)
    return; //TODO: fail

  // Clear outgoing message contents.
  m_message.clear();

  Read();
}

#pragma endregion // BaseWebSocketSession

#pragma region WebSocketSession

WebSocketSession::WebSocketSession(ip::tcp::socket socket, WebSocketServiceCallbacks& callbacks)
  : BaseWebSocketSession(callbacks)
{
  m_stream = std::make_shared<websocket::stream<boost::asio::ip::tcp::socket>>(std::move(socket));
  m_strand = std::make_shared<strand<io_context::executor_type>>(m_stream->get_executor());
}

WebSocketSession::~WebSocketSession() {}

#pragma endregion // WebSocketSession

#pragma region SecureWebSocketSession

SecureWebSocketSession::SecureWebSocketSession(ip::tcp::socket socket, WebSocketServiceCallbacks& callbacks)
  : BaseWebSocketSession(callbacks)
{
  auto context = ssl::context(ssl::context::sslv23_client);
  m_stream = std::make_shared<websocket::stream<ssl::stream<ip::tcp::socket>>>(std::move(socket), context);
  m_strand = std::make_shared<strand<io_context::executor_type>>(m_stream->get_executor());
}

SecureWebSocketSession::~SecureWebSocketSession() {}

#pragma region IWebSocketSession

void SecureWebSocketSession::Start() /*override*/
{
  //m_stream->next_layer().async_handshake(ssl::stream_base::server, bind_executor(*m_strand, std::bind(
  //  &SecureWebSocketSession::OnSslHandshake,
  //  this->shared_from_this(),
  //  _1 // ec
  //)));
}

void SecureWebSocketSession::OnSslHandshake(error_code ec)
{
  if (ec)
    return;//TODO: Handle error

  Accept();
}

#pragma endregion // IWebSocketSession

#pragma endregion // SecureWebSocketSession

#pragma region WebSocketServer

WebSocketServer::WebSocketServer(uint16_t port)
  : m_acceptor{ m_context }
  , m_socket{ m_context }
  , m_sessions{}
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
}

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
  m_acceptor.async_accept(m_socket, std::bind(&WebSocketServer::OnAccept, shared_from_this(), /*ec*/ _1));
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
    auto session = std::shared_ptr<IWebSocketSession>(new WebSocketSession(std::move(m_socket), m_callbacks));
    m_sessions.push_back(session);
    session->Start();
  }

  //TODO: Accept again.
  //Accept();
}

void WebSocketServer::SetOnConnection(function<void()>&& func)
{
  m_callbacks.OnConnection = std::move(func);
}

void WebSocketServer::SetOnHandshake(function<void(websocket::response_type&)>&& func)
{
  m_callbacks.OnHandshake = std::move(func);
}

void WebSocketServer::SetOnMessage(function<void(string)>&& func)
{
  m_callbacks.OnMessage = std::move(func);
}

void WebSocketServer::SetMessageFactory(function<string(string&&)>&& func)
{
  m_callbacks.MessageFactory = std::move(func);
}

void WebSocketServer::SetOnError(function<void(IWebSocket::Error&&)>&& func)
{
  m_callbacks.OnError = std::move(func);
}

#pragma endregion // WebSocketServer

} } } // Microsoft::React::Test
