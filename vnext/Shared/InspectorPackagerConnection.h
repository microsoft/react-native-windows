#pragma once

#include <hermes/hermes.h>
#include <hermes/DebuggerAPI.h>
#include <hermes/inspector/Inspector.h>
#include <hermes/inspector/chrome/Registration.h>
#include <jsinspector/InspectorInterfaces.h>

#include <WinRTWebSocketResource.h>


namespace Microsoft::ReactNative {

class InspectorPackagerConnection final {
 public:
  InspectorPackagerConnection(std::string&& url);
  
 private:
  friend class RemoteConnection;
  void sendMessageToPackager(std::string &&message) const;
  void sendMessageToVM(int64_t pageId, std::string &&message);
  
  std::unordered_map<int64_t, std::unique_ptr<facebook::react::ILocalConnection>> m_localConnections;
  std::shared_ptr<Microsoft::React::WinRTWebSocketResource> m_packagerWebSocketConnection;
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
