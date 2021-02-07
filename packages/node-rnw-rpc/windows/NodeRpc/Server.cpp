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
  auto weakServer = get_weak();
  auto cancellationToken = co_await get_cancellation_token();

  while (!cancellationToken()) {
    try {
      auto strongServer = weakServer.get();
      if (!strongServer) {
        co_return;
      }

      // FUTURE: We should share lessthrow_await_adapter from outside of vnext
      // and use it here and below to avoid exceptions during expected control flow.
      co_await strongServer->m_socket.ConnectAsync(HostName(L"127.0.0.1"), winrt::to_hstring(port));
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

  IAsyncAction pumpRequestsCoro(nullptr);

  // Make sure to let the server die while pumping. Do not keep a strong ref
  // after starting the coroutine
  if (auto strongServer = weakServer.get()) {
    if (!cancellationToken()) {
      pumpRequestsCoro = strongServer->PumpRequests();
    }
  }

  if (pumpRequestsCoro) {
    cancellationToken.callback([pumpRequestsCoro]() noexcept { pumpRequestsCoro.Cancel(); });
    co_await pumpRequestsCoro;
  }
}

IAsyncAction Server::PumpRequests() noexcept {
  DataReader reader(m_socket.InputStream());
  reader.ByteOrder(ByteOrder::LittleEndian);
  reader.UnicodeEncoding(UnicodeEncoding::Utf8);

  auto weakServer = get_weak();
  auto cancellationToken = co_await get_cancellation_token();

  while (!cancellationToken()) {
    try {
      co_await reader.LoadAsync(sizeof(uint32_t));
      auto messageSize = reader.ReadUInt32();
      co_await reader.LoadAsync(messageSize);
      auto message = reader.ReadString(messageSize);

      if (auto strongServer = weakServer.get()) {
        if (!cancellationToken()) {
          strongServer->m_requestProcessor.HandleRequest(message, strongServer->m_socket.OutputStream());
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
