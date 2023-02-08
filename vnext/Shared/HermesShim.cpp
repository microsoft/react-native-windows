// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "HermesShim.h"
#include "Crash.h"

#if _M_IX86
#define SYMBOL_PREFIX "_"
#else
#define SYMBOL_PREFIX
#endif

#define DECLARE_SYMBOL(symbol, importedSymbol) \
  decltype(&importedSymbol) s_##symbol{};      \
  constexpr const char symbol##Symbol[] = SYMBOL_PREFIX #importedSymbol;

#define INIT_SYMBOL(symbol)                                                                            \
  s_##symbol = reinterpret_cast<decltype(s_##symbol)>(GetProcAddress(s_hermesModule, symbol##Symbol)); \
  VerifyElseCrash(s_##symbol);

#define CRASH_ON_ERROR(result) VerifyElseCrash(result == hermes_ok);

namespace Microsoft::ReactNative {

namespace {

DECLARE_SYMBOL(createRuntime, hermes_create_runtime);
DECLARE_SYMBOL(createRuntimeWithWER, hermes_create_runtime_with_wer);
DECLARE_SYMBOL(deleteRuntime, hermes_delete_runtime);
DECLARE_SYMBOL(getNonAbiSafeRuntime, hermes_get_non_abi_safe_runtime);
DECLARE_SYMBOL(dumpCrashData, hermes_dump_crash_data);
DECLARE_SYMBOL(samplingProfilerEnable, hermes_sampling_profiler_enable);
DECLARE_SYMBOL(samplingProfilerDisable, hermes_sampling_profiler_disable);
DECLARE_SYMBOL(samplingProfilerAdd, hermes_sampling_profiler_add);
DECLARE_SYMBOL(samplingProfilerRemove, hermes_sampling_profiler_remove);
DECLARE_SYMBOL(samplingProfilerDumpToFile, hermes_sampling_profiler_dump_to_file);

HMODULE s_hermesModule{};
std::once_flag s_hermesLoading;

void EnsureHermesLoaded() noexcept {
  std::call_once(s_hermesLoading, []() {
    VerifyElseCrashSz(!s_hermesModule, "Invalid state: \"hermes.dll\" being loaded again.");

    s_hermesModule = LoadLibrary(L"hermes.dll");
    VerifyElseCrashSz(s_hermesModule, "Could not load \"hermes.dll\"");

    INIT_SYMBOL(createRuntime);
    INIT_SYMBOL(createRuntimeWithWER);
    INIT_SYMBOL(deleteRuntime);
    INIT_SYMBOL(getNonAbiSafeRuntime);
    INIT_SYMBOL(dumpCrashData);
    INIT_SYMBOL(samplingProfilerEnable);
    INIT_SYMBOL(samplingProfilerDisable);
    INIT_SYMBOL(samplingProfilerAdd);
    INIT_SYMBOL(samplingProfilerRemove);
    INIT_SYMBOL(samplingProfilerDumpToFile);
  });
}

struct RuntimeDeleter {
  RuntimeDeleter(std::shared_ptr<const HermesShim> &&hermesShimPtr) noexcept
      : hermesShimPtr_(std::move(hermesShimPtr)) {}

  void operator()(facebook::hermes::HermesRuntime * /*runtime*/) {
    // Do nothing. Instead, we rely on the RuntimeDeleter destructor.
  }

 private:
  std::shared_ptr<const HermesShim> hermesShimPtr_;
};

} // namespace

HermesShim::HermesShim(hermes_runtime runtime) noexcept : runtime_(runtime) {
  CRASH_ON_ERROR(s_getNonAbiSafeRuntime(runtime_, reinterpret_cast<void **>(&nonAbiSafeRuntime_)));
}

HermesShim::~HermesShim() {
  CRASH_ON_ERROR(s_deleteRuntime(runtime_));
}

/*static*/ std::shared_ptr<HermesShim> HermesShim::make() noexcept {
  EnsureHermesLoaded();
  hermes_runtime runtime{};
  CRASH_ON_ERROR(s_createRuntime(&runtime));
  return std::make_shared<HermesShim>(runtime);
}

/*static*/
std::shared_ptr<HermesShim> HermesShim::makeWithWER() noexcept {
  EnsureHermesLoaded();
  hermes_runtime runtime{};
  CRASH_ON_ERROR(s_createRuntimeWithWER(&runtime));
  return std::make_shared<HermesShim>(runtime);
}

std::shared_ptr<facebook::hermes::HermesRuntime> HermesShim::getRuntime() const noexcept {
  return std::shared_ptr<facebook::hermes::HermesRuntime>(nonAbiSafeRuntime_, RuntimeDeleter(shared_from_this()));
}

void HermesShim::dumpCrashData(int fileDescriptor) const noexcept {
  CRASH_ON_ERROR(s_dumpCrashData(runtime_, fileDescriptor));
}

/*static*/ void HermesShim::enableSamplingProfiler() noexcept {
  EnsureHermesLoaded();
  CRASH_ON_ERROR(s_samplingProfilerEnable());
}

/*static*/ void HermesShim::disableSamplingProfiler() noexcept {
  EnsureHermesLoaded();
  CRASH_ON_ERROR(s_samplingProfilerDisable());
}

/*static*/ void HermesShim::dumpSampledTraceToFile(const std::string &fileName) noexcept {
  EnsureHermesLoaded();
  CRASH_ON_ERROR(s_samplingProfilerDumpToFile(fileName.c_str()));
}
void HermesShim::addToProfiling() const noexcept {
  CRASH_ON_ERROR(s_samplingProfilerAdd(runtime_));
}

void HermesShim::removeFromProfiling() const noexcept {
  CRASH_ON_ERROR(s_samplingProfilerRemove(runtime_));
}

} // namespace Microsoft::ReactNative
