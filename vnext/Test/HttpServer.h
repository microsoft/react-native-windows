// clang-format off
#pragma once

#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/strand.hpp>
#include <boost/beast/core/flat_buffer.hpp>
#include <boost/beast/core/tcp_stream.hpp>
#include <boost/beast/http.hpp>

#include <thread>

namespace Microsoft::React::Test
{

using DynamicRequest = boost::beast::http::request<boost::beast::http::dynamic_body>;
using DynamicResponse = boost::beast::http::response<boost::beast::http::dynamic_body>;

#pragma region Utility functions

boost::beast::multi_buffer CreateStringResponseBody(std::string&& content);

#pragma endregion Utility functions

struct HttpCallbacks
{
  std::function<void()> OnResponseSent;
  std::function<DynamicResponse(const DynamicRequest &)> OnGet;
  std::function<DynamicResponse(const DynamicRequest &)> OnOptions;
  std::function<void()> OnRequest;
};

///
// Represents a client session in within an owning server instance.
// Generates and submits the appropriate HTTP response.
///
class HttpSession : public std::enable_shared_from_this<HttpSession>
{
  boost::beast::tcp_stream m_stream;
  boost::beast::flat_buffer m_buffer;
  DynamicRequest m_request;
  std::shared_ptr<DynamicResponse> m_response; // Generic response
  HttpCallbacks& m_callbacks;
  boost::asio::strand<boost::asio::io_context::executor_type>& m_readStrand;
  boost::asio::strand<boost::asio::io_context::executor_type>& m_writeStrand;
  boost::asio::io_context& m_context;

  void Read();
  void Respond();
  void Close();

  void OnRead(boost::system::error_code ec, std::size_t transferred);
  void OnWrite(bool close, boost::system::error_code ec, std::size_t transferred);

 public:
  HttpSession(boost::asio::ip::tcp::socket&& socket, HttpCallbacks &callbacks,
    boost::asio::strand<boost::asio::io_context::executor_type>& readStrand,
    boost::asio::strand<boost::asio::io_context::executor_type>& writeStrand,
    boost::asio::io_context& ioContext
  );

  ~HttpSession();

  void Start();
};

///
// Represents an HTTP server endpoint (IP:PORT).
// Accepts client connections and dispatches sessions for each incoming
// connection.
///
class HttpServer : public std::enable_shared_from_this<HttpServer>
{
  std::vector<std::thread> m_contextThreads;
  boost::asio::io_context m_context;
  boost::asio::strand<boost::asio::io_context::executor_type> m_readStrand;//EXPERIMENTAL
  boost::asio::strand<boost::asio::io_context::executor_type> m_writeStrand;//EXPERIMENTAL
  boost::asio::ip::tcp::acceptor m_acceptor;
  HttpCallbacks m_callbacks;

  void OnAccept(boost::system::error_code ec, boost::asio::ip::tcp::socket socket);

 public:
  ///
  // address - Valid IP address string (i.e. "127.0.0.1).
  // port    - TCP port number (i.e. 80).
  ///
  HttpServer(std::string &&address, std::uint16_t port);

  ~HttpServer();

  void Accept();
  void Start();
  void Stop();
  void Abort();

  ///
  // Callback to invoke after a successful response is sent.
  ///
  void SetOnResponseSent(std::function<void()> &&handler) noexcept;

  ///
  // Function that creates an HTTP response to send to the client on GET
  // requests.
  ///
  void SetOnGet(std::function<DynamicResponse(const DynamicRequest &)> &&handler) noexcept;

  ///
  // Function that creates an HTTP response to send to the client on GET
  // requests.
  ///
  void SetOnOptions(std::function<DynamicResponse(const DynamicRequest &)> &&handler) noexcept;
};

} // namespace Microsoft::React::Test
