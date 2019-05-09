#include "WebSocketServer.h"

using boost::asio::ip::tcp;
using boost::system::error_code;

namespace Microsoft {
namespace React {
namespace Test {

#pragma region WebSocketServer

WebSocketServer::WebSocketServer(uint16_t port)
  : m_acceptor{ m_context }
  , m_socket{ m_context }
{
  tcp::endpoint ep{boost::asio::ip::make_address("0.0.0.0"), port };
  error_code ec;

  m_acceptor.open(ep.protocol(), ec);
  if (ec)
  {
    return; //
  }

  m_acceptor.set_option(boost::asio::socket_base::reuse_address(true), ec);
  if (ec)
  {
    return; //
  }

  m_acceptor.bind(ep, ec);
  if (ec)
  {
    return; //
  }

  m_acceptor.listen(boost::asio::socket_base::max_listen_connections, ec);
  if (ec)
  {
    return; //
  }
}

#pragma endregion // WebSocketServer

} } } // Microsoft::React::Test
