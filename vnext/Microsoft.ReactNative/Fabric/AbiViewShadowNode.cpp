// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "AbiViewShadowNode.h"

#include <Fabric/Composition/ReactCompositionViewComponentBuilder.h>
#include <react/debug/react_native_assert.h>
#include <react/renderer/core/LayoutConstraints.h>
#include <react/renderer/core/LayoutContext.h>
#include <react/renderer/core/conversions.h>

#include <utility>

namespace winrt::Microsoft::ReactNative::implementation {

YogaLayoutableShadowNode::YogaLayoutableShadowNode(
    std::shared_ptr<const facebook::react::ShadowNode> shadowNode) noexcept
    : base_type(shadowNode) {}

void YogaLayoutableShadowNode::Layout(winrt::Microsoft::ReactNative::LayoutContext layoutContext) noexcept {
  if (auto shadowNode = m_shadowNode.lock()) {
    std::const_pointer_cast<facebook::react::YogaLayoutableShadowNode>(
        std::dynamic_pointer_cast<const facebook::react::YogaLayoutableShadowNode>(shadowNode))
        ->facebook::react::YogaLayoutableShadowNode::layout(
            winrt::get_self<winrt::Microsoft::ReactNative::implementation::LayoutContext>(layoutContext)
                ->m_layoutContext);
  }
}

} // namespace winrt::Microsoft::ReactNative::implementation

namespace Microsoft::ReactNative {

extern const char AbiViewComponentName[] = "AbiView";

facebook::react::Size AbiViewShadowNode::measureContent(
    const facebook::react::LayoutContext &layoutContext,
    const facebook::react::LayoutConstraints &layoutConstraints) const {
  if (auto measureContent =
          winrt::get_self<winrt::Microsoft::ReactNative::Composition::ReactCompositionViewComponentBuilder>(m_builder)
              ->MeasureContentHandler()) {
    winrt::Microsoft::ReactNative::LayoutConstraints winrtLayoutContraints;
    static_assert(
        winrt::Microsoft::ReactNative::LayoutDirection::Undefined ==
        static_cast<winrt::Microsoft::ReactNative::LayoutDirection>(facebook::react::LayoutDirection::Undefined));
    static_assert(
        winrt::Microsoft::ReactNative::LayoutDirection::LeftToRight ==
        static_cast<winrt::Microsoft::ReactNative::LayoutDirection>(facebook::react::LayoutDirection::LeftToRight));
    static_assert(
        winrt::Microsoft::ReactNative::LayoutDirection::RightToLeft ==
        static_cast<winrt::Microsoft::ReactNative::LayoutDirection>(facebook::react::LayoutDirection::RightToLeft));
    winrtLayoutContraints.LayoutDirection =
        static_cast<winrt::Microsoft::ReactNative::LayoutDirection>(layoutConstraints.layoutDirection);
    winrtLayoutContraints.MaximumSize = {layoutConstraints.maximumSize.width, layoutConstraints.maximumSize.height};
    winrtLayoutContraints.MinimumSize = {layoutConstraints.minimumSize.width, layoutConstraints.minimumSize.height};

    auto size = measureContent(
        m_proxy,
        winrt::make<winrt::Microsoft::ReactNative::implementation::LayoutContext>(layoutContext),
        winrtLayoutContraints);
    return {size.Width, size.Height};
  }

  return ConcreteViewShadowNode::measureContent(layoutContext, layoutConstraints);
}

void AbiViewShadowNode::layout(facebook::react::LayoutContext layoutContext) {
  if (auto layoutHandler =
          winrt::get_self<winrt::Microsoft::ReactNative::Composition::ReactCompositionViewComponentBuilder>(m_builder)
              ->LayoutHandler()) {
    layoutHandler(m_proxy, winrt::make<winrt::Microsoft::ReactNative::implementation::LayoutContext>(layoutContext));
  } else {
    ConcreteViewShadowNode::layout(layoutContext);
  }
}

void AbiViewShadowNode::Builder(winrt::Microsoft::ReactNative::IReactViewComponentBuilder builder) noexcept {
  m_builder = builder;
}

winrt::Microsoft::ReactNative::IReactViewComponentBuilder AbiViewShadowNode::Builder() const noexcept {
  return m_builder;
}

void AbiViewShadowNode::Proxy(winrt::Microsoft::ReactNative::ShadowNode proxy) noexcept {
  m_proxy = proxy;
}

winrt::Microsoft::ReactNative::ShadowNode AbiViewShadowNode::Proxy() const noexcept {
  return m_proxy;
}

} // namespace Microsoft::ReactNative
