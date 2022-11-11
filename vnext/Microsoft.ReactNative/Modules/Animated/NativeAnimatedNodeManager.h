#pragma once
// Copyright (c) Microsoft Corporation.
// Portions derived from React Native:
// Copyright (c) 2015-present, Facebook, Inc.
// Licensed under the MIT License.

#include <IReactInstance.h>
#include <UI.Xaml.Media.h>
#include <cxxreact/CxxModule.h>
#include <folly/dynamic.h>
#include "AnimatedNode.h"
#include "AnimationDriver.h"
#include "EventAnimationDriver.h"
#include "PropsAnimatedNode.h"
#include "StyleAnimatedNode.h"
#include "TrackingAnimatedNode.h"
#include "TransformAnimatedNode.h"
#include "ValueAnimatedNode.h"

#include "codegen/NativeAnimatedModuleSpec.g.h"

namespace Microsoft::ReactNative {
/// <summary>
/// This is the main class that coordinates how native animated JS
/// implementation drives UI changes.
///
/// It implements a management interface for animated nodes graph and
/// establishes a number of composistion animations and property sets to
/// drive the animating of the nodes to the Xaml elements off the UI Thread
///
/// </summary>

typedef std::function<void(bool)> EndCallback;
typedef std::function<void(double)> ValueListenerCallback;

class AnimatedNode;
class StyleAnimatedNode;
class PropsAnimatedNode;
class ValueAnimatedNode;
class TransformAnimatedNode;
class TrackingAnimatedNode;
class AnimationDriver;
class EventAnimationDriver;
class NativeAnimatedNodeManager {
 public:
  NativeAnimatedNodeManager(winrt::Microsoft::ReactNative::ReactContext const &reactContext);
  const winrt::Microsoft::ReactNative::ReactContext &ReactContext() const noexcept;
  comp::Compositor Compositor() const noexcept;
  void CreateAnimatedNode(
      int64_t tag,
      const winrt::Microsoft::ReactNative::JSValueObject &config,
      const winrt::Microsoft::ReactNative::ReactContext &context,
      const std::shared_ptr<NativeAnimatedNodeManager> &manager);
  void GetValue(int64_t animatedNodeTag, std::function<void(double)> const &endCallback);
  void ConnectAnimatedNodeToView(int64_t propsNodeTag, int64_t viewTag);
  void DisconnectAnimatedNodeToView(int64_t propsNodeTag, int64_t viewTag);
  void RestoreDefaultValues(int64_t tag);
  void ConnectAnimatedNode(int64_t parentNodeTag, int64_t childNodeTag);
  void DisconnectAnimatedNode(int64_t parentNodeTag, int64_t childNodeTag);
  void StopAnimation(int64_t animationId, bool isTrackingAnimation = false);
  void RestartTrackingAnimatedNode(
      int64_t animationId,
      int64_t animatedToValueTag,
      const std::shared_ptr<NativeAnimatedNodeManager> &manager);
  void StartTrackingAnimatedNode(
      int64_t animationId,
      int64_t animatedNodeTag,
      int64_t animatedToValueTag,
      const winrt::Microsoft::ReactNative::JSValueObject &animationConfig,
      const EndCallback &endCallback,
      const std::shared_ptr<NativeAnimatedNodeManager> &manager,
      bool track = true);
  void StartAnimatingNode(
      int64_t animationId,
      int64_t animatedNodeTag,
      const winrt::Microsoft::ReactNative::JSValueObject &animationConfig,
      const EndCallback &endCallback,
      const std::shared_ptr<NativeAnimatedNodeManager> &manager);
  void DropAnimatedNode(int64_t tag);
  void SetAnimatedNodeValue(int64_t tag, double value);
  void SetAnimatedNodeOffset(int64_t tag, double offset);
  void FlattenAnimatedNodeOffset(int64_t tag);
  void ExtractAnimatedNodeOffset(int64_t tag);
  void StartListeningToAnimatedNodeValue(int64_t tag, const ValueListenerCallback &callback);
  void StopListeningToAnimatedNodeValue(int64_t tag);
  void AddAnimatedEventToView(
      int64_t viewTag,
      const std::string &eventName,
      const ReactNativeSpecs::AnimatedModuleSpec_EventMapping &eventMapping,
      const std::shared_ptr<NativeAnimatedNodeManager> &manager);
  void RemoveAnimatedEventFromView(int64_t viewTag, const std::string &eventName, int64_t animatedValueTag);
  void ProcessDelayedPropsNodes();
  void AddDelayedPropsNode(int64_t propsNodeTag, const winrt::Microsoft::ReactNative::ReactContext &context);

