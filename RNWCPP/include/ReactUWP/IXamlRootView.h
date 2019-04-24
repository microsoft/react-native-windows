// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include <IReactRootView.h>
#include "InstanceFactory.h"
#include "XamlView.h"
#include <folly/dynamic.h>

namespace react { namespace uwp {

struct IXamlRootView : public facebook::react::IReactRootView
{
  virtual std::shared_ptr<IReactInstance> GetReactInstance() const noexcept = 0;
  virtual XamlView GetXamlView() const noexcept = 0;

  virtual void SetJSComponentName(std::string&& mainComponentName) noexcept = 0;
  virtual void SetInstanceCreator(const ReactInstanceCreator& instanceCreator) noexcept = 0;
  virtual void SetInitialProps(folly::dynamic&& initialProps) noexcept = 0;

  virtual void AttachRoot() noexcept = 0;
  virtual void DetachRoot() noexcept = 0;
};

} }
