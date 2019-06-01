#pragma once

#include <boost/asio/strand.hpp>
#include <boost/beast/websocket/ssl.hpp>
#include <boost/beast/websocket.hpp>
#include <IWebSocket.h>
#include <thread>

namespace Microsoft {
namespace React {
namespace Test {

struct WebSocketServiceCallbacks
{
  std::function<void()> OnConnection;
  std::function<void(boost::beast::websocket::response_type&)> OnHandshake;
  std::function<void(std::string)> OnMessage;
  std::function<std::string(std::string&&)> MessageFactory;
  std::function<void(IWebSocket::Error&&)> OnError;
};

struct IWebSocketSession
{
  virtual ~IWebSocketSession() {}

  virtual void Start() = 0;
};

template<typename NextLayer = boost::asio::ip::tcp::socket>
class BaseWebSocketSession : public std::enable_shared_from_this<BaseWebSocketSession<NextLayer>>, public IWebSocketSession
{
  //TODO: Used at all?
  enum class State : std::size_t
  {
    Started,
    Stopped
  };

  boost::beast::multi_buffer m_buffer;
  std::string m_message;
  WebSocketServiceCallbacks& m_callbacks;
  State m_state;//TODO: Used at all?

  std::function<void(IWebSocket::Error&&)> m_errorHandler;

  void Read();

  void OnAccept(boost::system::error_code ec);
  void OnHandshake(boost::beast::websocket::response_type& response);
  void OnRead(boost::system::error_code ec, std::size_t transferred);
  void OnWrite(boost::system::error_code ec, std::size_t transferred);

protected:
  std::shared_ptr<boost::beast::websocket::stream<NextLayer>> m_stream;
  std::shared_ptr<boost::asio::strand<boost::asio::io_context::executor_type>> m_strand;

  void Accept();

public:
  BaseWebSocketSession(WebSocketServiceCallbacks& callbacks);
  ~BaseWebSocketSession();

  virtual void Start() override;
};

class WebSocketSession
  : public BaseWebSocketSession<>
{
public:
  WebSocketSession(boost::asio::ip::tcp::socket socket, WebSocketServiceCallbacks& callbacks);
  ~WebSocketSession();
};

class SecureWebSocketSession
  : public BaseWebSocketSession<boost::asio::ssl::stream<boost::asio::ip::tcp::socket>>
{
  SecureWebSocketSession(boost::asio::ip::tcp::socket socket, WebSocketServiceCallbacks& callbacks);
  ~SecureWebSocketSession();

  void OnSslHandshake(boost::system::error_code ec);

  #pragma region IWebSocketSession

  void Start() override;

  #pragma endregion //IWebSocketSession
};

class WebSocketServer : public std::enable_shared_from_this<WebSocketServer>
{
  std::thread m_contextThread;
  boost::asio::io_context m_context;
  boost::asio::ip::tcp::acceptor m_acceptor;
  boost::asio::ip::tcp::socket m_socket;
  WebSocketServiceCallbacks m_callbacks;
  std::vector<std::shared_ptr<IWebSocketSession>> m_sessions;

  void Accept();

  void OnAccept(boost::system::error_code ec);

public:
  WebSocketServer(std::uint16_t port);
  WebSocketServer(int port);

  void Start();
  void Stop();

  void SetOnConnection(std::function<void()>&& func);
  void SetOnHandshake(std::function<void(boost::beast::websocket::response_type&)>&& func);
  void SetOnMessage(std::function<void(std::string)>&& func);
  void SetMessageFactory(std::function<std::string(std::string&&)>&& func);
  void SetOnError(std::function<void(IWebSocket::Error&&)>&& func);
};

} } } // Microsoft::React::Test
