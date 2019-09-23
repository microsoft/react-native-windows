#include "MemoryMappedBuffer.h"
#include "utilities.h"

#include <CppUnitTest.h>

#include <shlwapi.h>
#include <windows.h>

#include <cstring>
#include <memory>

using facebook::jsi::Buffer;
using facebook::jsi::JSINativeException;
using facebook::react::utilities::checkedReinterpretCast;
using Microsoft::JSI::MakeMemoryMappedBuffer;
using Microsoft::VisualStudio::CppUnitTestFramework::Assert;

namespace {

std::wstring GetTestFileName() {
  wchar_t tempPath[MAX_PATH];
  if (!GetTempPathW(MAX_PATH, tempPath)) {
    std::terminate();
  }

  wchar_t testFileName[MAX_PATH];
  if (!GetTempFileNameW(
          tempPath, L"MemoryMappedBufferUnitTests", 0, testFileName)) {
    std::terminate();
  }

  return std::wstring(testFileName);
}

uint32_t GetPageSize() noexcept {
  SYSTEM_INFO systemInfo;
  GetSystemInfo(&systemInfo);
  return systemInfo.dwPageSize;
}

} // anonymous namespace

namespace Microsoft::JSI::Test {

TEST_CLASS(MemoryMappedBufferUnitTests) {
 private:
  std::wstring m_testFileName;

  void WriteTestFile(const char *const content, size_t size) {
    FILE *testFilePtr;

    // We need pass in the "b" flag to ensure that fwrite modify its input when
    // outputting to disk.
    if (_wfopen_s(&testFilePtr, m_testFileName.c_str(), L"wb")) {
      std::terminate();
    }

    std::unique_ptr<FILE, decltype(&fclose)> testFilePtrWrapper(
        testFilePtr, fclose);

    if (fwrite(content, sizeof(*content), size, testFilePtrWrapper.get()) !=
        size) {
      std::terminate();
    }
  }

  void DeleteTestFile() {
    if (!DeleteFileW(m_testFileName.c_str()) &&
        GetLastError() != ERROR_FILE_NOT_FOUND) {
      std::terminate();
    }
    if (PathFileExistsW(m_testFileName.c_str())) {
      std::terminate();
    }
  }

  TEST_METHOD_INITIALIZE(Initialize) {
    DeleteTestFile();
  }

  TEST_METHOD_CLEANUP(CleanUp) {
    DeleteTestFile();
  }

 public:
  MemoryMappedBufferUnitTests() : m_testFileName{GetTestFileName()} {}

  TEST_METHOD(SimpleTest_NoOffest) {
    constexpr const char *const content = "This is a very interesting string.";
    const size_t size = strlen(content);
    WriteTestFile(content, size);

    std::shared_ptr<Buffer> buffer =
        MakeMemoryMappedBuffer(m_testFileName.c_str());

    Assert::IsTrue(buffer->size() == size);
    Assert::IsTrue(
        strcmp(checkedReinterpretCast<const char *>(buffer->data()), content) ==
        0);
  }

  TEST_METHOD(SimpleTest_WithOffest) {
    constexpr const char *const fileContent =
        "This is another very interesting string.";
    const size_t fileSize = strlen(fileContent);
    WriteTestFile(fileContent, fileSize);

    const size_t fileOffset = 3;
    std::shared_ptr<Buffer> buffer =
        MakeMemoryMappedBuffer(m_testFileName.c_str(), fileOffset);

    Assert::IsTrue(buffer->size() == fileSize - fileOffset);
    Assert::IsTrue(
        strcmp(
            checkedReinterpretCast<const char *>(buffer->data()),
            fileContent + fileOffset) == 0);
  }

  TEST_METHOD(EdgeCaseFileSizeTest_NoOffset) {
    std::string content("a", GetPageSize());
    WriteTestFile(content.c_str(), content.length());

    std::shared_ptr<Buffer> buffer =
        MakeMemoryMappedBuffer(m_testFileName.c_str());

    Assert::IsTrue(buffer->size() == content.length());
    Assert::IsTrue(
        strcmp(
            checkedReinterpretCast<const char *>(buffer->data()),
            content.c_str()) == 0);
  }

  TEST_METHOD(EdgeCaseFileSizeTest_WithOffset) {
    std::string content("a", GetPageSize());
    WriteTestFile(content.c_str(), content.length());

    const size_t fileOffset = 15;
    std::shared_ptr<Buffer> buffer =
        MakeMemoryMappedBuffer(m_testFileName.c_str(), fileOffset);

    Assert::IsTrue(buffer->size() == content.length() - fileOffset);
    Assert::IsTrue(
        strcmp(
            checkedReinterpretCast<const char *>(buffer->data()),
            content.c_str() + fileOffset) == 0);
  }

  TEST_METHOD(ErrorTest_NullptrFileName) {
    Assert::ExpectException<JSINativeException>([] {
      std::shared_ptr<Buffer> buffer = MakeMemoryMappedBuffer(nullptr);
    });
  }
  TEST_METHOD(ErrorTest_EmptyFile) {
    WriteTestFile("", 0);

    Assert::ExpectException<JSINativeException>([this] {
      std::shared_ptr<Buffer> buffer =
          MakeMemoryMappedBuffer(m_testFileName.c_str());
    });
  }

  TEST_METHOD(ErrorTest_InvalidOffset) {
    WriteTestFile("a", 1);

    Assert::ExpectException<JSINativeException>([this] {
      uint32_t badOffset = 3;
      std::shared_ptr<Buffer> buffer =
          MakeMemoryMappedBuffer(m_testFileName.c_str(), badOffset);
    });
  }
};

} // namespace Microsoft::JSI::Test
