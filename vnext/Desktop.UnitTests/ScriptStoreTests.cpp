// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include <BaseScriptStoreImpl.h>
#include <CppRuntimeOptions.h>
#include <CppUnitTest.h>

// Windows API
#include <Windows.h>
#include <winrt/Windows.System.Diagnostics.h>

// Standard Library
#include <cstdio>
#include <cstdlib>
#include <filesystem>
#include <string>

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

    auto prepped = preparedScriptStore->tryGetPreparedScript(scriptSignature, runtimeSignature, "prepareTag");
    Assert::AreEqual(fileSize, prepped->size());

    auto endWorkingSet =
        ProcessDiagnosticInfo::GetForCurrentProcess().MemoryUsage().GetReport().WorkingSetSizeInBytes();

    // Based on recurring local testing:
    // Without memory mapping: about 6.11 MB (fileSize + app overhead)
    // With memory mapping: about 2.14 MB (view overhead + app overhead)
    // Expected working set size should be lower than the actual file size, provided it is larger than the app overhead
    // plus 10% to account for memory used by hashing
    Assert::IsTrue(endWorkingSet - startWorkingSet < fileSize * 1.1);
  }
};

TEST_CLASS (PreparedScriptStoreTest) {
  // Returns a fixed buffer for any id, simulating a cache file with arbitrary content.
  struct FixedBufferStore : facebook::react::BufferStore {
    explicit FixedBufferStore(std::string content) : content_(std::move(content)) {}

    std::unique_ptr<const Buffer> getBuffer(const std::string &) noexcept override {
      return make_unique<StringBuffer>(content_);
    }

    bool persistBuffer(const std::string &, std::unique_ptr<const Buffer>) noexcept override {
      return false;
    }

    std::string content_;
  };

  TEST_METHOD(TruncatedPreparedScriptIsRejected) {
    const auto scriptSignature = ScriptSignature{"myscheme://my/path.js", 0};
    const auto runtimeSignature = JSRuntimeSignature{"Hermes", 1};

    for (const std::string content : {std::string{}, std::string{"RNWPREP"}, std::string(40, 'x')}) {
      facebook::react::BasePreparedScriptStoreImpl store{make_shared<FixedBufferStore>(content)};
      Assert::IsNull(store.tryGetPreparedScript(scriptSignature, runtimeSignature, "perf").get());
    }
  }

  TEST_METHOD(PersistAndRetrieveInNonAsciiDirectory) {
    wchar_t tempPath[MAX_PATH];
    Assert::AreNotEqual(0ul, GetTempPathW(MAX_PATH, tempPath));
    std::filesystem::path directory{tempPath};
    directory /= L"rnw_script_store_\u00e9\u4e2d_" + std::to_wstring(GetCurrentProcessId());
    std::filesystem::create_directories(directory);
    struct DirectoryCleanup {
      const std::filesystem::path &path;
      ~DirectoryCleanup() {
        std::error_code ec;
        std::filesystem::remove_all(path, ec);
      }
    } cleanup{directory};

    const std::string storeDirectory = winrt::to_string(directory.wstring() + L"\\");
    const auto scriptSignature = ScriptSignature{"myscheme://my/path.js", 0};
    const auto runtimeSignature = JSRuntimeSignature{"Hermes", 1};
    const std::string content = "prepared script content";

    for (bool memoryMapped : {false, true}) {
      React::SetRuntimeOptionBool("JSI.MemoryMappedScriptStore", memoryMapped);

      facebook::react::BasePreparedScriptStoreImpl store{storeDirectory};
      store.persistPreparedScript(make_shared<StringBuffer>(content), scriptSignature, runtimeSignature, "perf");

      auto prepared = store.tryGetPreparedScript(scriptSignature, runtimeSignature, "perf");
      Assert::IsNotNull(prepared.get());
      Assert::AreEqual(content, std::string{reinterpret_cast<const char *>(prepared->data()), prepared->size()});
    }

    // The file must be created in the requested directory, not in a mis-decoded (ANSI) path.
    size_t fileCount = 0;
    for ([[maybe_unused]] const auto &entry : std::filesystem::directory_iterator(directory)) {
      ++fileCount;
    }
    Assert::AreEqual<size_t>(1, fileCount);
  }
};
} // namespace Microsoft::JSI::Test
