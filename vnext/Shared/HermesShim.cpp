// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "HermesShim.h"
#include "Crash.h"

namespace Microsoft::ReactNative::HermesShim {

static HMODULE s_hermesModule{nullptr};
static decltype(&facebook::hermes::makeHermesRuntime) s_makeHermesRuntime{nullptr};
static decltype(&facebook::hermes::HermesRuntime::enableSamplingProfiler) s_enableSamplingProfiler{nullptr};
static decltype(&facebook::hermes::HermesRuntime::disableSamplingProfiler) s_disableSamplingProfiler{nullptr};
static decltype(&facebook::hermes::HermesRuntime::dumpSampledTraceToFile) s_dumpSampledTraceToFile{nullptr};

#if _M_X64
constexpr const char *makeHermesRuntimeSymbol =
    "?makeHermesRuntime@hermes@facebook@@YA?AV?$unique_ptr@VHermesRuntime@hermes@facebook@@U?$default_delete@VHermesRuntime@hermes@facebook@@@std@@@std@@AEBVRuntimeConfig@vm@1@@Z";
constexpr const char *enableSamlingProfilerSymbol = "?enableSamplingProfiler@HermesRuntime@hermes@facebook@@SAXXZ";
constexpr const char *disableSamlingProfilerSymbol = "?disableSamplingProfiler@HermesRuntime@hermes@facebook@@SAXXZ";
constexpr const char *dumpSampledTraceToFileSymbol =
    "?dumpSampledTraceToFile@HermesRuntime@hermes@facebook@@SAXAEBV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@@Z";
#endif

#if _M_ARM64
constexpr const char *makeHermesRuntimeSymbol =
    "?makeHermesRuntime@hermes@facebook@@YA?AV?$unique_ptr@VHermesRuntime@hermes@facebook@@U?$default_delete@VHermesRuntime@hermes@facebook@@@std@@@std@@AEBVRuntimeConfig@vm@1@@Z";
constexpr const char *enableSamlingProfilerSymbol = "?enableSamplingProfiler@HermesRuntime@hermes@facebook@@SAXXZ";
constexpr const char *disableSamlingProfilerSymbol = "?disableSamplingProfiler@HermesRuntime@hermes@facebook@@SAXXZ";
constexpr const char *dumpSampledTraceToFileSymbol =
    "?dumpSampledTraceToFile@HermesRuntime@hermes@facebook@@SAXAEBV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@@Z";
#endif

#if _M_IX86
constexpr const char *makeHermesRuntimeSymbol =
    "?makeHermesRuntime@hermes@facebook@@YA?AV?$unique_ptr@VHermesRuntime@hermes@facebook@@U?$default_delete@VHermesRuntime@hermes@facebook@@@std@@@std@@ABVRuntimeConfig@vm@1@@Z";
constexpr const char *enableSamlingProfilerSymbol = "?enableSamplingProfiler@HermesRuntime@hermes@facebook@@SAXXZ";
constexpr const char *disableSamlingProfilerSymbol = "?disableSamplingProfiler@HermesRuntime@hermes@facebook@@SAXXZ";
constexpr const char *dumpSampledTraceToFileSymbol =
    "?dumpSampledTraceToFile@HermesRuntime@hermes@facebook@@SAXABV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@@Z";
#endif

static std::once_flag s_hermesLoading;

static void EnsureHermesLoaded() noexcept {
  std::call_once(s_hermesLoading, []() {
    VerifyElseCrashSz(!s_hermesModule, "Invalid state: \"hermes.dll\" being loaded again.");

    s_hermesModule = LoadLibrary(L"hermes.dll");
    VerifyElseCrashSz(s_hermesModule, "Could not load \"hermes.dll\"");

    s_makeHermesRuntime =
        reinterpret_cast<decltype(s_makeHermesRuntime)>(GetProcAddress(s_hermesModule, makeHermesRuntimeSymbol));
    VerifyElseCrash(s_makeHermesRuntime);

    s_enableSamplingProfiler = reinterpret_cast<decltype(s_enableSamplingProfiler)>(
        GetProcAddress(s_hermesModule, enableSamlingProfilerSymbol));
    VerifyElseCrash(s_enableSamplingProfiler);

    s_disableSamplingProfiler = reinterpret_cast<decltype(s_disableSamplingProfiler)>(
        GetProcAddress(s_hermesModule, disableSamlingProfilerSymbol));
    VerifyElseCrash(s_disableSamplingProfiler);

    s_dumpSampledTraceToFile = reinterpret_cast<decltype(s_dumpSampledTraceToFile)>(
        GetProcAddress(s_hermesModule, dumpSampledTraceToFileSymbol));
    VerifyElseCrash(s_dumpSampledTraceToFile);
  });
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
