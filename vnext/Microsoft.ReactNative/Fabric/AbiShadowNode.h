// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include "LayoutContext.g.h"
#include "ShadowNode.g.h"
#include "YogaLayoutableShadowNode.g.h"
#include <react/components/rnwcore/EventEmitters.h>
#include <unordered_map>
#include "AbiEventEmitter.h"
#include "AbiState.h"
#include "AbiViewProps.h"

#include <react/renderer/components/view/ConcreteViewShadowNode.h>
#include <react/renderer/core/LayoutContext.h>

namespace winrt::Microsoft::ReactNative::implementation {

class AbiProps final : public facebook::react::Props {
 public:
  AbiProps() = default;
  AbiProps(
      const facebook::react::PropsParserContext &context,
      const AbiProps &sourceProps,
      const facebook::react::RawProps &rawProps);
  ~AbiProps();

  void SetUserProps(winrt::Microsoft::ReactNative::IComponentProps componentProps) noexcept;
  winrt::Microsoft::ReactNative::IComponentProps UserProps() const noexcept;

 private:
  winrt::Microsoft::ReactNative::IComponentProps m_componentProps{nullptr};
};

template <typename TStateData>
struct ShadowNode : ShadowNodeT<ShadowNode<TStateData>> {
  ShadowNode(std::shared_ptr<const facebook::react::ShadowNode> shadowNode) noexcept : m_shadowNode(shadowNode) {}

  void EnsureUnsealed() noexcept {
    if (auto shadowNode = m_shadowNode.lock()) {
      shadowNode->ensureUnsealed();
    }
  }

  winrt::Windows::Foundation::IInspectable Tag() const noexcept {
    return m_tag;
  }
  void Tag(winrt::Windows::Foundation::IInspectable tag) noexcept {
    m_tag = tag;
  }

  winrt::Windows::Foundation::IInspectable StateData() const noexcept {
    if (auto shadowNode = m_shadowNode.lock()) {
      auto state = shadowNode->getState();
      auto abiStateData = static_cast<const facebook::react::ConcreteState<TStateData> *>(state.get())->getData();
      return abiStateData.userdata;
    }
    return nullptr;
  }

  void StateData(winrt::Windows::Foundation::IInspectable tag) noexcept {
    if (auto shadowNode = m_shadowNode.lock()) {
      shadowNode->ensureUnsealed();
      auto &state = const_cast<facebook::react::State::Shared &>(shadowNode->getState());
      auto data = static_cast<const facebook::react::ConcreteState<TStateData> *>(state.get())->getData();
      data.userdata = tag;
      state = std::make_shared<const facebook::react::ConcreteState<TStateData>>(
          std::make_shared<const TStateData>(std::move(data)), *state);
    }
  }

  winrt::Microsoft::ReactNative::EventEmitter EventEmitter() const noexcept {
    if (auto shadowNode = m_shadowNode.lock()) {
      return winrt::make<winrt::Microsoft::ReactNative::implementation::EventEmitter>(shadowNode->getEventEmitter());
    }
    return nullptr;
  }

 protected:
  std::weak_ptr<const facebook::react::ShadowNode> m_shadowNode;
  winrt::Windows::Foundation::IInspectable m_tag;
};

} // namespace winrt::Microsoft::ReactNative::implementation

namespace Microsoft::ReactNative {

extern const char AbiComponentName[];

class AbiShadowNode final : public facebook::react::ConcreteShadowNode<
                                AbiComponentName,
                                facebook::react::ShadowNode,
                                winrt::Microsoft::ReactNative::implementation::AbiProps,
                                facebook::react::EventEmitter,
                                Microsoft::ReactNative::AbiStateData> {
 public:
  using ConcreteShadowNode::ConcreteShadowNode;

  static facebook::react::ShadowNodeTraits BaseTraits() {
    auto traits = facebook::react::ShadowNode::BaseTraits();
    traits.set(facebook::react::ShadowNodeTraits::Trait::FormsStackingContext);
    traits.set(facebook::react::ShadowNodeTraits::Trait::FormsView);
    return traits;
  }

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
