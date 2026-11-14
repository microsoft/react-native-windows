// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include <CppUnitTest.h>
#include <V8JsiRuntimeTest.h>
#include <Windows.h>
#include <jsi/instrumentation.h>
#include <winrt/base.h>

#include <filesystem>
#include <memory>
#include <type_traits>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
namespace jsi = facebook::jsi;

namespace Microsoft::React::Test {
namespace {

using Stats = v8runtime::HostProxyLifetimeStats;
using CreateModule = void(__cdecl *)(jsi::Runtime *, jsi::Object *);
using CreateRuntime = jsi::Runtime *(__cdecl *)();
using GetStats = void(__cdecl *)(Stats *) noexcept;

template <typename T>
T GetExport(HMODULE library, char const *name) {
  auto address = GetProcAddress(library, name);
  Assert::IsTrue(address != nullptr, L"Missing lifetime-test export; build both DLLs with lifetime tests enabled.");
  return reinterpret_cast<T>(address);
}

class LifetimeTestRuntime {
 public:
  LifetimeTestRuntime() {
    auto rnw = GetModuleHandleW(L"react-native-win32.dll");
    Assert::IsNotNull(rnw, L"The test must load react-native-win32.dll.");
    wchar_t path[MAX_PATH];
    auto length = GetModuleFileNameW(rnw, path, MAX_PATH);
    Assert::IsTrue(length != 0 && length < MAX_PATH, L"Cannot locate the RNW DLL.");
    auto v8Path = std::filesystem::path(path).parent_path() / L"v8jsi.dll";
    m_v8.reset(
        LoadLibraryExW(v8Path.c_str(), nullptr, LOAD_LIBRARY_SEARCH_DLL_LOAD_DIR | LOAD_LIBRARY_SEARCH_DEFAULT_DIRS));
    Assert::IsNotNull(m_v8.get(), L"Cannot load the instrumented v8jsi.dll beside react-native-win32.dll.");
#if defined(_M_IX86)
    constexpr char createModuleName[] = "_RNWCreateTurboModuleForLifetimeTest";
    constexpr char createRuntimeName[] = "_v8jsi_create_runtime_for_lifetime_test";
    constexpr char getStatsName[] = "_v8jsi_get_host_proxy_lifetime_stats";
#else
    constexpr char createModuleName[] = "RNWCreateTurboModuleForLifetimeTest";
    constexpr char createRuntimeName[] = "v8jsi_create_runtime_for_lifetime_test";
    constexpr char getStatsName[] = "v8jsi_get_host_proxy_lifetime_stats";
#endif
    m_createModule = GetExport<CreateModule>(rnw, createModuleName);
    m_createRuntime = GetExport<CreateRuntime>(m_v8.get(), createRuntimeName);
    m_getStats = GetExport<GetStats>(m_v8.get(), getStatsName);
  }

  std::unique_ptr<jsi::Runtime> Create() const {
    return std::unique_ptr<jsi::Runtime>{m_createRuntime()};
  }

  jsi::Object Module(jsi::Runtime &runtime) const {
    jsi::Object module(runtime);
    m_createModule(&runtime, &module);
    return module;
  }

  Stats Snapshot() const {
    Stats stats{};
    m_getStats(&stats);
    return stats;
  }

 private:
  std::unique_ptr<std::remove_pointer_t<HMODULE>, decltype(&FreeLibrary)> m_v8{nullptr, &FreeLibrary};
  CreateModule m_createModule{};
  CreateRuntime m_createRuntime{};
  GetStats m_getStats{};
};

void ExpectReclaimed(Stats before, Stats after) {
  Assert::AreEqual(
      after.proxiesAllocated - before.proxiesAllocated,
      after.proxiesFreed - before.proxiesFreed,
      L"Native proxy allocations were not freed.");
  Assert::AreEqual(
      after.trackersAllocated - before.trackersAllocated,
      after.trackersFreed - before.trackersFreed,
      L"Lifetime trackers were not freed.");
}

} // namespace

// Run this class alone in a test process: V8 counters are process-wide.
TEST_CLASS (TurboModuleLifetimeTests) {
  TEST_METHOD_INITIALIZE(Initialize) {
    winrt::uninit_apartment();
    winrt::init_apartment(winrt::apartment_type::multi_threaded);
  }

  TEST_METHOD_CLEANUP(Cleanup) {
    winrt::uninit_apartment();
  }

  TEST_METHOD(PropertyReadsReclaimProxiesAndTrackers) {
    LifetimeTestRuntime factory;
    auto baseline = factory.Snapshot();
    auto runtime = factory.Create();
    {
      auto module = factory.Module(*runtime);
      auto retained = module.getPropertyAsFunction(*runtime, "value");
      {
        auto other = module.getPropertyAsFunction(*runtime, "value");
        Assert::IsFalse(
            jsi::Object::strictEquals(*runtime, retained, other), L"The property cache must not mask this test.");
      }
      runtime->instrumentation().collectGarbage("lifetime test warmup");

      constexpr uint64_t batchSize = 10000;
      for (size_t batch = 0; batch != 10; ++batch) {
        auto before = factory.Snapshot();
        for (uint64_t i = 0; i != batchSize; ++i) {
          auto method = module.getProperty(*runtime, "value");
          Assert::IsTrue(method.isObject());
        }
        runtime->instrumentation().collectGarbage("TurboModule property read batch");
        auto after = factory.Snapshot();
        Assert::AreEqual(
            batchSize, after.proxiesAllocated - before.proxiesAllocated, L"Reads did not allocate distinct proxies.");
        Assert::AreEqual(batchSize, after.trackersAllocated - before.trackersAllocated);
        ExpectReclaimed(before, after);
        Assert::AreEqual(42.0, retained.call(*runtime).getNumber(), L"GC reclaimed a retained function prematurely.");
      }
    }
    runtime->instrumentation().collectGarbage("release retained module and function");
    ExpectReclaimed(baseline, factory.Snapshot());
    runtime.reset();
    ExpectReclaimed(baseline, factory.Snapshot());
  }

  TEST_METHOD(RuntimeTeardownReclaimsReachableWrappers) {
    LifetimeTestRuntime factory;
    for (size_t iteration = 0; iteration != 5; ++iteration) {
      auto baseline = factory.Snapshot();
      auto runtime = factory.Create();
      {
        auto module = factory.Module(*runtime);
        runtime->global().setProperty(*runtime, "retainedModule", module);
        runtime->global().setProperty(*runtime, "retainedMethod", module.getProperty(*runtime, "value"));
      }
      auto live = factory.Snapshot();
      Assert::AreEqual(uint64_t{2}, live.proxiesAllocated - baseline.proxiesAllocated);
      runtime.reset();
      ExpectReclaimed(baseline, factory.Snapshot());
    }
  }
};

} // namespace Microsoft::React::Test
