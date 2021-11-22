// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"

#include "NativeAnimatedNodeManager.h"
#include "ValueAnimatedNode.h"

namespace Microsoft::ReactNative {
ValueAnimatedNode::ValueAnimatedNode(
    int64_t tag,
    const folly::dynamic &config,
    const std::shared_ptr<NativeAnimatedNodeManager> &manager)
    : AnimatedNode(tag, manager) {
  // TODO: Islands - need to get the XamlView associated with this animation in order to
  // use the compositor Microsoft::ReactNative::GetCompositor(xamlView)
  m_propertySet = Microsoft::ReactNative::GetCompositor().CreatePropertySet();
  m_propertySet.InsertScalar(
      s_valueName, static_cast<float>(config.find(s_jsValueName).dereference().second.asDouble()));
  m_propertySet.InsertScalar(
      s_offsetName, static_cast<float>(config.find(s_jsOffsetName).dereference().second.asDouble()));
}

ValueAnimatedNode::ValueAnimatedNode(int64_t tag, const std::shared_ptr<NativeAnimatedNodeManager> &manager)
    : AnimatedNode(tag, manager) {
  // TODO: Islands - need to get the XamlView associated with this animation in order to
  // use the compositor Microsoft::ReactNative::GetCompositor(xamlView)
  m_propertySet = Microsoft::ReactNative::GetCompositor().CreatePropertySet();
  m_propertySet.InsertScalar(s_valueName, 0.0);
  m_propertySet.InsertScalar(s_offsetName, 0.0);
}

double ValueAnimatedNode::RawValue() {
  auto rawValue = 0.0f;
  m_propertySet.TryGetScalar(s_valueName, rawValue);
  return rawValue;
}

void ValueAnimatedNode::RawValue(double value) {
  if (RawValue() != value) {
    m_propertySet.InsertScalar(s_valueName, static_cast<float>(value));
    UpdateTrackingNodes();
  }
}

double ValueAnimatedNode::Offset() {
  auto offset = 0.0f;
  m_propertySet.TryGetScalar(s_offsetName, offset);
  return offset;
}

void ValueAnimatedNode::Offset(double offset) {
  if (Offset() != offset) {
    m_propertySet.InsertScalar(s_offsetName, static_cast<float>(offset));
    UpdateTrackingNodes();
  }
}

double ValueAnimatedNode::Value() {
  auto rawValue = 0.0f;
  auto offset = 0.0f;
  m_propertySet.TryGetScalar(s_valueName, rawValue);
  m_propertySet.TryGetScalar(s_offsetName, offset);
  return static_cast<double>(rawValue) + static_cast<double>(offset);
}

void ValueAnimatedNode::FlattenOffset() {
  RawValue(RawValue() + Offset());
  Offset(0.0f);
}

void ValueAnimatedNode::ExtractOffset() {
  Offset(RawValue() + Offset());
  RawValue(0.0f);
}

void ValueAnimatedNode::AddDependentPropsNode(int64_t propsNodeTag) {
  m_dependentPropsNodes.insert(propsNodeTag);
}

void ValueAnimatedNode::RemoveDependentPropsNode(int64_t propsNodeTag) {
  m_dependentPropsNodes.erase(propsNodeTag);
}

void ValueAnimatedNode::AddActiveAnimation(std::shared_ptr<AnimationDriver> animation) {
  m_activeAnimations.insert({animation->Id(), animation});
  if (m_activeAnimations.size() == 1) {
    if (const auto manager = m_manager.lock()) {
      for (const auto &props : m_dependentPropsNodes) {
        if (const auto propsNode = manager->GetPropsAnimatedNode(props))
          propsNode->ResumeSuspendedAnimations(Tag());
      }
    }
  }
}

void ValueAnimatedNode::RemoveActiveAnimation(int64_t animationTag) {
  m_activeAnimations.erase(animationTag);
  if (!m_activeAnimations.size()) {
    if (const auto manager = m_manager.lock()) {
      for (const auto &props : m_dependentPropsNodes) {
        if (const auto propsNode = manager->GetPropsAnimatedNode(props))
          propsNode->DisposeCompletedAnimation(Tag());
      }
    }
  }

  // Start any deferred animations. If the animation was stopped before this is
  // run, we will get a null pointer for the animation driver. We also remove
  // any deferred animation tags in StopAnimation.
  m_stoppedAnimations.erase(animationTag);
  if (!m_stoppedAnimations.size() && m_deferredAnimations.size()) {
    if (const auto manager = m_manager.lock()) {
      for (const auto &deferredId : m_deferredAnimations) {
        if (const auto animationDriver = manager->GetActiveAnimation(deferredId)) {
          animationDriver->StartAnimation();
        }
      }
      m_deferredAnimations.clear();
    }
  }
}

void ValueAnimatedNode::AddActiveTrackingNode(int64_t trackingNodeTag) {
  m_activeTrackingNodes.insert(trackingNodeTag);
}

void ValueAnimatedNode::RemoveActiveTrackingNode(int64_t trackingNodeTag) {
  m_activeTrackingNodes.erase(trackingNodeTag);
}

bool ValueAnimatedNode::HasStoppedAnimations() const noexcept {
  return m_stoppedAnimations.size();
}

void ValueAnimatedNode::DeferAnimation(int64_t animationTag) {
  m_deferredAnimations.insert(animationTag);
}

void ValueAnimatedNode::UpdateTrackingNodes() {
  if (auto const manager = m_manager.lock()) {
    for (auto trackingNodeTag : m_activeTrackingNodes) {
      if (auto trackingNode = manager->GetTrackingAnimatedNode(trackingNodeTag)) {
        trackingNode->Update();
      }
    }
  }
}

void ValueAnimatedNode::StopAnimation(int64_t animationTag) {
  if (m_deferredAnimations.count(animationTag)) {
    // This is not strictly necessary as the animation will be destroyed by the
    // NativeAnimatedNodesManager if it was stopped before its deferred start.
    m_deferredAnimations.erase(animationTag);
  } else {
    // The animation should already be running if it is not currently deferred.
    assert(m_activeAnimations.count(animationTag));
    m_stoppedAnimations.insert(animationTag);
  }
}

} // namespace Microsoft::ReactNative
