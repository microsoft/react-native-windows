// clang-format off

// C4996: 'gethostbyaddr': Use getnameinfo() or GetNameInfoW() instead
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include "WebSocketServer.h"

#include <boost/asio/bind_executor.hpp>
#include <boost/asio/strand.hpp>
#include <boost/beast/core/buffers_to_string.hpp>

using namespace boost::asio;

namespace websocket = boost::beast::websocket;

using boost::beast::bind_front_handler;
using boost::beast::tcp_stream;
using boost::system::error_code;
using std::function;
using std::string;
using std::vector;

using Error = Microsoft::React::Networking::IWebSocketResource::Error;
using ErrorType = Microsoft::React::Networking::IWebSocketResource::ErrorType;

namespace Microsoft::React::Test
{

#pragma region BaseWebSocketSession

template <typename SocketLayer>
BaseWebSocketSession<SocketLayer>::BaseWebSocketSession(WebSocketServiceCallbacks& callbacks)
  : m_callbacks{callbacks}
  , m_state{State::Stopped}{}

template <typename SocketLayer>
BaseWebSocketSession<SocketLayer>::~BaseWebSocketSession() {}

template <typename SocketLayer>
void BaseWebSocketSession<SocketLayer>::Start()
{
  Accept();
}

template <typename SocketLayer>
void BaseWebSocketSession<SocketLayer>::Accept()
{
  // Turn off the timeout on the tcp_stream, because
  // the websocket stream has its own timeout system.
  boost::beast::get_lowest_layer(*m_stream).expires_never();

  m_stream->set_option(
    websocket::stream_base::timeout::suggested(boost::beast::role_type::server)
  );

  m_stream->set_option(websocket::stream_base::decorator([self = this->SharedFromThis()](websocket::response_type& response)
  {
      response.set(boost::beast::http::field::server, string(BOOST_BEAST_VERSION_STRING) + "Test WebSocket Server");
      self->OnHandshake(response);
  }));

  m_stream->async_accept(
    bind_front_handler(&BaseWebSocketSession<SocketLayer>::OnAccept, this->SharedFromThis())
  );
}

template <typename SocketLayer>
void BaseWebSocketSession<SocketLayer>::OnHandshake(websocket::response_type& response)
{
  if (m_callbacks.OnHandshake)
    m_callbacks.OnHandshake(response);
}

template <typename SocketLayer>
void BaseWebSocketSession<SocketLayer>::OnAccept(error_code ec)
{
  if (ec)
  {
    if (m_callbacks.OnError)
      m_callbacks.OnError({ec.message(), ErrorType::Connection});

    return;
  }

  m_state = State::Started;

  if (m_callbacks.OnConnection)
    m_callbacks.OnConnection();

  Read();
}

template <typename SocketLayer>
void BaseWebSocketSession<SocketLayer>::Read()
{
  if (State::Stopped == m_state)
    return;

  m_stream->async_read(
    m_buffer,
    bind_front_handler(&BaseWebSocketSession<SocketLayer>::OnRead, this->SharedFromThis())
  );
}

template <typename SocketLayer>
void BaseWebSocketSession<SocketLayer>::OnRead(error_code ec, size_t /*transferred*/)
{
  if (websocket::error::closed == ec)
    return;

  if (ec)
  {
    if (ec.value() != error::connection_reset &&
        ec.value() != error::connection_aborted &&
        ec.value() != 335544539 /*short read*/)
      if (m_callbacks.OnError)
        m_callbacks.OnError({ec.message(), ErrorType::Receive});
  }

  if (m_stream->got_binary())
  {
    if (!m_callbacks.BinaryMessageFactory)
    {
      m_buffer.consume(m_buffer.size());
      return Read();
    }

    // Obtain raw pointer to underlying buffer memory.
    // multi_buffer -> ConstBufferSequence -> const_buffer -> void* data()
    // https://stackoverflow.com/questions/9510684
    auto data = boost::asio::buffer_cast<uint8_t*>(*boost::asio::buffer_sequence_begin(m_buffer.data()));
    m_binaryMessage = m_callbacks.BinaryMessageFactory({data, data + m_buffer.size()});
    m_buffer.consume(m_buffer.size());

    m_stream->binary(true);
    m_stream->async_write(
      buffer(m_binaryMessage),
      bind_front_handler(&BaseWebSocketSession<SocketLayer>::OnWrite, this->SharedFromThis())
    );
  }
  else
  {
    if (!m_callbacks.MessageFactory)
    {
      m_buffer.consume(m_buffer.size());
      return Read();
    }

    m_message = m_callbacks.MessageFactory(buffers_to_string(m_buffer.data()));
    m_buffer.consume(m_buffer.size());

    m_stream->text(true);
    m_stream->async_write(
      buffer(m_message),
      bind_front_handler(&BaseWebSocketSession<SocketLayer>::OnWrite, this->SharedFromThis())
    );
  }
}

template <typename SocketLayer>
void BaseWebSocketSession<SocketLayer>::OnWrite(error_code ec, size_t /*transferred*/)
{
  if (ec)
  {
    if (ec.value() != error::operation_aborted)
      if (m_callbacks.OnError)
        m_callbacks.OnError({ec.message(), ErrorType::Send});

    return;
  }

  // Clear outgoing message contents.
  m_message.clear();

  Read();
}

#pragma endregion BaseWebSocketSession

#pragma region WebSocketSession

WebSocketSession::WebSocketSession(ip::tcp::socket socket, WebSocketServiceCallbacks& callbacks)
  : BaseWebSocketSession(callbacks)
{
  m_stream = std::make_shared<websocket::stream<tcp_stream>>(std::move(socket));
}

WebSocketSession::~WebSocketSession() {}

#pragma region BaseWebSocketSession

std::shared_ptr<BaseWebSocketSession<tcp_stream>> WebSocketSession::SharedFromThis() /*override*/
{
  return this->shared_from_this();
}

#pragma endregion BaseWebSocketSession

#pragma endregion WebSocketSession

#pragma region WebSocketServer

WebSocketServer::WebSocketServer(uint16_t port, bool useTLS)
  : m_acceptor{make_strand(m_context)}, m_sessions{}, m_useTLS{useTLS}
{
  ip::tcp::endpoint ep{ip::make_address("0.0.0.0"), port};
  error_code ec;

  m_acceptor.open(ep.protocol(), ec);
  if (ec)
  {
    if (m_callbacks.OnError)
      m_callbacks.OnError({ec.message(), ErrorType::Connection});

    return;
  }

  m_acceptor.set_option(socket_base::reuse_address(true), ec);
  if (ec)
  {
    if (m_callbacks.OnError)
      m_callbacks.OnError({ec.message(), ErrorType::Connection});

    return;
  }

  m_acceptor.bind(ep, ec);
  if (ec)
  {
    if (m_callbacks.OnError)
      m_callbacks.OnError({ec.message(), ErrorType::Connection});

    return;
  }

  m_acceptor.listen(socket_base::max_listen_connections, ec);
  if (ec)
  {
    if (m_callbacks.OnError)
      m_callbacks.OnError({ec.message(), ErrorType::Connection});

    return;
  }
}

WebSocketServer::WebSocketServer(int port, bool useTLS) : WebSocketServer(static_cast<uint16_t>(port), useTLS) {}

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
  m_acceptor.async_accept(
    make_strand(m_context),
    bind_front_handler(&WebSocketServer::OnAccept, shared_from_this())
  );
}

void WebSocketServer::Stop()
{
  if (m_acceptor.is_open())
    m_acceptor.close();

  m_contextThread.join();
}

void WebSocketServer::OnAccept(error_code ec, ip::tcp::socket socket)
{
  if (ec)
  {
    if (ec.value() != error::operation_aborted)
      if (m_callbacks.OnError)
        m_callbacks.OnError({ec.message(), ErrorType::Connection});

    return;
  }

  std::shared_ptr<IWebSocketSession> session = std::shared_ptr<IWebSocketSession>(new WebSocketSession(std::move(socket), m_callbacks));

  m_sessions.push_back(session);
  session->Start();

  Accept();
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

void WebSocketServer::SetMessageFactory(function<vector<uint8_t>(vector<uint8_t>&&)>&& func)
{
  m_callbacks.BinaryMessageFactory = std::move(func);
}

void WebSocketServer::SetOnError(function<void(Error&&)>&& func)
{
  m_callbacks.OnError = std::move(func);
}

#pragma endregion WebSocketServer

} // namespace Microsoft::React::Test
