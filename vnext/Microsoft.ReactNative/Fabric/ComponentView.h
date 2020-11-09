
// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#pragma warning(push)
#pragma warning(disable : 4244)
#include <react/renderer/componentregistry/ComponentDescriptorProvider.h>
#include <react/renderer/core/LayoutMetrics.h>
#pragma warning(pop)

namespace Microsoft::ReactNative {

using RNComponentViewUpdateMask = std::uint_fast8_t;
/*
 * Bitmask for all types of possible updates performing during mounting.
 */
constexpr RNComponentViewUpdateMask RNComponentViewUpdateMaskNone{0};
constexpr RNComponentViewUpdateMask RNComponentViewUpdateMaskProps{1 << 0};
constexpr RNComponentViewUpdateMask RNComponentViewUpdateMaskEventEmitter{1 << 1};
constexpr RNComponentViewUpdateMask RNComponentViewUpdateMaskState{1 << 2};
constexpr RNComponentViewUpdateMask RNComponentViewUpdateMaskLayoutMetrics{1 << 3};

constexpr RNComponentViewUpdateMask RNComponentViewUpdateMaskAll = RNComponentViewUpdateMaskProps |
    RNComponentViewUpdateMaskEventEmitter | RNComponentViewUpdateMaskState | RNComponentViewUpdateMaskLayoutMetrics;

struct IComponentView {
  virtual std::vector<facebook::react::ComponentDescriptorProvider>
  supplementalComponentDescriptorProviders() noexcept = 0;
  virtual void mountChildComponentView(const IComponentView &childComponentView, uint32_t index) noexcept = 0;
  virtual void unmountChildComponentView(const IComponentView &childComponentView, uint32_t index) noexcept = 0;
  virtual void updateProps(
      facebook::react::Props::Shared const &props,
      facebook::react::Props::Shared const &oldProps) noexcept = 0;
  virtual void updateEventEmitter(facebook::react::EventEmitter::Shared const &eventEmitter) noexcept = 0;
  virtual void updateState(
      facebook::react::State::Shared const &state,
      facebook::react::State::Shared const &oldState) noexcept = 0;
  virtual void updateLayoutMetrics(
      facebook::react::LayoutMetrics const &layoutMetrics,
      facebook::react::LayoutMetrics const &oldLayoutMetrics) noexcept = 0;
  virtual void finalizeUpdates(RNComponentViewUpdateMask updateMask) noexcept = 0;
  virtual void prepareForRecycle() noexcept = 0;
  virtual facebook::react::SharedProps props() noexcept = 0;
};

} // namespace Microsoft::ReactNative
