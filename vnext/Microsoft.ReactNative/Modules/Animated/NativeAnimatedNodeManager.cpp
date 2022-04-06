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

namespace Microsoft::ReactNative {
void NativeAnimatedNodeManager::CreateAnimatedNode(
    int64_t tag,
    const folly::dynamic &config,
    const Mso::CntPtr<Mso::React::IReactContext> &context,
    const std::shared_ptr<NativeAnimatedNodeManager> &manager) {
  if (m_transformNodes.count(tag) > 0 || m_propsNodes.count(tag) > 0 || m_styleNodes.count(tag) > 0 ||
      m_valueNodes.count(tag) > 0) {
    throw std::invalid_argument("AnimatedNode with tag " + std::to_string(tag) + " already exists.");
    return;
  }

  switch (const auto type = AnimatedNodeTypeFromString(config.find("type").dereference().second.getString())) {
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

void NativeAnimatedNodeManager::GetValue(int64_t animatedNodeTag, const Callback &saveValueCallback) {
  if (const auto valueNode = m_valueNodes.at(animatedNodeTag).get()) {
    saveValueCallback(std::vector<folly::dynamic>{folly::dynamic(valueNode->Value())});
  }
}

void NativeAnimatedNodeManager::ConnectAnimatedNodeToView(int64_t propsNodeTag, int64_t viewTag) {
  m_propsNodes.at(propsNodeTag)->ConnectToView(viewTag);
}

void NativeAnimatedNodeManager::DisconnectAnimatedNodeToView(int64_t propsNodeTag, int64_t viewTag) {
  m_propsNodes.at(propsNodeTag)->DisconnectFromView(viewTag);
}

void NativeAnimatedNodeManager::ConnectAnimatedNode(int64_t parentNodeTag, int64_t childNodeTag) {
  if (const auto parentNode = GetAnimatedNode(parentNodeTag)) {
    parentNode->AddChild(childNodeTag);
  }
}

void NativeAnimatedNodeManager::DisconnectAnimatedNode(int64_t parentNodeTag, int64_t childNodeTag) {
  if (const auto parentNode = GetAnimatedNode(parentNodeTag)) {
    parentNode->RemoveChild(childNodeTag);
  }
}

void NativeAnimatedNodeManager::StopAnimation(int64_t animationId, bool isTrackingAnimation) {
  if (m_activeAnimations.count(animationId)) {
    if (const auto animation = m_activeAnimations.at(animationId)) {
      animation->StopAnimation(isTrackingAnimation);

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
      auto const animatedValueTag = animation->AnimatedValueTag();
      auto const animationConfig = animation->AnimationConfig();
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
    const folly::dynamic &animationConfig,
    const Callback &endCallback,
    const std::shared_ptr<NativeAnimatedNodeManager> &manager,
    bool track) {
  auto updatedAnimationConfig = animationConfig;
  for (auto &item : m_activeAnimations) {
    if (item.second->AnimatedValueTag() == animatedToValueTag) {
      updatedAnimationConfig.insert(static_cast<folly::StringPiece>(s_toValueIdName), item.second->ToValue());

      switch (AnimationTypeFromString(animationConfig.find("type").dereference().second.getString())) {
        case AnimationType::Frames:
          updatedAnimationConfig.insert(
              static_cast<folly::StringPiece>(s_framesName), [animationConfig, activeFrames = item.second->Frames()]() {
                auto frames = folly::dynamic::array();
                for (auto const &frame : animationConfig.find("frames").dereference().second) {
                  (void)frame;
                  frames.push_back(0.0);
                }
                for (auto const &frame : activeFrames) {
                  frames.push_back(frame);
                }
                return frames;
              }());
          break;
        case AnimationType::Spring:
          updatedAnimationConfig.insert(
              static_cast<folly::StringPiece>(s_dynamicToValuesName), [activeFrames = item.second->Frames()]() {
                auto dynamicToValues = folly::dynamic::array();
                for (auto const &frame : activeFrames) {
                  dynamicToValues.push_back(frame);
                }
                return dynamicToValues;
              }());
          break;
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
    const folly::dynamic &animationConfig,
    const Callback &endCallback,
    const std::shared_ptr<NativeAnimatedNodeManager> &manager) {
  switch (AnimationTypeFromString(animationConfig.find("type").dereference().second.getString())) {
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
      folly::dynamic dynamicValues = [animationConfig]() {
        const auto dynamicValues = animationConfig.count(s_dynamicToValuesName);
        return dynamicValues ? animationConfig.at(s_dynamicToValuesName) : folly::dynamic::array();
      }();
      m_activeAnimations.emplace(
          animationId,
          std::make_shared<SpringAnimationDriver>(
              animationId, animatedNodeTag, endCallback, animationConfig, manager, dynamicValues));
      break;
    }
    default:
      assert(false);
      break;
  }

  // If the animated value node has any stopped animations, defer start until
  // all stopped animations fire completion callback and have latest values.
  if (m_activeAnimations.count(animationId)) {
    if (m_valuesWithStoppedAnimation.count(animatedNodeTag)) {
      // Since only one animation can be active per value at a time, there will
      // not be any other deferred animations for the value node.
      assert(!m_deferredAnimationForValues.count(animatedNodeTag));
      // Add the animation to the deferred animation map for the value tag.
      m_deferredAnimationForValues.insert({animatedNodeTag, animationId});
    } else {
      StartAnimationAndTrackingNodes(animationId, animatedNodeTag, manager);
    }
  }
}

void NativeAnimatedNodeManager::DropAnimatedNode(int64_t tag) {
  m_valueNodes.erase(tag);
  m_propsNodes.erase(tag);
  m_styleNodes.erase(tag);
  m_transformNodes.erase(tag);
}

void NativeAnimatedNodeManager::SetAnimatedNodeValue(int64_t tag, double value) {
  if (const auto valueNode = m_valueNodes.at(tag).get()) {
    valueNode->RawValue(static_cast<float>(value));
  }
}

void NativeAnimatedNodeManager::SetAnimatedNodeOffset(int64_t tag, double offset) {
  if (const auto valueNode = m_valueNodes.at(tag).get()) {
    valueNode->Offset(static_cast<float>(offset));
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

void NativeAnimatedNodeManager::AddAnimatedEventToView(
    int64_t viewTag,
    const std::string &eventName,
    const folly::dynamic &eventMapping,
    const std::shared_ptr<NativeAnimatedNodeManager> &manager) {
  const auto valueNodeTag = static_cast<int64_t>(eventMapping.find("animatedValueTag").dereference().second.asDouble());
  const auto pathList = eventMapping.find("nativeEventPath").dereference().second;

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
    const Mso::CntPtr<Mso::React::IReactContext> &context) {
  m_delayedPropsNodes.push_back(propsNodeTag);
  if (m_delayedPropsNodes.size() <= 1) {
    if (const auto uiManger = Microsoft::ReactNative::GetNativeUIManager(*context).lock()) {
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
} // namespace Microsoft::ReactNative
