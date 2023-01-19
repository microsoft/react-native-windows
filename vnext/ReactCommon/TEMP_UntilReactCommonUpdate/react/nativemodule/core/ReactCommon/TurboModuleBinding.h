/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

// [Windows - see issue #11019, this file is an older version]

#pragma once
#include <string>
#include <ReactCommon/LongLivedObject.h>
#include <ReactCommon/TurboModule.h>
#include <jsi/jsi.h>
namespace facebook {
namespace react {

class JSCallInvoker;

enum class TurboModuleBindingMode : uint8_t {
  HostObject = 0,
  Prototype = 1,
  Eager = 2,
};
/**
 * Represents the JavaScript binding for the TurboModule system.
 */
class TurboModuleBinding {
 public:
  /*
   * Installs TurboModuleBinding into JavaScript runtime.
   * Thread synchronization must be enforced externally.
   */
  static void install(
      jsi::Runtime &runtime,
      const TurboModuleProviderFunctionType &&moduleProvider,
      TurboModuleBindingMode bindingMode,
      std::shared_ptr<LongLivedObjectCollection> longLivedObjectCollection);

 private:
  TurboModuleBinding(
      const TurboModuleProviderFunctionType &&moduleProvider,
      TurboModuleBindingMode bindingMode,
      std::shared_ptr<LongLivedObjectCollection> longLivedObjectCollection);
  virtual ~TurboModuleBinding();

  /**
   * A lookup function exposed to JS to get an instance of a TurboModule
   * for the given name.
   */
  jsi::Value getModule(
      jsi::Runtime &runtime,
      const jsi::Value &thisVal,
      const jsi::Value *args,
      size_t count);

  TurboModuleProviderFunctionType moduleProvider_;
  std::shared_ptr<LongLivedObjectCollection> longLivedObjectCollection_;
  TurboModuleBindingMode bindingMode_;
};

} // namespace react
} // namespace facebook