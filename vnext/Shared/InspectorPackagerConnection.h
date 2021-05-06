#pragma once

#include <WinRTWebSocketResource.h>
#include <jsinspector/InspectorInterfaces.h>

namespace Microsoft::ReactNative {

class InspectorPackagerConnection : public std::enable_shared_from_this<InspectorPackagerConnection> {
 public:
  InspectorPackagerConnection(std::string &&url);
  winrt::fire_and_forget connectAsync();
  winrt::fire_and_forget disconnectAsync();

 private:
  friend class RemoteConnection;

  winrt::fire_and_forget sendMessageToPackagerAsync(std::string &&message) const;
  void sendMessageToPackager(std::string &&message) const;

  // Note:: VM side Inspector processes the messages asynchronousely in a sequential executor with dedicated thread.
  // Hence, we don't bother invoking the inspector asynchronously.
  void sendMessageToVM(int64_t pageId, std::string &&message);

  std::unordered_map<int64_t, std::unique_ptr<facebook::react::ILocalConnection>> m_localConnections;
  std::shared_ptr<Microsoft::React::WinRTWebSocketResource> m_packagerWebSocketConnection;
  std::string m_url;
};

class RemoteConnection : public facebook::react::IRemoteConnection {
 public:
  RemoteConnection(int64_t pageId, const InspectorPackagerConnection &packagerConnection);
  void onMessage(std::string message) override;
  void onDisconnect() override;

 private:
  int64_t m_pageId;
  const InspectorPackagerConnection &m_packagerConnection;
};
} // namespace Microsoft::ReactNative
