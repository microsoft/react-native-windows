
// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include "ComponentView.h"

namespace Microsoft::ReactNative {

struct ComponentViewDescriptor final {
  std::shared_ptr<IComponentView> view{nullptr};

  /*
   * Indicates a requirement to call on the view methods from
   * `RCTMountingTransactionObserving` protocol.
   */
  bool observesMountingTransactionWillMount{false};
  bool observesMountingTransactionDidMount{false};
};

/* This could be expanded to have a pool of ComponentViewDescriptor's, like iOS does */
class ComponentViewRegistry final {
 public:
  ComponentViewDescriptor const &dequeueComponentViewWithComponentHandle(
      facebook::react::ComponentHandle componentHandle,
      facebook::react::Tag tag) noexcept;
  ComponentViewDescriptor const &componentViewDescriptorWithTag(facebook::react::Tag tag) noexcept;
  void enqueueComponentViewWithComponentHandle(
      facebook::react::ComponentHandle componentHandle,
      facebook::react::Tag tag,
      ComponentViewDescriptor componentViewDescriptor) noexcept;

 private:
  std::unordered_map<facebook::react::Tag, ComponentViewDescriptor> m_registry;
};

} // namespace Microsoft::ReactNative
