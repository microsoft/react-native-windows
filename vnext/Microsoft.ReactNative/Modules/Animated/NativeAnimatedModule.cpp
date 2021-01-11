// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"

#include "NativeAnimatedModule.h"

#include <IReactDispatcher.h>
#include <cxxreact/Instance.h>
#include <cxxreact/JsArgumentHelpers.h>

namespace react::uwp {
const char *NativeAnimatedModule::name{"NativeAnimatedModule"};

NativeAnimatedModule::NativeAnimatedModule(Mso::CntPtr<Mso::React::IReactContext> &&context)
    : m_context(std::move(context)) {
  m_nodesManager = std::make_shared<NativeAnimatedNodeManager>(NativeAnimatedNodeManager());
}

NativeAnimatedModule::~NativeAnimatedModule() {
  auto uiDispatcher =
      winrt::Microsoft::ReactNative::implementation::ReactDispatcher::GetUIDispatcher(m_context->Properties());

  // To make sure that we destroy UI components in UI thread.
  if (!uiDispatcher.HasThreadAccess()) {
    uiDispatcher.Post([manager = std::move(m_nodesManager)]() {});
  }
}

std::vector<facebook::xplat::module::CxxModule::Method> NativeAnimatedModule::getMethods() {
  return {
      Method(
          "startOperationBatch",
          [](folly::dynamic /*args*/) {
            // NYI #6161
          }),

      Method(
          "finishOperationBatch",
          [](folly::dynamic /*args*/) {
            // NYI #6161
          }),
      Method(
          "createAnimatedNode",
          [this](folly::dynamic args) {
            const auto tag = facebook::xplat::jsArgAsInt(args, 0);
            const auto config = facebook::xplat::jsArgAsObject(args, 1);
            NativeAnimatedModule::CreateAnimatedNode(tag, config);
          }),
      Method(
          "getValue",
          [this](folly::dynamic args, Callback saveValueCallback) {
            const auto animatedNodeTag = facebook::xplat::jsArgAsInt(args, 0);
            NativeAnimatedModule::GetValue(animatedNodeTag, saveValueCallback);
          }),
      Method(
          "connectAnimatedNodeToView",
          [this](folly::dynamic args) {
            const auto tag = facebook::xplat::jsArgAsInt(args, 0);
            const auto viewTag = facebook::xplat::jsArgAsInt(args, 1);
            NativeAnimatedModule::ConnectAnimatedNodeToView(tag, viewTag);
          }),
      Method(
          "disconnectAnimatedNodeFromView",
          [this](folly::dynamic args) {
            const auto tag = facebook::xplat::jsArgAsInt(args, 0);
            const auto viewTag = facebook::xplat::jsArgAsInt(args, 1);
            NativeAnimatedModule::DisconnectAnimatedNodeFromView(tag, viewTag);
          }),
      Method(
          "connectAnimatedNodes",
          [this](folly::dynamic args) {
            const auto parentTag = facebook::xplat::jsArgAsInt(args, 0);
            const auto childTag = facebook::xplat::jsArgAsInt(args, 1);
            NativeAnimatedModule::ConnectAnimatedNodes(parentTag, childTag);
          }),
      Method(
          "disconnectAnimatedNodes",
          [this](folly::dynamic args) {
            const auto parentTag = facebook::xplat::jsArgAsInt(args, 0);
            const auto childTag = facebook::xplat::jsArgAsInt(args, 1);
            NativeAnimatedModule::DisconnectAnimatedNodes(parentTag, childTag);
          }),
      Method(
          "stopAnimation",
          [this](folly::dynamic args) {
            const auto animationId = facebook::xplat::jsArgAsInt(args, 0);
            NativeAnimatedModule::StopAnimation(animationId);
          }),
      Method(
          "startAnimatingNode",
          [this](folly::dynamic args, Callback endCallback) {
            const auto animationId = facebook::xplat::jsArgAsInt(args, 0);
            const auto animatedNodeTag = facebook::xplat::jsArgAsInt(args, 1);
            const auto animationConfig = facebook::xplat::jsArgAsObject(args, 2);
            NativeAnimatedModule::StartAnimatingNode(animationId, animatedNodeTag, animationConfig, endCallback);
          }),
      Method(
          "dropAnimatedNode",
          [this](folly::dynamic args) {
            const auto tag = facebook::xplat::jsArgAsInt(args, 0);
            NativeAnimatedModule::DropAnimatedNode(tag);
          }),
      Method(
          "setAnimatedNodeValue",
          [this](folly::dynamic args) {
            const auto tag = facebook::xplat::jsArgAsInt(args, 0);
            const auto value = facebook::xplat::jsArgAsDouble(args, 1);
            NativeAnimatedModule::SetAnimatedNodeValue(tag, value);
          }),
      Method(
          "setAnimatedNodeOffset",
          [this](folly::dynamic args) {
            const auto tag = facebook::xplat::jsArgAsInt(args, 0);
            const auto value = facebook::xplat::jsArgAsDouble(args, 1);
            NativeAnimatedModule::SetAnimatedNodeOffset(tag, value);
          }),
      Method(
          "flattenAnimatedNodeOffset",
          [this](folly::dynamic args) {
            const auto tag = facebook::xplat::jsArgAsInt(args, 0);
            NativeAnimatedModule::FlattenAnimatedNodeOffset(tag);
          }),
      Method(
          "extractAnimatedNodeOffset",
          [this](folly::dynamic args) {
            const auto tag = facebook::xplat::jsArgAsInt(args, 0);
            NativeAnimatedModule::ExtractAnimatedNodeOffset(tag);
          }),
      Method(
          "addAnimatedEventToView",
          [this](folly::dynamic args) {
            const auto viewTag = facebook::xplat::jsArgAsInt(args, 0);
            const auto eventName = facebook::xplat::jsArgAsString(args, 1);
            const auto eventMapping = facebook::xplat::jsArgAsObject(args, 2);
            NativeAnimatedModule::AddAnimatedEventToView(viewTag, eventName, eventMapping);
          }),
      Method(
          "removeAnimatedEventFromView",
          [this](folly::dynamic args) {
            const auto viewTag = facebook::xplat::jsArgAsInt(args, 0);
            const auto eventName = facebook::xplat::jsArgAsString(args, 1);
            const auto animatedValueTag = facebook::xplat::jsArgAsInt(args, 2);
            NativeAnimatedModule::RemoveAnimatedEventFromView(viewTag, eventName, animatedValueTag);
          }),
      Method(
          "startListeningToAnimatedNodeValue",
          [this](folly::dynamic args) {
            const auto viewTag = facebook::xplat::jsArgAsInt(args, 0);
            NativeAnimatedModule::StartListeningToAnimatedNodeValue(viewTag);
          }),
      Method(
          "stopListeningToAnimatedNodeValue",
          [this](folly::dynamic args) {
            const auto viewTag = facebook::xplat::jsArgAsInt(args, 0);
            NativeAnimatedModule::StopListeningToAnimatedNodeValue(viewTag);
          }),
  };
}

void NativeAnimatedModule::CreateAnimatedNode(int64_t tag, const folly::dynamic &config) {
  m_nodesManager->CreateAnimatedNode(tag, config, m_context, m_nodesManager);
}

void NativeAnimatedModule::GetValue(int64_t animatedNodeTag, const Callback &saveValueCallback) {
  m_nodesManager->GetValue(animatedNodeTag, saveValueCallback);
}

void NativeAnimatedModule::ConnectAnimatedNodeToView(int64_t animatedNodeTag, int64_t viewTag) {
  m_nodesManager->ConnectAnimatedNodeToView(animatedNodeTag, viewTag);
}

void NativeAnimatedModule::DisconnectAnimatedNodeFromView(int64_t animatedNodeTag, int64_t viewTag) {
  m_nodesManager->DisconnectAnimatedNodeToView(animatedNodeTag, viewTag);
}

void NativeAnimatedModule::ConnectAnimatedNodes(int64_t parentNodeTag, int64_t childNodeTag) {
  m_nodesManager->ConnectAnimatedNode(parentNodeTag, childNodeTag);
}

void NativeAnimatedModule::DisconnectAnimatedNodes(int64_t parentNodeTag, int64_t childNodeTag) {
  m_nodesManager->DisconnectAnimatedNode(parentNodeTag, childNodeTag);
}

void NativeAnimatedModule::StartAnimatingNode(
    int64_t animationId,
    int64_t animatedNodeTag,
    const folly::dynamic &animationConfig,
    const Callback &endCallback) {
  m_nodesManager->StartAnimatingNode(animationId, animatedNodeTag, animationConfig, endCallback, m_nodesManager);
}

void NativeAnimatedModule::StopAnimation(int64_t animationId) {
  m_nodesManager->StopAnimation(animationId);
}

void NativeAnimatedModule::DropAnimatedNode(int64_t tag) {
  m_nodesManager->DropAnimatedNode(tag);
}

void NativeAnimatedModule::SetAnimatedNodeValue(int64_t tag, double value) {
  m_nodesManager->SetAnimatedNodeValue(tag, value);
}

void NativeAnimatedModule::SetAnimatedNodeOffset(int64_t tag, double offset) {
  m_nodesManager->SetAnimatedNodeOffset(tag, offset);
}

void NativeAnimatedModule::FlattenAnimatedNodeOffset(int64_t tag) {
  m_nodesManager->FlattenAnimatedNodeOffset(tag);
}

void NativeAnimatedModule::ExtractAnimatedNodeOffset(int64_t tag) {
  m_nodesManager->ExtractAnimatedNodeOffset(tag);
}

void NativeAnimatedModule::AddAnimatedEventToView(
    int64_t tag,
    const std::string &eventName,
    const folly::dynamic &eventMapping) {
  m_nodesManager->AddAnimatedEventToView(tag, eventName, eventMapping, m_nodesManager);
}

void NativeAnimatedModule::RemoveAnimatedEventFromView(
    int64_t tag,
    const std::string &eventName,
    int64_t animatedValueTag) {
  m_nodesManager->RemoveAnimatedEventFromView(tag, eventName, animatedValueTag);
}

void NativeAnimatedModule::StartListeningToAnimatedNodeValue(int64_t /*tag*/) {
  // NotImplemented
}

void NativeAnimatedModule::StopListeningToAnimatedNodeValue(int64_t /*tag*/) {
  // NotImplemented
}
} // namespace react::uwp
