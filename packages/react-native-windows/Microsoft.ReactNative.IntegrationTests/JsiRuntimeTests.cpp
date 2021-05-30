// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"
#include "jsi/JsiAbiApi.h"
#include "jsi/test/testlib.h"

using namespace winrt;
using namespace Microsoft::ReactNative;

namespace facebook::jsi {

std::vector<RuntimeFactory> runtimeGenerators() {
  return {RuntimeFactory([]() -> std::unique_ptr<Runtime> {
    JsiRuntime runtime{JsiRuntime::MakeChakraRuntime()};
    return std::make_unique<JsiAbiRuntime>(runtime);
  })};
}

} // namespace facebook::jsi
