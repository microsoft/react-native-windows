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

namespace react {
  namespace uwp {

    const char* NativeAnimatedModule::name = "NativeAnimatedModule";

    NativeAnimatedModule::NativeAnimatedModule(const std::shared_ptr<IReactInstance>& reactInstance) : m_wkReactInstance(reactInstance)
    {
      _nodesManager = std::make_shared<NativeAnimatedNodesManager>(NativeAnimatedNodesManager());
    }

    NativeAnimatedModule::~NativeAnimatedModule() = default;

    std::string NativeAnimatedModule::getName()
    {
      return name;
    }

    std::map<std::string, folly::dynamic> NativeAnimatedModule::getConstants()
    {
      return {};
    }

    std::vector<facebook::xplat::module::CxxModule::Method> NativeAnimatedModule::getMethods()
    {
      return {
        Method("createAnimatedNode", [this](folly::dynamic args)
        {
          auto tag = facebook::xplat::jsArgAsInt(args, 0);
          auto config = facebook::xplat::jsArgAsObject(args, 1);
          NativeAnimatedModule::createAnimatedNode(tag, config);
        }),
        Method("connectAnimatedNodeToView", [this](folly::dynamic args)
        {
          auto tag = facebook::xplat::jsArgAsInt(args, 0);
          auto viewTag = facebook::xplat::jsArgAsInt(args, 1);
          NativeAnimatedModule::connectAnimatedNodeToView(tag, viewTag);
        }),
        Method("disconnectAnimatedNodeFromView", [this](folly::dynamic args)
        {
          auto tag = facebook::xplat::jsArgAsInt(args, 0);
          auto viewTag = facebook::xplat::jsArgAsInt(args, 1);
          NativeAnimatedModule::disconnectAnimatedNodeFromView(tag, viewTag);
        }),
        Method("connectAnimatedNodes", [this](folly::dynamic args)
        {
          auto parentTag = facebook::xplat::jsArgAsInt(args, 0);
          auto childTag = facebook::xplat::jsArgAsInt(args, 1);
          NativeAnimatedModule::connectAnimatedNodes(parentTag, childTag);
        }),
        Method("disconnectAnimatedNodes", [this](folly::dynamic args)
        {
          auto parentTag = facebook::xplat::jsArgAsInt(args, 0);
          auto childTag = facebook::xplat::jsArgAsInt(args, 1);
          NativeAnimatedModule::disconnectAnimatedNodes(parentTag, childTag);
        }),
        Method("stopAnimation", [this](folly::dynamic args)
        {
          auto animationId = facebook::xplat::jsArgAsInt(args, 0);
          NativeAnimatedModule::stopAnimation(animationId);
        }),
        Method("startAnimatingNode", [this](folly::dynamic args, Callback endCallback)
        {
          auto animationId = facebook::xplat::jsArgAsInt(args, 0);
          auto animatedNodeTag = facebook::xplat::jsArgAsInt(args, 1);
          auto animationConfig = facebook::xplat::jsArgAsObject(args, 2);
          NativeAnimatedModule::startAnimatingNode(animationId, animatedNodeTag, animationConfig, endCallback);
        }),
        Method("dropAnimatedNode", [this](folly::dynamic args)
        {
          auto tag = facebook::xplat::jsArgAsInt(args, 0);
          NativeAnimatedModule::dropAnimatedNode(tag);
        }),
        Method("setAnimatedNodeValue", [this](folly::dynamic args)
        {
          auto tag = facebook::xplat::jsArgAsInt(args, 0);
          auto value = facebook::xplat::jsArgAsDouble(args, 1);
          NativeAnimatedModule::setAnimatedNodeValue(tag, value);
        }),
        Method("setAnimatedNodeOffset", [this](folly::dynamic args)
        {
          auto tag = facebook::xplat::jsArgAsInt(args, 0);
          auto value = facebook::xplat::jsArgAsDouble(args, 1);
          NativeAnimatedModule::setAnimatedNodeOffset(tag, value);
        }),
        Method("flattenAnimatedNodeOffset", [this](folly::dynamic args)
        {
          auto tag = facebook::xplat::jsArgAsInt(args, 0);
          NativeAnimatedModule::flattenAnimatedNodeOffset(tag);
        }),
        Method("extractAnimatedNodeOffset", [this](folly::dynamic args)
        {
          auto tag = facebook::xplat::jsArgAsInt(args, 0);
          NativeAnimatedModule::extractAnimatedNodeOffset(tag);
        }),
        Method("addAnimatedEventToView", [this](folly::dynamic args)
        {
          auto viewTag = facebook::xplat::jsArgAsInt(args, 0);
          auto eventName = facebook::xplat::jsArgAsString(args, 1);
          auto eventMapping = facebook::xplat::jsArgAsObject(args, 2);
          NativeAnimatedModule::addAnimatedEventToView(viewTag, eventName, eventMapping);
        }),
        Method("removeAnimatedEventFromView", [this](folly::dynamic args)
        {
          auto viewTag = facebook::xplat::jsArgAsInt(args, 0);
          auto eventName = facebook::xplat::jsArgAsString(args, 1);
          auto animatedValueTag = facebook::xplat::jsArgAsInt(args, 2);
          NativeAnimatedModule::removeAnimatedEventFromView(viewTag, eventName, animatedValueTag);
        }),
      };
    }

