// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"

#include "FacadeType.h"
#include "NativeAnimatedNodeManager.h"
#include "TransformAnimatedNode.h"

namespace Microsoft::ReactNative {
TransformAnimatedNode::TransformAnimatedNode(
    int64_t tag,
    const winrt::Microsoft::ReactNative::JSValueObject &config,
    const std::shared_ptr<NativeAnimatedNodeManager> &manager)
    : AnimatedNode(tag, config, manager) {
  for (const auto &transform : config[s_transformsName].AsArray()) {
    const auto property = transform[s_propertyName].AsString();
    if (transform[s_typeName].AsString() == s_animatedName) {
      const auto inputTag = transform[s_nodeTagName].AsInt64();
      assert(HasCompatibleAnimationDriver(inputTag));
      m_transformConfigs.push_back(TransformConfig{property, inputTag, 0});
    } else {
      m_transformConfigs.push_back(TransformConfig{property, s_unsetNodeTag, transform[s_valueName].AsDouble()});
    }
  }
}

std::unordered_map<FacadeType, int64_t> TransformAnimatedNode::GetMapping() {
  assert(m_useComposition);
  std::unordered_map<FacadeType, int64_t> mapping;
  for (const auto &config : m_transformConfigs) {
    if (config.nodeTag != s_unsetNodeTag) {
      const auto &facade = StringToFacadeType(config.property);
      if (facade != FacadeType::None) {
        mapping.insert({facade, config.nodeTag});
      }
    }
  }
  return mapping;
}

void TransformAnimatedNode::CollectViewUpdates(winrt::Microsoft::ReactNative::JSValueObject &props) {
  assert(!m_useComposition);
  winrt::Microsoft::ReactNative::JSValueArray transforms;
  if (const auto manager = m_manager.lock()) {
    for (const auto &transformConfig : m_transformConfigs) {
      std::optional<double> value;
      if (transformConfig.nodeTag == s_unsetNodeTag) {
        value = transformConfig.value;
      } else {
        if (const auto node = manager->GetValueAnimatedNode(transformConfig.nodeTag)) {
          value = node->Value();
        }
      }

      if (value) {
        transforms.emplace_back(
            winrt::Microsoft::ReactNative::JSValueObject{{transformConfig.property, value.value()}});
      }
    }
  }

  props[s_transformPropName] = std::move(transforms);
}

} // namespace Microsoft::ReactNative
