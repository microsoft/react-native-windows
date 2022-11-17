// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"

#include "AdditionAnimatedNode.h"
#include "DiffClampAnimatedNode.h"
#include "DivisionAnimatedNode.h"
#include "InterpolationAnimatedNode.h"
#include "ModulusAnimatedNode.h"
#include "MultiplicationAnimatedNode.h"
#include "NativeAnimatedNodeManager.h"
#include "StyleAnimatedNode.h"
#include "SubtractionAnimatedNode.h"
#include "TrackingAnimatedNode.h"

#include "DecayAnimationDriver.h"
#include "FrameAnimationDriver.h"
#include "SpringAnimationDriver.h"

#include "AnimatedNodeType.h"
#include "AnimationType.h"
#include "FacadeType.h"

#include <Modules/NativeUIManager.h>
#include <Modules/PaperUIManagerModule.h>
#include <Windows.Foundation.h>
#include <queue>

#ifdef USE_FABRIC
#include <Fabric/Composition/CompositionContextHelper.h>
#include <Fabric/Composition/CompositionUIService.h>
#endif

namespace Microsoft::ReactNative {

NativeAnimatedNodeManager::NativeAnimatedNodeManager(winrt::Microsoft::ReactNative::ReactContext const &reactContext)
    : m_context(reactContext) {}

const winrt::Microsoft::ReactNative::ReactContext &NativeAnimatedNodeManager::ReactContext() const noexcept {
  return m_context;
}

comp::Compositor NativeAnimatedNodeManager::Compositor() const noexcept {
#ifdef USE_FABRIC
  auto compositionContext =
      winrt::Microsoft::ReactNative::Composition::implementation::CompositionUIService::GetCompositionContext(
          m_context.Properties().Handle());
  if (compositionContext) {
    return winrt::Microsoft::ReactNative::Composition::implementation::CompositionContextHelper::InnerCompositor(
        compositionContext);
  }
#endif
  // TODO: Islands - need to get the XamlView associated with this animation in order to
  // use the compositor Microsoft::ReactNative::GetCompositor(xamlView)
  return Microsoft::ReactNative::GetCompositor();
}

void NativeAnimatedNodeManager::CreateAnimatedNode(
    int64_t tag,
    const ::React::JSValueObject &config,
    const winrt::Microsoft::ReactNative::ReactContext &context,
    const std::shared_ptr<NativeAnimatedNodeManager> &manager) {
  if (m_transformNodes.count(tag) > 0 || m_propsNodes.count(tag) > 0 || m_styleNodes.count(tag) > 0 ||
      m_valueNodes.count(tag) > 0) {
    throw std::invalid_argument("AnimatedNode with tag " + std::to_string(tag) + " already exists.");
    return;
  }

  switch (const auto type = AnimatedNodeTypeFromString(config["type"].AsString())) {
    case AnimatedNodeType::Style: {
      m_styleNodes.emplace(tag, std::make_unique<StyleAnimatedNode>(tag, config, manager));
      break;
    }
    case AnimatedNodeType::Value: {
      m_valueNodes.emplace(tag, std::make_unique<ValueAnimatedNode>(tag, config, manager));
      break;
    }
    case AnimatedNodeType::Props: {
      m_propsNodes.emplace(tag, std::make_unique<PropsAnimatedNode>(tag, config, context, manager));
      break;
    }
    case AnimatedNodeType::Interpolation: {
      m_valueNodes.emplace(tag, std::make_unique<InterpolationAnimatedNode>(tag, config, manager));
      break;
    }
    case AnimatedNodeType::Addition: {
      m_valueNodes.emplace(tag, std::make_unique<AdditionAnimatedNode>(tag, config, manager));
      break;
    }
    case AnimatedNodeType::Subtraction: {
      m_valueNodes.emplace(tag, std::make_unique<SubtractionAnimatedNode>(tag, config, manager));
      break;
    }
    case AnimatedNodeType::Division: {
      m_valueNodes.emplace(tag, std::make_unique<DivisionAnimatedNode>(tag, config, manager));
      break;
    }
    case AnimatedNodeType::Multiplication: {
      m_valueNodes.emplace(tag, std::make_unique<MultiplicationAnimatedNode>(tag, config, manager));
      break;
    }
    case AnimatedNodeType::Modulus: {
      m_valueNodes.emplace(tag, std::make_unique<ModulusAnimatedNode>(tag, config, manager));
      break;
    }
    case AnimatedNodeType::Diffclamp: {
      m_valueNodes.emplace(tag, std::make_unique<DiffClampAnimatedNode>(tag, config, manager));
      break;
    }
    case AnimatedNodeType::Transform: {
      m_transformNodes.emplace(tag, std::make_unique<TransformAnimatedNode>(tag, config, manager));
      break;
    }
    case AnimatedNodeType::Tracking: {
      m_trackingNodes.emplace(tag, std::make_unique<TrackingAnimatedNode>(tag, config, manager));
      break;
    }
    default: {
      assert(false);
      break;
    }
  }
}

void NativeAnimatedNodeManager::GetValue(
    int64_t animatedNodeTag,
    std::function<void(double)> const &saveValueCallback) {
  if (const auto valueNode = m_valueNodes.at(animatedNodeTag).get()) {
    saveValueCallback(valueNode->Value());
  }
}

void NativeAnimatedNodeManager::ConnectAnimatedNodeToView(int64_t propsNodeTag, int64_t viewTag) {
  const auto &propsNode = m_propsNodes.at(propsNodeTag);
  propsNode->ConnectToView(viewTag);
  if (!propsNode->UseComposition()) {
    m_updatedNodes.insert(propsNodeTag);
    EnsureRendering();
  }
}

void NativeAnimatedNodeManager::DisconnectAnimatedNodeToView(int64_t propsNodeTag, int64_t viewTag) {
  m_propsNodes.at(propsNodeTag)->DisconnectFromView(viewTag);
}

void NativeAnimatedNodeManager::RestoreDefaultValues(int64_t tag) {
  if (const auto propsNode = GetPropsAnimatedNode(tag)) {
    propsNode->RestoreDefaultValues();
  }
}

void NativeAnimatedNodeManager::ConnectAnimatedNode(int64_t parentNodeTag, int64_t childNodeTag) {
  if (const auto parentNode = GetAnimatedNode(parentNodeTag)) {
    parentNode->AddChild(childNodeTag);
    if (!parentNode->UseComposition()) {
      m_updatedNodes.insert(childNodeTag);
      EnsureRendering();
    }
  }
}

void NativeAnimatedNodeManager::DisconnectAnimatedNode(int64_t parentNodeTag, int64_t childNodeTag) {
  if (const auto parentNode = GetAnimatedNode(parentNodeTag)) {
    parentNode->RemoveChild(childNodeTag);
    if (!parentNode->UseComposition()) {
      m_updatedNodes.insert(childNodeTag);
      EnsureRendering();
    }
  }
}

void NativeAnimatedNodeManager::StopAnimation(int64_t animationId, bool isTrackingAnimation) {
  if (m_activeAnimations.count(animationId)) {
    if (const auto animation = m_activeAnimations.at(animationId)) {
      animation->StopAnimation(isTrackingAnimation);

      // We need to update the node manager state for composition animations
      // to ensure new animations on the same animated value do not start until
      // the completion callback has fired for the stopped animation.
      if (animation->UseComposition()) {
        // Insert the animation into the pending completion set to ensure it is
        // not destroyed before the callback occurs. It's safe to assume the
        // scoped batch completion callback has not run, since if it had, the
        // animation would have been removed from the set of active animations.
        m_pendingCompletionAnimations.insert({animationId, animation});

        const auto nodeTag = animation->AnimatedValueTag();
        if (nodeTag != -1) {
          const auto deferredAnimation = m_deferredAnimationForValues.find(nodeTag);
          if (deferredAnimation != m_deferredAnimationForValues.end()) {
            // We can assume that the currently deferred animation is the one
            // being stopped given the constraint that only one animation can
            // be active for a given value node.
            assert(deferredAnimation->second == animationId);
            // If the animation is deferred, just remove the deferred animation
            // entry as two animations cannot animate the same value concurrently.
            m_deferredAnimationForValues.erase(nodeTag);
          } else {
            // Since only one animation can be active at a time, there shouldn't
            // be any stopped animations for the value node if the animation has
            // not been deferred.
            assert(!m_valuesWithStoppedAnimation.count(nodeTag));
            // In this case, add the value tag to the set of values with stopped
            // animations. This is used to optimize the lookup when determining
            // if an animation needs to be deferred (rather than iterating over
            // the map of pending completion animations).
            m_valuesWithStoppedAnimation.insert(nodeTag);
          }
        }
      }

      m_activeAnimations.erase(animationId);
    }
  }
}

void NativeAnimatedNodeManager::RestartTrackingAnimatedNode(
    int64_t animationId,
    int64_t animatedToValueTag,
    const std::shared_ptr<NativeAnimatedNodeManager> &manager) {
  if (m_activeAnimations.count(animationId)) {
    if (const auto animation = m_activeAnimations.at(animationId).get()) {
      assert(animation->UseComposition());
      auto const animatedValueTag = animation->AnimatedValueTag();
      auto const &animationConfig = animation->AnimationConfig();
      auto const endCallback = animation->EndCallback();
      animation->StopAnimation(true);
      m_activeAnimations.erase(animationId);
      StartTrackingAnimatedNode(
          animationId,
          animatedValueTag,
          animatedToValueTag,
          animationConfig,
          endCallback,
          manager,
          /*track*/ false);
    }
  }
}

void NativeAnimatedNodeManager::StartTrackingAnimatedNode(
    int64_t animationId,
    int64_t animatedNodeTag,
    int64_t animatedToValueTag,
    const winrt::Microsoft::ReactNative::JSValueObject &animationConfig,
    const EndCallback &endCallback,
    const std::shared_ptr<NativeAnimatedNodeManager> &manager,
    bool track) {
  auto updatedAnimationConfig = animationConfig.Copy();
  for (auto &item : m_activeAnimations) {
    if (item.second->AnimatedValueTag() == animatedToValueTag) {
      updatedAnimationConfig[s_toValueIdName] = item.second->ToValue();

      switch (AnimationTypeFromString(animationConfig["type"].AsString())) {
        case AnimationType::Frames: {
          winrt::Microsoft::ReactNative::JSValueArray frames;
          for (auto const &frame : animationConfig["frames"].AsArray()) {
            frames.push_back(0.0);
          }
          for (auto const &frame : item.second->Frames()) {
            frames.push_back(frame);
          }
          updatedAnimationConfig.insert(
              std::make_pair(s_framesName, winrt::Microsoft::ReactNative::JSValue(std::move(frames))));
        } break;
        case AnimationType::Spring: {
          winrt::Microsoft::ReactNative::JSValueArray frames;
          for (auto const &frame : item.second->Frames()) {
            frames.push_back(frame);
          }
          updatedAnimationConfig.insert(
              std::make_pair(s_dynamicToValuesName, winrt::Microsoft::ReactNative::JSValue(std::move(frames))));
        } break;
          // Animated.Decay does not have a to value,
          // so they cannot track other nodes. So we'll never
          // have a decay tracking node.
        case AnimationType::Decay:
        default:
          assert(false);
          break;
      }

      break;
    }
  }
  if (track) {
    m_trackingAndLeadNodeTags.push_back(std::make_tuple(animationId, animatedToValueTag));
  }
  StartAnimatingNode(animationId, animatedNodeTag, updatedAnimationConfig, endCallback, manager);
}

void NativeAnimatedNodeManager::StartAnimatingNode(
    int64_t animationId,
    int64_t animatedNodeTag,
    const winrt::Microsoft::ReactNative::JSValueObject &animationConfig,
    const EndCallback &endCallback,
    const std::shared_ptr<NativeAnimatedNodeManager> &manager) {
  switch (AnimationTypeFromString(animationConfig["type"].AsString())) {
    case AnimationType::Decay:
      m_activeAnimations.emplace(
          animationId,
          std::make_shared<DecayAnimationDriver>(animationId, animatedNodeTag, endCallback, animationConfig, manager));
      break;
    case AnimationType::Frames:
      m_activeAnimations.emplace(
          animationId,
          std::make_shared<FrameAnimationDriver>(animationId, animatedNodeTag, endCallback, animationConfig, manager));
      break;
    case AnimationType::Spring: {
      m_activeAnimations.emplace(
          animationId,
          std::make_shared<SpringAnimationDriver>(
              animationId,
              animatedNodeTag,
              endCallback,
              animationConfig,
              manager,
              animationConfig[s_dynamicToValuesName].AsArray()));
      break;
    }
    default:
      assert(false);
      break;
  }

  // If the animated value node has any stopped animations, defer start until
  // all stopped animations fire completion callback and have latest values.
  if (m_activeAnimations.count(animationId)) {
    const auto &animation = m_activeAnimations.at(animationId);
    if (animation->UseComposition()) {
      if (m_valuesWithStoppedAnimation.count(animatedNodeTag)) {
        // Since only one animation can be active per value at a time, there will
        // not be any other deferred animations for the value node.
        assert(!m_deferredAnimationForValues.count(animatedNodeTag));
        // Add the animation to the deferred animation map for the value tag.
        m_deferredAnimationForValues.insert({animatedNodeTag, animationId});
      } else {
        StartAnimationAndTrackingNodes(animationId, animatedNodeTag, manager);
      }
    } else {
      EnsureRendering();
    }
  }
}

void NativeAnimatedNodeManager::DropAnimatedNode(int64_t tag) {
  m_valueNodes.erase(tag);
  m_propsNodes.erase(tag);
  m_styleNodes.erase(tag);
  m_transformNodes.erase(tag);
  m_updatedNodes.erase(tag);
}

void NativeAnimatedNodeManager::SetAnimatedNodeValue(int64_t tag, double value) {
  if (const auto valueNode = m_valueNodes.at(tag).get()) {
    valueNode->RawValue(static_cast<float>(value));
    if (!valueNode->UseComposition()) {
      StopAnimationsForNode(tag);
      m_updatedNodes.insert(tag);
      EnsureRendering();
    }
  }
}

void NativeAnimatedNodeManager::SetAnimatedNodeOffset(int64_t tag, double offset) {
  if (const auto valueNode = m_valueNodes.at(tag).get()) {
    valueNode->Offset(static_cast<float>(offset));
    if (!valueNode->UseComposition()) {
      m_updatedNodes.insert(tag);
      EnsureRendering();
    }
  }
}

void NativeAnimatedNodeManager::FlattenAnimatedNodeOffset(int64_t tag) {
  if (const auto valueNode = m_valueNodes.at(tag).get()) {
    valueNode->FlattenOffset();
  }
}

void NativeAnimatedNodeManager::ExtractAnimatedNodeOffset(int64_t tag) {
  if (const auto valueNode = m_valueNodes.at(tag).get()) {
    valueNode->ExtractOffset();
  }
}

void NativeAnimatedNodeManager::StartListeningToAnimatedNodeValue(int64_t tag, const ValueListenerCallback &callback) {
  if (const auto valueNode = m_valueNodes.at(tag).get()) {
    valueNode->ValueListener(callback);
  }
}

void NativeAnimatedNodeManager::StopListeningToAnimatedNodeValue(int64_t tag) {
  if (const auto valueNode = m_valueNodes.at(tag).get()) {
    valueNode->ValueListener(nullptr);
  }
}

void NativeAnimatedNodeManager::AddAnimatedEventToView(
    int64_t viewTag,
    const std::string &eventName,
    const ReactNativeSpecs::AnimatedModuleSpec_EventMapping &eventMapping,
    const std::shared_ptr<NativeAnimatedNodeManager> &manager) {
  const auto valueNodeTag = eventMapping.animatedValueTag ? static_cast<int64_t>(*eventMapping.animatedValueTag) : 0;
  const auto pathList = eventMapping.nativeEventPath;

  const auto key = std::make_tuple(viewTag, eventName);
  if (m_eventDrivers.count(key)) {
    m_eventDrivers.at(key).emplace_back(std::make_unique<EventAnimationDriver>(pathList, valueNodeTag, manager));
  } else {
    auto vector = std::vector<std::unique_ptr<EventAnimationDriver>>{};
    vector.emplace_back(std::make_unique<EventAnimationDriver>(pathList, valueNodeTag, manager));
    m_eventDrivers.insert({key, std::move(vector)});
  }
}

void NativeAnimatedNodeManager::RemoveAnimatedEventFromView(
    int64_t viewTag,
    const std::string &eventName,
    int64_t animatedValueTag) {
  const auto key = std::make_tuple(viewTag, eventName);
  if (m_eventDrivers.count(key)) {
    auto &drivers = m_eventDrivers.at(key);

    for (auto iterator = drivers.begin(); iterator != drivers.end();) {
      if (const auto value = iterator->get()->AnimatedValue()) {
        if (value->Tag() == animatedValueTag) {
          iterator = drivers.erase(iterator);
          continue;
        }
      }
      ++iterator;
    }

    if (!drivers.size()) {
      m_eventDrivers.erase(key);
    }
  }
}

void NativeAnimatedNodeManager::ProcessDelayedPropsNodes() {
  // If StartAnimations fails we'll put the props nodes back into this queue to
  // try again when the next batch completes. Because of this we need to copy
  // the props to change into a local and clear the member before we begin.
  const auto delayedPropsNodes = m_delayedPropsNodes;
  m_delayedPropsNodes.clear();
  for (const auto tag : delayedPropsNodes) {
    if (m_propsNodes.count(tag) > 0) {
      m_propsNodes.at(tag)->StartAnimations();
    }
  }
}

void NativeAnimatedNodeManager::AddDelayedPropsNode(
    int64_t propsNodeTag,
    const winrt::Microsoft::ReactNative::ReactContext &context) {
#if DEBUG
  assert(m_propsNodes.at(propsNodeTag)->UseComposition());
#endif
  m_delayedPropsNodes.push_back(propsNodeTag);
  if (m_delayedPropsNodes.size() <= 1) {
    if (const auto uiManger = Microsoft::ReactNative::GetNativeUIManager(context).lock()) {
      uiManger->AddBatchCompletedCallback([this]() { ProcessDelayedPropsNodes(); });
    }
  }
}

AnimatedNode *NativeAnimatedNodeManager::GetAnimatedNode(int64_t tag) {
  if (m_valueNodes.count(tag)) {
    return m_valueNodes.at(tag).get();
  }
  if (m_styleNodes.count(tag)) {
    return m_styleNodes.at(tag).get();
  }
  if (m_propsNodes.count(tag)) {
    return m_propsNodes.at(tag).get();
  }
  if (m_transformNodes.count(tag)) {
    return m_transformNodes.at(tag).get();
  }
  if (m_trackingNodes.count(tag)) {
    return m_trackingNodes.at(tag).get();
  }
  return static_cast<AnimatedNode *>(nullptr);
}

ValueAnimatedNode *NativeAnimatedNodeManager::GetValueAnimatedNode(int64_t tag) {
  if (m_valueNodes.count(tag)) {
    return m_valueNodes.at(tag).get();
  }
  return static_cast<ValueAnimatedNode *>(nullptr);
}

PropsAnimatedNode *NativeAnimatedNodeManager::GetPropsAnimatedNode(int64_t tag) {
  if (m_propsNodes.count(tag)) {
    return m_propsNodes.at(tag).get();
  }
  return static_cast<PropsAnimatedNode *>(nullptr);
}

StyleAnimatedNode *NativeAnimatedNodeManager::GetStyleAnimatedNode(int64_t tag) {
  if (m_styleNodes.count(tag)) {
    return m_styleNodes.at(tag).get();
  }
  return static_cast<StyleAnimatedNode *>(nullptr);
}

TransformAnimatedNode *NativeAnimatedNodeManager::GetTransformAnimatedNode(int64_t tag) {
  if (m_transformNodes.count(tag)) {
    return m_transformNodes.at(tag).get();
  }
  return static_cast<TransformAnimatedNode *>(nullptr);
}

TrackingAnimatedNode *NativeAnimatedNodeManager::GetTrackingAnimatedNode(int64_t tag) {
  if (m_trackingNodes.count(tag)) {
    return m_trackingNodes.at(tag).get();
  }
  return nullptr;
}

void NativeAnimatedNodeManager::RemoveActiveAnimation(int64_t tag) {
  m_activeAnimations.erase(tag);
}

void NativeAnimatedNodeManager::RemoveStoppedAnimation(
    int64_t tag,
    const std::shared_ptr<NativeAnimatedNodeManager> &manager) {
  if (m_pendingCompletionAnimations.count(tag)) {
    // Remove stopped animation for value node entry
    const auto animation = m_pendingCompletionAnimations.at(tag);
    const auto nodeTag = animation->AnimatedValueTag();
    // If the animation was stopped, attempt to start deferred animations.
    if (m_valuesWithStoppedAnimation.erase(nodeTag)) {
      StartDeferredAnimationsForValueNode(nodeTag, manager);
    }
    m_pendingCompletionAnimations.erase(tag);
  }
}

void NativeAnimatedNodeManager::StartDeferredAnimationsForValueNode(
    int64_t tag,
    const std::shared_ptr<NativeAnimatedNodeManager> &manager) {
  if (m_deferredAnimationForValues.count(tag)) {
    const auto deferredAnimationTag = m_deferredAnimationForValues.at(tag);
    StartAnimationAndTrackingNodes(deferredAnimationTag, tag, manager);
    m_deferredAnimationForValues.erase(tag);
  }
}

void NativeAnimatedNodeManager::StartAnimationAndTrackingNodes(
    int64_t tag,
    int64_t nodeTag,
    const std::shared_ptr<NativeAnimatedNodeManager> &manager) {
  if (m_activeAnimations.count(tag)) {
    m_activeAnimations.at(tag)->StartAnimation();
    for (auto const &trackingAndLead : m_trackingAndLeadNodeTags) {
      if (std::get<1>(trackingAndLead) == nodeTag) {
        RestartTrackingAnimatedNode(std::get<0>(trackingAndLead), std::get<1>(trackingAndLead), manager);
      }
    }
  }
}

void NativeAnimatedNodeManager::RunUpdates(winrt::TimeSpan renderingTime) {
  auto hasFinishedAnimations = false;
  std::unordered_set<int64_t> updatingNodes{};
  updatingNodes = std::move(m_updatedNodes);

  // Increment animation drivers
  for (auto id : m_activeAnimationIds) {
    auto &animation = m_activeAnimations.at(id);
    animation->RunAnimationStep(renderingTime);
    updatingNodes.insert(animation->AnimatedValueTag());
    if (animation->IsComplete()) {
      hasFinishedAnimations = true;
    }
  }

  UpdateNodes(updatingNodes);

  if (hasFinishedAnimations) {
    for (auto id : m_activeAnimationIds) {
      auto &animation = m_activeAnimations.at(id);
      if (animation->IsComplete()) {
        animation->DoCallback(true);
        m_activeAnimations.erase(id);
      }
    }
  }
}

void NativeAnimatedNodeManager::EnsureRendering() {
  m_renderingRevoker =
      xaml::Media::CompositionTarget::Rendering(winrt::auto_revoke, {this, &NativeAnimatedNodeManager::OnRendering});
}

void NativeAnimatedNodeManager::OnRendering(winrt::IInspectable const &sender, winrt::IInspectable const &args) {
  // The `UpdateActiveAnimationIds` method only tracks animations where
  // composition is not used, so if only UI.Composition animations are active,
  // this rendering callback will not run.
  UpdateActiveAnimationIds();
  if (m_activeAnimationIds.size() > 0 || m_updatedNodes.size() > 0) {
    if (const auto renderingArgs = args.try_as<xaml::Media::RenderingEventArgs>()) {
      RunUpdates(renderingArgs.RenderingTime());
    }
  } else {
    m_renderingRevoker.revoke();
  }
}

void NativeAnimatedNodeManager::StopAnimationsForNode(int64_t tag) {
  UpdateActiveAnimationIds();
  for (auto id : m_activeAnimationIds) {
    auto &animation = m_activeAnimations.at(id);
    if (tag == animation->AnimatedValueTag()) {
      animation->DoCallback(false);
      m_activeAnimations.erase(id);
    }
  }
}

void NativeAnimatedNodeManager::UpdateActiveAnimationIds() {
  m_activeAnimationIds.clear();
  for (const auto &pair : m_activeAnimations) {
    if (!pair.second->UseComposition()) {
      m_activeAnimationIds.push_back(pair.first);
    }
  }
}

void NativeAnimatedNodeManager::UpdateNodes(std::unordered_set<int64_t> &nodes) {
  auto activeNodesCount = 0;
  auto updatedNodesCount = 0;

  // BFS state
  std::unordered_map<int64_t, int64_t> bfsColors;
  std::unordered_map<int64_t, int64_t> incomingNodeCounts;

  // STEP 1.
  // BFS over graph of nodes starting from IDs in `nodes` argument and IDs that are attached to
  // active animations (from `m_activeAnimations)`. Update `incomingNodeCounts` map for each node
  // during that BFS. Store number of visited nodes in `activeNodesCount`. We "execute" active
  // animations as a part of this step.

  m_animatedGraphBFSColor++; /* use new color */
  if (m_animatedGraphBFSColor == 0) {
    // value "0" is used as an initial color for a new node, using it in BFS may cause some nodes to be skipped.
    m_animatedGraphBFSColor++;
  }

  std::queue<int64_t> nodesQueue{};
  for (auto id : nodes) {
    if (!bfsColors.count(id) || bfsColors.at(id) != m_animatedGraphBFSColor) {
      bfsColors[id] = m_animatedGraphBFSColor;
      activeNodesCount++;
      nodesQueue.push(id);
    }
  }

  while (nodesQueue.size() > 0) {
    auto id = nodesQueue.front();
    nodesQueue.pop();
    if (auto node = GetAnimatedNode(id)) {
      for (auto &childId : node->Children()) {
        if (!incomingNodeCounts.count(childId)) {
          incomingNodeCounts[childId] = 1;
        } else {
          incomingNodeCounts.at(childId)++;
        }

        if (!bfsColors.count(childId) || bfsColors.at(childId) != m_animatedGraphBFSColor) {
          bfsColors[childId] = m_animatedGraphBFSColor;
          activeNodesCount++;
          nodesQueue.push(childId);
        }
      }
    }
  }

  // STEP 2
  // BFS over the graph of active nodes in topological order -> visit node only when all its
  // "predecessors" in the graph have already been visited. It is important to visit nodes in that
  // order as they may often use values of their predecessors in order to calculate "next state"
  // of their own. We start by determining the starting set of nodes by looking for nodes with
  // `incomingNodeCounts[id] = 0` (those can only be the ones that we start BFS in the previous
  // step). We store number of visited nodes in this step in `updatedNodesCount`

  m_animatedGraphBFSColor++;
  if (m_animatedGraphBFSColor == 0) {
    // see reasoning for this check a few lines above
    m_animatedGraphBFSColor++;
  }

  // find nodes with zero "incoming nodes", those can be either nodes from `m_updatedNodes` or
  // ones connected to active animations
  for (auto id : nodes) {
    if (!incomingNodeCounts.count(id) ||
        incomingNodeCounts.at(id) == 0 && bfsColors.at(id) != m_animatedGraphBFSColor) {
      bfsColors[id] = m_animatedGraphBFSColor;
      updatedNodesCount++;
      nodesQueue.push(id);
    }
  }

  // Run main "update" loop
  while (nodesQueue.size() > 0) {
    auto id = nodesQueue.front();
    nodesQueue.pop();
    if (auto node = GetAnimatedNode(id)) {
      node->Update();
      if (auto propsNode = GetPropsAnimatedNode(id)) {
        propsNode->UpdateView();
      } else if (auto valueNode = GetValueAnimatedNode(id)) {
        valueNode->OnValueUpdate();
      }

      for (auto &childId : node->Children()) {
        auto &incomingCount = incomingNodeCounts.at(childId);
        auto &bfsColor = bfsColors.at(childId);
        incomingCount--;
        if (bfsColor != m_animatedGraphBFSColor && incomingCount == 0) {
          bfsColor = m_animatedGraphBFSColor;
          updatedNodesCount++;
          nodesQueue.push(childId);
        }
      }
    }
  }

  // Verify that we've visited *all* active nodes. Throw otherwise as this would mean there is a
  // cycle in animated node graph. We also take advantage of the fact that all active nodes are
  // visited in the step above so that `incomingNodeCounts` for all node IDs are set to zero
  assert(activeNodesCount == updatedNodesCount);
}
} // namespace Microsoft::ReactNative
