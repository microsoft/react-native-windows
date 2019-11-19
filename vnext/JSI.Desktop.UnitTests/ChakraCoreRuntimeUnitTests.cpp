#include "JsiRuntimeUnitTests.h"

#include "ChakraCoreRuntimeFactory.h"

#include <gtest/gtest.h>

#include <vector>

using facebook::jsi::JsiRuntimeUnitTests;
using facebook::jsi::Runtime;
using facebook::jsi::RuntimeFactory;
using Microsoft::JSI::ChakraCoreRuntimeArgs;
using Microsoft::JSI::makeChakraCoreRuntime;

// TODO (yicyao): #2729 We need to add tests for ChakraCoreRuntime specific
// behaviors such as ScriptStore. This may require us to bring back JSITestBase.

std::vector<RuntimeFactory> runtimeGenerators() {
  return {
      []() -> std::unique_ptr<Runtime> { return makeChakraCoreRuntime(std::make_shared<ChakraCoreRuntimeArgs>()); }};
}

INSTANTIATE_TEST_CASE_P(ChakraRuntimeTest, JsiRuntimeUnitTests, ::testing::ValuesIn(runtimeGenerators()));
