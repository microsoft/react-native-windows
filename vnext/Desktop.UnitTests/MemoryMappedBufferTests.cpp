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
using Microsoft::JSI::makeMemoryMappedBuffer;
using Microsoft::VisualStudio::CppUnitTestFramework::Assert;

namespace {

static constexpr const wchar_t *const testFileName = L"temp.txt";

static void writeTestFile(const char *const content, size_t size) {
  FILE *testFilePtr;

  // We need pass in the "b" flag to ensure that fwrite modify its input when
  // outputting to disk.
  if (_wfopen_s(&testFilePtr, testFileName, L"wb")) {
    return;
  }

  std::unique_ptr<FILE, decltype(&fclose)> testFilePtrWrapper(
      testFilePtr, fclose);

  fwrite(content, sizeof(*content), size, testFilePtrWrapper.get());
}

static void deleteTestFile() {
  if (!DeleteFileW(testFileName)) {
    Assert::IsTrue(GetLastError() == ERROR_FILE_NOT_FOUND);
  }
  Assert::IsTrue(!PathFileExistsW(testFileName));
}

uint32_t getPageSize() noexcept {
  SYSTEM_INFO systemInfo;
  GetSystemInfo(&systemInfo);
  return systemInfo.dwPageSize;
}

} // namespace

// Clang format does not work with the Microsoft Cpp Test Macros. Hence we turn
// it off here.
// clang-format off

TEST_CLASS(MemoryMappedBufferUnitTests){
 private:
   TEST_METHOD_INITIALIZE(initialize) {
     deleteTestFile();
   }

   TEST_METHOD_CLEANUP(cleanUp) {
     deleteTestFile();
   }

 public :
  TEST_METHOD(SimpleTest_NoOffest) {
    constexpr const char *const content =
      "This is a very interesting string.";
    const size_t size = strlen(content);
    writeTestFile(content, size);

    std::shared_ptr<Buffer> buffer = makeMemoryMappedBuffer(testFileName);

    Assert::IsTrue(buffer->size() == size);
    Assert::IsTrue(
      strcmp(checkedReinterpretCast<const char*>(buffer->data()), content)
      == 0);
  }

  TEST_METHOD(SimpleTest_WithOffest) {
    constexpr const char *const fileContent =
      "This is another very interesting string.";
    const size_t fileSize = strlen(fileContent);
    writeTestFile(fileContent, fileSize);

    const size_t fileOffset = 3;
    std::shared_ptr<Buffer> buffer =
      makeMemoryMappedBuffer(testFileName, fileOffset);

    Assert::IsTrue(buffer->size() == fileSize - fileOffset);
    Assert::IsTrue(
      strcmp(checkedReinterpretCast<const char*>(buffer->data()),
        fileContent + fileOffset)
      == 0);
  }

  TEST_METHOD(EdgeCaseFileSizeTest_NoOffset) {
    std::string content("a", getPageSize());
    writeTestFile(content.c_str(), content.length());

    std::shared_ptr<Buffer> buffer = makeMemoryMappedBuffer(testFileName);

    Assert::IsTrue(buffer->size() == content.length());
    Assert::IsTrue(
      strcmp(checkedReinterpretCast<const char*>(buffer->data()),
        content.c_str())
      == 0);
  }

  TEST_METHOD(EdgeCaseFileSizeTest_WithOffset) {
    std::string content("a", getPageSize());
    writeTestFile(content.c_str(), content.length());

    const size_t fileOffset = 15;
    std::shared_ptr<Buffer> buffer =
      makeMemoryMappedBuffer(testFileName, fileOffset);

    Assert::IsTrue(buffer->size() == content.length() - fileOffset);
    Assert::IsTrue(
      strcmp(checkedReinterpretCast<const char*>(buffer->data()),
        content.c_str() + fileOffset)
      == 0);
  }

  TEST_METHOD(ErrorTest_EmptyFile) {
    writeTestFile("", 0);

    try {
      std::shared_ptr<Buffer> buffer = makeMemoryMappedBuffer(testFileName);
    } catch (JSINativeException& e) {
      Assert::IsTrue(strcmp(e.what(), "Cannot memory map an empty file.") == 0);
    }
  }

  TEST_METHOD(ErrorTest_InvalidOffset) {
    writeTestFile("a", 1);

    try {
      size_t badOffset = 3;
      std::shared_ptr<Buffer> buffer =
        makeMemoryMappedBuffer(testFileName, badOffset);
    } catch (JSINativeException& e) {
      Assert::IsTrue(strcmp(e.what(), "Invalid offset.") == 0);
    }
  }
};

// clang-format on
