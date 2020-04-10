#include "WebSocketMocks.h"

using std::function;
using std::string;

namespace Microsoft::React::Test {
#pragma region IWebSocketResource overrides

void MockWebSocketResource::Connect(const Protocols &, const Options &) /*override*/
{
  m_onConnect();
}

void MockWebSocketResource::Ping() /*override*/ {}

void MockWebSocketResource::Send(const string &) /*override*/ {}

void MockWebSocketResource::SendBinary(const string &) /*override*/ {}

void MockWebSocketResource::Close(CloseCode, const string &) /*override*/ {}

IWebSocketResource::ReadyState MockWebSocketResource::GetReadyState() const /*override*/
{
  return ReadyState::Open;
}

void MockWebSocketResource::SetOnConnect(function<void()> &&onConnect) /*override*/
{
  m_onConnect = std::move(onConnect);
}

void MockWebSocketResource::SetOnPing(function<void()> &&) /*override*/ {}

void MockWebSocketResource::SetOnSend(function<void(size_t)> &&) /*override*/ {}

void MockWebSocketResource::SetOnMessage(function<void(size_t, const string &)> &&) /*override*/ {}

void MockWebSocketResource::SetOnClose(function<void(CloseCode, const string &)> &&) /*override*/ {}

void MockWebSocketResource::SetOnError(function<void(Error &&)> &&) /*override*/ {}

#pragma endregion IWebSocketResource overrides
} // namespace Microsoft::React::Test
