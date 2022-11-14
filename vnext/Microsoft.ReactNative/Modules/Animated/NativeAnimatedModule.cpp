// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"

#include "NativeAnimatedModule.h"

#include <DynamicWriter.h>
#include <ReactCoreInjection.h>

namespace Microsoft::ReactNative {

void NativeAnimatedModule::Initialize(winrt::Microsoft::ReactNative::ReactContext const &reactContext) noexcept {
  m_context = reactContext;
  m_nodesManager = std::make_shared<NativeAnimatedNodeManager>(reactContext);
}

void NativeAnimatedModule::startOperationBatch() noexcept {
  // NYI #6161
}

void NativeAnimatedModule::finishOperationBatch() noexcept {
  // NYI #6161
}

void NativeAnimatedModule::createAnimatedNode(double tag, ::React::JSValue &&config) noexcept {
  winrt::Microsoft::ReactNative::implementation::ReactCoreInjection::PostToUIBatchingQueue(
      m_context.Handle(),
      [wkThis = std::weak_ptr(this->shared_from_this()),
       config = config.MoveObject(),
       tag = static_cast<int64_t>(tag)]() {
        if (auto pThis = wkThis.lock()) {
          pThis->m_nodesManager->CreateAnimatedNode(tag, config, pThis->m_context, pThis->m_nodesManager);
        }
      });
}

// void NativeAnimatedModule::updateAnimatedNodeConfig(double tag, ::React::JSValue && config) noexcept {
//  NYI
//}

void NativeAnimatedModule::getValue(double tag, std::function<void(double)> const &saveValueCallback) noexcept {
  winrt::Microsoft::ReactNative::implementation::ReactCoreInjection::PostToUIBatchingQueue(
      m_context.Handle(),
      [wkThis = std::weak_ptr(this->shared_from_this()),
       tag = static_cast<int64_t>(tag),
       callback = std::move(saveValueCallback)]() {
        if (auto pThis = wkThis.lock()) {
          pThis->m_nodesManager->GetValue(
              tag, [context = pThis->m_context, callback = std::move(callback)](double value) {
                context.JSDispatcher().Post([callback = std::move(callback), value]() { callback(value); });
              });
        }
      });
}

void NativeAnimatedModule::startListeningToAnimatedNodeValue(double tag) noexcept {
  winrt::Microsoft::ReactNative::implementation::ReactCoreInjection::PostToUIBatchingQueue(
      m_context.Handle(), [wkThis = std::weak_ptr(this->shared_from_this()), nodeTag = static_cast<int64_t>(tag)]() {
        if (auto pThis = wkThis.lock()) {
          pThis->m_nodesManager->StartListeningToAnimatedNodeValue(
              nodeTag, [context = pThis->m_context, nodeTag](double value) {
                // This event could be coalesced, however it doesn't appear to be
                // coalesced on Android or iOS, so leaving it without coalescing.
                context.EmitJSEvent(
                    L"RCTDeviceEventEmitter",
                    L"onAnimatedValueUpdate",
                    ::React::JSValueObject{{"tag", nodeTag}, {"value", value}});
              });
        }
      });
}

void NativeAnimatedModule::stopListeningToAnimatedNodeValue(double tag) noexcept {
  winrt::Microsoft::ReactNative::implementation::ReactCoreInjection::PostToUIBatchingQueue(
      m_context.Handle(), [wkThis = std::weak_ptr(this->shared_from_this()), nodeTag = static_cast<int64_t>(tag)]() {
        if (auto pThis = wkThis.lock()) {
          pThis->m_nodesManager->StopListeningToAnimatedNodeValue(nodeTag);
        }
      });
}

void NativeAnimatedModule::connectAnimatedNodes(double parentTag, double childTag) noexcept {
  winrt::Microsoft::ReactNative::implementation::ReactCoreInjection::PostToUIBatchingQueue(
      m_context.Handle(),
      [wkThis = std::weak_ptr(this->shared_from_this()),
       parentTag = static_cast<int64_t>(parentTag),
       childTag = static_cast<int64_t>(childTag)]() {
        if (auto pThis = wkThis.lock()) {
          pThis->m_nodesManager->ConnectAnimatedNode(parentTag, childTag);
        }
      });
}

void NativeAnimatedModule::disconnectAnimatedNodes(double parentTag, double childTag) noexcept {
  winrt::Microsoft::ReactNative::implementation::ReactCoreInjection::PostToUIBatchingQueue(
      m_context.Handle(),
      [wkThis = std::weak_ptr(this->shared_from_this()),
       parentTag = static_cast<int64_t>(parentTag),
       childTag = static_cast<int64_t>(childTag)]() {
        if (auto pThis = wkThis.lock()) {
          pThis->m_nodesManager->DisconnectAnimatedNode(parentTag, childTag);
        }
      });
}

void NativeAnimatedModule::startAnimatingNode(
    double animationId,
    double nodeTag,
    ::React::JSValue &&config,
    std::function<void(ReactNativeSpecs::AnimatedModuleSpec_EndResult const &)> const &endCallback) noexcept {
  winrt::Microsoft::ReactNative::implementation::ReactCoreInjection::PostToUIBatchingQueue(
      m_context.Handle(),
      [wkThis = std::weak_ptr(this->shared_from_this()),
       animationId = static_cast<int64_t>(animationId),
       nodeTag = static_cast<int64_t>(nodeTag),
       animationConfig = std::move(config),
       endCallback = std::move(endCallback)]() {
        if (auto pThis = wkThis.lock()) {
          pThis->m_nodesManager->StartAnimatingNode(
              animationId,
              nodeTag,
              animationConfig.AsObject(),
              [context = pThis->m_context, endCallback = std::move(endCallback)](bool finished) {
                context.JSDispatcher().Post([finished, endCallback = std::move(endCallback)]() {
                  ReactNativeSpecs::AnimatedModuleSpec_EndResult result;
                  result.finished = finished;
                  endCallback(std::move(result));
                });
              },
              pThis->m_nodesManager);
        }
      });
}

void NativeAnimatedModule::stopAnimation(double animationId) noexcept {
  winrt::Microsoft::ReactNative::implementation::ReactCoreInjection::PostToUIBatchingQueue(
      m_context.Handle(),
      [wkThis = std::weak_ptr(this->shared_from_this()), animationId = static_cast<int64_t>(animationId)]() {
        if (auto pThis = wkThis.lock()) {
          pThis->m_nodesManager->StopAnimation(animationId);
        }
      });
}

void NativeAnimatedModule::setAnimatedNodeValue(double nodeTag, double value) noexcept {
  winrt::Microsoft::ReactNative::implementation::ReactCoreInjection::PostToUIBatchingQueue(
      m_context.Handle(),
      [wkThis = std::weak_ptr(this->shared_from_this()), nodeTag = static_cast<int64_t>(nodeTag), value]() {
        if (auto pThis = wkThis.lock()) {
          pThis->m_nodesManager->SetAnimatedNodeValue(nodeTag, value);
        }
      });
}

void NativeAnimatedModule::setAnimatedNodeOffset(double nodeTag, double offset) noexcept {
  winrt::Microsoft::ReactNative::implementation::ReactCoreInjection::PostToUIBatchingQueue(
      m_context.Handle(),
      [wkThis = std::weak_ptr(this->shared_from_this()), nodeTag = static_cast<int64_t>(nodeTag), offset]() {
        if (auto pThis = wkThis.lock()) {
          pThis->m_nodesManager->SetAnimatedNodeOffset(nodeTag, offset);
        }
      });
}

void NativeAnimatedModule::flattenAnimatedNodeOffset(double nodeTag) noexcept {
  winrt::Microsoft::ReactNative::implementation::ReactCoreInjection::PostToUIBatchingQueue(
      m_context.Handle(),
      [wkThis = std::weak_ptr(this->shared_from_this()), nodeTag = static_cast<int64_t>(nodeTag)]() {
        if (auto pThis = wkThis.lock()) {
          pThis->m_nodesManager->FlattenAnimatedNodeOffset(nodeTag);
        }
      });
}

void NativeAnimatedModule::extractAnimatedNodeOffset(double nodeTag) noexcept {
  winrt::Microsoft::ReactNative::implementation::ReactCoreInjection::PostToUIBatchingQueue(
      m_context.Handle(),
      [wkThis = std::weak_ptr(this->shared_from_this()), nodeTag = static_cast<int64_t>(nodeTag)]() {
        if (auto pThis = wkThis.lock()) {
          pThis->m_nodesManager->ExtractAnimatedNodeOffset(nodeTag);
        }
      });
}

void NativeAnimatedModule::connectAnimatedNodeToView(double nodeTag, double viewTag) noexcept {
  winrt::Microsoft::ReactNative::implementation::ReactCoreInjection::PostToUIBatchingQueue(
      m_context.Handle(),
      [wkThis = std::weak_ptr(this->shared_from_this()),
       nodeTag = static_cast<int64_t>(nodeTag),
       viewTag = static_cast<int64_t>(viewTag)]() {
        if (auto pThis = wkThis.lock()) {
          pThis->m_nodesManager->ConnectAnimatedNodeToView(nodeTag, viewTag);
        }
      });
}

void NativeAnimatedModule::disconnectAnimatedNodeFromView(double nodeTag, double viewTag) noexcept {
  winrt::Microsoft::ReactNative::implementation::ReactCoreInjection::PostToUIBatchingQueue(
      m_context.Handle(),
      [wkThis = std::weak_ptr(this->shared_from_this()),
       nodeTag = static_cast<int64_t>(nodeTag),
       viewTag = static_cast<int64_t>(viewTag)]() {
        if (auto pThis = wkThis.lock()) {
          pThis->m_nodesManager->RestoreDefaultValues(viewTag);
          pThis->m_nodesManager->DisconnectAnimatedNodeToView(nodeTag, viewTag);
        }
      });
}

void NativeAnimatedModule::restoreDefaultValues(double nodeTag) noexcept {
  winrt::Microsoft::ReactNative::implementation::ReactCoreInjection::PostToUIBatchingQueue(
      m_context.Handle(),
      [wkThis = std::weak_ptr(this->shared_from_this()), nodeTag = static_cast<int64_t>(nodeTag)]() {
        if (auto pThis = wkThis.lock()) {
          pThis->m_nodesManager->RestoreDefaultValues(nodeTag);
        }
      });
}

void NativeAnimatedModule::dropAnimatedNode(double tag) noexcept {
  winrt::Microsoft::ReactNative::implementation::ReactCoreInjection::PostToUIBatchingQueue(
      m_context.Handle(), [wkThis = std::weak_ptr(this->shared_from_this()), tag = static_cast<int64_t>(tag)]() {
        if (auto pThis = wkThis.lock()) {
          pThis->m_nodesManager->DropAnimatedNode(tag);
        }
      });
}

void NativeAnimatedModule::addAnimatedEventToView(
    double viewTag,
    std::string eventName,
    ReactNativeSpecs::AnimatedModuleSpec_EventMapping &&eventMapping) noexcept {
  winrt::Microsoft::ReactNative::implementation::ReactCoreInjection::PostToUIBatchingQueue(
      m_context.Handle(),
      [wkThis = std::weak_ptr(this->shared_from_this()),
       viewTag = static_cast<int64_t>(viewTag),
       eventName = std::move(eventName),
       eventMapping = std::move(eventMapping)]() {
        if (auto pThis = wkThis.lock()) {
          pThis->m_nodesManager->AddAnimatedEventToView(viewTag, eventName, eventMapping, pThis->m_nodesManager);
        }
      });
}

void NativeAnimatedModule::removeAnimatedEventFromView(
    double viewTag,
    std::string eventName,
    double animatedNodeTag) noexcept {
  winrt::Microsoft::ReactNative::implementation::ReactCoreInjection::PostToUIBatchingQueue(
      m_context.Handle(),
      [wkThis = std::weak_ptr(this->shared_from_this()),
       viewTag = static_cast<int64_t>(viewTag),
       eventName = std::move(eventName),
       animatedNodeTag = static_cast<int64_t>(animatedNodeTag)]() {
        if (auto pThis = wkThis.lock()) {
          pThis->m_nodesManager->RemoveAnimatedEventFromView(viewTag, eventName, animatedNodeTag);
        }
      });
}

void NativeAnimatedModule::addListener(std::string eventName) noexcept {
  // no-op
}

void NativeAnimatedModule::removeListeners(double count) noexcept {
  // no-op
}

void NativeAnimatedModule::queueAndExecuteBatchedOperations(::React::JSValueArray &&operationsAndArgs) noexcept {
  assert(false); // JS currently only calls this on Android
}

} // namespace Microsoft::ReactNative
