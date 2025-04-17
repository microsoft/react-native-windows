// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "WinRTWebSocketResource.h"

#include <Utilities.h>
#include <Utils/CppWinrtLessExceptions.h>
#include <Utils/WinRTConversions.h>

// Boost Libraries
#include <boost/algorithm/string.hpp>

// MSO
#include <dispatchQueue/dispatchQueue.h>

// Windows API
#include <windows.Networking.Sockets.h>
#include <windows.Storage.Streams.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Security.Cryptography.h>

using Microsoft::Common::Utilities::CheckedReinterpretCast;

using Mso::DispatchQueue;

using std::function;
using std::lock_guard;
using std::mutex;
using std::string;
using std::vector;

using winrt::fire_and_forget;
using winrt::hresult;
using winrt::hresult_error;
using winrt::hstring;
using winrt::resume_background;
using winrt::resume_on_signal;
using winrt::Windows::Foundation::IAsyncAction;
using winrt::Windows::Foundation::Uri;
using winrt::Windows::Networking::Sockets::IMessageWebSocket;
using winrt::Windows::Networking::Sockets::IMessageWebSocketMessageReceivedEventArgs;
using winrt::Windows::Networking::Sockets::IWebSocket;
using winrt::Windows::Networking::Sockets::IWebSocketClosedEventArgs;
using winrt::Windows::Networking::Sockets::MessageWebSocket;
using winrt::Windows::Networking::Sockets::SocketMessageType;
using winrt::Windows::Networking::Sockets::WebSocketClosedEventArgs;
using winrt::Windows::Security::Cryptography::CryptographicBuffer;
using winrt::Windows::Security::Cryptography::Certificates::ChainValidationResult;
using winrt::Windows::Storage::Streams::DataWriter;
using winrt::Windows::Storage::Streams::DataWriterStoreOperation;
using winrt::Windows::Storage::Streams::IDataReader;
using winrt::Windows::Storage::Streams::IDataWriter;
using winrt::Windows::Storage::Streams::UnicodeEncoding;

namespace {

///
/// Implements an awaiter for Mso::DispatchQueue
///
auto resume_in_queue(const DispatchQueue &queue) noexcept {
  struct awaitable {
    awaitable(const DispatchQueue &queue) noexcept : m_queue{queue} {}

    bool await_ready() const noexcept {
      return false;
    }

    void await_resume() const noexcept {}

    void await_suspend(winrt::impl::coroutine_handle<> resume) noexcept {
      m_callback = [context = resume.address()]() noexcept {
        winrt::impl::coroutine_handle<>::from_address(context)();
      };
      m_queue.Post(std::move(m_callback));
    }

   private:
    Mso::DispatchQueue m_queue;
    Mso::VoidFunctor m_callback;
  };

  return awaitable{queue};
} // resume_in_queue

DispatchQueue GetCurrentOrSerialQueue() noexcept {
  auto queue = DispatchQueue::CurrentQueue();
  if (!queue)
    queue = DispatchQueue::MakeSerialQueue();

  return queue;
}
} // namespace

