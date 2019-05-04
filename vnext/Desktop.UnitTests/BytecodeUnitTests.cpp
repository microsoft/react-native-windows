// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "../Chakra/ChakraHelpers.h"
#include "../Chakra/ChakraValue.h"
#include "cxxreact/JSBigString.h"
#include "Logging.h"
#include <Windows.h>
#include <shlwapi.h>
#include <CppUnitTest.h>
#include <algorithm>
#include <functional>
#include <iterator>
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

std::vector<char> ReadFile(const char* filename)
{
  std::ifstream file(filename, std::ios::binary); // | std::ios::ate);
  Assert::IsTrue(bool(file));
  file.seekg(0);

  return std::vector<char>(
    std::istreambuf_iterator<char>(file),
    std::istreambuf_iterator<char>());
}

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

  template<class Fn1, class Fn2, class Fn3>
  void UpdateTestHelper(
    Fn1 outputCorruptBytecode,
    Fn2 preUpdateCheck,
    Fn3 postUpdateCheck)
  {
    GenerateBytecode();
    auto correctBytecode = ReadFile(testScriptBytecodeFilename);
    Assert::IsTrue(correctBytecode.size() >= bytecodeIndex);

    DeleteBytecode();
    outputCorruptBytecode(correctBytecode);

    auto corruptBytecode = ReadFile(testScriptBytecodeFilename);
    preUpdateCheck(corruptBytecode, correctBytecode);

    // This should update the bytecode file.
    ExecuteBytecodeWithFallback();

    auto currentBytecode = ReadFile(testScriptBytecodeFilename);
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
      [](const std::vector<char>& correctBytecode)
      {
        const uint64_t wrongBytecodeFileFormatVersion = 0;
        std::ofstream bytecodeFile(testScriptBytecodeFilename, std::ios::binary);
        Assert::IsTrue(bool(bytecodeFile));

        bytecodeFile.write(reinterpret_cast<const char*>(&wrongBytecodeFileFormatVersion), sizeof(wrongBytecodeFileFormatVersion));

        bytecodeFile.write(&correctBytecode[bundleVersionIndex],
          correctBytecode.size() - bundleVersionIndex);

        Assert::IsTrue(bool(bytecodeFile));
        bytecodeFile.close();
      },
      /* preUpdateCheck */
      [](const std::vector<char>& corruptBytecodeFile, const std::vector<char>& correctBytecodeFile)
      {
        Assert::IsTrue(corruptBytecodeFile.size() == correctBytecodeFile.size());
        Assert::IsTrue(corruptBytecodeFile != correctBytecodeFile);
        Assert::IsFalse(std::equal(
          correctBytecodeFile.begin() + bytecodeFileFormatVersionIndex,
          correctBytecodeFile.begin() + bundleVersionIndex,
          corruptBytecodeFile.begin() + bytecodeFileFormatVersionIndex));
        Assert::IsTrue(std::equal(
          correctBytecodeFile.begin() + bundleVersionIndex,
          correctBytecodeFile.end(),
          corruptBytecodeFile.begin() + bundleVersionIndex));
      },
      /* postUpdateCheck*/
      [this](const std::vector<char>& currentBytecodeFile, const std::vector<char>& correctBytecodeFile)
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
      [](const std::vector<char>& correctBytecode)
      {
        std::ofstream bytecodeFile(testScriptBytecodeFilename, std::ios::binary);
        Assert::IsTrue(bool(bytecodeFile));

        const uint64_t wrongBundleVersion = 0;

        bytecodeFile.write(&correctBytecode[0], bundleVersionIndex);

        bytecodeFile.write(reinterpret_cast<const char*>(&wrongBundleVersion),
          ChakraVersionInfoIndex - bundleVersionIndex);

        bytecodeFile.write(&correctBytecode[ChakraVersionInfoIndex],
          correctBytecode.size() - ChakraVersionInfoIndex);

        Assert::IsTrue(bool(bytecodeFile));
        bytecodeFile.close();
      },
      /* preUpdateCheck */
      [](const std::vector<char>& corruptBytecodeFile, const std::vector<char>& correctBytecodeFile)
      {
        Assert::IsTrue(corruptBytecodeFile.size() == correctBytecodeFile.size());
        Assert::IsTrue(corruptBytecodeFile != correctBytecodeFile);
        Assert::IsTrue(std::equal(
          correctBytecodeFile.begin() + bytecodeFileFormatVersionIndex,
          correctBytecodeFile.begin() + bundleVersionIndex,
          corruptBytecodeFile.begin() + bytecodeFileFormatVersionIndex));
        Assert::IsFalse(std::equal(
          correctBytecodeFile.begin() + bundleVersionIndex,
          correctBytecodeFile.begin() + ChakraVersionInfoIndex,
          corruptBytecodeFile.begin() + bundleVersionIndex));
        Assert::IsTrue(std::equal(
          corruptBytecodeFile.begin() + ChakraVersionInfoIndex,
          corruptBytecodeFile.end(),
          correctBytecodeFile.begin() + ChakraVersionInfoIndex));
      },
      /* postUpdateCheck*/
      [this](const std::vector<char>& currentBytecodeFile, const std::vector<char>& correctBytecodeFile)
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
      [](const std::vector<char>& correctBytecode)
      {
        std::ofstream bytecodeFile(testScriptBytecodeFilename, std::ios::binary);
        Assert::IsTrue(bool(bytecodeFile));

        uint32_t wrongChakraVersionInfo[4] = { 0, 0, 0, 0 };

        bytecodeFile.write(&correctBytecode[0], ChakraVersionInfoIndex);

        bytecodeFile.write(reinterpret_cast<char*>(&wrongChakraVersionInfo[0]),
          bytecodeIndex - ChakraVersionInfoIndex);

        bytecodeFile.write(&correctBytecode[bytecodeIndex],
          correctBytecode.size() - bytecodeIndex);

        Assert::IsTrue(bool(bytecodeFile));
        bytecodeFile.close();
      },
      /* preUpdateCheck */
      [](const std::vector<char>& corruptBytecodeFile, const std::vector<char>& correctBytecodeFile)
      {
        Assert::IsTrue(corruptBytecodeFile.size() == correctBytecodeFile.size());
        Assert::IsTrue(corruptBytecodeFile != correctBytecodeFile);
        Assert::IsTrue(std::equal(
          correctBytecodeFile.begin() + bytecodeFileFormatVersionIndex,
          correctBytecodeFile.begin() + ChakraVersionInfoIndex,
          corruptBytecodeFile.begin() + bytecodeFileFormatVersionIndex));
        Assert::IsFalse(std::equal(
          correctBytecodeFile.begin() + ChakraVersionInfoIndex,
          correctBytecodeFile.begin() + bytecodeIndex,
          corruptBytecodeFile.begin() + ChakraVersionInfoIndex));
        Assert::IsTrue(std::equal(
          corruptBytecodeFile.begin() + bytecodeIndex,
          corruptBytecodeFile.end(),
          correctBytecodeFile.begin() + bytecodeIndex));
      },
      /* postUpdateCheck*/
      [this](const std::vector<char>& currentBytecodeFile, const std::vector<char>& correctBytecodeFile)
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
      [](const std::vector<char>& correctBytecode)
      {
        std::ofstream bytecodeFile(testScriptBytecodeFilename);
        bytecodeFile.close();
      },
      /* preUpdateCheck */
      [](const std::vector<char>& corruptBytecodeFile, const std::vector<char>& correctBytecodeFile)
      {
        Assert::IsTrue(corruptBytecodeFile.size() == 0);
        Assert::IsTrue(corruptBytecodeFile != correctBytecodeFile);
      },
      /* postUpdateCheck*/
      [this](const std::vector<char>& currentBytecodeFile, const std::vector<char>& correctBytecodeFile)
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
      [](const std::vector<char>& correctBytecode)
      {
        std::ofstream bytecodeFile(testScriptBytecodeFilename, std::ios::binary);
        Assert::IsTrue(bool(bytecodeFile));

        std::vector<char> badBytecode(correctBytecode.size() - bytecodeIndex, 0);

        bytecodeFile.write((char*)&correctBytecode[0], bytecodeIndex);

        bytecodeFile.write((char*)badBytecode.data(),
          correctBytecode.size() - bytecodeIndex);

        Assert::IsTrue(bool(bytecodeFile));
        bytecodeFile.close();
      },
      /* preUpdateCheck */
      [](const std::vector<char>& corruptBytecodeFile, const std::vector<char>& correctBytecodeFile)
      {
        Assert::IsTrue(corruptBytecodeFile.size() == correctBytecodeFile.size());
        Assert::IsTrue(corruptBytecodeFile != correctBytecodeFile);
        Assert::IsTrue(std::equal(
          correctBytecodeFile.begin() + bytecodeFileFormatVersionIndex,
          correctBytecodeFile.begin() + bytecodeIndex,
          corruptBytecodeFile.begin() + bytecodeFileFormatVersionIndex));
        Assert::IsFalse(std::equal(
          corruptBytecodeFile.begin() + bytecodeIndex,
          corruptBytecodeFile.end(),
          correctBytecodeFile.begin() + bytecodeIndex));
      },
      /* postUpdateCheck*/
      [this](const std::vector<char>& currentBytecodeFile, const std::vector<char>& correctBytecodeFile)
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
