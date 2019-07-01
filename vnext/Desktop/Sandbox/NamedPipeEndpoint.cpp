// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"

#include "NamedPipeEndpoint.h"
#include "unicode.h"

#include <Sddl.h>

#include <folly/Optional.h>
#include <folly/dynamic.h>
#include <folly/json.h>
#include <glog/logging.h>
#include <chrono>

namespace facebook {
namespace react {

#define ReturnIfFailed(f) \
  do {                    \
    if (!(f)) {           \
      return false;       \
    }                     \
  } while (0)

class IPCConn; // Forward declaration

class IoThreadPool {
 public:
  IoThreadPool();
  ~IoThreadPool();

  static DWORD WINAPI ThreadProc(LPVOID lpParameter);

  bool AddAsyncIO(IPCConn *conn);
  void StartShutdown();
  void Shutdown();

 private:
  HANDLE m_ioPort;
  HANDLE m_thread;
  DWORD m_threadId{0};
  std::atomic<bool> m_shutdown{false};
};

// Simple message header only includes message length.
using IPCMessageHeader = unsigned;

struct AsyncIO {
  enum class IoType { ACCEPT, READ, WRITE };

  AsyncIO() = delete;
  AsyncIO(const AsyncIO &) = delete;
  explicit AsyncIO(IoType ioType, IPCConn *conn);

  IPCConn *m_conn;
  OVERLAPPED m_overlapped;
  IoType m_ioType;
};

struct AsyncWriteIO : AsyncIO {
  AsyncWriteIO() = delete;
  AsyncWriteIO(const AsyncWriteIO &) = delete;
  explicit AsyncWriteIO(
      IPCConn *conn,
      std::string &&message,
      const SendRequestCallback &callback);
  unsigned MessageSize() const;

  std::atomic<unsigned> m_writeOpCount{0};
  IPCMessageHeader m_messageHeader{0};
  std::string m_message;

  SendRequestCallback m_completed;
};

const unsigned NAMED_PIPE_BUFFER_SIZE = 16 * 1024;
using NamedPipeMessageHandler =
    std::function<void(std::unique_ptr<std::string> message)>;

class IPCConn {
 private:
  const unsigned MaxMessageLength = 16 * 1024 * 1024;
  const unsigned CONNECTION_TIMEOUT_MS = 5000;

  enum State {
    Start,
    WaitingToConnect,
    Connected,
    Listening,
    Broken,
    Disconnected,
    Closed
  };

 public:
  IPCConn() = delete;
  explicit IPCConn(const std::wstring &pipeName);
  ~IPCConn();

  HANDLE GetHandle() {
    return m_handle;
  }

  State GetState() {
    return m_state;
  }

  bool IsConnected() {
    return m_state == Connected;
  }

  void RegisterMessageHandler(const NamedPipeMessageHandler &messageHandler);

  bool AddListener();
  bool Listen(); // For server
  bool Connect(); // For client

  void Close();
  // void OnAsyncDisconnect();

  void BeginRead();
  void BeginWrite(std::string &&message, const SendRequestCallback &callback);

  void OnIoCompleted(AsyncIO *asyncIoResult, DWORD bytes);

 private:
  bool ProcessReadBuffer(DWORD bytes);
  void CompleteWriteOp(AsyncWriteIO *op);

  static folly::Optional<SECURITY_ATTRIBUTES> CreatePipeSecurity();

  std::wstring m_pipeName;
  HANDLE m_handle{INVALID_HANDLE_VALUE};
  State m_state{Start};

  AsyncIO m_acceptOp;
  AsyncIO m_readOp;

  BYTE m_buffer[NAMED_PIPE_BUFFER_SIZE];
  static_assert(
      NAMED_PIPE_BUFFER_SIZE > sizeof(IPCMessageHeader),
      "Buffer size must be bigger than header size");

  IPCMessageHeader m_bufferOffset{0};

