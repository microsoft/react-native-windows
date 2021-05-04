// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include <BaseScriptStoreImpl.h>
#include <CppUnitTest.h>

// Windows API
#include <Windows.h>

using namespace facebook::jsi;
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

using std::make_shared;
using std::make_unique;
using std::unique_ptr;

namespace Microsoft::JSI::Test {

TEST_CLASS(ScriptStoreIntegrationTest)
{
  TEST_METHOD(V8_PersistPreparedScript)
  {
    unique_ptr<PreparedScriptStore> preparedScriptStore = nullptr;

    char tempPath[MAX_PATH];
    if (GetTempPathA(MAX_PATH, tempPath))
    {
      preparedScriptStore = make_unique<facebook::react::BasePreparedScriptStoreImpl>(tempPath);
    }
    else
    {
      Assert::Fail(L"Could not get temporary folder");
    }

    auto script = make_shared<StringBuffer>("{}");
    const auto scriptSignature = ScriptSignature{ "myscheme://my/path.js", 0};
    const auto runtimeSignature = JSRuntimeSignature{"V8", 8};
    const char *prepareTag = "prepareTag";
    preparedScriptStore->persistPreparedScript(script, scriptSignature, runtimeSignature, "prepareTag");
  }
};
} // namespace Microsoft::JSI::Test
