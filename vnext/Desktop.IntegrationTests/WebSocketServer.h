#pragma once

#include <boost/asio/strand.hpp>
#include <boost/beast/websocket.hpp>
#include <IWebSocket.h>
#include <thread>

namespace Microsoft {
namespace React {
namespace Test {

class WebSocketSession : std::enable_shared_from_this<WebSocketSession>
{
  boost::asio::io_context& m_context;
  boost::beast::websocket::stream<boost::asio::ip::tcp::socket> m_stream;
  boost::asio::strand<boost::asio::io_context::executor_type> m_strand;
  boost::beast::multi_buffer m_buffer;

  std::function<void()> m_connectHandler;
  std::function<void()> m_pingHandler;
  std::function<void(std::size_t)> m_writeHandler;
  std::function<void(std::size_t, const std::string&)> m_readHandler;
  std::function<void(IWebSocket::CloseCode, const std::string&)> m_closeHandler;
  std::function<void(IWebSocket::Error&&)> m_errorHandler;

public:
  WebSocketSession(boost::asio::io_context& context);//TODO: Remove?
  WebSocketSession(boost::asio::io_context& context, boost::asio::ip::tcp::socket socket);

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

  void Accept();

  void OnAccept(boost::system::error_code ec);

public:
  WebSocketServer(std::uint16_t port);

  void Start();
};

} } } // Microsoft::React::Test