  std::unique_ptr<std::string> m_message;
  unsigned m_messageBytesToRead{0};

  NamedPipeMessageHandler m_messageReceiveHandler;
};

AsyncIO::AsyncIO(IoType ioType, IPCConn *conn)
    : m_ioType(ioType), m_conn(conn) {
  memset(&m_overlapped, 0, sizeof(m_overlapped));
}

AsyncWriteIO::AsyncWriteIO(
    IPCConn *conn,
    std::string &&message,
    const SendRequestCallback &callback)
    : AsyncIO(IoType::WRITE, conn),
      m_message(std::move(message)),
      m_completed(callback) {
  m_messageHeader = (IPCMessageHeader)m_message.size();
}

unsigned AsyncWriteIO::MessageSize() const {
  return m_messageHeader;
}

IoThreadPool::IoThreadPool() {
  m_ioPort = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);

  const SIZE_T IoThreadStackSize = 1024 * 16;

  // Just create one thread.
  m_thread = CreateThread(
      NULL,
      IoThreadStackSize,
      &IoThreadPool::ThreadProc,
      (LPVOID)this,
      0,
      &m_threadId);
}

IoThreadPool::~IoThreadPool() {
  // At this point, all thread pool threads should have exited
  CloseHandle(m_ioPort);
}

void IoThreadPool::StartShutdown() {
  m_shutdown = true;

  // Queue null op to wake up a thread
  PostQueuedCompletionStatus(m_ioPort, 0, NULL, NULL);
}

void IoThreadPool::Shutdown() {
  if (WaitForSingleObject(m_thread, 200) == WAIT_TIMEOUT) {
    TerminateThread(m_thread, 0);
  }
}

/**
 * IPC IO thread proc
 */
DWORD WINAPI IoThreadPool::ThreadProc(LPVOID lpParameter) {
  IoThreadPool *threadPool = (IoThreadPool *)lpParameter;

  while (!threadPool->m_shutdown) {
    OVERLAPPED *overlapped;
    ULONG_PTR completionKey;
    DWORD bytes;
    BOOL success = (GetQueuedCompletionStatus(
        threadPool->m_ioPort,
        &bytes,
        (PULONG_PTR)&completionKey,
        &overlapped,
        INFINITE));

    if (success == FALSE) {
      // TODO: Error handling.
      DWORD hr = GetLastError();
      if (hr == ERROR_BROKEN_PIPE) {
        LOG(DEBUG) << "Pipe closed";
      }
    }

    if (overlapped) {
      AsyncIO *asyncIo = CONTAINING_RECORD(overlapped, AsyncIO, m_overlapped);
      if (asyncIo->m_conn) {
        asyncIo->m_conn->OnIoCompleted(asyncIo, bytes);
      }
    }
  }

  return 0;
}

bool IoThreadPool::AddAsyncIO(IPCConn *conn) {
  CHECK(conn->GetHandle() != NULL);

  // Associate the IO handle with IOCP
  if (CreateIoCompletionPort(conn->GetHandle(), m_ioPort, 0, 0)) {
    return true;
  } else {
    LOG(ERROR) << "IoThreadPool: Associating handle to IOCP failed";
    return false;
  }
}

IPCConn::IPCConn(const std::wstring &pipeName)
    : m_pipeName(pipeName),
      m_acceptOp(AsyncIO::IoType::ACCEPT, this),
      m_readOp(AsyncIO::IoType::READ, this) {}

IPCConn::~IPCConn() {
  Close();
}

void IPCConn::RegisterMessageHandler(
    const NamedPipeMessageHandler &messageHandler) {
  m_messageReceiveHandler = messageHandler;
}

bool IPCConn::Connect() {
  auto start = std::chrono::steady_clock::now();

  while (true) {
    if (!WaitNamedPipeW(m_pipeName.c_str(), CONNECTION_TIMEOUT_MS)) {
      if (GetLastError() == ERROR_FILE_NOT_FOUND &&
          std::chrono::duration_cast<std::chrono::milliseconds>(
              std::chrono::steady_clock::now() - start)
                  .count() < CONNECTION_TIMEOUT_MS) {
        continue;
      }

      // Failed to connect to the named pipe. RaidMonitor is not running?
      LOG(WARN) << "NamedPipe: Connection failed.";
      m_handle = INVALID_HANDLE_VALUE;
      break;
    }

    // Connect to the named pipe
    m_handle = CreateFileW(
        m_pipeName.c_str(), // pipe name
        GENERIC_READ | // read and write access
            GENERIC_WRITE,
        0, // no sharing
        NULL, // default security attributes
        OPEN_EXISTING, // opens existing pipe
        FILE_FLAG_OVERLAPPED, // default attributes
        NULL); // no template file

    if (m_handle == INVALID_HANDLE_VALUE) {
      if (GetLastError() == ERROR_PIPE_BUSY &&
          std::chrono::duration_cast<std::chrono::milliseconds>(
              std::chrono::steady_clock::now() - start)
                  .count() < CONNECTION_TIMEOUT_MS) {
        continue;
      } else {
        break;
      }
    }

    m_state = Connected;
    break;
  }

  return (m_state == Connected);
}

bool IPCConn::AddListener() {
  auto sa = CreatePipeSecurity();
  if (!sa) {
    return false;
  }

  // Attempt to open the named pipe for listening
  m_handle = CreateNamedPipeW(
      m_pipeName.c_str(),
      (PIPE_ACCESS_DUPLEX | FILE_FLAG_OVERLAPPED |
       FILE_FLAG_FIRST_PIPE_INSTANCE),
      (PIPE_TYPE_BYTE | PIPE_READMODE_BYTE | PIPE_WAIT),
      PIPE_UNLIMITED_INSTANCES, // max instances
      4096, // outbuf
      4096, // inbuf
      0, // timeout ms
      sa.get_pointer()); // securityattrib

  if (m_handle == INVALID_HANDLE_VALUE) {
    return false;
  }

  return true;
}

bool IPCConn::Listen() {
  if (!ConnectNamedPipe(m_handle, &m_acceptOp.m_overlapped)) {
    DWORD result = GetLastError();
    if (result != ERROR_IO_PENDING && result != ERROR_PIPE_CONNECTED) {
      return false;
    }
  }

  m_state = Listening;
  return true;
}

void IPCConn::Close() {
  if (m_handle != INVALID_HANDLE_VALUE) {
    CloseHandle(m_handle);
    m_handle = INVALID_HANDLE_VALUE;
    m_state = Closed;

    // if (m_notify)
    //  m_notify->OnDisconnected();
  }
}

void IPCConn::BeginRead() {
  if (m_state != Connected) {
    LOG(WARN) << "Try to read on not connected IPC";
    return;
  }

  BOOL bResult = ReadFile(
      GetHandle(),
      m_buffer + m_bufferOffset, // buffer
      sizeof(m_buffer) - m_bufferOffset, // size of buffer
      NULL, // number of bytes read
      &m_readOp.m_overlapped);

  // bResult == TRUE: Completed inline and result will be posted to IOCP.

  if (!bResult) {
    DWORD hr = GetLastError();
    // Async normal case
    if (hr == ERROR_IO_PENDING) {
      return;
    }

    // ReadFile failed
    if (hr == ERROR_MORE_DATA) {
      LOG(ERROR) << "Not enough pipe read buffer";
    } else if (hr == ERROR_BROKEN_PIPE || hr == ERROR_PIPE_NOT_CONNECTED) {
      m_state = Broken;
      LOG(WARN) << "Named pipe disconnected";
    } else {
      m_state = Broken;
      LOG(WARN) << "Unexpected error";
    }
  }

  return;
}

void IPCConn::BeginWrite(
    std::string &&message,
    const SendRequestCallback &callback) {
  if (m_state != Connected) {
    if (callback) {
      callback(false);
    }
    return;
  }

  auto writeOp = new AsyncWriteIO(this, std::move(message), callback);

  // Write message header first and then write message body
  writeOp->m_writeOpCount = 2;
  DWORD dwWritten = 0;
  BOOL bResult = WriteFile(
      GetHandle(),
      (const void *)&writeOp->m_messageHeader,
      sizeof(writeOp->m_messageHeader),
      &dwWritten,
      &writeOp->m_overlapped);

  DWORD hr = GetLastError();
  if (bResult || hr == ERROR_IO_PENDING) {
    bResult = WriteFile(
        GetHandle(),
        (const void *)writeOp->m_message.c_str(),
        writeOp->MessageSize(),
        &dwWritten,
        &writeOp->m_overlapped);

    hr = GetLastError();
  }

  // bResult == TRUE: Completed inline and result will still be posted to IOCP.
  // bResult == FALSE && hr == ERROR_IO_PENDING: Async operation queued and
  // result will be posted to IOCP. Other cases: Unexpected error.
  if (!bResult && hr != ERROR_IO_PENDING) {
    // TODO: Error handling.
    if (writeOp->m_completed) {
      writeOp->m_completed(false);
    }
    delete writeOp;
  }
}

bool IPCConn::ProcessReadBuffer(DWORD bytes) {
  if (bytes == 0) {
    return false;
  }

  const char *msg = (const char *)m_buffer;
  while (bytes > 0) {
    if (m_messageBytesToRead == 0) {
      if (bytes >= sizeof(IPCMessageHeader)) {
        m_messageBytesToRead = *(IPCMessageHeader *)msg;
        if (m_messageBytesToRead > MaxMessageLength) {
          return false;
        }

        msg += sizeof(IPCMessageHeader);
        bytes -= sizeof(IPCMessageHeader);
        m_message = std::make_unique<std::string>();
        m_message->reserve(m_messageBytesToRead);
        m_bufferOffset = 0;
        continue;
      } else {
        // Handle case that header has not been fully read.
        if (msg != (const char *)m_buffer) {
          memmove(m_buffer, msg, bytes);
        }
        m_bufferOffset = bytes;
        break;
      }
    }

    // Need more data to get whole message?
    if (bytes < m_messageBytesToRead) {
      m_message->append(msg, bytes);
      m_messageBytesToRead -= bytes;
      break;
    }

    // Consume one message
    m_message->append(msg, m_messageBytesToRead);
    msg += m_messageBytesToRead;
    bytes -= m_messageBytesToRead;
    m_messageBytesToRead = 0;

    if (m_messageReceiveHandler) {
      m_messageReceiveHandler(std::move(m_message));
    }
  }

  return true;
}

void IPCConn::CompleteWriteOp(AsyncWriteIO *op) {
  op->m_writeOpCount--;
  if (op->m_writeOpCount == 0) {
    if (op->m_completed) {
      op->m_completed(true);
    }
    delete op;
  }
}

void IPCConn::OnIoCompleted(AsyncIO *asyncIoOp, DWORD bytes) {
  switch (asyncIoOp->m_ioType) {
    case AsyncIO::IoType::ACCEPT:
      m_state = Connected;
      BeginRead();
      break;

    case AsyncIO::IoType::WRITE:
      CompleteWriteOp((AsyncWriteIO *)asyncIoOp);
      break;

    case AsyncIO::IoType::READ:
      if (ProcessReadBuffer(bytes + m_bufferOffset)) {
        // Start next read
        BeginRead();
      } else {
        Close();
      }
      break;
  }
}

/* static */
folly::Optional<SECURITY_ATTRIBUTES> IPCConn::CreatePipeSecurity() {
  PSECURITY_DESCRIPTOR pSd = NULL;
  SECURITY_ATTRIBUTES sa;

  // Define the SDDL for the security descriptor.
  PCWSTR szSDDL =
      L"D:" // Discretionary ACL
      L"(A;OICI;GRGW;;;AU)" // Allow read/write to authenticated users
      L"(A;OICI;GA;;;BA)"; // Allow full control to administrators

  if (!ConvertStringSecurityDescriptorToSecurityDescriptorW(
          szSDDL, SDDL_REVISION_1, &pSd, NULL)) {
    return {};
  }

  sa.nLength = sizeof(sa);
  sa.lpSecurityDescriptor = pSd;
  sa.bInheritHandle = FALSE;

  return folly::Optional<SECURITY_ATTRIBUTES>(sa);
}

struct NamedPipeEndpoint::Impl {
  const wchar_t *PIPE_NAME_PREFIX = L"\\\\.\\pipe\\";

