// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include <IDevSupportManager.h>

#include <DevServerHelper.h>

#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Networking.Sockets.h>
#include <atomic>
#include <functional>
#include <future>
#include <memory>
#include <string>

#include <InspectorPackagerConnection.h>

namespace facebook {
namespace react {
struct DevSettings;
}
} // namespace facebook

namespace winrt {

template <typename K, typename V>
struct pair_impl : implements<pair_impl<K, V>, Windows::Foundation::Collections::IKeyValuePair<K, V>> {
  pair_impl(K k, V v) : m_k(k), m_v(v) {}
  K Key() {
    return m_k;
  }
  V Value() {
    return m_v;
  }

 private:
  K m_k;
  V m_v;
};

template <typename K, typename V>
Windows::Foundation::Collections::IKeyValuePair<K, V> pair(K key, V value) {
  return make<pair_impl<K, V>>(key, value);
}
} // namespace winrt

namespace Microsoft::ReactNative {

std::pair<std::string, bool> GetJavaScriptFromServer(
    const std::string &sourceBundleHost,
    const uint16_t sourceBundlePort,
    const std::string &jsBundleName,
    const std::string &platform,
    const std::string &bundleAppId,
    bool dev,
    bool hot,
    bool inlineSourceMap,
    uint32_t hermesBytecodeVersion);

class DevSupportManager final : public facebook::react::IDevSupportManager {
 public:
  DevSupportManager() = default;
  ~DevSupportManager();

  virtual facebook::react::JSECreator LoadJavaScriptInProxyMode(
      const facebook::react::DevSettings &settings,
      std::function<void()> &&errorCallback) override;
  virtual void StartPollingLiveReload(
      const std::string &sourceBundleHost,
      const uint16_t sourceBundlePort,
      std::function<void()> onChangeCallback) override;
  virtual void StopPollingLiveReload() override;

  virtual void EnsureHermesInspector(const std::string &packagerHost, const uint16_t packagerPort) noexcept override;
  virtual void UpdateBundleStatus(bool isLastDownloadSuccess, int64_t updateTimestamp) noexcept override;

 private:
  std::atomic_bool m_cancellation_token;

  std::shared_ptr<InspectorPackagerConnection> m_inspectorPackagerConnection;

  struct BundleStatusProvider : public InspectorPackagerConnection::IBundleStatusProvider {
    virtual InspectorPackagerConnection::BundleStatus getBundleStatus() {
      return m_bundleStatus;
    }

    void updateBundleStatus(bool isLastDownloadSuccess, int64_t updateTimestamp) {
      m_bundleStatus.m_isLastDownloadSuccess = isLastDownloadSuccess;
      m_bundleStatus.m_updateTimestamp = updateTimestamp;
    }

   private:
    InspectorPackagerConnection::BundleStatus m_bundleStatus;
  };
  std::shared_ptr<BundleStatusProvider> m_BundleStatusProvider = std::make_shared<BundleStatusProvider>();
};

} // namespace Microsoft::ReactNative
