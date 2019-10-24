// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"

#include "NativeAnimatedModule.h"

#include <cxxreact/Instance.h>
#include <cxxreact/JsArgumentHelpers.h>
#include <winrt/Windows.ApplicationModel.DataTransfer.h>
#include "Unicode.h"

namespace react {
namespace uwp {
const char *NativeAnimatedModule::name{"NativeAnimatedModule"};
const char *NativeAnimatedModule::s_createAnimatedNodeName{
    "createAnimatedNode"};
const char *NativeAnimatedModule::s_connectAnimatedNodeToViewName{
    "connectAnimatedNodeToView"};
const char *NativeAnimatedModule::s_disconnectAnimatedNodeFromViewName{
    "disconnectAnimatedNodeFromView"};
const char *NativeAnimatedModule::s_connectAnimatedNodesName{
    "connectAnimatedNodes"};
const char *NativeAnimatedModule::s_disconnectAnimatedNodesName{
    "disconnectAnimatedNodes"};
const char *NativeAnimatedModule::s_stopAnimationName{"stopAnimation"};
const char *NativeAnimatedModule::s_startAnimatingNodeName{
    "startAnimatingNode"};
const char *NativeAnimatedModule::s_dropAnimatedNodeName{"dropAnimatedNode"};
const char *NativeAnimatedModule::s_setAnimatedNodeValueName{
    "setAnimatedNodeValue"};
const char *NativeAnimatedModule::s_setAnimatedNodeOffsetName{
    "setAnimatedNodeOffset"};
const char *NativeAnimatedModule::s_flattenAnimatedNodeOffsetName{
    "flattenAnimatedNodeOffset"};
const char *NativeAnimatedModule::s_extractAnimatedNodeOffsetName{
    "extractAnimatedNodeOffset"};
const char *NativeAnimatedModule::s_addAnimatedEventToViewName{
    "addAnimatedEventToView"};
const char *NativeAnimatedModule::s_removeAnimatedEventFromViewName{
    "removeAnimatedEventFromView"};
const char *NativeAnimatedModule::s_startListeningToAnimatedNodeValueName{
    "startListeningToAnimatedNodeValue" };
const char *NativeAnimatedModule::s_stopListeningToAnimatedNodeValueName{
    "stopListeningToAnimatedNodeValue" };

NativeAnimatedModule::NativeAnimatedModule(
    const std::weak_ptr<IReactInstance> &reactInstance)
    : m_wkReactInstance(reactInstance) {
  m_nodesManager =
      std::make_shared<NativeAnimatedNodeManager>(NativeAnimatedNodeManager());
}

std::vector<facebook::xplat::module::CxxModule::Method>
NativeAnimatedModule::getMethods() {
  return {
      Method(
          s_createAnimatedNodeName,
          [this](folly::dynamic args) {
            const auto tag = facebook::xplat::jsArgAsInt(args, 0);
            const auto config = facebook::xplat::jsArgAsObject(args, 1);
            NativeAnimatedModule::CreateAnimatedNode(tag, config);
          }),
      Method(
          s_connectAnimatedNodeToViewName,
          [this](folly::dynamic args) {
            const auto tag = facebook::xplat::jsArgAsInt(args, 0);
            const auto viewTag = facebook::xplat::jsArgAsInt(args, 1);
            NativeAnimatedModule::ConnectAnimatedNodeToView(tag, viewTag);
          }),
      Method(
          s_disconnectAnimatedNodeFromViewName,
          [this](folly::dynamic args) {
            const auto tag = facebook::xplat::jsArgAsInt(args, 0);
            const auto viewTag = facebook::xplat::jsArgAsInt(args, 1);
            NativeAnimatedModule::DisconnectAnimatedNodeFromView(tag, viewTag);
          }),
      Method(
          s_connectAnimatedNodesName,
          [this](folly::dynamic args) {
            const auto parentTag = facebook::xplat::jsArgAsInt(args, 0);
            const auto childTag = facebook::xplat::jsArgAsInt(args, 1);
            NativeAnimatedModule::ConnectAnimatedNodes(parentTag, childTag);
          }),
      Method(
          s_disconnectAnimatedNodesName,
          [this](folly::dynamic args) {
            const auto parentTag = facebook::xplat::jsArgAsInt(args, 0);
            const auto childTag = facebook::xplat::jsArgAsInt(args, 1);
            NativeAnimatedModule::DisconnectAnimatedNodes(parentTag, childTag);
          }),
      Method(
          s_stopAnimationName,
          [this](folly::dynamic args) {
            const auto animationId = facebook::xplat::jsArgAsInt(args, 0);
            NativeAnimatedModule::StopAnimation(animationId);
          }),
      Method(
          s_startAnimatingNodeName,
          [this](folly::dynamic args, Callback endCallback) {
            const auto animationId = facebook::xplat::jsArgAsInt(args, 0);
            const auto animatedNodeTag = facebook::xplat::jsArgAsInt(args, 1);
            const auto animationConfig =
                facebook::xplat::jsArgAsObject(args, 2);
            NativeAnimatedModule::StartAnimatingNode(
                animationId, animatedNodeTag, animationConfig, endCallback);
          }),
      Method(
          s_dropAnimatedNodeName,
          [this](folly::dynamic args) {
            const auto tag = facebook::xplat::jsArgAsInt(args, 0);
            NativeAnimatedModule::DropAnimatedNode(tag);
          }),
      Method(
          s_setAnimatedNodeValueName,
          [this](folly::dynamic args) {
            const auto tag = facebook::xplat::jsArgAsInt(args, 0);
            const auto value = facebook::xplat::jsArgAsDouble(args, 1);
            NativeAnimatedModule::SetAnimatedNodeValue(tag, value);
          }),
      Method(
          s_setAnimatedNodeOffsetName,
          [this](folly::dynamic args) {
            const auto tag = facebook::xplat::jsArgAsInt(args, 0);
            const auto value = facebook::xplat::jsArgAsDouble(args, 1);
            NativeAnimatedModule::SetAnimatedNodeOffset(tag, value);
          }),
      Method(
          s_flattenAnimatedNodeOffsetName,
          [this](folly::dynamic args) {
            const auto tag = facebook::xplat::jsArgAsInt(args, 0);
            NativeAnimatedModule::FlattenAnimatedNodeOffset(tag);
          }),
      Method(
          s_extractAnimatedNodeOffsetName,
          [this](folly::dynamic args) {
            const auto tag = facebook::xplat::jsArgAsInt(args, 0);
            NativeAnimatedModule::ExtractAnimatedNodeOffset(tag);
          }),
      Method(
          s_addAnimatedEventToViewName,
          [this](folly::dynamic args) {
            const auto viewTag = facebook::xplat::jsArgAsInt(args, 0);
            const auto eventName = facebook::xplat::jsArgAsString(args, 1);
            const auto eventMapping = facebook::xplat::jsArgAsObject(args, 2);
            NativeAnimatedModule::AddAnimatedEventToView(
                viewTag, eventName, eventMapping);
          }),
      Method(
          s_removeAnimatedEventFromViewName,
          [this](folly::dynamic args) {
            const auto viewTag = facebook::xplat::jsArgAsInt(args, 0);
            const auto eventName = facebook::xplat::jsArgAsString(args, 1);
            const auto animatedValueTag = facebook::xplat::jsArgAsInt(args, 2);
            NativeAnimatedModule::RemoveAnimatedEventFromView(
                viewTag, eventName, animatedValueTag);
          }),
      Method(
          s_startListeningToAnimatedNodeValueName,
          [this](folly::dynamic args) {
            const auto viewTag = facebook::xplat::jsArgAsInt(args, 0);
            NativeAnimatedModule::StartListeningToAnimatedNodeValue(viewTag);
          }),
      Method(
          s_stopListeningToAnimatedNodeValueName,
          [this](folly::dynamic args) {
            const auto viewTag = facebook::xplat::jsArgAsInt(args, 0);
            NativeAnimatedModule::StopListeningToAnimatedNodeValue(viewTag);
          }),
  };
}

void NativeAnimatedModule::CreateAnimatedNode(
    int64_t tag,
    const folly::dynamic &config) {
  m_nodesManager->CreateAnimatedNode(
      tag, config, m_wkReactInstance, m_nodesManager);
}

void NativeAnimatedModule::ConnectAnimatedNodeToView(
    int64_t animatedNodeTag,
    int64_t viewTag) {
  m_nodesManager->ConnectAnimatedNodeToView(animatedNodeTag, viewTag);
}

void NativeAnimatedModule::DisconnectAnimatedNodeFromView(
    int64_t animatedNodeTag,
    int64_t viewTag) {
  m_nodesManager->DisconnectAnimatedNodeToView(animatedNodeTag, viewTag);
}

void NativeAnimatedModule::ConnectAnimatedNodes(
    int64_t parentNodeTag,
    int64_t childNodeTag) {
  m_nodesManager->ConnectAnimatedNode(parentNodeTag, childNodeTag);
}

void NativeAnimatedModule::DisconnectAnimatedNodes(
    int64_t parentNodeTag,
    int64_t childNodeTag) {
  m_nodesManager->DisconnectAnimatedNode(parentNodeTag, childNodeTag);
}

void NativeAnimatedModule::StartAnimatingNode(
    int64_t animationId,
    int64_t animatedNodeTag,
    const folly::dynamic &animationConfig,
    const Callback &endCallback) {
  m_nodesManager->StartAnimatingNode(
      animationId,
      animatedNodeTag,
      animationConfig,
      endCallback,
      m_nodesManager);
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
  m_nodesManager->AddAnimatedEventToView(
      tag, eventName, eventMapping, m_nodesManager);
}

void NativeAnimatedModule::RemoveAnimatedEventFromView(
    int64_t tag,
    const std::string &eventName,
    int64_t animatedValueTag) {
  m_nodesManager->RemoveAnimatedEventFromView(tag, eventName, animatedValueTag);
}

void NativeAnimatedModule::StartListeningToAnimatedNodeValue(int64_t tag) {
  // NotImplemented
}

void NativeAnimatedModule::StopListeningToAnimatedNodeValue(int64_t tag) {
  // NotImplemented
}
} // namespace uwp
} // namespace react
