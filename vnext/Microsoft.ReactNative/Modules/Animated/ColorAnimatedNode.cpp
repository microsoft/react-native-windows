// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"

#include <UI.Composition.h>
#include <Utils/ValueUtils.h>
#include "ColorAnimatedNode.h"
#include "NativeAnimatedNodeManager.h"

namespace Microsoft::ReactNative {
ColorAnimatedNode::ColorAnimatedNode(
    int64_t tag,
    const winrt::Microsoft::ReactNative::JSValueObject &config,
    const std::shared_ptr<NativeAnimatedNodeManager> &manager)
    : AnimatedNode(tag, config, manager) {
  m_rNodeId = config[s_rNodeName].AsInt32();
  m_gNodeId = config[s_gNodeName].AsInt32();
  m_bNodeId = config[s_bNodeName].AsInt32();
  m_aNodeId = config[s_aNodeName].AsInt32();
  m_nativeColor = config[s_nativeColorName].Copy();

  if (!m_useComposition) {
    TryApplyNativeColor();
  } else {
    assert(false && "ColorAnimatedNode not supported");
  }
}

uint32_t ColorAnimatedNode::GetColor() {
  uint32_t r = 0;
  uint32_t g = 0;
  uint32_t b = 0;
  uint32_t a = 0;

  if (const auto manager = m_manager.lock()) {
    if (const auto rNode = manager->GetValueAnimatedNode(m_rNodeId)) {
      r = std::clamp(static_cast<uint32_t>(std::round(rNode->Value())), 0u, 255u);
    }
    if (const auto gNode = manager->GetValueAnimatedNode(m_gNodeId)) {
      g = std::clamp(static_cast<uint32_t>(std::round(gNode->Value())), 0u, 255u);
    }
    if (const auto bNode = manager->GetValueAnimatedNode(m_bNodeId)) {
      b = std::clamp(static_cast<uint32_t>(std::round(bNode->Value())), 0u, 255u);
    }
    if (const auto aNode = manager->GetValueAnimatedNode(m_aNodeId)) {
      a = std::clamp(static_cast<uint32_t>(std::round(aNode->Value() * 255)), 0u, 255u);
    }
  }

  const auto result = (a << 24) | (r << 16) | (g << 8) | b;
  return result;
}

void ColorAnimatedNode::TryApplyNativeColor() {
  if (m_nativeColor.IsNull()) {
    return;
  }

  const auto brush = BrushFromColorObject(m_nativeColor).try_as<xaml::Media::SolidColorBrush>();
  if (!brush) {
    return;
  }

  if (const auto manager = m_manager.lock()) {
    if (const auto rNode = manager->GetValueAnimatedNode(m_rNodeId)) {
      rNode->RawValue(brush.Color().R);
    }
    if (const auto gNode = manager->GetValueAnimatedNode(m_gNodeId)) {
      gNode->RawValue(brush.Color().G);
    }
    if (const auto bNode = manager->GetValueAnimatedNode(m_bNodeId)) {
      bNode->RawValue(brush.Color().B);
    }
    if (const auto aNode = manager->GetValueAnimatedNode(m_aNodeId)) {
      aNode->RawValue(static_cast<double>(brush.Color().A) / 255);
    }
  }
}

} // namespace Microsoft::ReactNative
