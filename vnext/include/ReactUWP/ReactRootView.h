// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include "IXamlRootView.h"

#include <stdint.h>
#include <memory>
#include <string>

namespace react {
namespace uwp {

struct IReactInstance;
class ReactControl;

class ReactRootView : public IXamlRootView {
 public:
  ReactRootView(XamlView rootView);
  virtual ~ReactRootView();

 public: // IXamlRootView implementations
  std::shared_ptr<IReactInstance> GetReactInstance() const noexcept override;
  XamlView GetXamlView() const noexcept override;
  void SetJSComponentName(std::string &&mainComponentName) noexcept override;
  void SetInitialProps(folly::dynamic &&initialProps) noexcept override;
  void SetInstanceCreator(const ReactInstanceCreator &instanceCreator) noexcept override;
  void AttachRoot() noexcept override;
  void DetachRoot() noexcept override;
  std::shared_ptr<::react::uwp::IXamlReactControl> GetXamlReactControl() const noexcept override;

 public: // IReactRootView implementations
  virtual void ResetView() override;
  virtual std::string JSComponentName() const noexcept override;
  virtual int64_t GetActualHeight() const override;
  virtual int64_t GetActualWidth() const override;
  virtual int64_t GetTag() const override;
  virtual void SetTag(int64_t tag) override;

 private:
  std::shared_ptr<ReactControl> m_pimpl;
};

} // namespace uwp
} // namespace react
