
#include "pch.h"

#include <folly/json.h>
#include "InspectorPackagerConnection.h"

namespace Microsoft::ReactNative {

namespace {

struct InspectorProtocol {
  static constexpr const char InspectorMessage_EVENT[] = "event";
  static constexpr const char InspectorMessage_PAGEID[] = "pageId";
  static constexpr const char InspectorMessage_PAYLOAD[] = "payload";

  static constexpr const char InspectorMessage_eventName_wrappedEvent[] = "wrappedEvent";
  static constexpr const char InspectorMessage_eventName_getPages[] = "getPages";
  static constexpr const char InspectorMessage_eventName_connect[] = "connect";
  static constexpr const char InspectorMessage_eventName_disconnect[] = "disconnect";

  enum class EventType { GetPages, WrappedEvent, Connect, Disconnect };

  static EventType getEventType(const folly::dynamic &messageFromPackager) {
    std::string event = messageFromPackager.at(InspectorProtocol::InspectorMessage_EVENT).getString();
    if (!event.compare(InspectorMessage_eventName_getPages))
      return EventType::GetPages;
    if (!event.compare(InspectorMessage_eventName_wrappedEvent))
      return EventType::WrappedEvent;
    if (!event.compare(InspectorMessage_eventName_connect))
      return EventType::Connect;
    if (!event.compare(InspectorMessage_eventName_disconnect))
      return EventType::Disconnect;
    assert(false && "Unknown event!");
    std::abort();
  }

  static folly::dynamic getMessageForPackager(EventType eventType, folly::dynamic &&payload) {
    folly::dynamic response = folly::dynamic::object;

    switch (eventType) {
      case EventType::GetPages:
        response[InspectorProtocol::InspectorMessage_EVENT] = InspectorProtocol::InspectorMessage_eventName_getPages;
        break;
      case EventType::WrappedEvent:
        response[InspectorProtocol::InspectorMessage_EVENT] =
            InspectorProtocol::InspectorMessage_eventName_wrappedEvent;
        break;
      case EventType::Connect:
        response[InspectorProtocol::InspectorMessage_EVENT] = InspectorProtocol::InspectorMessage_eventName_connect;
        break;
      case EventType::Disconnect:
        response[InspectorProtocol::InspectorMessage_EVENT] = InspectorProtocol::InspectorMessage_eventName_disconnect;
        break;
      default:
        assert(false && "Unknown event Type.");
        std::abort();
    }

    response[InspectorProtocol::InspectorMessage_PAYLOAD] = std::move(payload);
    return response;
  }

  static folly::dynamic getGetPagesResponseForPackagerPayload(
      const std::vector<facebook::react::InspectorPage> &pages) {
    folly::dynamic payload = folly::dynamic::array;
    for (facebook::react::InspectorPage page : pages) {
      folly::dynamic pageDyn = folly::dynamic::object;
      pageDyn["id"] = page.id;
      pageDyn["title"] = page.title;
      pageDyn["vm"] = page.vm;

      pageDyn["isLastBundleDownloadSuccess"] = true;
      pageDyn["bundleUpdateTimestamp"] = "0";

      payload.push_back(pageDyn);
    }
    return payload;
  }

  static folly::dynamic getVMResponseForPackagerPayload(int64_t pageId, std::string &&messageFromVM) {
    folly::dynamic payload = folly::dynamic::object;
    payload[InspectorProtocol::InspectorMessage_eventName_wrappedEvent] = messageFromVM;
    payload[InspectorProtocol::InspectorMessage_PAGEID] = pageId;
    return payload;
  }