  AnimatedNode *GetAnimatedNode(int64_t tag);
  ValueAnimatedNode *GetValueAnimatedNode(int64_t tag);
  PropsAnimatedNode *GetPropsAnimatedNode(int64_t tag);
  StyleAnimatedNode *GetStyleAnimatedNode(int64_t tag);
  TransformAnimatedNode *GetTransformAnimatedNode(int64_t tag);
  TrackingAnimatedNode *GetTrackingAnimatedNode(int64_t tag);

  void RemoveActiveAnimation(int64_t tag);
  void RemoveStoppedAnimation(int64_t tag, const std::shared_ptr<NativeAnimatedNodeManager> &manager);
  void StartDeferredAnimationsForValueNode(
      int64_t valueNodeTag,
      const std::shared_ptr<NativeAnimatedNodeManager> &manager);
  void StartAnimationAndTrackingNodes(
      int64_t tag,
      int64_t nodeTag,
      const std::shared_ptr<NativeAnimatedNodeManager> &manager);

 private:
  void EnsureRendering();
  void OnRendering(winrt::IInspectable const &sender, winrt::IInspectable const &args);
  void RunUpdates(winrt::TimeSpan renderingTime);
  void StopAnimationsForNode(int64_t tag);
  void UpdateActiveAnimationIds();
  void UpdateNodes(std::unordered_set<int64_t> &nodes);

  std::unordered_map<int64_t, std::unique_ptr<ValueAnimatedNode>> m_valueNodes{};
  std::unordered_map<int64_t, std::unique_ptr<PropsAnimatedNode>> m_propsNodes{};
  std::unordered_map<int64_t, std::unique_ptr<StyleAnimatedNode>> m_styleNodes{};
  std::unordered_map<int64_t, std::unique_ptr<TransformAnimatedNode>> m_transformNodes{};
  std::unordered_map<int64_t, std::unique_ptr<TrackingAnimatedNode>> m_trackingNodes{};
  std::unordered_map<std::tuple<int64_t, std::string>, std::vector<std::unique_ptr<EventAnimationDriver>>>
      m_eventDrivers{};
  std::unordered_map<int64_t, std::shared_ptr<AnimationDriver>> m_activeAnimations{};
  std::unordered_map<int64_t, std::shared_ptr<AnimationDriver>> m_pendingCompletionAnimations{};
  std::unordered_set<int64_t> m_valuesWithStoppedAnimation{};
  std::unordered_map<int64_t, int64_t> m_deferredAnimationForValues{};
  std::vector<std::tuple<int64_t, int64_t>> m_trackingAndLeadNodeTags{};
  std::vector<int64_t> m_delayedPropsNodes{};
  winrt::Microsoft::ReactNative::ReactContext m_context;

  std::unordered_set<int64_t> m_updatedNodes{};
  std::vector<int64_t> m_activeAnimationIds{};
  int64_t m_animatedGraphBFSColor{};
  xaml::Media::CompositionTarget::Rendering_revoker m_renderingRevoker;

  static constexpr std::string_view s_toValueIdName{"toValue"};
  static constexpr std::string_view s_framesName{"frames"};
  static constexpr std::string_view s_dynamicToValuesName{"dynamicToValues"};
};
} // namespace Microsoft::ReactNative
