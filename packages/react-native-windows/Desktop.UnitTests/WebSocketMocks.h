#pragma once

#include <IWebSocketResource.h>

namespace Microsoft::React::Test {
struct MockWebSocketResource : public IWebSocketResource {
  ~MockWebSocketResource();

  struct Mocks {
    std::function<void(const Protocols &, const Options &)> Connect;
    std::function<void()> Ping;
    std::function<void(const std::string &)> Send;
    std::function<void(const std::string &)> SendBinary;
    std::function<void(CloseCode, const std::string &)> Close;
    std::function<ReadyState() /*const*/> GetReadyState;
    std::function<void(std::function<void()> &&)> SetOnConnect;
    std::function<void(std::function<void()> &&)> SetOnPing;
    std::function<void(std::function<void(std::size_t)> &&)> SetOnSend;
    std::function<void(std::function<void(std::size_t, const std::string &, bool)> &&)> SetOnMessage;
    std::function<void(std::function<void(CloseCode, const std::string &)> &&)> SetOnClose;
    std::function<void(std::function<void(Error &&)> &&)> SetOnError;
  };

  Mocks Mocks;

#pragma region IWebSocketResource overrides

  void Connect(const Protocols &, const Options &) noexcept override;

  void Ping() noexcept override;

  void Send(std::string &&) noexcept override;

  void SendBinary(std::string &&) noexcept override;

  void Close(CloseCode, const std::string &) noexcept override;

  ReadyState GetReadyState() const noexcept override;

  void SetOnConnect(std::function<void()> &&onConnect) noexcept override;

  void SetOnPing(std::function<void()> &&) noexcept override;

  void SetOnSend(std::function<void(std::size_t)> &&) noexcept override;

  void SetOnMessage(std::function<void(std::size_t, const std::string &, bool)> &&) noexcept override;

  void SetOnClose(std::function<void(CloseCode, const std::string &)> &&) noexcept override;

  void SetOnError(std::function<void(Error &&)> &&) noexcept override;

#pragma endregion IWebSocketResource overrides

  void OnConnect();
  void OnPing();
  void OnSend(std::size_t size);
  void OnMessage(std::size_t, const std::string &message, bool isBinary);
  void OnClose(CloseCode code, const std::string &reason);
  void OnError(Error &&error);

 private:
  std::function<void()> m_connectHandler;
  std::function<void()> m_pingHandler;
  std::function<void(std::size_t)> m_writeHandler;
  std::function<void(std::size_t, const std::string &, bool)> m_readHandler;
  std::function<void(CloseCode, const std::string &)> m_closeHandler;
  std::function<void(Error &&)> m_errorHandler;
};

} // namespace Microsoft::React::Test
