#include "WebSocketMocks.h"

using std::exception;
using std::function;
using std::string;

namespace Microsoft::React::Test {

MockWebSocketResource::~MockWebSocketResource() {}

#pragma region IWebSocketResource overrides

void MockWebSocketResource::Connect(const Protocols &protocols, const Options &options) /*override*/
{
  if (ConnectMock)
    return ConnectMock(protocols, options);

  m_connectHandler();
}

void MockWebSocketResource::Ping() /*override*/
{
  if (PingMock)
    return PingMock();

  // m_pingHandler();
}

void MockWebSocketResource::Send(const string &message) /*override*/
{
  if (SendMock)
    return SendMock(message);

  // m_writeHandler(message.length());
}

void MockWebSocketResource::SendBinary(const string &message) /*override*/
{
  if (SendBinaryMock)
    return SendBinaryMock(message);

  // m_writeHandler(message.length());
}

void MockWebSocketResource::Close(CloseCode code, const string &reason) /*override*/
{
  if (CloseMock)
    return CloseMock(code, reason);

  // m_closeHandler(code, reason);
}

IWebSocketResource::ReadyState MockWebSocketResource::GetReadyState() const /*override*/
{
  if (GetReadyStateMock)
    return GetReadyStateMock();

  throw exception("Not implemented");
}

void MockWebSocketResource::SetOnConnect(function<void()> &&handler) /*override*/
{
  if (SetOnConnectMock)
    return SetOnConnect(std::move(handler));

  m_connectHandler = std::move(handler);
}

void MockWebSocketResource::SetOnPing(function<void()> &&handler) /*override*/
{
  if (SetOnPingMock)
    return SetOnPingMock(std::move(handler));

  m_pingHandler = std::move(handler);
}

void MockWebSocketResource::SetOnSend(function<void(size_t)> &&handler) /*override*/
{
  if (SetOnSendMock)
    return SetOnSendMock(std::move(handler));

  m_writeHandler = std::move(handler);
}

void MockWebSocketResource::SetOnMessage(function<void(size_t, const string &)> &&handler) /*override*/
{
  if (SetOnMessageMock)
    return SetOnMessageMock(std::move(handler));

  m_readHandler = std::move(handler);
}

void MockWebSocketResource::SetOnClose(function<void(CloseCode, const string &)> &&handler) /*override*/
{
  if (SetOnCloseMock)
    return SetOnCloseMock(std::move(handler));

  m_closeHandler = std::move(handler);
}

void MockWebSocketResource::SetOnError(function<void(Error &&)> &&handler) /*override*/
{
  if (SetOnErrorMock)
    return SetOnErrorMock(std::move(handler));

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
