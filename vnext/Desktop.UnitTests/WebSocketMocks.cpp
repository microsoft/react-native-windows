#include "WebSocketMocks.h"

using std::exception;
using std::function;
using std::string;

namespace Microsoft::React::Test {

MockWebSocketResource::~MockWebSocketResource() {}

#pragma region IWebSocketResource overrides

void MockWebSocketResource::Connect(const Protocols &protocols, const Options &options) noexcept /*override*/
{
  if (Mocks.Connect)
    return Mocks.Connect(protocols, options);
}

void MockWebSocketResource::Ping() noexcept /*override*/
{
  if (Mocks.Connect)
    return Mocks.Ping();
}

void MockWebSocketResource::Send(string &&message) noexcept /*override*/
{
  if (Mocks.Send)
    return Mocks.Send(std::move(message));
}

void MockWebSocketResource::SendBinary(string &&message) noexcept /*override*/
{
  if (Mocks.SendBinary)
    return Mocks.SendBinary(std::move(message));
}

void MockWebSocketResource::Close(CloseCode code, const string &reason) noexcept /*override*/
{
  if (Mocks.Close)
    return Mocks.Close(code, reason);
}

IWebSocketResource::ReadyState MockWebSocketResource::GetReadyState() const noexcept /*override*/
{
  if (Mocks.GetReadyState)
    return Mocks.GetReadyState();

  return ReadyState::Connecting;
}

void MockWebSocketResource::SetOnConnect(function<void()> &&handler) noexcept /*override*/
{
  if (Mocks.SetOnConnect)
    return SetOnConnect(std::move(handler));

  m_connectHandler = std::move(handler);
}

void MockWebSocketResource::SetOnPing(function<void()> &&handler) noexcept /*override*/
{
  if (Mocks.SetOnPing)
    return Mocks.SetOnPing(std::move(handler));

  m_pingHandler = std::move(handler);
}

void MockWebSocketResource::SetOnSend(function<void(size_t)> &&handler) noexcept /*override*/
{
  if (Mocks.SetOnSend)
    return Mocks.SetOnSend(std::move(handler));

  m_writeHandler = std::move(handler);
}

void MockWebSocketResource::SetOnMessage(function<void(size_t, const string &, bool)> &&handler) noexcept /*override*/
{
  if (Mocks.SetOnMessage)
    return Mocks.SetOnMessage(std::move(handler));

  m_readHandler = std::move(handler);
}

void MockWebSocketResource::SetOnClose(function<void(CloseCode, const string &)> &&handler) noexcept /*override*/
{
  if (Mocks.SetOnClose)
    return Mocks.SetOnClose(std::move(handler));

  m_closeHandler = std::move(handler);
}

void MockWebSocketResource::SetOnError(function<void(Error &&)> &&handler) noexcept /*override*/
{
  if (Mocks.SetOnError)
    return Mocks.SetOnError(std::move(handler));

  m_errorHandler = std::move(handler);
}

#pragma endregion IWebSocketResource overrides

void MockWebSocketResource::OnConnect() {
  if (m_connectHandler)
    m_connectHandler();
}

void MockWebSocketResource::OnPing() {
  if (m_pingHandler)
    m_pingHandler();
}

void MockWebSocketResource::OnSend(size_t size) {
  if (m_writeHandler)
    m_writeHandler(size);
}

void MockWebSocketResource::OnMessage(size_t size, const string &message, bool isBinary) {
  if (m_readHandler)
    m_readHandler(size, message, isBinary);
}

void MockWebSocketResource::OnClose(CloseCode code, const string &reason) {
  if (m_closeHandler)
    m_closeHandler(code, reason);
}

void MockWebSocketResource::OnError(Error &&error) {
  if (m_errorHandler)
    m_errorHandler(std::move(error));
}

} // namespace Microsoft::React::Test
