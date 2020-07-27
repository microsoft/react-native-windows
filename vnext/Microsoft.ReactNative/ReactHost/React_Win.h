// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#ifndef CORE_ABI
#include <INativeUIManager.h>
#include <IReactInstance.h>
#endif

#include <functional/functor.h>
#include <string>
#include "React.h"

namespace Mso::React {

using ErrorCallback = Mso::Functor<void(const std::string &error)>;

//! Members of this interface are going to be moved somewhere else.
MSO_GUID(ILegacyReactInstance, "89de446a-b754-4ac5-8a91-be318dfaa7d3")
struct ILegacyReactInstance : IUnknown {
  /// Queues up a call to a JS function in the loaded bundle
  virtual void CallJsFunction(std::string &&moduleName, std::string &&method, folly::dynamic &&params) noexcept = 0;
  virtual void DispatchEvent(int64_t viewTag, std::string &&eventName, folly::dynamic &&eventData) noexcept = 0;
#ifndef CORE_ABI
  virtual facebook::react::INativeUIManager *NativeUIManager() noexcept = 0;
#endif
  virtual std::shared_ptr<facebook::react::Instance> GetInnerInstance() noexcept = 0;
  virtual std::string GetBundleRootPath() noexcept = 0;

  virtual bool IsLoaded() const noexcept = 0;
#ifndef CORE_ABI
  virtual std::shared_ptr<react::uwp::IReactInstance> UwpReactInstance() noexcept = 0;

  virtual void AttachMeasuredRootView(
      facebook::react::IReactRootView *rootView,
      folly::dynamic &&initialProps) noexcept = 0;
  virtual void DetachRootView(facebook::react::IReactRootView *rootView) noexcept = 0;
#endif
};

} // namespace Mso::React
