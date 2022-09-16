
// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include <Fabric/IComponentViewRegistry.h>

#include <Fabric/Composition/CompositionHelpers.h>

namespace Microsoft::ReactNative {

/* This could be expanded to have a pool of ComponentViewDescriptor's, like iOS does */
class ComponentViewRegistry final : public IComponentViewRegistry {
 public:
  void Initialize(winrt::Microsoft::ReactNative::ReactContext const &reactContext) noexcept override;

  ComponentViewDescriptor const &dequeueComponentViewWithComponentHandle(
      facebook::react::ComponentHandle componentHandle,
      facebook::react::Tag tag,
      const winrt::Microsoft::ReactNative::Composition::ICompositionContext &compContext) noexcept override;
  ComponentViewDescriptor const &componentViewDescriptorWithTag(facebook::react::Tag tag) const noexcept override;
  std::shared_ptr<IComponentView> findComponentViewWithTag(facebook::react::Tag tag) const noexcept override;
  void enqueueComponentViewWithComponentHandle(
      facebook::react::ComponentHandle componentHandle,
      facebook::react::Tag tag,
      ComponentViewDescriptor componentViewDescriptor) noexcept override;

 private:
  std::unordered_map<facebook::react::Tag, ComponentViewDescriptor> m_registry;
  winrt::Microsoft::ReactNative::ReactContext m_context;
};

} // namespace Microsoft::ReactNative