  static folly::dynamic getVMResponseOnDisconnectForPackagerPayload(int64_t pageId) {
    folly::dynamic payload = folly::dynamic::object;
    payload[InspectorProtocol::InspectorMessage_PAGEID] = pageId;
    return payload;
  }
};

} // namespace

RemoteConnection::RemoteConnection(int64_t pageId, const InspectorPackagerConnection &packagerConnection)
    : m_packagerConnection(packagerConnection), m_pageId(pageId) {}

void RemoteConnection::onMessage(std::string message) {
  folly::dynamic response = InspectorProtocol::getMessageForPackager(
      InspectorProtocol::EventType::WrappedEvent,
      InspectorProtocol::getVMResponseForPackagerPayload(m_pageId, std::move(message)));
  std::string responsestr = folly::toJson(response);
  m_packagerConnection.sendMessageToPackager(std::move(responsestr));
}

void RemoteConnection::onDisconnect() {
  folly::dynamic response = InspectorProtocol::getMessageForPackager(
      InspectorProtocol::EventType::Disconnect,
      InspectorProtocol::getVMResponseOnDisconnectForPackagerPayload(m_pageId));

  std::string responsestr = folly::toJson(response);
  m_packagerConnection.sendMessageToPackager(std::move(responsestr));
}

void InspectorPackagerConnection::sendMessageToPackager(std::string &&message) const {
  m_packagerWebSocketConnection->Send(std::move(message));
}

void InspectorPackagerConnection::sendMessageToVM(int64_t pageId, std::string &&message) {
  m_localConnections[pageId]->sendMessage(std::move(message));
}

InspectorPackagerConnection::InspectorPackagerConnection(std::string &&url) {
  std::vector<winrt::Windows::Security::Cryptography::Certificates::ChainValidationResult> certExceptions;
  m_packagerWebSocketConnection =
      std::make_shared<Microsoft::React::WinRTWebSocketResource>(std::move(url), std::move(certExceptions));

  m_packagerWebSocketConnection->SetOnError([this](const Microsoft::React::IWebSocketResource::Error &err) {
    assert(false && "Websocket connection failed !");
  });

  m_packagerWebSocketConnection->SetOnMessage([this](size_t length, const std::string &message, bool isBinary) {
    assert(!isBinary && "We don't expect any binary messages !");
    folly::dynamic messageDyn = folly::parseJson(message);

    InspectorProtocol::EventType eventType = InspectorProtocol::getEventType(messageDyn);
    switch (eventType) {
      case InspectorProtocol::EventType::GetPages: {
        std::vector<facebook::react::InspectorPage> inspetorPages = facebook::react::getInspectorInstance().getPages();
        folly::dynamic response = InspectorProtocol::getMessageForPackager(
            InspectorProtocol::EventType::GetPages,
            InspectorProtocol::getGetPagesResponseForPackagerPayload(inspetorPages));

        std::string responsestr = folly::toJson(response);
        sendMessageToPackager(std::move(responsestr));
      } break;

      case InspectorProtocol::EventType::WrappedEvent: {
        folly::dynamic payload = messageDyn[InspectorProtocol::InspectorMessage_PAYLOAD];
        int64_t pageId = payload[InspectorProtocol::InspectorMessage_PAGEID].asInt();
        std::string wrappedEvent = payload[InspectorProtocol::InspectorMessage_eventName_wrappedEvent].getString();
        sendMessageToVM(pageId, std::move(wrappedEvent));
      } break;

      case InspectorProtocol::EventType::Connect: {
        folly::dynamic payload = messageDyn[InspectorProtocol::InspectorMessage_PAYLOAD];
        int64_t pageId = payload[InspectorProtocol::InspectorMessage_PAGEID].asInt();
        m_localConnections[pageId] = facebook::react::getInspectorInstance().connect(
            static_cast<int>(pageId), std::make_unique<RemoteConnection>(pageId, *this));
      } break;

      case InspectorProtocol::EventType::Disconnect: {
        folly::dynamic payload = messageDyn[InspectorProtocol::InspectorMessage_PAYLOAD];
        int64_t pageId = payload[InspectorProtocol::InspectorMessage_PAGEID].asInt();
        m_localConnections.erase(pageId);
      } break;
    }
  });

  Microsoft::React::IWebSocketResource::Protocols protocols;
  Microsoft::React::IWebSocketResource::Options options;
  m_packagerWebSocketConnection->Connect(protocols, options);
}

} // namespace Microsoft::ReactNative
