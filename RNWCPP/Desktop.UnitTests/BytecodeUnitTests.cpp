// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "../Chakra/ChakraHelpers.h"
#include "../Chakra/ChakraValue.h"
#include "cxxreact/JSBigString.h"
#include "Logging.h"
#include <Windows.h>
#include <shlwapi.h>
#include <CppUnitTest.h>
#include <functional>
#include <memory>
#include <fstream>

using Microsoft::VisualStudio::CppUnitTestFramework::Assert;
using facebook::react::ChakraString;
using facebook::react::JSBigString;
using facebook::react::JSBigStdString;
using facebook::react::MinimalChakraRuntime;
using facebook::react::evaluateScriptWithBytecode;
using facebook::react::RCTLogLevel;
using facebook::react::InitializeLogging;

namespace {

template <typename T>
bool arrayEquals(T* array1, T* array2, uint64_t size)
{
  for (uint64_t i = 0; i < size; ++i)
  {
    if (array1[i] != array2[i])
    {
      return false;
    }
  }

  return true;
}

struct BinaryFile
{
  std::unique_ptr<uint8_t[]> m_data;
  uint64_t m_size;

  BinaryFile(const char* const filename)
    : m_data{ nullptr }
    , m_size{ 0 }
  {
    std::ifstream file(filename, std::ios::binary | std::ios::ate);
    Assert::IsTrue(bool(file));

    m_size = file.tellg();
    m_data = std::make_unique<uint8_t[]>(m_size);

    file.seekg(0);
    Assert::IsTrue(bool(file));

    file.read(reinterpret_cast<char*>(m_data.get()), m_size);
    Assert::IsTrue(bool(file));

    file.close();
  }

  bool operator==(const BinaryFile& rhs) const
  {
    if (m_size != rhs.m_size)
    {
      return false;
    }

    return arrayEquals(m_data.get(), rhs.m_data.get(), m_size);
  }

  bool operator!=(const BinaryFile& rhs) const
  {
    return !(*this == rhs);
  }
};

}

