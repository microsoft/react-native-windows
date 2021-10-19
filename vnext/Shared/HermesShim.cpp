// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "HermesShim.h"

namespace Microsoft::ReactNative::HermesShim {

static HMODULE s_hermesModule{nullptr};
static decltype(&facebook::hermes::makeHermesRuntime) s_makeHermesRuntime{nullptr};
static decltype(&facebook::hermes::HermesRuntime::enableSamplingProfiler) s_enableSamplingProfiler{nullptr};
static decltype(&facebook::hermes::HermesRuntime::disableSamplingProfiler) s_disableSamplingProfiler{nullptr};
static decltype(&facebook::hermes::HermesRuntime::dumpSampledTraceToFile) s_dumpSampledTraceToFile{nullptr};

static void EnsureHermesLoaded() noexcept {
  if (!s_hermesModule) {
    s_hermesModule = LoadLibrary(L"hermes.dll");
    VerifyElseCrashSz(s_hermesModule, "Could not load \"hermes.dll\"");

    s_makeHermesRuntime = reinterpret_cast<decltype(s_makeHermesRuntime)>(GetProcAddress(
        s_hermesModule,
        "?makeHermesRuntime@hermes@facebook@@YA?AV?$unique_ptr@VHermesRuntime@hermes@facebook@@U?$default_delete@VHermesRuntime@hermes@facebook@@@std@@@std@@AEBVRuntimeConfig@vm@1@@Z"));
    VerifyElseCrash(s_makeHermesRuntime);

    s_enableSamplingProfiler = reinterpret_cast<decltype(s_enableSamplingProfiler)>(
        GetProcAddress(s_hermesModule, "?enableSamplingProfiler@HermesRuntime@hermes@facebook@@SAXXZ"));
    VerifyElseCrash(s_enableSamplingProfiler);

    s_disableSamplingProfiler = reinterpret_cast<decltype(s_disableSamplingProfiler)>(
        GetProcAddress(s_hermesModule, "?disableSamplingProfiler@HermesRuntime@hermes@facebook@@SAXXZ"));
    VerifyElseCrash(s_disableSamplingProfiler);

    s_dumpSampledTraceToFile = reinterpret_cast<decltype(s_dumpSampledTraceToFile)>(GetProcAddress(
        s_hermesModule,
        "?dumpSampledTraceToFile@HermesRuntime@hermes@facebook@@SAXAEBV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@@Z"));
    VerifyElseCrash(s_dumpSampledTraceToFile);
  }
}

std::unique_ptr<facebook::hermes::HermesRuntime> makeHermesRuntime(const hermes::vm::RuntimeConfig &runtimeConfig) {
  EnsureHermesLoaded();
  return s_makeHermesRuntime(runtimeConfig);
}

void enableSamplingProfiler() {
  EnsureHermesLoaded();
  s_enableSamplingProfiler();
}

void disableSamplingProfiler() {
  EnsureHermesLoaded();
  s_disableSamplingProfiler();
}

void dumpSampledTraceToFile(const std::string &fileName) {
  EnsureHermesLoaded();
  s_dumpSampledTraceToFile(fileName);
}

} // namespace Microsoft::ReactNative::HermesShim
