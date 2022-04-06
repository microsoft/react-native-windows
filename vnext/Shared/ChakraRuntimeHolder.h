// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include <DevSettings.h>

#include <JSI/ChakraRuntimeArgs.h>
#include <JSI/RuntimeHolder.h>

#include <Logging.h>

namespace Microsoft::JSI {

class ChakraRuntimeHolder final : public Microsoft::JSI::RuntimeHolderLazyInit {
 public:
  std::shared_ptr<facebook::jsi::Runtime> getRuntime() noexcept override;
  facebook::react::JSIEngineOverride getRuntimeType() noexcept override;

  ChakraRuntimeHolder(
      std::shared_ptr<facebook::react::DevSettings> devSettings,
      std::shared_ptr<facebook::react::MessageQueueThread> jsQueue,
      std::unique_ptr<facebook::jsi::ScriptStore> &&scriptStore,
      std::unique_ptr<facebook::jsi::PreparedScriptStore> &&preparedScriptStore) noexcept
      : args_(RuntimeArgsFromDevSettings(devSettings)) {
    args_.jsQueue = std::move(jsQueue);
    args_.scriptStore = std::move(scriptStore);
    args_.preparedScriptStore = std::move(preparedScriptStore);
  }

 private:
  Microsoft::JSI::ChakraRuntimeArgs RuntimeArgsFromDevSettings(
      std::shared_ptr<facebook::react::DevSettings> devSettings) noexcept;
  Microsoft::JSI::Logger ChakraRuntimeLoggerFromReactLogger(
      facebook::react::NativeLoggingHook loggingCallback) noexcept;

  void initRuntime() noexcept;

  Microsoft::JSI::ChakraRuntimeArgs args_;
  std::shared_ptr<facebook::jsi::Runtime> runtime_;

  std::once_flag once_flag_;
  std::thread::id own_thread_id_;
};

} // namespace Microsoft::JSI
