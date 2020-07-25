// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include <IReactRootView.h>
#include <folly/dynamic.h>
#include "XamlView.h"

namespace react::uwp {

struct IXamlReactControl {
  virtual void blur(XamlView const &xamlView) noexcept = 0;
};

struct IXamlRootView : public facebook::react::IReactRootView {
  virtual std::shared_ptr<IReactInstance> GetReactInstance() const noexcept = 0;
  virtual XamlView GetXamlView() const noexcept = 0;

  virtual void SetJSComponentName(std::string &&mainComponentName) noexcept = 0;
  virtual void SetInitialProps(folly::dynamic &&initialProps) noexcept = 0;

  virtual void AttachRoot() noexcept = 0;
  virtual void DetachRoot() noexcept = 0;

  virtual std::shared_ptr<IXamlReactControl> GetXamlReactControl() const noexcept = 0;
};

} // namespace react::uwp
