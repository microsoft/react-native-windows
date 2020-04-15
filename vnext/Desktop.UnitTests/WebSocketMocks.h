#pragma once

#include <IWebSocketResource.h>

namespace Microsoft::React::Test {
struct MockWebSocketResource : public IWebSocketResource {
  ~MockWebSocketResource();

#pragma region Mocks

  std::function<void(const Protocols &, const Options &)> ConnectMock;
  std::function<void()> PingMock;
  std::function<void(const std::string &)> SendMock;
  std::function<void(const std::string &)> SendBinaryMock;
  std::function<void(CloseCode, const std::string &)> CloseMock;
  std::function<ReadyState() /*const*/> GetReadyStateMock;
  std::function<void(std::function<void()> &&)> SetOnConnectMock;
  std::function<void(std::function<void()> &&)> SetOnPingMock;
  std::function<void(std::function<void(std::size_t)> &&)> SetOnSendMock;
  std::function<void(std::function<void(std::size_t, const std::string &)> &&)> SetOnMessageMock;
  std::function<void(std::function<void(CloseCode, const std::string &)> &&)> SetOnCloseMock;
  std::function<void(std::function<void(Error &&)> &&)> SetOnErrorMock;

#pragma endregion Mocks

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
