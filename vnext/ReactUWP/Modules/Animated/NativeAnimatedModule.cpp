// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include "NativeAnimatedModule.h"

#include <winrt/Windows.ApplicationModel.DataTransfer.h>
#include "unicode.h"
#include <cxxreact/Instance.h>
#include <cxxreact/JsArgumentHelpers.h>

//#pragma warning(push)
//#pragma warning(disable : 4146)
//#include <cxxreact/JsArgumentHelpers.h>
//#pragma warning(pop)

#if _MSC_VER <= 1913
// VC 19 (2015-2017.6) cannot optimize co_await/cppwinrt usage
#pragma optimize( "", off )
#endif

namespace react { namespace uwp {
  const char* NativeAnimatedModule::name = "NativeAnimatedModule";

  NativeAnimatedModule::NativeAnimatedModule(const std::weak_ptr<IReactInstance>& reactInstance) : m_wkReactInstance(reactInstance)
  {
    m_nodesManager = std::make_shared<NativeAnimatedNodeManager>(NativeAnimatedNodeManager());
  }

  std::vector<facebook::xplat::module::CxxModule::Method> NativeAnimatedModule::getMethods()
  {
    return {
      Method("createAnimatedNode", [this](folly::dynamic args)
      {
        auto tag = facebook::xplat::jsArgAsInt(args, 0);
        auto config = facebook::xplat::jsArgAsObject(args, 1);
        NativeAnimatedModule::CreateAnimatedNode(tag, config);
      }),
      Method("connectAnimatedNodeToView", [this](folly::dynamic args)
      {
        auto tag = facebook::xplat::jsArgAsInt(args, 0);
        auto viewTag = facebook::xplat::jsArgAsInt(args, 1);
        NativeAnimatedModule::ConnectAnimatedNodeToView(tag, viewTag);
      }),
      Method("disconnectAnimatedNodeFromView", [this](folly::dynamic args)
      {
        auto tag = facebook::xplat::jsArgAsInt(args, 0);
        auto viewTag = facebook::xplat::jsArgAsInt(args, 1);
        NativeAnimatedModule::DisconnectAnimatedNodeFromView(tag, viewTag);
      }),
      Method("connectAnimatedNodes", [this](folly::dynamic args)
      {
        auto parentTag = facebook::xplat::jsArgAsInt(args, 0);
        auto childTag = facebook::xplat::jsArgAsInt(args, 1);
        NativeAnimatedModule::ConnectAnimatedNodes(parentTag, childTag);
      }),
      Method("disconnectAnimatedNodes", [this](folly::dynamic args)
      {
        auto parentTag = facebook::xplat::jsArgAsInt(args, 0);
        auto childTag = facebook::xplat::jsArgAsInt(args, 1);
        NativeAnimatedModule::DisconnectAnimatedNodes(parentTag, childTag);
      }),
      Method("stopAnimation", [this](folly::dynamic args)
      {
        auto animationId = facebook::xplat::jsArgAsInt(args, 0);
        NativeAnimatedModule::StopAnimation(animationId);
      }),
      Method("startAnimatingNode", [this](folly::dynamic args, Callback endCallback)
      {
        auto animationId = facebook::xplat::jsArgAsInt(args, 0);
        auto animatedNodeTag = facebook::xplat::jsArgAsInt(args, 1);
        auto animationConfig = facebook::xplat::jsArgAsObject(args, 2);
        NativeAnimatedModule::StartAnimatingNode(animationId, animatedNodeTag, animationConfig, endCallback);
      }),
      Method("dropAnimatedNode", [this](folly::dynamic args)
      {
        auto tag = facebook::xplat::jsArgAsInt(args, 0);
        NativeAnimatedModule::DropAnimatedNode(tag);
      }),
      Method("setAnimatedNodeValue", [this](folly::dynamic args)
      {
        auto tag = facebook::xplat::jsArgAsInt(args, 0);
        auto value = facebook::xplat::jsArgAsDouble(args, 1);
        NativeAnimatedModule::SetAnimatedNodeValue(tag, value);
      }),
      Method("setAnimatedNodeOffset", [this](folly::dynamic args)
      {
        auto tag = facebook::xplat::jsArgAsInt(args, 0);
        auto value = facebook::xplat::jsArgAsDouble(args, 1);
        NativeAnimatedModule::SetAnimatedNodeOffset(tag, value);
      }),
      Method("flattenAnimatedNodeOffset", [this](folly::dynamic args)
      {
        auto tag = facebook::xplat::jsArgAsInt(args, 0);
        NativeAnimatedModule::FlattenAnimatedNodeOffset(tag);
      }),
      Method("extractAnimatedNodeOffset", [this](folly::dynamic args)
      {
        auto tag = facebook::xplat::jsArgAsInt(args, 0);
        NativeAnimatedModule::ExtractAnimatedNodeOffset(tag);
      }),
      Method("addAnimatedEventToView", [this](folly::dynamic args)
      {
        auto viewTag = facebook::xplat::jsArgAsInt(args, 0);
        auto eventName = facebook::xplat::jsArgAsString(args, 1);
        auto eventMapping = facebook::xplat::jsArgAsObject(args, 2);
        NativeAnimatedModule::AddAnimatedEventToView(viewTag, eventName, eventMapping);
      }),
      Method("removeAnimatedEventFromView", [this](folly::dynamic args)
      {
        auto viewTag = facebook::xplat::jsArgAsInt(args, 0);
        auto eventName = facebook::xplat::jsArgAsString(args, 1);
        auto animatedValueTag = facebook::xplat::jsArgAsInt(args, 2);
        NativeAnimatedModule::RemoveAnimatedEventFromView(viewTag, eventName, animatedValueTag);
      }),
    };
  }

