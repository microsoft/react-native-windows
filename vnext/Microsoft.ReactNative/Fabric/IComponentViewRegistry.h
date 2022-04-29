
// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include "ComponentView.h"

#include <Microsoft.ReactNative.Cxx/ReactContext.h>

#ifdef USE_WINCOMP
#include <Fabric/WinComp/CompHelpers.h>
#endif

namespace Microsoft::ReactNative {

struct ComponentViewDescriptor final {
  std::shared_ptr<IComponentView> view{nullptr};
};

struct IComponentViewRegistry {
 public:
  virtual void Initialize(winrt::Microsoft::ReactNative::ReactContext const &reactContext) noexcept = 0;

  virtual ComponentViewDescriptor const &dequeueComponentViewWithComponentHandle(
      facebook::react::ComponentHandle componentHandle,
      facebook::react::Tag tag
#ifdef USE_WINCOMP
      ,
      const std::shared_ptr<Microsoft::ReactNative::CompContext> &compContext
#endif
      ) noexcept = 0;
  virtual ComponentViewDescriptor const &componentViewDescriptorWithTag(facebook::react::Tag tag) const noexcept = 0;
  virtual std::shared_ptr<IComponentView> findComponentViewWithTag(facebook::react::Tag tag) const noexcept = 0;
  virtual void enqueueComponentViewWithComponentHandle(
      facebook::react::ComponentHandle componentHandle,
      facebook::react::Tag tag,
      ComponentViewDescriptor componentViewDescriptor) noexcept = 0;
};

} // namespace Microsoft::ReactNative
