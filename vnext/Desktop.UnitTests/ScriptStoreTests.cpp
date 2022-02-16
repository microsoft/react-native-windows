// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include <BaseScriptStoreImpl.h>
#include <CppUnitTest.h>
#include <RuntimeOptions.h>

// Windows API
#include <Windows.h>
#include <winrt/Windows.System.Diagnostics.h>

// Standard Library
#include <cstdio>
#include <cstdlib>

using namespace facebook::jsi;
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

using std::make_shared;
using std::make_unique;
using std::unique_ptr;
using winrt::Windows::System::Diagnostics::ProcessDiagnosticInfo;

namespace Microsoft::JSI::Test {

TEST_CLASS (ScriptStoreIntegrationTest) {
  TEST_CLASS_INITIALIZE(Init) {
    React::SetRuntimeOptionBool("JSI.MemoryMappedScriptStore", true);
  }

  // Do not run this test in parallel with others.
  // It uses process telemetry and should run on isolation.
  TEST_METHOD(RetrievePreparedScriptMemoryUsage) {
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

    auto startWorkingSet =
        ProcessDiagnosticInfo::GetForCurrentProcess().MemoryUsage().GetReport().WorkingSetSizeInBytes();

    auto prepd = preparedScriptStore->tryGetPreparedScript(scriptSignature, runtimeSignature, "prepareTag");
    Assert::AreEqual(fileSize, prepd->size());

    auto endWorkingSet =
        ProcessDiagnosticInfo::GetForCurrentProcess().MemoryUsage().GetReport().WorkingSetSizeInBytes();

    // Based on recurring local testing:
    // Without memory mapping: about 6.11 MB (fileSize + app overhead)
    // With memory mapping: about 2.14 MB (view overhead + app overhead)
    // Expected working set size should be lower than the actual file size, provided it is larger than the app overhead
    Assert::IsTrue(endWorkingSet - startWorkingSet < fileSize);
  }
};
} // namespace Microsoft::JSI::Test
