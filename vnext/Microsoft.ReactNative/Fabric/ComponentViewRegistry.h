
// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include "ComponentView.h"

#include <Microsoft.ReactNative.Cxx/ReactContext.h>

namespace Microsoft::ReactNative {

struct ComponentViewDescriptor final {
  std::shared_ptr<IComponentView> view{nullptr};
};

/* This could be expanded to have a pool of ComponentViewDescriptor's, like iOS does */
class ComponentViewRegistry final {
 public:
  void Initialize(winrt::Microsoft::ReactNative::ReactContext const &reactContext) noexcept;

  ComponentViewDescriptor const &dequeueComponentViewWithComponentHandle(
      facebook::react::ComponentHandle componentHandle,
      facebook::react::Tag tag) noexcept;
  ComponentViewDescriptor const &componentViewDescriptorWithTag(facebook::react::Tag tag) const noexcept;
  void enqueueComponentViewWithComponentHandle(
      facebook::react::ComponentHandle componentHandle,
      facebook::react::Tag tag,
      ComponentViewDescriptor componentViewDescriptor) noexcept;

 private:
  std::unordered_map<facebook::react::Tag, ComponentViewDescriptor> m_registry;
  winrt::Microsoft::ReactNative::ReactContext m_context;
};

} // namespace Microsoft::ReactNative
