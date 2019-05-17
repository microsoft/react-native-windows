#pragma once

#include <DevSettings.h>

#include <jsi/ScriptStore.h>
#include <jsi/RuntimeHolder.h>

#include <Chakra/ChakraJsiRuntimeArgs.h>

#include <Logging.h>

namespace facebook {
namespace react {
namespace test {

class ChakraJSIRuntimeHolder : public facebook::jsi::RuntimeHolderLazyInit {
public:
  std::shared_ptr<facebook::jsi::Runtime> getRuntime() noexcept override;

  ChakraJSIRuntimeHolder(std::shared_ptr<facebook::react::DevSettings> devSettings,
    std::shared_ptr<facebook::react::MessageQueueThread> jsQueue,
    std::unique_ptr<facebook::jsi::ScriptStore>&& scriptStore,
    std::unique_ptr<facebook::jsi::PreparedScriptStore>&& preparedScriptStore) noexcept
    : args_(RuntimeArgsFromDevSettings(devSettings)) {
	    args_.jsQueue = std::move(jsQueue);
      args_.scriptStore = std::move(scriptStore);
      args_.preparedScriptStore = std::move(preparedScriptStore);
  }

private:
  facebook::jsi::chakraruntime::ChakraJsiRuntimeArgs RuntimeArgsFromDevSettings(std::shared_ptr<facebook::react::DevSettings> devSettings) noexcept;
  facebook::jsi::chakraruntime::Logger ChakraRuntimeLoggerFromReactLogger(facebook::react::NativeLoggingHook loggingCallback) noexcept;

  void initRuntime() noexcept;

  facebook::jsi::chakraruntime::ChakraJsiRuntimeArgs args_;
  std::shared_ptr<facebook::jsi::Runtime> runtime_;

  std::once_flag once_flag_;
  std::thread::id own_thread_id_;

};

}}}
