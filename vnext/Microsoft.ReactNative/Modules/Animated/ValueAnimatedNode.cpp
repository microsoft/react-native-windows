// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"

#include "NativeAnimatedNodeManager.h"
#include "ValueAnimatedNode.h"

namespace Microsoft::ReactNative {
ValueAnimatedNode::ValueAnimatedNode(
    int64_t tag,
    const winrt::Microsoft::ReactNative::JSValueObject &config,
    const std::shared_ptr<NativeAnimatedNodeManager> &manager)
    : AnimatedNode(tag, config, manager) {
  auto value = 0.0;
  auto offset = 0.0;
  if (config.count(s_jsValueName) && config.count(s_jsOffsetName)) {
    value = config[s_jsValueName].AsDouble();
    offset = config[s_jsOffsetName].AsDouble();
  }

  if (m_useComposition) {
    m_propertySet = manager->Compositor().CreatePropertySet();
    m_propertySet.InsertScalar(s_valueName, static_cast<float>(value));
    m_propertySet.InsertScalar(s_offsetName, static_cast<float>(offset));
  } else {
    m_value = value;
    m_offset = offset;
  }
}

double ValueAnimatedNode::RawValue() {
  if (m_useComposition) {
    auto rawValue = 0.0f;
    m_propertySet.TryGetScalar(s_valueName, rawValue);
    return rawValue;
  } else {
    return m_value;
  }
}

void ValueAnimatedNode::RawValue(double value) {
  if (RawValue() != value) {
    if (m_useComposition) {
      m_propertySet.InsertScalar(s_valueName, static_cast<float>(value));
      UpdateTrackingNodes();
    } else {
      m_value = value;
    }
  }
}

double ValueAnimatedNode::Offset() {
  if (m_useComposition) {
    auto offset = 0.0f;
    m_propertySet.TryGetScalar(s_offsetName, offset);
    return offset;
  } else {
    return m_offset;
  }
}

void ValueAnimatedNode::Offset(double offset) {
  if (Offset() != offset) {
    if (m_useComposition) {
      m_propertySet.InsertScalar(s_offsetName, static_cast<float>(offset));
      UpdateTrackingNodes();
    } else {
      m_offset = offset;
    }
  }
}

double ValueAnimatedNode::Value() {
  return RawValue() + Offset();
}

void ValueAnimatedNode::FlattenOffset() {
  RawValue(RawValue() + Offset());
  Offset(0.0f);
}

void ValueAnimatedNode::ExtractOffset() {
  Offset(RawValue() + Offset());
  RawValue(0.0f);
}

void ValueAnimatedNode::OnValueUpdate() {
  if (m_valueListener) {
    m_valueListener(Value());
  }
}

void ValueAnimatedNode::ValueListener(const ValueListenerCallback &callback) {
  m_valueListener = callback;
}

void ValueAnimatedNode::AddDependentPropsNode(int64_t propsNodeTag) {
  assert(m_useComposition);
  m_dependentPropsNodes.insert(propsNodeTag);
}

void ValueAnimatedNode::RemoveDependentPropsNode(int64_t propsNodeTag) {
  assert(m_useComposition);
  m_dependentPropsNodes.erase(propsNodeTag);
}

void ValueAnimatedNode::AddActiveAnimation(int64_t animationTag) {
  assert(m_useComposition);
  m_activeAnimations.insert(animationTag);
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
  assert(m_useComposition);
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
  assert(m_useComposition);
  m_activeTrackingNodes.insert(trackingNodeTag);
}

void ValueAnimatedNode::RemoveActiveTrackingNode(int64_t trackingNodeTag) {
  assert(m_useComposition);
  m_activeTrackingNodes.erase(trackingNodeTag);
}

void ValueAnimatedNode::UpdateTrackingNodes() {
  assert(m_useComposition);
  if (auto const manager = m_manager.lock()) {
    for (auto trackingNodeTag : m_activeTrackingNodes) {
      if (auto trackingNode = manager->GetTrackingAnimatedNode(trackingNodeTag)) {
        trackingNode->Update();
      }
    }
  }
}

} // namespace Microsoft::ReactNative
