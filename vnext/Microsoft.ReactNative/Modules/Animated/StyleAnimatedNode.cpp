// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.
#include "pch.h"

#include "FacadeType.h"
#include "NativeAnimatedNodeManager.h"
#include "StyleAnimatedNode.h"

namespace Microsoft::ReactNative {
StyleAnimatedNode::StyleAnimatedNode(
    int64_t tag,
    const winrt::Microsoft::ReactNative::JSValueObject &config,
    const std::shared_ptr<NativeAnimatedNodeManager> &manager)
    : AnimatedNode(tag, config, manager) {
  for (const auto &entry : config[s_styleName].AsObject()) {
    const auto inputTag = entry.second.AsInt64();
    assert(HasCompatibleAnimationDriver(inputTag));
    m_propMapping.insert({entry.first, inputTag});
  }
}

void StyleAnimatedNode::CollectViewUpdates(winrt::Microsoft::ReactNative::JSValueObject &propsMap) {
  assert(!m_useComposition);
  auto rawValue = 0.0;
  for (const auto &propMapping : m_propMapping) {
    if (const auto manager = m_manager.lock()) {
      if (const auto transformNode = manager->GetTransformAnimatedNode(propMapping.second)) {
        transformNode->CollectViewUpdates(propsMap);
      } else if (const auto node = manager->GetValueAnimatedNode(propMapping.second)) {
        propsMap[propMapping.first] = node->Value();
      }
    }
  }
}

std::unordered_map<FacadeType, int64_t> StyleAnimatedNode::GetMapping() {
  assert(m_useComposition);
  std::unordered_map<FacadeType, int64_t> mapping;
  for (const auto &prop : m_propMapping) {
    if (const auto manager = m_manager.lock()) {
      if (const auto transformNode = manager->GetTransformAnimatedNode(prop.second)) {
        const auto transformMapping = transformNode->GetMapping();
        mapping.insert(transformMapping.begin(), transformMapping.end());
        break;
      }
    }

    const auto &facade = StringToFacadeType(prop.first);
    if (facade != FacadeType::None) {
      mapping.insert({facade, prop.second});
    }
  }
  return mapping;
}
} // namespace Microsoft::ReactNative
