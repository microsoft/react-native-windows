// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"
#include "Server.h"
#include "Server.g.cpp"

using namespace winrt::Windows::Foundation;
using namespace winrt::Windows::Networking;
using namespace winrt::Windows::Networking::Sockets;
using namespace winrt::Windows::Storage::Streams;

namespace winrt::NodeRpc::implementation {
Server::Server(const winrt::NodeRpc::Handler &handler) : m_requestProcessor(handler) {}

IAsyncAction Server::ProcessAllClientRequests(uint16_t port, Windows::Foundation::TimeSpan pollInterval) noexcept {
  auto weakThis = get_weak();
  auto cancellationToken = co_await get_cancellation_token();

  while (!cancellationToken()) {
    try {
      auto strongThis = weakThis.get();
      if (!strongThis) {
        co_return;
      }

      // FUTURE: We should share lessthrow_await_adapter and use it here/below
      co_await strongThis->m_socket.ConnectAsync(HostName(L"127.0.0.1"), winrt::to_hstring(port));
      break;
    } catch (const winrt::hresult_error &ex) {
      auto socketError = SocketError::GetStatus(ex.code());
      if (!IsPollContinueCondition(socketError)) {
        throw;
      }
    }

    winrt::apartment_context context;
    co_await pollInterval;
    co_await context;
  }

  if (!cancellationToken()) {
    if (auto strongThis = weakThis.get()) {
      auto pumpRequestsCoro = strongThis->PumpRequests();
      cancellationToken.callback([pumpRequestsCoro]() noexcept { pumpRequestsCoro.Cancel(); });
      co_await pumpRequestsCoro;
    }
  }
}

IAsyncAction Server::PumpRequests() noexcept {
  DataReader reader(m_socket.InputStream());
  reader.ByteOrder(ByteOrder::LittleEndian);
  reader.UnicodeEncoding(UnicodeEncoding::Utf8);

  auto weakThis = get_weak();
  auto cancellationToken = co_await get_cancellation_token();

  while (!cancellationToken()) {
    try {
      co_await reader.LoadAsync(sizeof(uint32_t));
      auto messageSize = reader.ReadUInt32();
      co_await reader.LoadAsync(messageSize);
      auto message = reader.ReadString(messageSize);

      if (auto strongThis = weakThis.get()) {
        if (!cancellationToken()) {
          strongThis->m_requestProcessor.HandleRequest(message, strongThis->m_socket.OutputStream());
        }
      }

    } catch (const winrt::hresult_error ex) {
      // Connection gone
      auto socketError = SocketError::GetStatus(ex.code());
      if (IsExpectedConnectionEnd(socketError)) {
        co_return;
      }

      // TODO do we need to handle stream closed separately or is it propagated as socket error?

      // Unexpected error. Throw to noexcept boundary to crash
      throw;
    }
  }
}

bool Server::IsPollContinueCondition(Windows::Networking::Sockets::SocketErrorStatus socketError) noexcept {
  switch (socketError) {
    case SocketErrorStatus::ConnectionTimedOut:
    case SocketErrorStatus::ConnectionRefused:
    case SocketErrorStatus::HostIsDown:
    case SocketErrorStatus::HostNotFound:
      return true;

    default:
      return false;
  }
}

bool Server::IsExpectedConnectionEnd(Windows::Networking::Sockets::SocketErrorStatus socketError) noexcept {
  switch (socketError) {
    case SocketErrorStatus::OperationAborted:
    case SocketErrorStatus::ConnectionResetByPeer:
    case SocketErrorStatus::SoftwareCausedConnectionAbort:
      return true;

    default:
      return false;
  }
}

} // namespace winrt::NodeRpc::implementation
