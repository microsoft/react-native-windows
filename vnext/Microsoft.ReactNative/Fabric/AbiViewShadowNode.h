// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include "LayoutContext.g.h"
#include "ShadowNode.g.h"
#include "YogaLayoutableShadowNode.g.h"
#include <react/components/rnwcore/EventEmitters.h>
#include <unordered_map>
#include "AbiShadowNode.h"
#include "AbiState.h"
#include "AbiViewProps.h"

#include <react/renderer/components/view/ConcreteViewShadowNode.h>
#include <react/renderer/core/LayoutContext.h>

namespace winrt::Microsoft::ReactNative::implementation {

struct LayoutContext : LayoutContextT<LayoutContext> {
  LayoutContext(facebook::react::LayoutContext layoutContext) : m_layoutContext(layoutContext){};

  float PointScaleFactor() const noexcept {
    return m_layoutContext.pointScaleFactor;
  }
  void PointScaleFactor(float value) noexcept {
    m_layoutContext.pointScaleFactor = value;
  }
  bool SwapLeftAndRightInRTL() const noexcept {
    return m_layoutContext.swapLeftAndRightInRTL;
  }
  void SwapLeftAndRightInRTL(bool value) noexcept {
    m_layoutContext.swapLeftAndRightInRTL = value;
  }
  float FontSizeMultiplier() const noexcept {
    return m_layoutContext.fontSizeMultiplier;
  }
  void FontSizeMultiplier(float value) noexcept {
    m_layoutContext.fontSizeMultiplier = value;
  }
  winrt::Windows::Foundation::Point ViewportOffset() const noexcept {
    return {m_layoutContext.viewportOffset.x, m_layoutContext.viewportOffset.y};
  }
  void ViewportOffset(winrt::Windows::Foundation::Point value) noexcept {
    m_layoutContext.viewportOffset = {value.X, value.Y};
  }

  facebook::react::LayoutContext m_layoutContext;
};

struct YogaLayoutableShadowNode : YogaLayoutableShadowNodeT<YogaLayoutableShadowNode, implementation::ShadowNode> {
  YogaLayoutableShadowNode(std::shared_ptr<const facebook::react::ShadowNode> shadowNode) noexcept;

  void Layout(winrt::Microsoft::ReactNative::LayoutContext layoutContext) noexcept;
};

} // namespace winrt::Microsoft::ReactNative::implementation

namespace Microsoft::ReactNative {

extern const char AbiViewComponentName[];

class AbiViewShadowNode final : public facebook::react::ConcreteViewShadowNode<
                                    AbiViewComponentName,
                                    AbiViewProps,
                                    facebook::react::ViewEventEmitter,
                                    Microsoft::ReactNative::AbiStateData> {
 public:
  using ConcreteViewShadowNode::ConcreteViewShadowNode;

  facebook::react::Size measureContent(
      const facebook::react::LayoutContext &layoutContext,
      const facebook::react::LayoutConstraints &layoutConstraints) const override;
  void layout(facebook::react::LayoutContext layoutContext) override;

  void OnClone(const facebook::react::ShadowNode &sourceShadowNode) noexcept;
  void Builder(winrt::Microsoft::ReactNative::IReactViewComponentBuilder builder) noexcept;
  winrt::Microsoft::ReactNative::IReactViewComponentBuilder Builder() const noexcept;
  void Proxy(winrt::Microsoft::ReactNative::ShadowNode handle) noexcept;
  winrt::Microsoft::ReactNative::ShadowNode Proxy() const noexcept;

 private:
  winrt::Microsoft::ReactNative::ShadowNode m_proxy{nullptr};
  winrt::Microsoft::ReactNative::IReactViewComponentBuilder m_builder{nullptr};
};

} // namespace Microsoft::ReactNative
