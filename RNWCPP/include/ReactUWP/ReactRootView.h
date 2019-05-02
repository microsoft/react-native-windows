// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include "IXamlRootView.h"

#include <memory>
#include <stdint.h>
#include <string>

namespace react { namespace uwp {

struct IReactInstance;
class ReactControl;

class ReactRootView : public IXamlRootView
{
public:
  ReactRootView(XamlView rootView);
  virtual ~ReactRootView();

  // IXamlRootView implementations
  std::shared_ptr<IReactInstance> GetReactInstance() const noexcept override;
  XamlView GetXamlView() const noexcept override;
  void SetJSComponentName(std::string&& mainComponentName) noexcept override;
  void SetInitialProps(folly::dynamic&& initialProps) noexcept override;
  void SetInstanceCreator(const ReactInstanceCreator& instanceCreator) noexcept;
  void AttachRoot() noexcept override;
  void DetachRoot() noexcept override;

  // IReactRootView implementations
  virtual void ResetView() override;
  virtual std::string JSComponentName() const noexcept;
  virtual int64_t GetActualHeight() const;
  virtual int64_t GetActualWidth() const;
  virtual int64_t GetTag() const override;
  virtual void SetTag(int64_t tag) override;

private:
  std::shared_ptr<ReactControl> m_pimpl;
};

} }
