#pragma once

#include <boost/asio/strand.hpp>
#include <boost/beast/websocket.hpp>
#include <IWebSocket.h>
#include <thread>

namespace Microsoft {
namespace React {
namespace Test {

struct WebSocketServiceCallbacks
{
  std::function<void()> OnConnection;
  std::function<void(std::string)> OnMessage;
  std::function<std::string(std::string&&)> MessageFactory;
  std::function<void(IWebSocket::Error&&)> OnError;
};

class WebSocketSession : public std::enable_shared_from_this<WebSocketSession>
{
  boost::beast::websocket::stream<boost::asio::ip::tcp::socket> m_stream;
  boost::asio::strand<boost::asio::io_context::executor_type> m_strand;
  boost::beast::multi_buffer m_buffer;
  std::string m_message;
  WebSocketServiceCallbacks& m_callbacks;

  std::function<void(IWebSocket::Error&&)> m_errorHandler;

public:
  WebSocketSession(boost::asio::ip::tcp::socket socket, WebSocketServiceCallbacks& callbacks);

  void Start();
  void Read();

  void OnAccept(boost::system::error_code ec);
  void OnRead(boost::system::error_code ec, std::size_t transferred);
  void OnWrite(boost::system::error_code ec, std::size_t transferred);
};

class WebSocketServer : public std::enable_shared_from_this<WebSocketServer>
{
  std::thread m_contextThread;
  boost::asio::io_context m_context;
  boost::asio::ip::tcp::acceptor m_acceptor;
  boost::asio::ip::tcp::socket m_socket;
  WebSocketServiceCallbacks m_callbacks;

  void Accept();

  void OnAccept(boost::system::error_code ec);

public:
  WebSocketServer(std::uint16_t port);
  WebSocketServer(int port);

  void Start();
  void Stop();

  void SetOnConnection(std::function<void()>&& func);
  void SetOnMessage(std::function<void(std::string)>&& func);
  void SetMessageFactory(std::function<std::string(std::string&&)>&& func);
  void SetOnError(std::function<void(IWebSocket::Error&&)>&& func);
};

} } } // Microsoft::React::Test