namespace Microsoft::React::Networking {

#pragma region WinRTWebSocketResource2

WinRTWebSocketResource2::WinRTWebSocketResource2(
    IMessageWebSocket &&socket,
    IDataWriter &&writer,
    vector<ChainValidationResult> &&certExceptions,
    DispatchQueue callingQueue)
    : m_socket{std::move(socket)},
      m_writer(std::move(writer)),
      m_readyState{ReadyState::Connecting},
      m_connectPerformed{CreateEvent(/*attributes*/ nullptr, /*manual reset*/ true, /*state*/ false, /*name*/ nullptr)},
      m_callingQueue{callingQueue} {
  for (const auto &certException : certExceptions) {
    m_socket.Control().IgnorableServerCertificateErrors().Append(certException);
  }
}

// private
WinRTWebSocketResource2::WinRTWebSocketResource2(
    IMessageWebSocket &&socket,
    vector<ChainValidationResult> &&certExceptions)
    : WinRTWebSocketResource2(
          std::move(socket),
          DataWriter{socket.OutputStream()},
          std::move(certExceptions),
          GetCurrentOrSerialQueue()) {}

WinRTWebSocketResource2::WinRTWebSocketResource2(vector<ChainValidationResult> &&certExceptions)
    : WinRTWebSocketResource2(MessageWebSocket{}, std::move(certExceptions)) {}

WinRTWebSocketResource2::~WinRTWebSocketResource2() noexcept /*override*/
{}

void WinRTWebSocketResource2::Fail(string &&message, ErrorType type) noexcept {
  auto self = shared_from_this();

  self->m_backgroundQueue.Post([self, message = std::move(message), type]() {
    self->m_readyState = ReadyState::Closed;
    self->m_callingQueue.Post([self, message = std::move(message), type]() {
      if (self->m_errorHandler) {
        self->m_errorHandler({std::move(message), type});
      }
    });
  });
}

void WinRTWebSocketResource2::Fail(hresult &&error, ErrorType type) noexcept {
  Fail(Utilities::HResultToString(std::move(error)), type);
}

void WinRTWebSocketResource2::Fail(hresult_error const &error, ErrorType type) noexcept {
  Fail(Utilities::HResultToString(error), type);
}

void WinRTWebSocketResource2::OnMessageReceived(
    IMessageWebSocket const &,
    IMessageWebSocketMessageReceivedEventArgs const &args) {
  auto self = shared_from_this();
  string response;

  IDataReader reader{nullptr};
  // Use WinRT ABI to avoid throwing exceptions on expected code paths
  HRESULT hr =
      reinterpret_cast<ABI::Windows::Networking::Sockets::IMessageWebSocketMessageReceivedEventArgs *>(
          winrt::get_abi(args))
          ->GetDataReader(reinterpret_cast<ABI::Windows::Storage::Streams::IDataReader **>(winrt::put_abi(reader)));

  if (FAILED(hr)) {
    string errorMessage;
    ErrorType errorType;
    // See
    // https://docs.microsoft.com/uwp/api/windows.networking.sockets.messagewebsocketmessagereceivedeventargs.getdatareader?view=winrt-22621#remarks
    if (hr == WININET_E_CONNECTION_ABORTED) {
      errorMessage = "[0x80072EFE] Underlying TCP connection suddenly terminated";
      errorType = ErrorType::Connection;
      // Note: It is not clear whether all read-related errors should close the socket.
      Close(CloseCode::BadPayload, std::move(errorMessage));
    } else {
      errorMessage = Utilities::HResultToString(hr);
      errorType = ErrorType::Receive;
    }

    self->Fail(std::move(errorMessage), errorType);

    return;
  }

  try {
    auto len = reader.UnconsumedBufferLength();
    if (args.MessageType() == SocketMessageType::Utf8) {
      reader.UnicodeEncoding(UnicodeEncoding::Utf8);
      vector<uint8_t> data(len);
      reader.ReadBytes(data);

      response = string(CheckedReinterpretCast<char *>(data.data()), data.size());
    } else {
      auto buffer = reader.ReadBuffer(len);
      auto data = CryptographicBuffer::EncodeToBase64String(buffer);

      response = winrt::to_string(std::wstring_view(data));
    }
  } catch (hresult_error const &e) {
    return self->Fail(e, ErrorType::Receive);
  }

  // Posting inside try-catch block causes errors.
  self->m_callingQueue.Post([self, response = std::move(response), messageType = args.MessageType()]() {
    if (self->m_readHandler) {
      self->m_readHandler(response.length(), response, messageType == SocketMessageType::Binary);
    }
  });
}

void WinRTWebSocketResource2::OnClosed(IWebSocket const &sender, IWebSocketClosedEventArgs const &args) {
  auto self = shared_from_this();

  self->m_backgroundQueue.Post([self]() { self->m_readyState = ReadyState::Closed; });

  self->m_callingQueue.Post([self]() {
    if (self->m_closeHandler) {
      self->m_closeHandler(self->m_closeCode, self->m_closeReason);
    }
  });
}

fire_and_forget WinRTWebSocketResource2::PerformConnect(Uri &&uri) noexcept {
  auto self = shared_from_this();
  auto coUri = std::move(uri);

  co_await resume_in_queue(self->m_backgroundQueue);

  auto async = self->m_socket.ConnectAsync(coUri);
  co_await lessthrow_await_adapter<IAsyncAction>{async};

  co_await resume_in_queue(self->m_callingQueue);

  auto result = async.ErrorCode();

  try {
    if (result >= 0) { // Non-failing HRESULT
      co_await resume_in_queue(self->m_backgroundQueue);
      self->m_readyState = ReadyState::Open;

      co_await resume_in_queue(self->m_callingQueue);
      if (self->m_connectHandler) {
        self->m_connectHandler();
      }
    } else {
      self->Fail(std::move(result), ErrorType::Connection);
    }
  } catch (hresult_error const &e) {
    self->Fail(e, ErrorType::Connection);
  } catch (std::exception const &e) {
    self->Fail(e.what(), ErrorType::Connection);
  }

  SetEvent(self->m_connectPerformed.get());
}

fire_and_forget WinRTWebSocketResource2::PerformClose() noexcept {
  auto self = shared_from_this();

  co_await resume_on_signal(self->m_connectPerformed.get());

  co_await resume_in_queue(self->m_backgroundQueue);

  // See https://developer.mozilla.org/en-US/docs/Web/API/WebSocket/close
  co_await self->SendPendingMessages();

  try {
    self->m_socket.Close(static_cast<uint16_t>(m_closeCode), winrt::to_hstring(m_closeReason));
    self->m_readyState = ReadyState::Closing;
  } catch (winrt::hresult_invalid_argument const &e) {
    Fail(e, ErrorType::Close);
  } catch (hresult_error const &e) {
    Fail(e, ErrorType::Close);
  } catch (const std::exception &e) {
    Fail(e.what(), ErrorType::Close);
  }
}

fire_and_forget WinRTWebSocketResource2::PerformWrite(string &&message, bool isBinary) noexcept {
  auto self = shared_from_this();
  string coMessage = std::move(message);

  co_await resume_in_queue(self->m_backgroundQueue); // Ensure writes happen sequentially
  self->m_outgoingMessages.emplace(std::move(coMessage), isBinary);

  co_await resume_on_signal(self->m_connectPerformed.get());

  co_await resume_in_queue(self->m_backgroundQueue);

  co_await self->SendPendingMessages();
}

IAsyncAction WinRTWebSocketResource2::EnqueueWrite(string &&message, bool isBinary) noexcept {
  auto self = shared_from_this();
  // TODO: Review captures
  return m_sequencer.QueueTaskAsync([=]()
  {
    return DequeueWrite(string{message}, isBinary);
    //return IAsyncAction{};
  });
}

IAsyncAction WinRTWebSocketResource2::DequeueWrite(string &&message, bool isBinary) noexcept
{
  auto self = shared_from_this();

  size_t length = 0;
  string messageLocal = std::move(message);
  bool isBinaryLocal = isBinary;
  try {
    //std::tie(messageLocal, isBinaryLocal) = self->m_outgoingMessages.front();
    //self->m_outgoingMessages.pop();
    if (isBinaryLocal) {
      self->m_socket.Control().MessageType(SocketMessageType::Binary);

      auto buffer = CryptographicBuffer::DecodeFromBase64String(winrt::to_hstring(messageLocal));
      if (buffer) {
        length = buffer.Length();
        self->m_writer.WriteBuffer(buffer);
      }
    } else {
      self->m_socket.Control().MessageType(SocketMessageType::Utf8);

      length = messageLocal.size();
      winrt::array_view<const uint8_t> view(
          CheckedReinterpretCast<const uint8_t *>(messageLocal.c_str()),
          CheckedReinterpretCast<const uint8_t *>(messageLocal.c_str()) + messageLocal.length());
      self->m_writer.WriteBytes(view);
    }
  } catch (hresult_error const &e) { // TODO: Remove after fixing unit tests exceptions.
    self->Fail(e, ErrorType::Send);
    //co_return;
    return;
  } catch (const std::exception &e) {
    self->Fail(e.what(), ErrorType::Send);
    //co_return;
    return;
  }

  auto async = self->m_writer.StoreAsync();
  co_await lessthrow_await_adapter<DataWriterStoreOperation>{async};

  auto result = async.ErrorCode();
  if (result < 0) {
    Fail(std::move(result), ErrorType::Send);
    co_return;
  }
}

IAsyncAction WinRTWebSocketResource2::SendPendingMessages() noexcept {
  // Enforcing execution in the background queue.
  // Awaiting of this coroutine will schedule its execution in the thread pool, ignoring the intended dispatch queue.
  co_await resume_in_queue(m_backgroundQueue);

  auto self = shared_from_this();

  while (!self->m_outgoingMessages.empty()) {
    if (self->m_readyState != ReadyState::Open) {
      co_return;
    }

    size_t length = 0;
    string messageLocal;
    bool isBinaryLocal;
    try {
      std::tie(messageLocal, isBinaryLocal) = self->m_outgoingMessages.front();
      self->m_outgoingMessages.pop();
      if (isBinaryLocal) {
        self->m_socket.Control().MessageType(SocketMessageType::Binary);

        auto buffer = CryptographicBuffer::DecodeFromBase64String(winrt::to_hstring(messageLocal));
        if (buffer) {
          length = buffer.Length();
          self->m_writer.WriteBuffer(buffer);
        }
      } else {
        self->m_socket.Control().MessageType(SocketMessageType::Utf8);

        length = messageLocal.size();
        winrt::array_view<const uint8_t> view(
            CheckedReinterpretCast<const uint8_t *>(messageLocal.c_str()),
            CheckedReinterpretCast<const uint8_t *>(messageLocal.c_str()) + messageLocal.length());
        self->m_writer.WriteBytes(view);
      }
    } catch (hresult_error const &e) { // TODO: Remove after fixing unit tests exceptions.
      self->Fail(e, ErrorType::Send);
      co_return;
    } catch (const std::exception &e) {
      self->Fail(e.what(), ErrorType::Send);
      co_return;
    }

    auto async = self->m_writer.StoreAsync();
    co_await lessthrow_await_adapter<DataWriterStoreOperation>{async};

    auto result = async.ErrorCode();
    if (result < 0) {
      Fail(std::move(result), ErrorType::Send);
      co_return;
    }
  }
}

#pragma region IWebSocketResource

void WinRTWebSocketResource2::Connect(string &&url, const Protocols &protocols, const Options &options) noexcept {
  // Register MessageReceived BEFORE calling Connect
  // https://learn.microsoft.com/en-us/uwp/api/windows.networking.sockets.messagewebsocket.messagereceived?view=winrt-22621
  m_socket.MessageReceived([self = shared_from_this()](
                               IMessageWebSocket const &sender, IMessageWebSocketMessageReceivedEventArgs const &args) {
    self->OnMessageReceived(sender, args);
  });

  m_socket.Closed([self = shared_from_this()](IWebSocket const &sender, IWebSocketClosedEventArgs const &args) {
    self->OnClosed(sender, args);
  });

  auto supportedProtocols = m_socket.Control().SupportedProtocols();
  for (const auto &protocol : protocols) {
    supportedProtocols.Append(winrt::to_hstring(protocol));
  }

  Uri uri{nullptr};
  bool hasOriginHeader{false};
  try {
    uri = Uri{winrt::to_hstring(url)};

    for (const auto &header : options) {
      m_socket.SetRequestHeader(header.first, winrt::to_hstring(header.second));
      if (boost::iequals(header.first, L"Origin")) {
        hasOriginHeader = true;
      }
    }

    // #12626 - If Origin header is not provided, set to connect endpoint.
    if (!hasOriginHeader) {
      auto scheme = uri.SchemeName();
      auto host = uri.Host();
      auto port = uri.Port();

      if (scheme == L"ws") {
        scheme = L"http";
      } else if (scheme == L"wss") {
        scheme = L"https";
      }

      // Only add a port if a port is defined.
      hstring originPort = port != 0 ? L":" + winrt::to_hstring(port) : L"";
      auto origin = hstring{scheme + L"://" + host + originPort};

      m_socket.SetRequestHeader(L"Origin", std::move(origin));
    }
  } catch (hresult_error const &e) {
    Fail(e, ErrorType::Connection);

    SetEvent(m_connectPerformed.get());

    return;
  }

  PerformConnect(std::move(uri));
}

void WinRTWebSocketResource2::Ping() noexcept {}

void WinRTWebSocketResource2::Send(string &&message) noexcept {
  PerformWrite(std::move(message), false);
}

void WinRTWebSocketResource2::SendBinary(string &&base64String) noexcept {
  PerformWrite(std::move(base64String), true);
}

void WinRTWebSocketResource2::Close(CloseCode code, const string &reason) noexcept {
  m_closeCode = code;
  m_closeReason = reason;
  PerformClose();
}

IWebSocketResource::ReadyState WinRTWebSocketResource2::GetReadyState() const noexcept {
  return m_readyState;
}

void WinRTWebSocketResource2::SetOnConnect(function<void()> &&handler) noexcept {
  m_connectHandler = std::move(handler);
}

void WinRTWebSocketResource2::SetOnPing(function<void()> && /*handler*/) noexcept {}

void WinRTWebSocketResource2::SetOnSend(function<void(size_t)> && /*handler*/) noexcept {}

void WinRTWebSocketResource2::SetOnMessage(function<void(size_t, const string &, bool isBinary)> &&handler) noexcept {
  m_readHandler = std::move(handler);
}

void WinRTWebSocketResource2::SetOnClose(function<void(CloseCode, const string &)> &&handler) noexcept {
  m_closeHandler = std::move(handler);
}

void WinRTWebSocketResource2::SetOnError(function<void(Error &&)> &&handler) noexcept {
  m_errorHandler = std::move(handler);
}

#pragma endregion IWebSocketResource

#pragma endregion WinRTWebSocketResource2

#pragma region Legacy resource
// private
WinRTWebSocketResource::WinRTWebSocketResource(
    IMessageWebSocket &&socket,
    vector<ChainValidationResult> &&certExceptions)
    : WinRTWebSocketResource(std::move(socket), DataWriter{socket.OutputStream()}, std::move(certExceptions)) {}

WinRTWebSocketResource::WinRTWebSocketResource(
    IMessageWebSocket &&socket,
    IDataWriter &&writer,
    vector<ChainValidationResult> &&certExceptions)
    : m_socket{std::move(socket)}, m_writer{std::move(writer)} {
  for (const auto &certException : certExceptions) {
    m_socket.Control().IgnorableServerCertificateErrors().Append(certException);
  }
}

WinRTWebSocketResource::WinRTWebSocketResource(vector<ChainValidationResult> &&certExceptions)
    : WinRTWebSocketResource(MessageWebSocket{}, std::move(certExceptions)) {}

WinRTWebSocketResource::~WinRTWebSocketResource() noexcept /*override*/
{
  Close(CloseCode::GoingAway, "Disposed");

  m_closePerformed.Wait();
}

#pragma region Private members

IAsyncAction WinRTWebSocketResource::PerformConnect(Uri &&uri) noexcept {
  auto self = shared_from_this();
  auto coUri = std::move(uri);

  co_await resume_background();

  try {
    auto async = self->m_socket.ConnectAsync(coUri);

    co_await lessthrow_await_adapter<IAsyncAction>{async};

    auto result = async.ErrorCode();
    if (result >= 0) {
      self->m_readyState = ReadyState::Open;
      if (self->m_connectHandler) {
        self->m_connectHandler();
      }
    } else {
      if (self->m_errorHandler) {
        self->m_errorHandler({Utilities::HResultToString(std::move(result)), ErrorType::Connection});
      }
    }
  } catch (hresult_error const &e) {
    if (self->m_errorHandler) {
      self->m_errorHandler({Utilities::HResultToString(e), ErrorType::Connection});
    }
  }

  SetEvent(self->m_connectPerformed.get());
  self->m_connectPerformedPromise.set_value();
  self->m_connectRequested = false;
}

fire_and_forget WinRTWebSocketResource::PerformPing() noexcept {
  auto self = shared_from_this();

  co_await resume_background();

  co_await resume_on_signal(self->m_connectPerformed.get());

  if (self->m_readyState != ReadyState::Open) {
    self = nullptr;
    co_return;
  }

  try {
    self->m_socket.Control().MessageType(SocketMessageType::Utf8);

    string s{};
    winrt::array_view<const uint8_t> arr(
        CheckedReinterpretCast<const uint8_t *>(s.c_str()),
        CheckedReinterpretCast<const uint8_t *>(s.c_str()) + s.length());
    self->m_writer.WriteBytes(arr);

    auto async = self->m_writer.StoreAsync();

    co_await lessthrow_await_adapter<DataWriterStoreOperation>{async};

    auto result = async.ErrorCode();
    if (result >= 0) {
      if (self->m_pingHandler) {
        self->m_pingHandler();
      }
    } else {
      if (self->m_errorHandler) {
        self->m_errorHandler({Utilities::HResultToString(std::move(result)), ErrorType::Ping});
      }
    }
  } catch (hresult_error const &e) {
    if (self->m_errorHandler) {
      self->m_errorHandler({Utilities::HResultToString(e), ErrorType::Ping});
    }
  }
}

fire_and_forget WinRTWebSocketResource::PerformWrite(string &&message, bool isBinary) noexcept {
  auto self = shared_from_this();
  {
    auto guard = lock_guard<mutex>{m_writeQueueMutex};
    m_writeQueue.emplace(std::move(message), isBinary);
  }

  co_await resume_background();

  co_await resume_on_signal(self->m_connectPerformed.get()); // Ensure connection attempt has finished

  co_await resume_in_queue(self->m_dispatchQueue); // Ensure writes happen sequentially

  if (self->m_readyState != ReadyState::Open) {
    self = nullptr;
    co_return;
  }

  try {
    size_t length = 0;
    string messageLocal;
    bool isBinaryLocal;
    {
      auto guard = lock_guard<mutex>{m_writeQueueMutex};
      std::tie(messageLocal, isBinaryLocal) = m_writeQueue.front();
      m_writeQueue.pop();
    }

    if (isBinaryLocal) {
      self->m_socket.Control().MessageType(SocketMessageType::Binary);

      auto buffer = CryptographicBuffer::DecodeFromBase64String(winrt::to_hstring(messageLocal));
      if (buffer) {
        length = buffer.Length();
        self->m_writer.WriteBuffer(buffer);
      }
    } else {
      self->m_socket.Control().MessageType(SocketMessageType::Utf8);

      // TODO: Use char_t instead of uint8_t?
      length = messageLocal.size();
      winrt::array_view<const uint8_t> view(
          CheckedReinterpretCast<const uint8_t *>(messageLocal.c_str()),
          CheckedReinterpretCast<const uint8_t *>(messageLocal.c_str()) + messageLocal.length());
      self->m_writer.WriteBytes(view);
    }

    auto async = self->m_writer.StoreAsync();

    co_await lessthrow_await_adapter<DataWriterStoreOperation>{async};

    auto result = async.ErrorCode();
    if (result >= 0) {
      if (self->m_writeHandler) {
        self->m_writeHandler(length);
      }
    } else {
      if (self->m_errorHandler) {
        self->m_errorHandler({Utilities::HResultToString(std::move(result)), ErrorType::Send});
      }
    }
  } catch (std::exception const &e) {
    if (self->m_errorHandler) {
      self->m_errorHandler({e.what(), ErrorType::Ping});
    }
  } catch (hresult_error const &e) {
    // TODO: Remove after fixing unit tests exceptions.
    if (self->m_errorHandler) {
      self->m_errorHandler({Utilities::HResultToString(e), ErrorType::Ping});
    }
  }
}

fire_and_forget WinRTWebSocketResource::PerformClose() noexcept {
  co_await resume_background();

  co_await resume_on_signal(m_connectPerformed.get());

  try {
    m_socket.Close(static_cast<uint16_t>(m_closeCode), winrt::to_hstring(m_closeReason));

    if (m_closeHandler) {
      m_closeHandler(m_closeCode, m_closeReason);
    }
  } catch (winrt::hresult_invalid_argument const &e) {
    if (m_errorHandler) {
      m_errorHandler({winrt::to_string(e.message()), ErrorType::Close});
    }
  } catch (hresult_error const &e) {
    if (m_errorHandler) {
      m_errorHandler({Utilities::HResultToString(e), ErrorType::Close});
    }
  }

  m_readyState = ReadyState::Closed;
  m_closePerformed.Set();
}

void WinRTWebSocketResource::Synchronize() noexcept {
  // Ensure sequence of other operations
  if (m_connectRequested) {
    m_connectPerformedPromise.get_future().wait();
  }
}

#pragma endregion Private members

#pragma region IWebSocketResource

void WinRTWebSocketResource::Connect(string &&url, const Protocols &protocols, const Options &options) noexcept {
  m_socket.MessageReceived([self = shared_from_this()](
                               IWebSocket const &sender, IMessageWebSocketMessageReceivedEventArgs const &args) {
    try {
      string response;

      IDataReader reader{nullptr};
      // Use ABI version to avoid throwing exceptions on expected code paths
      HRESULT hr =
          reinterpret_cast<ABI::Windows::Networking::Sockets::IMessageWebSocketMessageReceivedEventArgs *>(
              winrt::get_abi(args))
              ->GetDataReader(reinterpret_cast<ABI::Windows::Storage::Streams::IDataReader **>(winrt::put_abi(reader)));

      if (FAILED(hr)) {
        // See
        // https://docs.microsoft.com/uwp/api/windows.networking.sockets.messagewebsocketmessagereceivedeventargs.getdatareader?view=winrt-22621#remarks
        if (hr == WININET_E_CONNECTION_ABORTED) {
          string errorMessage{"[0x80072EFE] Underlying TCP connection suddenly terminated"};
          self->m_errorHandler({errorMessage, ErrorType::Connection});
          // Note: We are not clear whether all read-related errors should close the socket.
          self->Close(CloseCode::BadPayload, std::move(errorMessage));
        } else {
          self->m_errorHandler({Utilities::HResultToString(hr), ErrorType::Receive});
        }
        return;
      }

      auto len = reader.UnconsumedBufferLength();
      if (args.MessageType() == SocketMessageType::Utf8) {
        reader.UnicodeEncoding(UnicodeEncoding::Utf8);
        vector<uint8_t> data(len);
        reader.ReadBytes(data);

        response = string(CheckedReinterpretCast<char *>(data.data()), data.size());
      } else {
        auto buffer = reader.ReadBuffer(len);
        hstring data = CryptographicBuffer::EncodeToBase64String(buffer);

        response = winrt::to_string(std::wstring_view(data));
      }

      if (self->m_readHandler) {
        self->m_readHandler(response.length(), response, args.MessageType() == SocketMessageType::Binary);
      }
    } catch (hresult_error const &e) {
      if (self->m_errorHandler) {
        string errorMessage;
        ErrorType errorType;
        errorMessage = Utilities::HResultToString(e);
        errorType = ErrorType::Receive;
        self->m_errorHandler({errorMessage, errorType});
      }
    }
  });

  m_readyState = ReadyState::Connecting;

  bool hasOriginHeader = false;
  for (const auto &header : options) {
    m_socket.SetRequestHeader(header.first, winrt::to_hstring(header.second));
    if (boost::iequals(header.first, L"Origin")) {
      hasOriginHeader = true;
    }
  }

  winrt::Windows::Foundation::Collections::IVector<hstring> supportedProtocols =
      m_socket.Control().SupportedProtocols();
  for (const auto &protocol : protocols) {
    supportedProtocols.Append(winrt::to_hstring(protocol));
  }

  Uri uri{nullptr};
  try {
    uri = Uri{winrt::to_hstring(url)};

    // #12626 - If Origin header is not provided, set to connect endpoint.
    if (!hasOriginHeader) {
      auto scheme = uri.SchemeName();
      auto host = uri.Host();
      auto port = uri.Port();

      if (scheme == L"ws") {
        scheme = L"http";
      } else if (scheme == L"wss") {
        scheme = L"https";
      }

      // Only add a port if a port is defined
      hstring originPort = port != 0 ? L":" + winrt::to_hstring(port) : L"";
      auto origin = hstring{scheme + L"://" + host + originPort};

      m_socket.SetRequestHeader(L"Origin", std::move(origin));
    }

  } catch (hresult_error const &e) {
    if (m_errorHandler) {
      m_errorHandler({Utilities::HResultToString(e), ErrorType::Connection});
    }

    // Abort - Mark connection as concluded.
    SetEvent(m_connectPerformed.get());
    m_connectPerformedPromise.set_value();
    m_connectRequested = false;

    return;
  }

  m_connectRequested = true;

  PerformConnect(std::move(uri));
}

void WinRTWebSocketResource::Ping() noexcept {
  PerformPing();
}

void WinRTWebSocketResource::Send(string &&message) noexcept {
  PerformWrite(std::move(message), false);
}

void WinRTWebSocketResource::SendBinary(string &&base64String) noexcept {
  PerformWrite(std::move(base64String), true);
}

void WinRTWebSocketResource::Close(CloseCode code, const string &reason) noexcept {
  if (m_readyState == ReadyState::Closing || m_readyState == ReadyState::Closed)
    return;

  Synchronize();

  m_readyState = ReadyState::Closing;
  m_closeCode = code;
  m_closeReason = reason;
  PerformClose();
}

IWebSocketResource::ReadyState WinRTWebSocketResource::GetReadyState() const noexcept {
  return m_readyState;
}

void WinRTWebSocketResource::SetOnConnect(function<void()> &&handler) noexcept {
  m_connectHandler = std::move(handler);
}

void WinRTWebSocketResource::SetOnPing(function<void()> &&handler) noexcept {
  m_pingHandler = std::move(handler);
}

void WinRTWebSocketResource::SetOnSend(function<void(size_t)> &&handler) noexcept {
  m_writeHandler = std::move(handler);
}

void WinRTWebSocketResource::SetOnMessage(function<void(size_t, const string &, bool isBinary)> &&handler) noexcept {
  m_readHandler = std::move(handler);
}

void WinRTWebSocketResource::SetOnClose(function<void(CloseCode, const string &)> &&handler) noexcept {
  m_closeHandler = std::move(handler);
}

void WinRTWebSocketResource::SetOnError(function<void(Error &&)> &&handler) noexcept {
  m_errorHandler = std::move(handler);
}

#pragma endregion IWebSocketResource

#pragma endregion Legacy resource

} // namespace Microsoft::React::Networking
