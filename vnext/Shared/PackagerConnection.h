// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include <winrt/Windows.Networking.Sockets.h>
#include "DevSettings.h"
#include "ReactNotificationService.h"

namespace Microsoft::ReactNative {

/*
 * Maintains a list of connections to packaging servers which can notify the instance to reload, or show the dev menu
 * Generally an app will only ever talk to one server, so if there are mutliple instances running in an app this allows
 * them to share a connection to that pacakger.
 */
struct PackagerConnection : std::enable_shared_from_this<PackagerConnection> {
  PackagerConnection(const std::string &sourceBundleHost, uint16_t sourceBundlePort);
  static void CreateOrReusePackagerConnection(const facebook::react::DevSettings &settings);

 private:
  winrt::fire_and_forget Connect();
  void SubscribeReloadEvent(winrt::Microsoft::ReactNative::ReactNotificationHandler const &handler);
  void SubscribeDevMenuEvent(winrt::Microsoft::ReactNative::ReactNotificationHandler const &handler);

  static std::shared_ptr<PackagerConnection> GetOrCreate(
      const std::string &sourceBundleHost,
      uint16_t sourceBundlePort);

  std::string m_sourceBundleHost;
  uint16_t m_sourceBundlePort;
  winrt::Microsoft::ReactNative::IReactNotificationService m_notificationService;
  winrt::Windows::Networking::Sockets::MessageWebSocket m_ws;
  winrt::Windows::Networking::Sockets::MessageWebSocket::MessageReceived_revoker m_wsMessageRevoker;
  static std::mutex m_mutexPackagers;
  static std::unordered_map<std::pair<std::string, uint16_t>, std::shared_ptr<PackagerConnection>> m_connections;
};

} // namespace Microsoft::ReactNative
