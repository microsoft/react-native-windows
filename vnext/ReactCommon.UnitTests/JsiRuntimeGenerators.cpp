#include <JSI/ChakraRuntimeArgs.h>
#include <JSI/ChakraRuntimeFactory.h>
#include <jsi/jsi/test/testlib.h>
#if defined(USE_V8)
#include <JSI/V8AbiRuntimeHolder.h>
#include <V8JsiRuntime.h>
#include <stdexcept>
#include "V8AbiTestUtils.h"
#endif

// TODO: #2730 Introduces a vcxitem for shared test code and move this
// there.
#include <Threading/MessageQueueThreadFactory.h>

// Standard Library
#include <vector>

using facebook::jsi::Runtime;
using facebook::jsi::RuntimeFactory;
using facebook::react::MessageQueueThread;
using Microsoft::JSI::ChakraRuntimeArgs;
using Microsoft::JSI::makeChakraRuntime;
using Microsoft::ReactNative::MakeJSQueueThread;

// TODO: #2729 We need to add tests for ChakraCoreRuntime specific
// behaviors such as ScriptStore. This may require us to bring back JSITestBase.
namespace facebook::jsi {
std::vector<RuntimeFactory> runtimeGenerators() {
#if defined(USE_V8)
  return {
      []() -> std::unique_ptr<Runtime> {
        v8runtime::V8RuntimeArgs args;

        return v8runtime::makeV8Runtime(std::move(args));
      },
      []() -> std::unique_ptr<Runtime> {
        std::string error;
        auto runtime = Microsoft::ReactNative::MakeV8AbiRuntime(LoadV8JsiAbiForTesting(), {}, error);
        if (!runtime) {
          throw std::runtime_error(error);
        }
        return runtime;
      }};
#else
  return {[]() -> std::unique_ptr<Runtime> {
    ChakraRuntimeArgs args{};

    args.jsQueue = MakeJSQueueThread();

    return makeChakraRuntime(std::move(args));
  }};
#endif // defined(USE_V8)
}

} // namespace facebook::jsi
