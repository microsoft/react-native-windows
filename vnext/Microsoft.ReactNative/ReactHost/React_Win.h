// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include <functional>
#include <memory>
#include <tuple>
#include <vector>
#include "React.h"
#include "functional/functor.h"

// Forward declarations
// We would ideally include ViewManager.h here instead of forward declaring,
// but folly headers are problematic for cpp def auto gen tools.
namespace facebook::react {
class IUIManager;
struct InstanceWrapper;
} // namespace facebook::react

namespace Mso::React {

using ErrorCallback = Mso::Functor<void(const std::string &error)>;

//! Windows specific interface of a React Instance.
MSO_STRUCT_GUID(IReactInstanceWin32, "3c9e9e94-4d2a-4521-b166-864984d26f60")
struct IReactInstanceWin32 : IUnknown {
  //! Returns the inner react native instance object.
  virtual std::shared_ptr<facebook::react::InstanceWrapper> InstanceObject() const noexcept = 0;

  //! Returns the UIManager object associated with this react instance.
  virtual std::shared_ptr<facebook::react::IUIManager> UIManager() const noexcept = 0;
};

MSO_STRUCT_GUID(IFBReactInstance, "81240204-c3b5-4b10-957d-66544e5a301e")
struct IFBReactInstance : public IUnknown {
  /// Queues up a call to a JS function in the loaded bundle
  virtual void CallJsFunction(std::string &&moduleName, std::string &&method, folly::dynamic &&params) noexcept = 0;

  /// Returns a queue on which the native methods are going to invoked by default,
  /// that is unless the native module did not explicitly specify a queue during its registration.
  virtual std::shared_ptr<facebook::react::MessageQueueThread> DefaultNativeMessageQueueThread() const noexcept = 0;

  /// Returns a queue on which synchronous native methods can be called.
  virtual std::shared_ptr<facebook::react::MessageQueueThread> JSMessageQueueThread() const noexcept = 0;
};

} // namespace Mso::React