  void NativeAnimatedModule::CreateAnimatedNode(int64_t tag, const folly::dynamic& config)
  {
    m_nodesManager->CreateAnimatedNode(tag, config, m_wkReactInstance, m_nodesManager);
  }

  void NativeAnimatedModule::ConnectAnimatedNodeToView(int64_t animatedNodeTag, int64_t viewTag)
  {
    m_nodesManager->ConnectAnimatedNodeToView(animatedNodeTag, viewTag);
  }

  void NativeAnimatedModule::DisconnectAnimatedNodeFromView(int64_t animatedNodeTag, int64_t viewTag)
  {
    m_nodesManager->DisconnectAnimatedNodeToView(animatedNodeTag, viewTag);
  }

  void NativeAnimatedModule::ConnectAnimatedNodes(int64_t parentNodeTag, int64_t childNodeTag)
  {
    m_nodesManager->ConnectAnimatedNode(parentNodeTag, childNodeTag);
  }

  void NativeAnimatedModule::DisconnectAnimatedNodes(int64_t parentNodeTag, int64_t childNodeTag)
  {
    m_nodesManager->DisconnectAnimatedNode(parentNodeTag, childNodeTag);
  }

  void NativeAnimatedModule::StartAnimatingNode(int64_t animationId, int64_t animatedNodeTag, const folly::dynamic& animationConfig, const Callback& endCallback)
  {
    m_nodesManager->StartAnimatingNode(animationId, animatedNodeTag, animationConfig, endCallback);
  }

  void NativeAnimatedModule::StopAnimation(int64_t animationId)
  {
    m_nodesManager->StopAnimation(animationId);
  }

  void NativeAnimatedModule::DropAnimatedNode(int64_t tag)
  {
    m_nodesManager->DropAnimatedNode(tag);
  }

  void NativeAnimatedModule::SetAnimatedNodeValue(int64_t tag, double value)
  {
    m_nodesManager->SetAnimatedNodeValue(tag, value);
  }

  void NativeAnimatedModule::SetAnimatedNodeOffset(int64_t tag, double offset)
  {
    m_nodesManager->SetAnimatedNodeOffset(tag, offset);
  }

  void NativeAnimatedModule::FlattenAnimatedNodeOffset(int64_t tag)
  {
    m_nodesManager->FlattenAnimatedNodeOffset(tag);
  }

  void NativeAnimatedModule::ExtractAnimatedNodeOffset(int64_t tag)
  {
    m_nodesManager->ExtractAnimatedNodeOffset(tag);
  }

  void NativeAnimatedModule::AddAnimatedEventToView(int64_t tag, const std::string& eventName, const folly::dynamic& eventMapping)
  {
    m_nodesManager->AddAnimatedEventToView(tag, eventName, eventMapping);
  }

  void NativeAnimatedModule::RemoveAnimatedEventFromView(int64_t tag, const std::string& eventName, int64_t animatedValueTag)
  {
    m_nodesManager->RemoveAnimatedEventFromView(tag, eventName, animatedValueTag);
  }

  void NativeAnimatedModule::StartListeningToAnimatedNodeValue(int64_t tag)
  {
    //NotImplemented
  }

  void NativeAnimatedModule::StopListeningToAnimatedNodeValue(int64_t tag)
  {
    //NotImplemented
  }
} } // namespace react::uwp
