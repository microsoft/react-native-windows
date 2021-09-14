// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"

#include <gtest/gtest.h>

// JSI
#include <jsi/test/testlib.h>

// NAPI
#include <Shared/JSI/NapiJsiRuntime.h>

using facebook::jsi::Runtime;
using facebook::jsi::RuntimeFactory;

namespace Microsoft::Test {

std::vector<RuntimeFactory> runtimeGenerators() {
  return {[]() -> std::unique_ptr<Runtime> {
    napi_ext_env_settings settings{};

    settings.this_size = sizeof(settings);
    settings.flags.enable_gc_api = true;
    napi_env env{};
    napi_ext_create_env(&settings, &env);

    return Microsoft::JSI::MakeNapiJsiRuntime2(env);
  }};
}

} // namespace Microsoft::Test
