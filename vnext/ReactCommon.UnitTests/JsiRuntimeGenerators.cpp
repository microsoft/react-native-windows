#include <JSI/ChakraRuntimeArgs.h>
#include <JSI/ChakraRuntimeFactory.h>
#include <jsi/jsi/test/testlib.h>
#include "MemoryTracker.h"
#if defined(USE_V8)
#include <V8JsiRuntime.h>
#endif

// TODO: #2730 Introduces a vcxitem for shared test code and move this
// there.
#include <Threading/MessageQueueThreadFactory.h>

// Standard Library
#include <vector>

using facebook::jsi::Runtime;
using facebook::jsi::RuntimeFactory;
using facebook::react::CreateMemoryTracker;
using facebook::react::MessageQueueThread;
using Microsoft::JSI::ChakraRuntimeArgs;
using Microsoft::JSI::makeChakraRuntime;
using react::uwp::MakeJSQueueThread;

// TODO: #2729 We need to add tests for ChakraCoreRuntime specific
// behaviors such as ScriptStore. This may require us to bring back JSITestBase.
namespace facebook::jsi {
std::vector<RuntimeFactory> runtimeGenerators() {
#if defined(USE_V8)
  return {[]() -> std::unique_ptr<Runtime> {
    v8runtime::V8RuntimeArgs args;

    return v8runtime::makeV8Runtime(std::move(args));
  }};
#else
  return {[]() -> std::unique_ptr<Runtime> {
    ChakraRuntimeArgs args{};

    args.jsQueue = MakeJSQueueThread();

    std::shared_ptr<MessageQueueThread> memoryTrackerCallbackQueue = MakeJSQueueThread();

    args.memoryTracker = CreateMemoryTracker(std::move(memoryTrackerCallbackQueue));

    return makeChakraRuntime(std::move(args));
  }};
#endif // defined(USE_V8)
}

} // namespace facebook::jsi
