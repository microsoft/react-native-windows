// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"

#include "NativeAnimatedNodeManager.h"
#include "ValueAnimatedNode.h"

namespace react {
namespace uwp {
ValueAnimatedNode::ValueAnimatedNode(
    int64_t tag,
    const folly::dynamic &config,
    const std::shared_ptr<NativeAnimatedNodeManager> &manager)
    : AnimatedNode(tag, manager) {
  m_propertySet = winrt::Window::Current().Compositor().CreatePropertySet();
  m_propertySet.InsertScalar(
      s_valueName,
      static_cast<float>(
          config.find(s_jsValueName).dereference().second.asDouble()));
  m_propertySet.InsertScalar(
      s_offsetName,
      static_cast<float>(
          config.find(s_jsOffsetName).dereference().second.asDouble()));
}

ValueAnimatedNode::ValueAnimatedNode(
    int64_t tag,
    const std::shared_ptr<NativeAnimatedNodeManager> &manager)
    : AnimatedNode(tag, manager) {
  m_propertySet = winrt::Window::Current().Compositor().CreatePropertySet();
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

void ValueAnimatedNode::AddActiveAnimation(int64_t animationTag) {
  m_activeAnimations.insert(animationTag);
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
}

void ValueAnimatedNode::AddActiveTrackingNode(int64_t trackingNodeTag) {
  m_activeTrackingNodes.insert(trackingNodeTag);
}

void ValueAnimatedNode::RemoveActiveTrackingNode(int64_t trackingNodeTag) {
  m_activeTrackingNodes.erase(trackingNodeTag);
}

void ValueAnimatedNode::UpdateTrackingNodes() {
  if (auto const manager = m_manager.lock()) {
    for (auto trackingNodeTag : m_activeTrackingNodes) {
      if (auto trackingNode =
              manager->GetTrackingAnimatedNode(trackingNodeTag)) {
        trackingNode->Update();
      }
    }
  }
}

} // namespace uwp
} // namespace react