  explicit Impl(const std::string &pipeName);
  ~Impl();

  bool Start(EndpointType endpointType);
  void Shutdown();
  void RegisterJSCallRequestHandler(const JSCallRequestHandler &handler);
  void RegisterReplyHandler(const ReplyMessageHandler &handler);
  void RegisterNativeModuleCallHandler(const NativeModuleCallHandler &handler);

  void SendRequest(
      int64_t requestId,
      const std::string &methodName,
      const folly::dynamic &arguments,
      SendRequestCallback &&callback);
  void Send(std::string &&message);

  void OnMessageReceived_Host(std::unique_ptr<std::string> message);
  void OnMessageReceived_Sandbox(std::unique_ptr<std::string> message);

 private:
  EndpointType m_endpointType;
  std::wstring m_pipeName;
  std::unique_ptr<IoThreadPool> m_ioThreadPool;
  std::shared_ptr<IPCConn> m_conn;
  JSCallRequestHandler m_jsCallRequestHandler;
  ReplyMessageHandler m_replyHandler;
  NativeModuleCallHandler m_nativeModuleCallHandler;
};

NamedPipeEndpoint::Impl::Impl(const std::string &pipeName) {
  m_pipeName = PIPE_NAME_PREFIX + unicode::utf8ToUtf16(pipeName);
  m_ioThreadPool = std::make_unique<IoThreadPool>();
  m_conn = std::make_shared<IPCConn>(m_pipeName);
}

NamedPipeEndpoint::Impl::~Impl() {
  m_ioThreadPool->Shutdown();
}

bool NamedPipeEndpoint::Impl::Start(EndpointType endpointType) {
  m_endpointType = endpointType;

  if (m_endpointType == EndpointType::Host) { // Client
    m_conn->RegisterMessageHandler(
        [this](std::unique_ptr<std::string> message) {
          OnMessageReceived_Host(std::move(message));
        });

    ReturnIfFailed(m_conn->Connect());
    ReturnIfFailed(m_ioThreadPool->AddAsyncIO(m_conn.get()));
    m_conn->BeginRead();
  } else if (m_endpointType == EndpointType::Sandbox) { // Server
    m_conn->RegisterMessageHandler(
        [this](std::unique_ptr<std::string> message) {
          OnMessageReceived_Sandbox(std::move(message));
        });

    // Add one listener.
    ReturnIfFailed(m_conn->AddListener());
    ReturnIfFailed(m_ioThreadPool->AddAsyncIO(m_conn.get()));
    ReturnIfFailed(m_conn->Listen());
  }

  return true;
}

void NamedPipeEndpoint::Impl::Shutdown() {
  m_ioThreadPool->StartShutdown();
  m_ioThreadPool->Shutdown();
  m_conn->Close();
}

void NamedPipeEndpoint::Impl::RegisterJSCallRequestHandler(
    const JSCallRequestHandler &handler) {
  m_jsCallRequestHandler = handler;
}

void NamedPipeEndpoint::Impl::RegisterReplyHandler(
    const ReplyMessageHandler &handler) {
  m_replyHandler = handler;
}

void NamedPipeEndpoint::Impl::RegisterNativeModuleCallHandler(
    const NativeModuleCallHandler &handler) {
  m_nativeModuleCallHandler = handler;
}

void NamedPipeEndpoint::Impl::SendRequest(
    int64_t requestId,
    const std::string &methodName,
    const folly::dynamic &arguments,
    SendRequestCallback &&callback) {
  folly::dynamic request = folly::dynamic::object("id", requestId)(
      "method", methodName)("arguments", std::move(arguments));
  std::string requestJson = folly::toJson(request);
  m_conn->BeginWrite(std::move(requestJson), callback);
}

void NamedPipeEndpoint::Impl::Send(std::string &&message) {
  m_conn->BeginWrite(std::move(message), nullptr);
}

void NamedPipeEndpoint::Impl::OnMessageReceived_Host(
    std::unique_ptr<std::string> message) {
  int64_t replyId = 0;

  std::chrono::high_resolution_clock::time_point t1 =
      std::chrono::high_resolution_clock::now();

  folly::dynamic parsed = folly::parseJson(*message.get());

  // Native module calls??
  if (parsed.isArray() || parsed.isNull()) {
    if (m_nativeModuleCallHandler) {
      m_nativeModuleCallHandler(std::move(parsed));
    }
  } else {
    // Ack reply for a Native to JS call.
    auto it_parsed = parsed.find("replyID");
    auto it_end = parsed.items().end();
    if (it_parsed != it_end) {
      replyId = it_parsed->second.getInt();
      if (m_replyHandler) {
        m_replyHandler(replyId);
      }
    }
  }

  std::chrono::high_resolution_clock::time_point t2 =
      std::chrono::high_resolution_clock::now();
  auto duration =
      std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count();

  OutputDebugStringA(message->c_str());
  OutputDebugStringA("\n");

  char buffer[256];
  sprintf_s(
      buffer,
      "(%03lld) OnMessageReceived, elapsed time = %d us\n",
      replyId,
      (int)duration);

  OutputDebugStringA(buffer);
}

void NamedPipeEndpoint::Impl::OnMessageReceived_Sandbox(
    std::unique_ptr<std::string> message) {
  if (m_jsCallRequestHandler != nullptr) {
    folly::dynamic parsed = folly::parseJson(*message.get());
    auto requestId = parsed["id"].asInt();
    auto jsonRPCMethod = parsed["method"].asString();
    auto arguments = parsed["arguments"];

    m_jsCallRequestHandler(requestId, jsonRPCMethod, std::move(arguments));
  }
}

NamedPipeEndpoint::NamedPipeEndpoint(const std::string &pipeName) {
  m_pimpl = std::make_unique<Impl>(pipeName);
}

NamedPipeEndpoint::~NamedPipeEndpoint() {}

bool NamedPipeEndpoint::Start(EndpointType endpointType) {
  return m_pimpl->Start(endpointType);
}

void NamedPipeEndpoint::Shutdown() {
  return m_pimpl->Shutdown();
}

void NamedPipeEndpoint::RegisterJSCallRequestHandler(
    const JSCallRequestHandler &handler) {
  return m_pimpl->RegisterJSCallRequestHandler(handler);
}

void NamedPipeEndpoint::RegisterReplyHandler(
    const ReplyMessageHandler &handler) {
  return m_pimpl->RegisterReplyHandler(handler);
}

void NamedPipeEndpoint::RegisterNativeModuleCallHandler(
    const NativeModuleCallHandler &handler) {
  return m_pimpl->RegisterNativeModuleCallHandler(handler);
}

void NamedPipeEndpoint::SendRequest(
    int64_t requestId,
    const std::string &methodName,
    const folly::dynamic &arguments,
    SendRequestCallback &&callback) {
  return m_pimpl->SendRequest(
      requestId, methodName, arguments, std::move(callback));
}

void NamedPipeEndpoint::Send(std::string &&message) {
  return m_pimpl->Send(std::move(message));
}

} // namespace react
} // namespace facebook
