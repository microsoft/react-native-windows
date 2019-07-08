// Copyright (c) Microsoft Corporation. All rights reserved.
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

#include "DecayAnimationDriver.h"
#include "FrameAnimationDriver.h"

#include "AnimatedNodeType.h"
#include "AnimationType.h"
#include "FacadeType.h"

#include <ReactUWP\Modules\NativeUIManager.h>
#include <Windows.Foundation.h>

namespace react {
namespace uwp {
void NativeAnimatedNodeManager::CreateAnimatedNode(
    int64_t tag,
    const folly::dynamic &config,
    const std::weak_ptr<IReactInstance> &instance,
    const std::shared_ptr<NativeAnimatedNodeManager> &manager) {
  if (m_animationNodes.count(tag) > 0 || m_propsNodes.count(tag) > 0 ||
      m_styleNodes.count(tag) > 0 || m_valueNodes.count(tag) > 0) {
    throw new std::invalid_argument(
        "AnimatedNode with tag " + std::to_string(tag) + " already exists.");
    return;
  }

  switch (const auto type = AnimatedNodeTypeFromString(
              config.find("type").dereference().second.getString())) {
    case AnimatedNodeType::Style: {
      m_styleNodes.emplace(
          tag, std::make_unique<StyleAnimatedNode>(tag, config, manager));
      break;
    }
    case AnimatedNodeType::Value: {
      m_valueNodes.emplace(
          tag, std::make_unique<ValueAnimatedNode>(tag, config, manager));
      break;
    }
    case AnimatedNodeType::Props: {
      m_propsNodes.emplace(
          tag,
          std::make_unique<PropsAnimatedNode>(tag, config, instance, manager));
      break;
    }
    case AnimatedNodeType::Interpolation: {
      m_valueNodes.emplace(
          tag,
          std::make_unique<InterpolationAnimatedNode>(tag, config, manager));
      break;
    }
    case AnimatedNodeType::Addition: {
      m_valueNodes.emplace(
          tag, std::make_unique<AdditionAnimatedNode>(tag, config, manager));
      break;
    }
    case AnimatedNodeType::Subtraction: {
      m_valueNodes.emplace(
          tag, std::make_unique<SubtractionAnimatedNode>(tag, config, manager));
      break;
    }
    case AnimatedNodeType::Division: {
      m_valueNodes.emplace(
          tag, std::make_unique<DivisionAnimatedNode>(tag, config, manager));
      break;
    }
    case AnimatedNodeType::Multiplication: {
      m_valueNodes.emplace(
          tag,
          std::make_unique<MultiplicationAnimatedNode>(tag, config, manager));
      break;
    }
    case AnimatedNodeType::Modulus: {
      m_valueNodes.emplace(
          tag, std::make_unique<ModulusAnimatedNode>(tag, config, manager));
      break;
    }
    case AnimatedNodeType::Diffclamp: {
      m_valueNodes.emplace(
          tag, std::make_unique<DiffClampAnimatedNode>(tag, config, manager));
      break;
    }
    case AnimatedNodeType::Transform: {
      m_transformNodes.emplace(
          tag, std::make_unique<TransformAnimatedNode>(tag, config, manager));
      break;
    }
    case AnimatedNodeType::Tracking: {
      break;
    }
    default: {
      assert(false);
      break;
    }
  }
}

void NativeAnimatedNodeManager::ConnectAnimatedNodeToView(
    int64_t propsNodeTag,
    int64_t viewTag) {
  m_propsNodes.at(propsNodeTag)->ConnectToView(viewTag);
}

void NativeAnimatedNodeManager::DisconnectAnimatedNodeToView(
    int64_t propsNodeTag,
    int64_t viewTag) {
  m_propsNodes.at(propsNodeTag)->DisconnectFromView(viewTag);
}

void NativeAnimatedNodeManager::ConnectAnimatedNode(
    int64_t parentNodeTag,
    int64_t childNodeTag) {
  if (const auto parentNode = GetAnimatedNode(parentNodeTag)) {
    parentNode->AddChild(childNodeTag);
  }
}

void NativeAnimatedNodeManager::DisconnectAnimatedNode(
    int64_t parentNodeTag,
    int64_t childNodeTag) {
  if (const auto parentNode = GetAnimatedNode(parentNodeTag)) {
    parentNode->RemoveChild(childNodeTag);
  }
}

void NativeAnimatedNodeManager::StopAnimation(int64_t animationId) {
  if (m_activeAnimations.count(animationId)) {
    if (const auto animation = m_activeAnimations.at(animationId).get()) {
      animation->StopAnimation();
      m_activeAnimations.erase(animationId);
    }
  }
}

void NativeAnimatedNodeManager::StartAnimatingNode(
    int64_t animationId,
    int64_t animatedNodeTag,
    const folly::dynamic &animationConfig,
    const Callback &endCallback,
    const std::shared_ptr<NativeAnimatedNodeManager> &manager) {
  switch (AnimationTypeFromString(
      animationConfig.find("type").dereference().second.getString())) {
    case AnimationType::Decay:
      m_activeAnimations.emplace(
          animationId,
          std::make_unique<DecayAnimationDriver>(
              animationId,
              animatedNodeTag,
              endCallback,
              animationConfig,
              manager));
      break;
    case AnimationType::Frames:
      m_activeAnimations.emplace(
          animationId,
          std::make_unique<FrameAnimationDriver>(
              animationId,
              animatedNodeTag,
              endCallback,
              animationConfig,
              manager));
      break;
    case AnimationType::Spring:
      // TODO: implement spring animations tracked by issue #2681
      break;
    default:
      assert(false);
      break;
  }

  if (m_activeAnimations.count(animationId)) {
    m_activeAnimations.at(animationId)->StartAnimation();
  }
}

void NativeAnimatedNodeManager::DropAnimatedNode(int64_t tag) {
  m_valueNodes.erase(tag);
  m_propsNodes.erase(tag);
  m_styleNodes.erase(tag);
  m_transformNodes.erase(tag);
}

void NativeAnimatedNodeManager::SetAnimatedNodeValue(
    int64_t tag,
    double value) {
  if (const auto valueNode = m_valueNodes.at(tag).get()) {
    valueNode->RawValue(static_cast<float>(value));
  }
}

void NativeAnimatedNodeManager::SetAnimatedNodeOffset(
    int64_t tag,
    double offset) {
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
  const auto valueNodeTag = static_cast<int64_t>(
	  eventMapping.find("animatedValueTag").dereference().second.asDouble());
  const auto pathList = 
	  eventMapping.find("nativeEventPath").dereference().second;

  const auto key = std::make_tuple(viewTag, eventName);
  if (m_eventDrivers.count(key)) {
    m_eventDrivers.at(key).emplace_back(std::make_unique<EventAnimationDriver>(
        pathList, valueNodeTag, manager));
  } else {
    auto vector = std::vector<std::unique_ptr<EventAnimationDriver>>{};
    vector.emplace_back(std::make_unique<EventAnimationDriver>(
        pathList, valueNodeTag, manager));
    m_eventDrivers.insert({key, std::move(vector)});
  }
}

void NativeAnimatedNodeManager::RemoveAnimatedEventFromView(
	int64_t viewTag,
	const std::string &eventName,
	int64_t animatedValueTag) {
  const auto key = std::make_tuple(viewTag, eventName);
  if (m_eventDrivers.count(key)) {
    auto& drivers = m_eventDrivers.at(key);

    for (auto iterator = drivers.begin(); iterator != drivers.end();) {
      if (const auto value = iterator->get()->AnimatedValue()) {
        if(value->Tag() == animatedValueTag) {
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
  // try again when the next batch completes. Because of this we need to copy the
  // props to change into a local and clear the member before we begin.
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
    const std::shared_ptr<IReactInstance> &instance) {
  m_delayedPropsNodes.push_back(propsNodeTag);
  if (m_delayedPropsNodes.size() <= 1) {
    static_cast<NativeUIManager *>(instance->NativeUIManager())
        ->AddBatchCompletedCallback([this]() { ProcessDelayedPropsNodes(); });
  }
}

AnimationDriver *NativeAnimatedNodeManager::GetAnimationNode(int64_t tag) {
  if (m_animationNodes.count(tag)) {
    return m_animationNodes.at(tag).get();
  }
  return static_cast<AnimationDriver *>(nullptr);
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
  return static_cast<AnimatedNode *>(nullptr);
}

ValueAnimatedNode *NativeAnimatedNodeManager::GetValueAnimatedNode(
    int64_t tag) {
  if (m_valueNodes.count(tag)) {
    return m_valueNodes.at(tag).get();
  }
  return static_cast<ValueAnimatedNode *>(nullptr);
}

PropsAnimatedNode *NativeAnimatedNodeManager::GetPropsAnimatedNode(
    int64_t tag) {
  if (m_propsNodes.count(tag)) {
    return m_propsNodes.at(tag).get();
  }
  return static_cast<PropsAnimatedNode *>(nullptr);
}

StyleAnimatedNode *NativeAnimatedNodeManager::GetStyleAnimatedNode(
    int64_t tag) {
  if (m_styleNodes.count(tag)) {
    return m_styleNodes.at(tag).get();
  }
  return static_cast<StyleAnimatedNode *>(nullptr);
}

TransformAnimatedNode *NativeAnimatedNodeManager::GetTransformAnimatedNode(
    int64_t tag) {
  if (m_transformNodes.count(tag)) {
    return m_transformNodes.at(tag).get();
  }
  return static_cast<TransformAnimatedNode *>(nullptr);
}
} // namespace uwp
} // namespace react