TEST_CLASS(BytecodeUnitTests)
{
private:

  MinimalChakraRuntime m_chakraRuntime;

  static constexpr const uint64_t bytecodeFileFormatVersionIndex = 0;
  static constexpr const uint64_t bundleVersionIndex =
    sizeof(uint64_t) /* size of bytecodeFileFormatVersion */;
  static constexpr const uint64_t ChakraVersionInfoIndex =
    bundleVersionIndex +
    sizeof(uint64_t) /* size of bundleVersion */;
  static constexpr const uint64_t bytecodeIndex =
    ChakraVersionInfoIndex +
    4 * sizeof(uint32_t) /* size of ChakraVersionInfo */;

  static constexpr const uint64_t scirptVersion = 42;
  static constexpr int testScriptResult = 6;
  static constexpr const char* const testScriptBytecodeFilename = "testScript.bytecode";
  static constexpr const char* const testScript =
    "function getRectangleArea(width, height) {\n"
    "  if (isNaN(width)) {\n"
    "    throw \"Parameter width is not a number.\";\n"
    "  }\n"
    "\n"
    "  if (isNaN(height)) {\n"
    "    throw \"Parameter height is not a number.\";\n"
    "  }\n"
    "\n"
    "  return width * height;\n"
    "}\n"
    "\n"
    "(() =>\n"
    "{\n"
    "  var a = getRectangleArea(3, 4);\n"
    "  var b = getRectangleArea(3, 2);\n"
    "  return a - b;\n"
    "})();\n";

  JsValueRef testScriptFriendlyName()
  {
    constexpr const char* const testScriptFriendlyName = "testScript.js";
    return JsValueRef(ChakraString(testScriptFriendlyName));
  }

  bool isCorrectTestScriptResult(JsValueRef value)
  {
    int result = 0;
    JsErrorCode lastError = JsNumberToInt(value, &result);
    Assert::IsTrue(lastError == JsNoError);
    return result == testScriptResult;
  }

  static void DeleteBytecode()
  {
    if (!DeleteFileA(testScriptBytecodeFilename))
    {
      Assert::IsTrue(GetLastError() == ERROR_FILE_NOT_FOUND);
    }
    Assert::IsTrue(!PathFileExistsA(testScriptBytecodeFilename));
  }

  void ExecuteBytecodeWithFallback()
  {
    JsValueRef result = evaluateScriptWithBytecode(std::make_unique<const JSBigStdString>(testScript),
      scirptVersion, testScriptFriendlyName(), testScriptBytecodeFilename, false /* asyncBytecodeGeneration */);
    Assert::IsTrue(isCorrectTestScriptResult(result));
  }

  void ExecuteBytecodeWithoutFallback()
  {
    // Notice that we pass an empty string as the script to ensure that we are executing bytecode
    // instead of source code.
    JsValueRef result = evaluateScriptWithBytecode(std::make_unique<const JSBigStdString>(""),
      scirptVersion, testScriptFriendlyName(), testScriptBytecodeFilename);
    Assert::IsTrue(isCorrectTestScriptResult(result));
  }

  void GenerateBytecode()
  {
    DeleteBytecode();
    ExecuteBytecodeWithFallback();

    // Verify that bytecode has been generated.
    Assert::IsTrue(PathFileExistsA(testScriptBytecodeFilename));
  }

  void UpdateTestHelper(std::function<void (const BinaryFile&)> outputCorruptBytecode,
    std::function<void (const BinaryFile&, const BinaryFile&)> preUpdateCheck,
    std::function<void (const BinaryFile&, const BinaryFile&)> postUpdateCheck)
  {
    GenerateBytecode();
    BinaryFile correctBytecode(testScriptBytecodeFilename);
    Assert::IsTrue(correctBytecode.m_size >= bytecodeIndex);

    DeleteBytecode();
    outputCorruptBytecode(correctBytecode);

    BinaryFile corruptBytecode(testScriptBytecodeFilename);
    preUpdateCheck(corruptBytecode, correctBytecode);

    // This should update the bytecode file.
    ExecuteBytecodeWithFallback();

    BinaryFile currentBytecode(testScriptBytecodeFilename);
    postUpdateCheck(currentBytecode, correctBytecode);
  }

public:

  BytecodeUnitTests()
    : m_chakraRuntime(true /* multithreaded */)
  {
    InitializeLogging([](RCTLogLevel logLevel, const char* message) {});
  }

  TEST_METHOD(GenerationTest)
  {
    GenerateBytecode();
  }

  TEST_METHOD(ExecutionTest)
  {
    GenerateBytecode();
    ExecuteBytecodeWithoutFallback();
  }

  TEST_METHOD(UpdateTest_WrongBytecodeFileFormatVersion)
  {
    UpdateTestHelper(
      /* outputCorruptBytecode */
      []
        (const BinaryFile& correctBytecode)
      {
        constexpr const uint64_t wrongBytecodeFileFormatVersion = 0;
        std::ofstream bytecodeFile(testScriptBytecodeFilename, std::ios::binary);
        Assert::IsTrue(bool(bytecodeFile));

        bytecodeFile.write((char*)&wrongBytecodeFileFormatVersion, sizeof(wrongBytecodeFileFormatVersion));

        bytecodeFile.write((char*)&correctBytecode.m_data[bundleVersionIndex],
          correctBytecode.m_size - bundleVersionIndex);

        Assert::IsTrue(bool(bytecodeFile));
        bytecodeFile.close();
      },
      /* preUpdateCheck */
      []
        (const BinaryFile& corruptBytecodeFile, const BinaryFile& correctBytecodeFile)
      {
        Assert::IsTrue(corruptBytecodeFile.m_size == correctBytecodeFile.m_size);
        Assert::IsTrue(corruptBytecodeFile != correctBytecodeFile);
        Assert::IsFalse(arrayEquals(
          &corruptBytecodeFile.m_data[bytecodeFileFormatVersionIndex],
          &correctBytecodeFile.m_data[bytecodeFileFormatVersionIndex],
          bundleVersionIndex - bytecodeFileFormatVersionIndex));
        Assert::IsTrue(arrayEquals(
          &corruptBytecodeFile.m_data[bundleVersionIndex],
          &correctBytecodeFile.m_data[bundleVersionIndex],
          corruptBytecodeFile.m_size - bundleVersionIndex));
      },
      /* postUpdateCheck*/
      [this](const BinaryFile& currentBytecodeFile, const BinaryFile& correctBytecodeFile)
      {
        Assert::IsTrue(currentBytecodeFile == correctBytecodeFile);
        ExecuteBytecodeWithoutFallback();
      }
    );
  }

  TEST_METHOD(UpdateTest_WrongBundleVersion)
  {
    UpdateTestHelper(
      /* outputCorruptBytecode */
      [](const BinaryFile& correctBytecode)
      {
        std::ofstream bytecodeFile(testScriptBytecodeFilename, std::ios::binary);
        Assert::IsTrue(bool(bytecodeFile));

        uint64_t wrongBundleVersion = 0;

        bytecodeFile.write((char*)&correctBytecode.m_data[0], bundleVersionIndex);

        bytecodeFile.write((char*)&wrongBundleVersion,
          ChakraVersionInfoIndex - bundleVersionIndex);

        bytecodeFile.write((char*)&correctBytecode.m_data[ChakraVersionInfoIndex],
          correctBytecode.m_size - ChakraVersionInfoIndex);

        Assert::IsTrue(bool(bytecodeFile));
        bytecodeFile.close();
      },
      /* preUpdateCheck */
      [](const BinaryFile& corruptBytecodeFile, const BinaryFile& correctBytecodeFile)
      {
        Assert::IsTrue(corruptBytecodeFile.m_size == correctBytecodeFile.m_size);
        Assert::IsTrue(corruptBytecodeFile != correctBytecodeFile);
        Assert::IsTrue(arrayEquals(
          &corruptBytecodeFile.m_data[bytecodeFileFormatVersionIndex],
          &correctBytecodeFile.m_data[bytecodeFileFormatVersionIndex],
          bundleVersionIndex - bytecodeFileFormatVersionIndex));
        Assert::IsFalse(arrayEquals(
          &corruptBytecodeFile.m_data[bundleVersionIndex],
          &correctBytecodeFile.m_data[bundleVersionIndex],
          ChakraVersionInfoIndex - bundleVersionIndex));
        Assert::IsTrue(arrayEquals(
          &corruptBytecodeFile.m_data[ChakraVersionInfoIndex],
          &correctBytecodeFile.m_data[ChakraVersionInfoIndex],
          corruptBytecodeFile.m_size - ChakraVersionInfoIndex));
      },
      /* postUpdateCheck*/
      [this](const BinaryFile& currentBytecodeFile, const BinaryFile& correctBytecodeFile)
      {
        Assert::IsTrue(currentBytecodeFile == correctBytecodeFile);
        ExecuteBytecodeWithoutFallback();
      }
    );
  }

  TEST_METHOD(UpdateTest_WrongChakraVersionInfo)
  {
    UpdateTestHelper(
      /* outputCorruptBytecode */
      [](const BinaryFile& correctBytecode)
      {
        std::ofstream bytecodeFile(testScriptBytecodeFilename, std::ios::binary);
        Assert::IsTrue(bool(bytecodeFile));

        uint32_t wrongChakraVersionInfo[4] = { 0, 0, 0, 0 };

        bytecodeFile.write((char*)&correctBytecode.m_data[0], ChakraVersionInfoIndex);

        bytecodeFile.write((char*)&wrongChakraVersionInfo[0],
          bytecodeIndex - ChakraVersionInfoIndex);

        bytecodeFile.write((char*)&correctBytecode.m_data[bytecodeIndex],
          correctBytecode.m_size - bytecodeIndex);

        Assert::IsTrue(bool(bytecodeFile));
        bytecodeFile.close();
      },
      /* preUpdateCheck */
      [](const BinaryFile& corruptBytecodeFile, const BinaryFile& correctBytecodeFile)
      {
        Assert::IsTrue(corruptBytecodeFile.m_size == correctBytecodeFile.m_size);
        Assert::IsTrue(corruptBytecodeFile != correctBytecodeFile);
        Assert::IsTrue(arrayEquals(
          &corruptBytecodeFile.m_data[bytecodeFileFormatVersionIndex],
          &correctBytecodeFile.m_data[bytecodeFileFormatVersionIndex],
          ChakraVersionInfoIndex - bytecodeFileFormatVersionIndex));
        Assert::IsFalse(arrayEquals(
          &corruptBytecodeFile.m_data[ChakraVersionInfoIndex],
          &correctBytecodeFile.m_data[ChakraVersionInfoIndex],
          bytecodeIndex - ChakraVersionInfoIndex));
        Assert::IsTrue(arrayEquals(
          &corruptBytecodeFile.m_data[bytecodeIndex],
          &correctBytecodeFile.m_data[bytecodeIndex],
          corruptBytecodeFile.m_size - bytecodeIndex));
      },
      /* postUpdateCheck*/
      [this](const BinaryFile& currentBytecodeFile, const BinaryFile& correctBytecodeFile)
      {
        Assert::IsTrue(currentBytecodeFile == correctBytecodeFile);
        ExecuteBytecodeWithoutFallback();
      }
    );
  }

  TEST_METHOD(UpdateTest_EmptyBytecodeFile)
  {
    UpdateTestHelper(
      /* outputCorruptBytecode */
      [](const BinaryFile& correctBytecode)
      {
        std::ofstream bytecodeFile(testScriptBytecodeFilename);
        bytecodeFile.close();
      },
      /* preUpdateCheck */
      [](const BinaryFile& corruptBytecodeFile, const BinaryFile& correctBytecodeFile)
      {
        Assert::IsTrue(corruptBytecodeFile.m_size == 0);
        Assert::IsTrue(corruptBytecodeFile != correctBytecodeFile);
      },
      /* postUpdateCheck*/
      [this](const BinaryFile& currentBytecodeFile, const BinaryFile& correctBytecodeFile)
      {
        Assert::IsTrue(currentBytecodeFile == correctBytecodeFile);
        ExecuteBytecodeWithoutFallback();
      }
    );
  }

  TEST_METHOD(UpdateTest_BadBytecode)
  {
    UpdateTestHelper(
      /* outputCorruptBytecode */
      [](const BinaryFile& correctBytecode)
      {
        std::ofstream bytecodeFile(testScriptBytecodeFilename, std::ios::binary);
        Assert::IsTrue(bool(bytecodeFile));

        std::vector<uint8_t> badBytecode(correctBytecode.m_size - bytecodeIndex, 0);

        bytecodeFile.write((char*)&correctBytecode.m_data[0], bytecodeIndex);

        bytecodeFile.write((char*)badBytecode.data(),
          correctBytecode.m_size - bytecodeIndex);

        Assert::IsTrue(bool(bytecodeFile));
        bytecodeFile.close();
      },
      /* preUpdateCheck */
      [](const BinaryFile& corruptBytecodeFile, const BinaryFile& correctBytecodeFile)
      {
        Assert::IsTrue(corruptBytecodeFile.m_size == correctBytecodeFile.m_size);
        Assert::IsTrue(corruptBytecodeFile != correctBytecodeFile);
        Assert::IsTrue(arrayEquals(
          &corruptBytecodeFile.m_data[bytecodeFileFormatVersionIndex],
          &correctBytecodeFile.m_data[bytecodeFileFormatVersionIndex],
          bytecodeIndex - bytecodeFileFormatVersionIndex));
        Assert::IsFalse(arrayEquals(
          &corruptBytecodeFile.m_data[bytecodeIndex],
          &correctBytecodeFile.m_data[bytecodeIndex],
          corruptBytecodeFile.m_size - bytecodeIndex));
      },
      /* postUpdateCheck*/
      [this](const BinaryFile& currentBytecodeFile, const BinaryFile& correctBytecodeFile)
      {
        ExecuteBytecodeWithFallback();
        // We don't do anything here because we currently do not
        // regenerate bytecode when the bytecode itself (not its prefix)
        // is corrupt. See comments in evaluateScriptWithBytecode for
        // more details as for why.
      }
    );
  }
};
