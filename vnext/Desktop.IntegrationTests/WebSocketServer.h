#pragma once

#include <boost/beast/websocket.hpp>
#include <thread>

namespace Microsoft {
namespace React {
namespace Test {

class WebSocketSession : std::enable_shared_from_this<WebSocketSession>
{
  boost::asio::io_context& m_context;

public:
  WebSocketSession(boost::asio::io_context& context);
};

class WebSocketServer : public std::enable_shared_from_this<WebSocketServer>
{
  std::thread m_contextThread;
  boost::asio::io_context m_context;
  boost::asio::ip::tcp::acceptor m_acceptor;
  boost::asio::ip::tcp::socket m_socket;

public:
  WebSocketServer(std::uint16_t port);

  void Start();
};

} } } // Microsoft::React::Test
