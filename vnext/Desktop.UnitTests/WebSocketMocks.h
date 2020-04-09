#pragma once

#include <IWebSocketResource.h>

namespace Microsoft::React::Test
{
struct MockWebSocketResource : public IWebSocketResource {

#pragma region IWebSocketResource overrides

  void Connect(const Protocols&, const Options&) override;

  void Ping() override;

  void Send(const std::string&) override;

  void SendBinary(const std::string&) override;

  void Close(CloseCode, const std::string&) override;

  ReadyState GetReadyState() const override;

  void SetOnConnect(std::function<void()>&& onConnect) override;

  void SetOnPing(std::function<void()>&&) override;

  void SetOnSend(std::function<void(size_t)>&&) override;

  void SetOnMessage(std::function<void(size_t, const std::string&)>&&) override;

  void SetOnClose(std::function<void(CloseCode, const std::string&)>&&) override;

  void SetOnError(std::function<void(Error&&)>&&) override;

#pragma endregion IWebSocketResource overrides

private:
  std::function<void()> m_onConnect;
};

} // namespace Microsoft::React::Test
