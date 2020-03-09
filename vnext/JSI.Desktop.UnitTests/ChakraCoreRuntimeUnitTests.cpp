#include "JsiRuntimeUnitTests.h"

#include "JSI/Shared/ChakraRuntimeArgs.h"
#include "JSI/Shared/ChakraRuntimeFactory.h"
#include "MemoryTracker.h"
#if defined(USE_V8)
#include <V8JsiRuntime.h>
#endif

// TODO (yicyao): #2730 Introduces a vcxitem for shared test code and move this
// there.
#include <IntegrationTests/TestMessageQueueThread.h>

#include <gtest/gtest.h>

#include <vector>

using facebook::jsi::JsiRuntimeUnitTests;
using facebook::jsi::JsiRuntimeUnitTests_Chakra;
using facebook::jsi::Runtime;
using facebook::jsi::RuntimeFactory;
using facebook::react::CreateMemoryTracker;
using facebook::react::MessageQueueThread;
using Microsoft::JSI::ChakraRuntimeArgs;
using Microsoft::JSI::makeChakraRuntime;
using Microsoft::React::Test::TestMessageQueueThread;

// TODO (yicyao): #2729 We need to add tests for ChakraCoreRuntime specific
// behaviors such as ScriptStore. This may require us to bring back JSITestBase.

std::vector<RuntimeFactory> runtimeGenerators() {
  return {[]() -> std::unique_ptr<Runtime> {
    ChakraRuntimeArgs args{};

    args.jsQueue = std::make_shared<TestMessageQueueThread>();

    std::shared_ptr<MessageQueueThread> memoryTrackerCallbackQueue = std::make_shared<TestMessageQueueThread>();

    args.memoryTracker = CreateMemoryTracker(std::move(memoryTrackerCallbackQueue));

    return makeChakraRuntime(std::move(args));
  }};
}

INSTANTIATE_TEST_CASE_P(ChakraRuntimeTest_Base, JsiRuntimeUnitTests, ::testing::ValuesIn(runtimeGenerators()));
INSTANTIATE_TEST_CASE_P(ChakraRuntimeTest, JsiRuntimeUnitTests_Chakra, ::testing::ValuesIn(runtimeGenerators()));

#if defined(USE_V8)

RuntimeFactory getV8Runtime() {
  return []() -> std::unique_ptr<Runtime> {
    v8runtime::V8RuntimeArgs args;

    return v8runtime::makeV8Runtime(std::move(args));
  };
}

INSTANTIATE_TEST_CASE_P(V8RuntimeTest, JsiRuntimeUnitTests, ::testing::Values(getV8Runtime()));
#endif
