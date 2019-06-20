#pragma once
// Copyright (c) Microsoft Corporation. All rights reserved.
// Portions derived from React Native:
// Copyright (c) 2015-present, Facebook, Inc.
// Licensed under the MIT License.
#include <folly/dynamic.h>
#include "AnimatedNode.h"
#include "PropsAnimatedNode.h"
#include "ValueAnimatedNode.h"
#include "AnimationDriver.h"
#include "TransformAnimatedNode.h"
#include "EventAnimationDriver.h"
#include <cxxreact/CxxModule.h>
#include <IReactInstance.h>

namespace react {
  namespace uwp {
      /// <summary>
      /// This is the main class that coordinates how native animated JS
      /// implementation drives UI changes.
      /// 
      /// It implements a management interface for animated nodes graph as well
      /// as implements a graph traversal algorithm that is run for each
      /// animation frame.
      /// 
      /// For each animation frame we visit animated nodes that might've been 
      /// updated as well as their children that may use parent's values to
      /// update themselves. At the end of the traversal algorithm we expect to
      /// reach a special type of the node: PropsAnimatedNode that is then 
      /// responsible for calculating prop map which can be sent to native
      /// view hierarchy to update the view.
      /// </summary>
      /// <remarks>
      /// This class should be accessed only from the dispatcher thread.
      /// </remarks>

    typedef std::function<void(std::vector<folly::dynamic>)> Callback;

    enum AnimatedNodeType
    {
      style,
      value,
      props,
      interpolation,
      addition,
      subtraction,
      division,
      multiplication,
      modulus,
      diffclamp,
      transform,
      tracking,
    };

    enum AnimationType
    {
      decay,
      frames,
      spring,
    };

    enum FacadeType
    {
      opacity,
      rotation,
      rotationX,
      rotationY,
      scale,
      scaleX,
      scaleY,
      translateX,
      translateY,
      perspective,
    };

    class StyleAnimatedNode;
    class PropsAnimatedNode;
    class ValueAnimatedNode;
    class TransformAnimatedNode;
    class AnimationDriver;
    class EventAnimationDriver;
    class NativeAnimatedNodesManager// : IEventDispatcherListener
    {
    public:
      void CreateAnimatedNode(int64_t tag, const folly::dynamic& config, const std::weak_ptr<IReactInstance>& instance, const std::shared_ptr<NativeAnimatedNodesManager>& manager);
      void ConnectAnimatedNodeToView(int64_t propsNodeTag, int64_t viewTag);
      void DisconnectAnimatedNodeToView(int64_t propsNodeTag, int64_t viewTag);
      void ConnectAnimatedNode(int64_t parentNodeTag, int64_t childNodeTag);
      void DisconnectAnimatedNode(int64_t parentNodeTag, int64_t childNodeTag);
      void StopAnimation(int64_t animationId);
      void StartAnimatingNode(int64_t animationId, int64_t animatedNodeTag, const folly::dynamic& animationConfig, Callback endCallback);
      void DropAnimatedNode(int64_t tag);
      void SetAnimatedNodeValue(int64_t tag, double value);
      void SetAnimatedNodeOffset(int64_t tag, double offset);
      void FlattenAnimatedNodeOffset(int64_t tag);
      void ExtractAnimatedNodeOffset(int64_t tag);
      void AddAnimatedEventToView(int64_t viewTag, const std::string& eventName, const folly::dynamic& eventMapping);
      void RemoveAnimatedEventFromView(int64_t viewTag, const std::string& eventName, int64_t animatedValueTag);
      void ProcessDelayedPropsNodes();

      static AnimatedNodeType AnimatedNodeTypeFromString(const std::string& string);
      static AnimationType AnimationTypeFromString(const std::string& string);
      static FacadeType StringToFacadeType(const std::string& string);

      std::unordered_map<int64_t, std::shared_ptr<AnimationDriver>> m_animationNodes{};
      std::unordered_map<int64_t, std::shared_ptr<ValueAnimatedNode>> m_valueNodes{};
      std::unordered_map<int64_t, std::shared_ptr<PropsAnimatedNode>> m_propsNodes{};
      std::vector<int64_t> m_delayedPropsNodes{};
      std::unordered_map<int64_t, std::shared_ptr<StyleAnimatedNode>> m_styleNodes{};
      std::unordered_map<int64_t, std::shared_ptr<TransformAnimatedNode>> m_transformNodes{};
      std::unordered_map<std::tuple<int64_t, std::string>, std::unordered_set<std::shared_ptr<EventAnimationDriver>>> m_eventDrivers{};
      std::unordered_map<int64_t, std::shared_ptr<AnimatedNode>> m_nodes{};
    private:
      std::unordered_map<int64_t, std::shared_ptr<AnimationDriver>> m_activeAnimations{};
    };
    }
  }
