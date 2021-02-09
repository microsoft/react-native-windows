// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"
#include "Server.h"
#include "Server.g.cpp"
#include "CppWinrtLessExceptions.h"

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
    auto strongServer = weakServer.get();
    if (!strongServer) {
      co_return;
    }

    strongServer->m_socket = StreamSocket();
    auto connection = strongServer->m_socket.ConnectAsync(HostName(L"127.0.0.1"), winrt::to_hstring(port));
    co_await lessthrow_await_adapter<IAsyncAction>{connection};

    auto hr = connection.ErrorCode();
    if (SUCCEEDED(hr)) {
      break;
    }

    if (!IsPollContinueCondition(hr)) {
      throw winrt::hresult_error(hr);
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
    auto loadSize = reader.LoadAsync(sizeof(uint32_t));
    co_await lessthrow_await_adapter<DataReaderLoadOperation>{loadSize};
    auto loadSizeHr = loadSize.ErrorCode();

    if (FAILED(loadSizeHr)) {
      if (IsExpectedConnectionEnd(loadSizeHr)) {
        co_return;
      } else {
        throw winrt::hresult_error(loadSizeHr);
      }
    }

    auto messageSize = reader.ReadUInt32();

    auto loadMessage = reader.LoadAsync(messageSize);
    co_await lessthrow_await_adapter<DataReaderLoadOperation>{loadMessage};
    auto loadMessageHr = loadMessage.ErrorCode();

    if (FAILED(loadMessageHr)) {
      if (IsExpectedConnectionEnd(loadMessageHr)) {
        co_return;
      } else {
        throw winrt::hresult_error(loadMessageHr);
      }
    }

    auto message = reader.ReadString(messageSize);

    if (auto strongServer = weakServer.get()) {
      if (!cancellationToken()) {
        strongServer->m_requestProcessor.HandleRequest(message, strongServer->m_socket.OutputStream());
      }
    }
  }
}

bool Server::IsPollContinueCondition(winrt::hresult hr) noexcept {
  auto socketError = SocketError::GetStatus(hr);

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

bool Server::IsExpectedConnectionEnd(winrt::hresult hr) noexcept {
  auto socketError = SocketError::GetStatus(hr);

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
