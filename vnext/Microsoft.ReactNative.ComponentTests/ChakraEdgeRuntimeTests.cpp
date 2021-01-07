// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

#include <JSI/ChakraRuntimeArgs.h>
#include <JSI/ChakraRuntimeFactory.h>
#include <gtest/gtest.h>
#include "jsi/test/testlib.h"

using namespace Microsoft::JSI;

namespace facebook::jsi {

std::vector<RuntimeFactory> runtimeGenerators() {
  return {RuntimeFactory([]() -> std::unique_ptr<Runtime> {
    ChakraRuntimeArgs args{};
    return makeChakraRuntime(std::move(args));
  })};
}

} // namespace facebook::jsi
