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

using EmptyResponse = boost::beast::http::response<boost::beast::http::empty_body>;

using FileResponse = boost::beast::http::response<boost::beast::http::file_body>;

using StringResponse = boost::beast::http::response<boost::beast::http::string_body>;

class ResponseWrapper
{
public:
  enum class ResponseType : size_t { Empty, Dynamic, File, String };

private:
  std::shared_ptr<void> m_response;
  ResponseType m_type;

public:
  ResponseWrapper(DynamicResponse&& response);

  ResponseWrapper(EmptyResponse&& response);

  ResponseWrapper(FileResponse&& response);

  ResponseWrapper(StringResponse&& response);

  ResponseWrapper(DynamicResponse const& response);

  ResponseWrapper(EmptyResponse const& response);

  ResponseWrapper(StringResponse const& response);

  ResponseWrapper(ResponseWrapper&&) = default;

  std::shared_ptr<void> Response();

  ResponseType Type();
};

#pragma region Utility functions

boost::beast::multi_buffer CreateStringResponseBody(std::string&& content);

#pragma endregion Utility functions

struct HttpCallbacks
{
  std::function<void()> OnResponseSent;
  std::function<void()> OnRequest;
  std::function<ResponseWrapper(const DynamicRequest &)> OnGet;
  std::function<ResponseWrapper(const DynamicRequest &)> OnPost;
  std::function<ResponseWrapper(const DynamicRequest &)> OnPatch;
  std::function<ResponseWrapper(const DynamicRequest &)> OnOptions;

  std::function<ResponseWrapper(const DynamicRequest &)> OnConnect;
  std::function<ResponseWrapper(const DynamicRequest &)> OnTrace;

  //Not supported by Boost/Beast
  #if 0
  std::function<ResponseWrapper(const DynamicRequest &)> OnTrack;   
  #endif // 0
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
  std::shared_ptr<ResponseWrapper> m_response; // Generic response
  HttpCallbacks& m_callbacks;
  std::function<void(Microsoft::React::Test::ResponseWrapper&&)> m_sendLambda;

  void Read();
  void Respond();
  void Close();

  void OnRead(boost::system::error_code ec, std::size_t transferred);
  void OnWrite(bool close, boost::system::error_code ec, std::size_t transferred);

 public:
  HttpSession(boost::asio::ip::tcp::socket&& socket, HttpCallbacks &callbacks,
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
  size_t m_ioThreadCount;
  std::vector<std::thread> m_ioThreads;
  boost::asio::io_context m_ioContext;
  boost::asio::ip::tcp::acceptor m_acceptor;
  HttpCallbacks m_callbacks;

  void OnAccept(boost::system::error_code ec, boost::asio::ip::tcp::socket socket);

 public:
  ///
  // address - Valid IP address string (i.e. "127.0.0.1).
  // port    - TCP port number (i.e. 80).
  ///
  HttpServer(std::string &&address, uint16_t port, int concurrency = 1);
  HttpServer(uint16_t port, int concurrency = 1);

  ~HttpServer();

  void Accept();
  void Start();
  void Stop();
  void Abort();//TODO: Remove?

  HttpCallbacks& Callbacks();
};

} // namespace Microsoft::React::Test
