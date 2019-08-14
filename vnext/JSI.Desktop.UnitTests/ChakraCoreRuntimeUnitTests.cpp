#include "JsiRuntimeUnitTests.h"

#include "Chakra/ChakraJsiRuntimeArgs.h"
#include "Chakra/ChakraJsiRuntimeFactory.h"
#include "JSI/Desktop/ChakraCoreRuntimeFactory.h"
#include "MemoryTracker.h"

// TODO (yicyao): #2730 Introduces a vcxitem for shared test code and move this
// there.
#include <IntegrationTests/TestMessageQueueThread.h>

#include <gtest/gtest.h>

#include <vector>

using facebook::jsi::JsiRuntimeUnitTests;
using facebook::jsi::Runtime;
using facebook::jsi::RuntimeFactory;
using facebook::jsi::chakra::ChakraRuntimeArgs;
using facebook::jsi::chakra::makeChakraCoreRuntime;
using facebook::jsi::chakraruntime::ChakraJsiRuntimeArgs;
using facebook::jsi::chakraruntime::makeChakraJsiRuntime;
using facebook::react::CreateMemoryTracker;
using facebook::react::MessageQueueThread;
using Microsoft::React::Test::TestMessageQueueThread;

// TODO (yicyao): #2729 We need to add tests for ChakraCoreRuntime specific
// behaviors such as ScriptStore. This may require us to bring back JSITestBase.

std::vector<RuntimeFactory> runtimeGenerators() {
  return {[]() -> std::unique_ptr<Runtime> {
            ChakraJsiRuntimeArgs args{};

            args.jsQueue = std::make_shared<TestMessageQueueThread>();

            std::shared_ptr<MessageQueueThread> memoryTrackerCallbackQueue =
                std::make_shared<TestMessageQueueThread>();

            args.memoryTracker =
                CreateMemoryTracker(std::move(memoryTrackerCallbackQueue));

            return makeChakraJsiRuntime(std::move(args));
          },
          []() -> std::unique_ptr<Runtime> {
            return makeChakraCoreRuntime({false, false});
          }};
}

INSTANTIATE_TEST_CASE_P(
    ChakraRuntimeTest,
    JsiRuntimeUnitTests,
    ::testing::ValuesIn(runtimeGenerators()));
