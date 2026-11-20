// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include <JSI/V8AbiRuntimeHolder.h>
#include <Threading/MessageQueueThreadFactory.h>
#include <V8JsiRuntime.h>
#include "V8AbiTestUtils.h"

#include <atomic>

using facebook::jsi::Buffer;
using facebook::jsi::JSRuntimeSignature;
using facebook::jsi::PreparedScriptStore;
using facebook::jsi::Runtime;
using facebook::jsi::ScriptSignature;
using facebook::jsi::StringBuffer;
using Microsoft::ReactNative::MakeV8AbiRuntime;
using Microsoft::ReactNative::V8AbiRuntimeArgs;

namespace {

// Keeps prepared scripts in memory and counts the calls.
struct TestPreparedScriptStore : PreparedScriptStore {
  std::shared_ptr<const Buffer> tryGetPreparedScript(
      const ScriptSignature & /*scriptSignature*/,
      const JSRuntimeSignature & /*runtimeSignature*/,
      const char * /*prepareTag*/) noexcept override {
    ++getCount;
    return preparedScript;
  }

  void persistPreparedScript(
      std::shared_ptr<const Buffer> script,
      const ScriptSignature & /*scriptMetadata*/,
      const JSRuntimeSignature & /*runtimeMetadata*/,
      const char * /*prepareTag*/) noexcept override {
    ++persistCount;
    preparedScript = std::move(script);
  }

  std::shared_ptr<const Buffer> preparedScript;
  std::atomic<int> getCount{0};
  std::atomic<int> persistCount{0};
};

} // namespace

TEST(V8AbiRuntimeTest, ReportsUnloadedV8Jsi) {
  std::string error;
  auto runtime = MakeV8AbiRuntime(nullptr, V8AbiRuntimeArgs{}, error);

  EXPECT_EQ(runtime, nullptr);
  EXPECT_NE(error.find("v8jsi2.dll could not be loaded"), std::string::npos) << error;
}

TEST(V8AbiRuntimeTest, ReportsV8JsiWithoutAbi) {
  // v8jsi.dll from ReactNative.V8Jsi.Windows, used by the direct V8 tests, does not export the V8 JSI ABI.
  HMODULE oldV8Jsi = Microsoft::ReactNative::SafeLoadLibrary(
      (GetTestExeDir() + L"v8jsi.dll").c_str(), LOAD_LIBRARY_SEARCH_DLL_LOAD_DIR);
  ASSERT_NE(oldV8Jsi, nullptr);

  std::string error;
  auto runtime = MakeV8AbiRuntime(oldV8Jsi, V8AbiRuntimeArgs{}, error);

  EXPECT_EQ(runtime, nullptr);
  EXPECT_NE(error.find("does not export v8_jsi_abi_runtime_create"), std::string::npos) << error;
}

TEST(V8AbiRuntimeTest, RunsAlongsideDirectV8) {
  auto directRuntime = v8runtime::makeV8Runtime(v8runtime::V8RuntimeArgs{});
  ASSERT_NE(directRuntime, nullptr);

  std::string error;
  auto abiRuntime = MakeV8AbiRuntime(LoadV8JsiAbiForTesting(), V8AbiRuntimeArgs{}, error);
  ASSERT_NE(abiRuntime, nullptr) << error;

  EXPECT_EQ(directRuntime->evaluateJavaScript(std::make_shared<StringBuffer>("6 * 7"), "direct.js").getNumber(), 42);
  EXPECT_EQ(abiRuntime->evaluateJavaScript(std::make_shared<StringBuffer>("40 + 2"), "abi.js").getNumber(), 42);

  HMODULE directModule = ::GetModuleHandleW(L"v8jsi.dll");
  HMODULE abiModule = ::GetModuleHandleW(L"v8jsi2.dll");
  ASSERT_NE(directModule, nullptr);
  ASSERT_NE(abiModule, nullptr);
  EXPECT_NE(directModule, abiModule);
}

TEST(V8AbiRuntimeTest, EvaluatesJavaScript) {
  std::string error;
  auto runtime = MakeV8AbiRuntime(LoadV8JsiAbiForTesting(), V8AbiRuntimeArgs{}, error);
  ASSERT_NE(runtime, nullptr) << error;

  auto result = runtime->evaluateJavaScript(std::make_shared<StringBuffer>("6 * 7"), "test.js");

  EXPECT_EQ(result.getNumber(), 42);
}

namespace {

// Runs the script in two runtimes that share the store, each created on the JS queue.
void RunTwiceWithScriptCache(const std::shared_ptr<TestPreparedScriptStore> &store, bool usePreparedScript) {
  auto jsQueue = Microsoft::ReactNative::MakeJSQueueThread();
  auto script = std::make_shared<StringBuffer>("function add(a, b) { return a + b; } add(40, 2);");

  for (int i = 0; i < 2; ++i) {
    double value{};
    std::string error;
    jsQueue->runOnQueueSync([&]() {
      V8AbiRuntimeArgs args;
      args.jsQueue = jsQueue;
      args.preparedScriptStore = store;
      args.debuggerRuntimeName = "V8AbiRuntimeTest";
      auto runtime = MakeV8AbiRuntime(LoadV8JsiAbiForTesting(), std::move(args), error);
      if (runtime) {
        value = usePreparedScript
            ? runtime->evaluatePreparedJavaScript(runtime->prepareJavaScript(script, "cached.js")).getNumber()
            : runtime->evaluateJavaScript(script, "cached.js").getNumber();
      }
    });
    EXPECT_TRUE(error.empty()) << error;
    EXPECT_EQ(value, 42);
  }

  jsQueue->quitSynchronous();
}

} // namespace

// React Native loads bundles with evaluateJavaScript.
TEST(V8AbiRuntimeTest, EvaluateUsesScriptCache) {
  auto store = std::make_shared<TestPreparedScriptStore>();
  RunTwiceWithScriptCache(store, /*usePreparedScript*/ false);

  // The first runtime stores the compiled script; the second one reads it back.
  EXPECT_EQ(store->persistCount, 1);
  EXPECT_EQ(store->getCount, 2);
}

TEST(V8AbiRuntimeTest, PreparedScriptUsesScriptCache) {
  auto store = std::make_shared<TestPreparedScriptStore>();
  RunTwiceWithScriptCache(store, /*usePreparedScript*/ true);

  EXPECT_EQ(store->persistCount, 1);
  EXPECT_EQ(store->getCount, 2);
}
