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
    constexpr size_t bufferSize = fileSize + 1; // +1 for trailing \0
    char *buffer = static_cast<char *>(std::malloc(bufferSize * sizeof(char)));
    int cursor = 0;
    char id = 0;
    while (cursor < fileSize) {
      cursor += sprintf_s(buffer + cursor, bufferSize - cursor, "var v%019X = 'x';\n", id++);
    }

    auto stringBuffer = make_shared<StringBuffer>(buffer);
    std::free(buffer);
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
