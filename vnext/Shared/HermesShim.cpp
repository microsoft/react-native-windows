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
static decltype(&facebook::hermes::makeHermesRuntimeWithWER) s_makeHermesRuntimeWithWER{nullptr};
static decltype(&facebook::hermes::hermesCrashHandler) s_hermesCrashHandler{nullptr};

#if _M_X64
constexpr const char *makeHermesRuntimeSymbol =
    "?makeHermesRuntime@hermes@facebook@@YA?AV?$unique_ptr@VHermesRuntime@hermes@facebook@@U?$default_delete@VHermesRuntime@hermes@facebook@@@std@@@std@@AEBVRuntimeConfig@vm@1@@Z";
constexpr const char *enableSamlingProfilerSymbol = "?enableSamplingProfiler@HermesRuntime@hermes@facebook@@SAXXZ";
constexpr const char *disableSamlingProfilerSymbol = "?disableSamplingProfiler@HermesRuntime@hermes@facebook@@SAXXZ";
constexpr const char *dumpSampledTraceToFileSymbol =
    "?dumpSampledTraceToFile@HermesRuntime@hermes@facebook@@SAXAEBV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@@Z";
constexpr const char *makeHermesRuntimeWithWERSymbol =
    "?makeHermesRuntimeWithWER@hermes@facebook@@YA?AV?$unique_ptr@VHermesRuntime@hermes@facebook@@U?$default_delete@VHermesRuntime@hermes@facebook@@@std@@@std@@XZ";
constexpr const char *hermesCrashHandlerSymbol = "?hermesCrashHandler@hermes@facebook@@YAXAEAVHermesRuntime@12@H@Z";
#endif

#if _M_ARM64
constexpr const char *makeHermesRuntimeSymbol =
    "?makeHermesRuntime@hermes@facebook@@YA?AV?$unique_ptr@VHermesRuntime@hermes@facebook@@U?$default_delete@VHermesRuntime@hermes@facebook@@@std@@@std@@AEBVRuntimeConfig@vm@1@@Z";
constexpr const char *enableSamlingProfilerSymbol = "?enableSamplingProfiler@HermesRuntime@hermes@facebook@@SAXXZ";
constexpr const char *disableSamlingProfilerSymbol = "?disableSamplingProfiler@HermesRuntime@hermes@facebook@@SAXXZ";
constexpr const char *dumpSampledTraceToFileSymbol =
    "?dumpSampledTraceToFile@HermesRuntime@hermes@facebook@@SAXAEBV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@@Z";
constexpr const char *makeHermesRuntimeWithWERSymbol =
    "?makeHermesRuntimeWithWER@hermes@facebook@@YA?AV?$unique_ptr@VHermesRuntime@hermes@facebook@@U?$default_delete@VHermesRuntime@hermes@facebook@@@std@@@std@@XZ";
constexpr const char *hermesCrashHandlerSymbol = "?hermesCrashHandler@hermes@facebook@@YAXAEAVHermesRuntime@12@H@Z";
#endif

#if _M_IX86
constexpr const char *makeHermesRuntimeSymbol =
    "?makeHermesRuntime@hermes@facebook@@YA?AV?$unique_ptr@VHermesRuntime@hermes@facebook@@U?$default_delete@VHermesRuntime@hermes@facebook@@@std@@@std@@ABVRuntimeConfig@vm@1@@Z";
constexpr const char *enableSamlingProfilerSymbol = "?enableSamplingProfiler@HermesRuntime@hermes@facebook@@SAXXZ";
constexpr const char *disableSamlingProfilerSymbol = "?disableSamplingProfiler@HermesRuntime@hermes@facebook@@SAXXZ";
constexpr const char *dumpSampledTraceToFileSymbol =
    "?dumpSampledTraceToFile@HermesRuntime@hermes@facebook@@SAXABV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@@Z";
constexpr const char *makeHermesRuntimeWithWERSymbol =
    "?makeHermesRuntimeWithWER@hermes@facebook@@YA?AV?$unique_ptr@VHermesRuntime@hermes@facebook@@U?$default_delete@VHermesRuntime@hermes@facebook@@@std@@@std@@XZ";
constexpr const char *hermesCrashHandlerSymbol = "?hermesCrashHandler@hermes@facebook@@YAXAAVHermesRuntime@12@H@Z";
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

    s_makeHermesRuntimeWithWER = reinterpret_cast<decltype(s_makeHermesRuntimeWithWER)>(
        GetProcAddress(s_hermesModule, makeHermesRuntimeWithWERSymbol));
    VerifyElseCrash(s_makeHermesRuntimeWithWER);

    s_hermesCrashHandler =
        reinterpret_cast<decltype(s_hermesCrashHandler)>(GetProcAddress(s_hermesModule, hermesCrashHandlerSymbol));
    VerifyElseCrash(s_hermesCrashHandler);
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

std::unique_ptr<facebook::hermes::HermesRuntime> makeHermesRuntimeWithWER() {
  EnsureHermesLoaded();
  return s_makeHermesRuntimeWithWER();
}

void hermesCrashHandler(facebook::hermes::HermesRuntime &runtime, int fileDescriptor) {
  EnsureHermesLoaded();
  s_hermesCrashHandler(runtime, fileDescriptor);
}

} // namespace Microsoft::ReactNative::HermesShim