    void NativeAnimatedModule::createAnimatedNode(int64_t tag, const folly::dynamic& config)
    {
      _nodesManager->CreateAnimatedNode(tag, config, m_wkReactInstance, _nodesManager);
    }

    void NativeAnimatedModule::connectAnimatedNodeToView(int64_t animatedNodeTag, int64_t viewTag)
    {
      _nodesManager->ConnectAnimatedNodeToView(animatedNodeTag, viewTag);
    }

    void NativeAnimatedModule::disconnectAnimatedNodeFromView(int64_t animatedNodeTag, int64_t viewTag)
    {
      _nodesManager->DisconnectAnimatedNodeToView(animatedNodeTag, viewTag);
    }

    void NativeAnimatedModule::connectAnimatedNodes(int64_t parentNodeTag, int64_t childNodeTag)
    {
      _nodesManager->ConnectAnimatedNode(parentNodeTag, childNodeTag);
    }

    void NativeAnimatedModule::disconnectAnimatedNodes(int64_t parentNodeTag, int64_t childNodeTag)
    {
      _nodesManager->DisconnectAnimatedNode(parentNodeTag, childNodeTag);
    }

    void NativeAnimatedModule::startAnimatingNode(int64_t animationId, int64_t animatedNodeTag, const folly::dynamic& animationConfig, Callback endCallback)
    {
      _nodesManager->StartAnimatingNode(animationId, animatedNodeTag, animationConfig, endCallback);
    }

    void NativeAnimatedModule::stopAnimation(int64_t animationId)
    {
      _nodesManager->StopAnimation(animationId);
    }

    void NativeAnimatedModule::dropAnimatedNode(int64_t tag)
    {
      _nodesManager->StopAnimation(tag);
    }

    void NativeAnimatedModule::setAnimatedNodeValue(int64_t tag, double value)
    {
      _nodesManager->SetAnimatedNodeValue(tag, value);
    }

    void NativeAnimatedModule::setAnimatedNodeOffset(int64_t tag, double offset)
    {
      _nodesManager->SetAnimatedNodeOffset(tag, offset);
    }

    void NativeAnimatedModule::flattenAnimatedNodeOffset(int64_t tag)
    {
      _nodesManager->FlattenAnimatedNodeOffset(tag);
    }

    void NativeAnimatedModule::extractAnimatedNodeOffset(int64_t tag)
    {
      _nodesManager->ExtractAnimatedNodeOffset(tag);
    }

    void NativeAnimatedModule::addAnimatedEventToView(int64_t tag, const std::string& eventName, const folly::dynamic& eventMapping)
    {
      _nodesManager->AddAnimatedEventToView(tag, eventName, eventMapping);
    }

    void NativeAnimatedModule::removeAnimatedEventFromView(int64_t tag, const std::string& eventName, int64_t animatedValueTag)
    {
      //NotImplemented
    }

    void NativeAnimatedModule::startListeningToAnimatedNodeValue(int64_t tag)
    {
      //NotImplemented
    }

    void NativeAnimatedModule::stopListeningToAnimatedNodeValue(int64_t tag)
    {
      //NotImplemented
    }
  }
} // namespace react::uwp
