// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"

#include "PackagerConnection.h"

#include <Shared/DevServerHelper.h>
#include <Utils/CppWinrtLessExceptions.h>
#include <Windows.Storage.Streams.h>
#include <winrt/Microsoft.ReactNative.h>
#include <winrt/Windows.Data.Json.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Storage.Streams.h>
#include "Unicode.h"
#include "Utilities.h"

namespace Microsoft::ReactNative {

/*static*/ std::mutex PackagerConnection::m_mutexPackagers;
/*static*/ std::unordered_map<std::pair<std::string, uint16_t>, std::shared_ptr<PackagerConnection>>
    PackagerConnection::m_connections;

winrt::Microsoft::ReactNative::IReactPropertyNamespace PackagerNamespace() {
  static const winrt::Microsoft::ReactNative::IReactPropertyNamespace ns =
      winrt::Microsoft::ReactNative::ReactPropertyBagHelper::GetNamespace(L"ReactNative.PackagerConnection");
  return ns;
}

winrt::Microsoft::ReactNative::IReactPropertyName ReloadEventName() {
  static const winrt::Microsoft::ReactNative::IReactPropertyName propName =
      winrt::Microsoft::ReactNative::ReactPropertyBagHelper::GetName(PackagerNamespace(), L"Reload");
  return propName;
}

winrt::Microsoft::ReactNative::IReactPropertyName DevMenuEventName() {
  static const winrt::Microsoft::ReactNative::IReactPropertyName propName =
      winrt::Microsoft::ReactNative::ReactPropertyBagHelper::GetName(PackagerNamespace(), L"DevMenu");
  return propName;
}

/* static */ std::shared_ptr<PackagerConnection> PackagerConnection::GetOrCreate(
    const std::string &sourceBundleHost,
    uint16_t sourceBundlePort) {
  std::shared_ptr<PackagerConnection> packager;
  {
    std::lock_guard<std::mutex> guard(m_mutexPackagers);
    auto key = std::pair<std::string, uint16_t>(sourceBundleHost, sourceBundlePort);
    auto it = m_connections.find(key);

    if (it != m_connections.end()) {
      return it->second;
    }

    packager = std::make_shared<PackagerConnection>(sourceBundleHost, sourceBundlePort);
    m_connections.emplace(key, packager);
  }

  packager->Connect();
  return packager;
}

void PackagerConnection::CreateOrReusePackagerConnection(const facebook::react::DevSettings &settings) {
  auto packager = GetOrCreate(settings.sourceBundleHost, settings.sourceBundlePort);
  packager->SubscribeReloadEvent(
      [callback = settings.liveReloadCallback](
          winrt::Windows::Foundation::IInspectable const & /*sender*/,
          winrt::Microsoft::ReactNative::IReactNotificationArgs const & /*args*/) { callback(); });
  packager->SubscribeDevMenuEvent(
      [callback = settings.showDevMenuCallback](
          winrt::Windows::Foundation::IInspectable const & /*sender*/,
          winrt::Microsoft::ReactNative::IReactNotificationArgs const & /*args*/) { callback(); });
}

PackagerConnection::PackagerConnection(const std::string &sourceBundleHost, uint16_t sourceBundlePort)
    : m_sourceBundleHost(sourceBundleHost),
      m_sourceBundlePort(sourceBundlePort),
      m_notificationService(
          winrt::Microsoft::ReactNative::ReactNotificationServiceHelper::CreateNotificationService()) {}

void PackagerConnection::SubscribeReloadEvent(winrt::Microsoft::ReactNative::ReactNotificationHandler const &handler) {
  m_notificationService.Subscribe(ReloadEventName(), nullptr, handler);
}

void PackagerConnection::SubscribeDevMenuEvent(winrt::Microsoft::ReactNative::ReactNotificationHandler const &handler) {
  m_notificationService.Subscribe(DevMenuEventName(), nullptr, handler);
}

winrt::fire_and_forget PackagerConnection::Connect() {
  m_ws = winrt::Windows::Networking::Sockets::MessageWebSocket();

  m_wsMessageRevoker = m_ws.MessageReceived(
      winrt::auto_revoke, [weakThis = this->weak_from_this()](auto && /*sender*/, auto &&args) noexcept {
        try {
          if (auto strongThis = weakThis.lock()) {
            // Use ABI to avoid throwing exceptions on standard errors
            winrt::com_ptr<ABI::Windows::Networking::Sockets::IMessageWebSocketMessageReceivedEventArgs> abiArgs{
                args.as<ABI::Windows::Networking::Sockets::IMessageWebSocketMessageReceivedEventArgs>()};
            winrt::com_ptr<ABI::Windows::Storage::Streams::IDataReader> abiReader;
            auto hr = abiArgs->GetDataReader(abiReader.put());
            if (FAILED(hr)) {
              return;
            }

            winrt::Windows::Storage::Streams::DataReader reader{
                abiReader.as<winrt::Windows::Storage::Streams::DataReader>()};

            uint32_t len = reader.UnconsumedBufferLength();
            if (args.MessageType() == winrt::Windows::Networking::Sockets::SocketMessageType::Utf8) {
              reader.UnicodeEncoding(winrt::Windows::Storage::Streams::UnicodeEncoding::Utf8);
              std::vector<uint8_t> data(len);
              reader.ReadBytes(data);

              auto response =
                  std::string(Microsoft::Common::Utilities::CheckedReinterpretCast<char *>(data.data()), data.size());
              auto json =
                  winrt::Windows::Data::Json::JsonObject::Parse(Microsoft::Common::Unicode::Utf8ToUtf16(response));

              auto version = (int)json.GetNamedNumber(L"version", 0.0);
              if (version != 2) {
                return;
              }
              auto method = std::wstring(json.GetNamedString(L"method", L""));
              if (method.empty()) {
                return;
              }

              if (!method.compare(L"reload")) {
                strongThis->m_notificationService.SendNotification(ReloadEventName(), nullptr, nullptr);
              } else if (!method.compare(L"devMenu")) {
                strongThis->m_notificationService.SendNotification(DevMenuEventName(), nullptr, nullptr);
              } else if (!method.compare(L"captureHeap")) {
                // TODO captureHeap
              }
            }
          }
        } catch (winrt::hresult_error const &e) {
        }
      });

  winrt::Windows::Foundation::Uri uri(Microsoft::Common::Unicode::Utf8ToUtf16(
      facebook::react::DevServerHelper::get_PackagerConnectionUrl(m_sourceBundleHost, m_sourceBundlePort)));
  auto async = m_ws.ConnectAsync(uri);

#ifdef DEFAULT_CPPWINRT_EXCEPTIONS
  co_await async;
#else
  co_await lessthrow_await_adapter<winrt::Windows::Foundation::IAsyncAction>{async};
#endif
}

} // namespace Microsoft::ReactNative
