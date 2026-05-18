// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"
#include "jsi/JsiAbiApi.h"
#include "jsi/test/testlib.h"

using namespace winrt;

namespace facebook::jsi {

std::vector<RuntimeFactory> runtimeGenerators() {
  return {RuntimeFactory([]() -> std::unique_ptr<Runtime> {
    Microsoft::ReactNative::JsiRuntime runtime{Microsoft::ReactNative::JsiRuntime::MakeRuntime()};
    return std::make_unique<Microsoft::ReactNative::JsiAbiRuntime>(runtime);
  })};
}

} // namespace facebook::jsi
