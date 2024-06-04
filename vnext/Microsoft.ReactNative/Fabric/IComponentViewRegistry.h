
// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include "ComponentView.h"

#include <Fabric/Composition/CompositionHelpers.h>
#include <Microsoft.ReactNative.Cxx/ReactContext.h>

namespace Microsoft::ReactNative {

struct ComponentViewDescriptor final {
  winrt::Microsoft::ReactNative::ComponentView view{nullptr};
};

struct IComponentViewRegistry {
 public:
  virtual void Initialize(winrt::Microsoft::ReactNative::ReactContext const &reactContext) noexcept = 0;

  virtual ComponentViewDescriptor const &dequeueComponentViewWithComponentHandle(
      facebook::react::ComponentHandle componentHandle,
      facebook::react::Tag tag,
      const winrt::Microsoft::ReactNative::Composition::Experimental::ICompositionContext &compContext) noexcept = 0;
  virtual ComponentViewDescriptor const &componentViewDescriptorWithTag(facebook::react::Tag tag) const noexcept = 0;
  virtual winrt::Microsoft::ReactNative::ComponentView findComponentViewWithTag(
      facebook::react::Tag tag) const noexcept = 0;
  virtual void enqueueComponentViewWithComponentHandle(
      facebook::react::ComponentHandle componentHandle,
      facebook::react::Tag tag,
      ComponentViewDescriptor componentViewDescriptor) noexcept = 0;
};

} // namespace Microsoft::ReactNative
