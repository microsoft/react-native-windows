#include "WebSocketMocks.h"

using std::exception;
using std::function;
using std::string;

namespace Microsoft::React::Test {

MockWebSocketResource::~MockWebSocketResource() {}

#pragma region IWebSocketResource overrides

void MockWebSocketResource::Connect(const Protocols &protocols, const Options &options) /*override*/
{
  if (Mocks.Connect)
    return Mocks.Connect(protocols, options);

  throw exception("Not implemented");
}

void MockWebSocketResource::Ping() /*override*/
{
  if (Mocks.Connect)
    return Mocks.Ping();

  throw exception("Not implemented");
}

void MockWebSocketResource::Send(const string &message) /*override*/
{
  if (Mocks.Send)
    return Mocks.Send(message);

  throw exception("Not implemented");
}

void MockWebSocketResource::SendBinary(const string &message) /*override*/
{
  if (Mocks.SendBinary)
    return Mocks.SendBinary(message);

  throw exception("Not implemented");
}

void MockWebSocketResource::Close(CloseCode code, const string &reason) /*override*/
{
  if (Mocks.Close)
    return Mocks.Close(code, reason);

  throw exception("Not implemented");
}

IWebSocketResource::ReadyState MockWebSocketResource::GetReadyState() const /*override*/
{
  if (Mocks.GetReadyState)
    return Mocks.GetReadyState();

  throw exception("Not implemented");
}

void MockWebSocketResource::SetOnConnect(function<void()> &&handler) /*override*/
{
  if (Mocks.SetOnConnect)
    return SetOnConnect(std::move(handler));

  m_connectHandler = std::move(handler);
}

void MockWebSocketResource::SetOnPing(function<void()> &&handler) /*override*/
{
  if (Mocks.SetOnPing)
    return Mocks.SetOnPing(std::move(handler));

  m_pingHandler = std::move(handler);
}

void MockWebSocketResource::SetOnSend(function<void(size_t)> &&handler) /*override*/
{
  if (Mocks.SetOnSend)
    return Mocks.SetOnSend(std::move(handler));

  m_writeHandler = std::move(handler);
}

void MockWebSocketResource::SetOnMessage(function<void(size_t, const string &)> &&handler) /*override*/
{
  if (Mocks.SetOnMessage)
    return Mocks.SetOnMessage(std::move(handler));

  m_readHandler = std::move(handler);
}

void MockWebSocketResource::SetOnClose(function<void(CloseCode, const string &)> &&handler) /*override*/
{
  if (Mocks.SetOnClose)
    return Mocks.SetOnClose(std::move(handler));

  m_closeHandler = std::move(handler);
}

void MockWebSocketResource::SetOnError(function<void(Error &&)> &&handler) /*override*/
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

void MockWebSocketResource::OnMessage(size_t size, const string &message) {
  if (m_readHandler)
    m_readHandler(size, message);
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
