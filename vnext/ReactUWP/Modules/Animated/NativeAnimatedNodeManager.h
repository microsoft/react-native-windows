#pragma once
// Copyright (c) Microsoft Corporation. All rights reserved.
// Portions derived from React Native:
// Copyright (c) 2015-present, Facebook, Inc.
// Licensed under the MIT License.

#include <IReactInstance.h>
#include <cxxreact/CxxModule.h>
#include <folly/dynamic.h>
#include "AnimatedNode.h"
#include "AnimationDriver.h"
#include "EventAnimationDriver.h"
#include "PropsAnimatedNode.h"
#include "StyleAnimatedNode.h"
#include "TransformAnimatedNode.h"
#include "ValueAnimatedNode.h"

namespace react {
namespace uwp {
/// <summary>
/// This is the main class that coordinates how native animated JS
/// implementation drives UI changes.
///
/// It implements a management interface for animated nodes graph and
/// establishes a number of composistion animations and property sets to
/// drive the animating of the nodes to the Xaml elements off the UI Thread
///
/// </summary>

typedef std::function<void(std::vector<folly::dynamic>)> Callback;

class AnimatedNode;
class StyleAnimatedNode;
class PropsAnimatedNode;
class ValueAnimatedNode;
class TransformAnimatedNode;
class AnimationDriver;
class EventAnimationDriver;
class NativeAnimatedNodeManager {
 public:
  void CreateAnimatedNode(
      int64_t tag,
      const folly::dynamic &config,
      const std::weak_ptr<IReactInstance> &instance,
      const std::shared_ptr<NativeAnimatedNodeManager> &manager);
  void ConnectAnimatedNodeToView(int64_t propsNodeTag, int64_t viewTag);
  void DisconnectAnimatedNodeToView(int64_t propsNodeTag, int64_t viewTag);
  void ConnectAnimatedNode(int64_t parentNodeTag, int64_t childNodeTag);
  void DisconnectAnimatedNode(int64_t parentNodeTag, int64_t childNodeTag);
  void StopAnimation(int64_t animationId);
  void StartAnimatingNode(
      int64_t animationId,
      int64_t animatedNodeTag,
      const folly::dynamic &animationConfig,
      const Callback &endCallback,
      const std::shared_ptr<NativeAnimatedNodeManager> &manager);
  void DropAnimatedNode(int64_t tag);
  void SetAnimatedNodeValue(int64_t tag, double value);
  void SetAnimatedNodeOffset(int64_t tag, double offset);
  void FlattenAnimatedNodeOffset(int64_t tag);
  void ExtractAnimatedNodeOffset(int64_t tag);
  void AddAnimatedEventToView(
      int64_t viewTag,
      const std::string &eventName,
      const folly::dynamic &eventMapping,
      const std::shared_ptr<NativeAnimatedNodeManager> &manager);
  void RemoveAnimatedEventFromView(
      int64_t viewTag,
      const std::string &eventName,
      int64_t animatedValueTag);
  void ProcessDelayedPropsNodes();
  void AddDelayedPropsNode(
      int64_t propsNodeTag,
      const std::shared_ptr<IReactInstance> &instance);

  AnimationDriver *GetAnimationNode(int64_t tag);

  AnimatedNode *GetAnimatedNode(int64_t tag);
  ValueAnimatedNode *GetValueAnimatedNode(int64_t tag);
  PropsAnimatedNode *GetPropsAnimatedNode(int64_t tag);
  StyleAnimatedNode *GetStyleAnimatedNode(int64_t tag);
  TransformAnimatedNode *GetTransformAnimatedNode(int64_t tag);

 private:
  std::unordered_map<int64_t, std::unique_ptr<AnimationDriver>>
      m_animationNodes{};
  std::unordered_map<int64_t, std::unique_ptr<ValueAnimatedNode>>
      m_valueNodes{};
  std::unordered_map<int64_t, std::unique_ptr<PropsAnimatedNode>>
      m_propsNodes{};
  std::unordered_map<int64_t, std::unique_ptr<StyleAnimatedNode>>
      m_styleNodes{};
  std::unordered_map<int64_t, std::unique_ptr<TransformAnimatedNode>>
      m_transformNodes{};
  std::unordered_map<
      std::tuple<int64_t, std::string>,
      std::vector<std::unique_ptr<EventAnimationDriver>>>
      m_eventDrivers{};
  std::unordered_map<int64_t, std::unique_ptr<AnimationDriver>>
      m_activeAnimations{};
  std::vector<int64_t> m_delayedPropsNodes{};
};
} // namespace uwp
} // namespace react
