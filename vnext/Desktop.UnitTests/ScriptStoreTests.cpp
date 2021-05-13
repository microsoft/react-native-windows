// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include <BaseScriptStoreImpl.h>
#include <CppUnitTest.h>

// Windows API
#include <Windows.h>

// Standard Library
#include <cstdio>
#include <cstdlib>

using namespace facebook::jsi;
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

using std::make_shared;
using std::make_unique;
using std::unique_ptr;

namespace Microsoft::JSI::Test {

TEST_CLASS (ScriptStoreIntegrationTest) {
  TEST_METHOD(V8_PersistPreparedScript) {
    unique_ptr<PreparedScriptStore> preparedScriptStore = nullptr;

    char tempPath[MAX_PATH];
    if (GetTempPathA(MAX_PATH, tempPath)) {
      preparedScriptStore = make_unique<facebook::react::BasePreparedScriptStoreImpl>(tempPath);
    } else {
      Assert::Fail(L"Could not get temporary folder");
    }

    constexpr size_t fileSize = 4 * 1024 * 1024;
    std::vector<char> chars{};
    while (chars.size() < fileSize) {
      chars.emplace_back(static_cast<char>('0' + chars.size() % 16));
    }

    auto stringBuffer = make_shared<StringBuffer>(std::string{chars.cbegin(), chars.cend()});
    const auto scriptSignature = ScriptSignature{"myscheme://my/path.js", 0};
    const auto runtimeSignature = JSRuntimeSignature{"V8", 8};
    const char *prepareTag = "prepareTag";
    preparedScriptStore->persistPreparedScript(stringBuffer, scriptSignature, runtimeSignature, "prepareTag");

    auto prepd = preparedScriptStore->tryGetPreparedScript(scriptSignature, runtimeSignature, "prepareTag");
    auto size = prepd->size();

    Assert::AreEqual(fileSize, size);
  }
};
} // namespace Microsoft::JSI::Test
