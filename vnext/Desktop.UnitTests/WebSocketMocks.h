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
    std::function<void(std::function<void(std::size_t, const std::string &)> &&)> SetOnMessage;
    std::function<void(std::function<void(CloseCode, const std::string &)> &&)> SetOnClose;
    std::function<void(std::function<void(Error &&)> &&)> SetOnError;
  };

  Mocks Mocks;

#pragma region IWebSocketResource overrides

  void Connect(const Protocols &, const Options &) override;

  void Ping() override;

  void Send(const std::string &) override;

  void SendBinary(const std::string &) override;

  void Close(CloseCode, const std::string &) override;

  ReadyState GetReadyState() const override;

  void SetOnConnect(std::function<void()> &&onConnect) override;

  void SetOnPing(std::function<void()> &&) override;

  void SetOnSend(std::function<void(std::size_t)> &&) override;

  void SetOnMessage(std::function<void(std::size_t, const std::string &)> &&) override;

  void SetOnClose(std::function<void(CloseCode, const std::string &)> &&) override;

  void SetOnError(std::function<void(Error &&)> &&) override;

#pragma endregion IWebSocketResource overrides

  void OnConnect();
  void OnPing();
  void OnSend(std::size_t size);
  void OnMessage(std::size_t, const std::string &message);
  void OnClose(CloseCode code, const std::string &reason);
  void OnError(Error &&error);

 private:
  std::function<void()> m_connectHandler;
  std::function<void()> m_pingHandler;
  std::function<void(std::size_t)> m_writeHandler;
  std::function<void(std::size_t, const std::string &)> m_readHandler;
  std::function<void(CloseCode, const std::string &)> m_closeHandler;
  std::function<void(Error &&)> m_errorHandler;
};

} // namespace Microsoft::React::Test
