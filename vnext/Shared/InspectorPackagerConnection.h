// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include <Networking/WinRTWebSocketResource.h>
#include <jsinspector-modern/InspectorInterfaces.h>

namespace Microsoft::ReactNative {

class InspectorPackagerConnection final : public std::enable_shared_from_this<InspectorPackagerConnection> {
 public:
  winrt::fire_and_forget connectAsync();
  winrt::fire_and_forget disconnectAsync();

  class BundleStatus {
   public:
    bool m_isLastDownloadSuccess;
    int64_t m_updateTimestamp = -1;

    BundleStatus(bool isLastDownloadSuccess, long updateTimestamp)
        : m_isLastDownloadSuccess(isLastDownloadSuccess), m_updateTimestamp(updateTimestamp) {}
    BundleStatus() : m_isLastDownloadSuccess(false), m_updateTimestamp(-1) {}
  };

  struct IBundleStatusProvider {
    virtual BundleStatus getBundleStatus() = 0;
  };

  InspectorPackagerConnection(std::string &&url, std::shared_ptr<IBundleStatusProvider> bundleStatusProvider);

 private:
  friend class RemoteConnection;

  winrt::fire_and_forget sendMessageToPackagerAsync(std::string &&message) const;
  void sendMessageToPackager(std::string &&message) const;

  // Note:: VM side Inspector processes the messages asynchronously in a sequential executor with dedicated thread.
  // Hence, we don't bother invoking the inspector asynchronously.
  void sendMessageToVM(int32_t pageId, std::string &&message);

 private:
  std::unordered_map<int32_t, std::unique_ptr<facebook::react::jsinspector_modern::ILocalConnection>>
      m_localConnections;
  std::shared_ptr<Microsoft::React::Networking::WinRTWebSocketResource> m_packagerWebSocketConnection;
  std::shared_ptr<IBundleStatusProvider> m_bundleStatusProvider;
  std::string m_url;
};

class RemoteConnection final : public facebook::react::jsinspector_modern::IRemoteConnection {
 public:
  RemoteConnection(int32_t pageId, const InspectorPackagerConnection &packagerConnection);
  void onMessage(std::string message) override;
  void onDisconnect() override;

 private:
  int32_t m_pageId;
  const InspectorPackagerConnection &m_packagerConnection;
};

} // namespace Microsoft::ReactNative
