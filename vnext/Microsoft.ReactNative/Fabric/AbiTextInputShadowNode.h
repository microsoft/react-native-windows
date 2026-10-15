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
#include "AbiTextInputProps.h"

#include <react/renderer/core/LayoutContext.h>
#include "Composition/TextInput/AbiTextInputState.h"
#include "Composition/TextInput/WindowsTextInputShadowNode.h"

namespace Microsoft::ReactNative {

extern const char AbiTextInputComponentName[];

class AbiTextInputShadowNode final : public facebook::react::ConcreteTextInputShadowNode<
                                         AbiTextInputComponentName,
                                         AbiTextInputProps,
                                         facebook::react::WindowsTextInputEventEmitter,
                                         Microsoft::ReactNative::AbiTextInputStateData> {
 public:
  using Super = facebook::react::ConcreteTextInputShadowNode<
      AbiTextInputComponentName,
      AbiTextInputProps,
      facebook::react::WindowsTextInputEventEmitter,
      Microsoft::ReactNative::AbiTextInputStateData>;
  using Super::Super;

  facebook::react::Size measureContent(
      const facebook::react::LayoutContext &layoutContext,
      const facebook::react::LayoutConstraints &layoutConstraints) const override;
  void layout(facebook::react::LayoutContext layoutContext) override;

  void OnClone(const facebook::react::ShadowNode &sourceShadowNode) noexcept;
  void Builder(winrt::Microsoft::ReactNative::IReactViewComponentBuilder builder) noexcept;
  winrt::Microsoft::ReactNative::IReactViewComponentBuilder Builder() const noexcept;
  void Proxy(winrt::Microsoft::ReactNative::ShadowNode handle) noexcept;
  winrt::Microsoft::ReactNative::ShadowNode Proxy() const noexcept;

  void updateTextInputState(
      facebook::react::AttributedStringBox attributedStringBox,
      facebook::react::AttributedString &reactTreeAttributedString,
      facebook::react::ParagraphAttributes paragraphAttributes,
      int64_t mostRecentEventCount) override {
    Super::setStateData(AbiTextInputStateData{
        attributedStringBox,
        reactTreeAttributedString,
        paragraphAttributes,
        mostRecentEventCount,
        getStateData().userdata});
  }

 private:
  winrt::Microsoft::ReactNative::ShadowNode m_proxy{nullptr};
  winrt::Microsoft::ReactNative::IReactViewComponentBuilder m_builder{nullptr};
};

} // namespace Microsoft::ReactNative
