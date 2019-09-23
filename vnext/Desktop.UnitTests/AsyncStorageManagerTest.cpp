// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include <cstdlib>
#include <ctime>
#include <iostream>

#include <CppUnitTest.h>
#include "AsyncStorageTestClass.h"

#include <AsyncStorage/AsyncStorageManager.h>
#include <AsyncStorage/FollyDynamicConverter.h>

using namespace facebook::react;
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace {

std::vector<folly::dynamic> returnedValues;
std::condition_variable_any cv;
std::recursive_mutex m;

folly::dynamic dynamicNULL;
vector<string> emptyStringVector = {};

static const string SAMPLE_KEY_1 =
    "21QQYrTfnTS2kKSb47H3mJs0NxfyMBYIkX1pTCuDeVHYqLhQhUo8OObPz8M8tZy2kJrFRy94JQuWH5yUivxkBsXAblv0TqvNwLOpq1Rgx5l6LNc8AMByHXk814GmcoKr";
static const string SAMPLE_VAL_1 =
    "JRGlj6sMXY2m0ctODf4h7puN0vCzjgPPLWgAH1H5159JABgywXIgCGcGc8xOwI0bYWbmOct12MsWinSC7eirmS255oZJxM2rDN2FaRXE0UAaIGxlAyRaeMyAEtybiUUW";
static const string SAMPLE_KEY_2 =
    "ZR6ztxV2QksUIS1HHSQFL8uTXy2Tb23Vq3uuJlpwGAFtCPkVnMC7zf87eZSzLJFwNa4WKGZ6gkpBPPqsVUFJlVHwEAMMiYUUqb5JYi4tTsOgfV2q4lLC0j9QeDwTCN7Y";
static const string SAMPLE_VAL_2 =
    "5Hwi8pRDPSotVgZunHlAA1IYg4sqa5DjiI8BkqjNVCymUnR6gXBgv3K65xHe4Wy3sEG8UhMMexLAJSlXCwDRzhEk7bgiqoaIJ04omWN0eriXP217sFHcFgSTQQjDCqMe";

// Callback method that stores its results on the global variable.
// Used for async operations. Call cv.wait(m) after executing an
// async operation and then check the returnedValues.
//
// Do NOT call cv.wait(m) for synchronous operations. The
// condition will never be signalled.
void storeCallbackArgAndNotify(vector<folly::dynamic> args) {
  std::lock_guard<std::recursive_mutex> lock(m);
  returnedValues = args;
  cv.notify_one();
}

void throwLastErrorMessage() {
  char errorMessageBuffer[1025] = {0};
  FormatMessageA(
      FORMAT_MESSAGE_FROM_SYSTEM,
      nullptr,
      GetLastError(),
      MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
      errorMessageBuffer,
      1024,
      nullptr);
  throw std::exception(errorMessageBuffer);
}

} // namespace

namespace Microsoft::React::Test {

TEST_CLASS(AsyncStorageManagerTest) {
  const WCHAR *m_storageFileName = L"testdomain";

  inline void cleanupStorageFiles() {
    WCHAR wzMyAppDataDirPathArr[MAX_PATH];
    HRESULT hr = SHGetFolderPathW(
        nullptr,
        CSIDL_LOCAL_APPDATA,
        nullptr,
        SHGFP_TYPE_CURRENT,
        wzMyAppDataDirPathArr);
    if (hr != S_OK)
      throwLastErrorMessage();

    const std::wstring strLocalAppDataPath{wzMyAppDataDirPathArr};
    const std::wstring strMicrosoftFullPath =
        strLocalAppDataPath + L"\\Microsoft";
    const std::wstring strOfficeFullPath = strMicrosoftFullPath + L"\\Office";
    const std::wstring strStorageFolderFullPath =
        strOfficeFullPath + L"\\SDXStorage";
    const std::wstring strStorageFileExtension = L".txt";
    const std::wstring strStorageFileFullPath = strStorageFolderFullPath +
        L"\\" + m_storageFileName + strStorageFileExtension;
    // full path should be like -
    // C:\Users\<username>\AppData\Local\Microsoft\Office\SDXStorage\ReactNativeAsyncStorage.txt

    DeleteFileW(strStorageFileFullPath.c_str());
  }

  TEST_METHOD_INITIALIZE(Setup) {
    cleanupStorageFiles();
  }

  TEST_METHOD_CLEANUP(TearDown) {
    cleanupStorageFiles();
  }

  TEST_METHOD(AsyncStorageManagerTest_SimpleOperations) {
    AsyncStorageManager kvManager(this->m_storageFileName);
    std::function<void(vector<folly::dynamic>)> callback =
        storeCallbackArgAndNotify;

    // Empty the storage.
    std::unique_lock<std::recursive_mutex> lock(m);
    kvManager.executeKVOperation(
        AsyncStorageManager::AsyncStorageOperation::clear,
        FollyDynamicConverter::stringVectorAsRetVal(emptyStringVector),
        callback);
    cv.wait(lock);

    Assert::IsTrue(returnedValues[0] == dynamicNULL);

    // Set two KV pairs. Note that it waits until the condition variable is
    // signalled from the Callback function.
    vector<tuple<string, string>> setArgs = {
        make_tuple(SAMPLE_KEY_1, SAMPLE_VAL_1),
        make_tuple(SAMPLE_KEY_2, SAMPLE_VAL_2)};
    folly::dynamic jsSetArgs = folly::dynamic::array;
    jsSetArgs.push_back(
        FollyDynamicConverter::tupleStringVectorAsRetVal(setArgs));
    kvManager.executeKVOperation(
        AsyncStorageManager::AsyncStorageOperation::multiSet,
        jsSetArgs,
        callback);
    cv.wait(lock);

    // Check if any error was returned.
    Assert::IsTrue(returnedValues[0] == dynamicNULL);

    // Verify that the keys were stored.
    vector<string> getArgs = {SAMPLE_KEY_1, SAMPLE_KEY_2};
    folly::dynamic jsGetArgs = folly::dynamic::array;
    jsGetArgs.push_back(FollyDynamicConverter::stringVectorAsRetVal(getArgs));
    kvManager.executeKVOperation(
        AsyncStorageManager::AsyncStorageOperation::multiGet,
        jsGetArgs,
        callback);

    folly::dynamic jsReturnValues = folly::dynamic::array;
    jsReturnValues.push_back(returnedValues[1]);
    vector<tuple<string, string>> retTupleVals =
        FollyDynamicConverter::jsArgAsTupleStringVector(jsReturnValues);
    Assert::IsTrue(returnedValues[0] == dynamicNULL);
    Assert::IsTrue(retTupleVals == setArgs);

    // Verify GetAllKeys.
    kvManager.executeKVOperation(
        AsyncStorageManager::AsyncStorageOperation::getAllKeys,
        FollyDynamicConverter::stringVectorAsRetVal(emptyStringVector),
        callback);

    folly::dynamic jsAllKeys = folly::dynamic::array;
    jsAllKeys.push_back(returnedValues[1]);
    vector<string> retVals =
        FollyDynamicConverter::jsArgAsStringVector(jsAllKeys);
    Assert::IsTrue(returnedValues[0] == dynamicNULL);
    Assert::IsTrue(retVals == getArgs);

    // Empty the storage.
    kvManager.executeKVOperation(
        AsyncStorageManager::AsyncStorageOperation::clear,
        FollyDynamicConverter::stringVectorAsRetVal(emptyStringVector),
        callback);
    cv.wait(lock);

    Assert::IsTrue(returnedValues[0] == dynamicNULL);

    lock.unlock();
  }

  TEST_METHOD(AsyncStorageManagerTest_QueueHeavyLoad) {
    AsyncStorageManager kvManager(this->m_storageFileName);
    std::function<void(vector<folly::dynamic>)> callback =
        storeCallbackArgAndNotify;

    // Clear the storage.
    std::unique_lock<std::recursive_mutex> lock(m);
    kvManager.executeKVOperation(
        AsyncStorageManager::AsyncStorageOperation::clear,
        FollyDynamicConverter::stringVectorAsRetVal(emptyStringVector),
        callback);
    cv.wait(lock);

    Assert::IsTrue(returnedValues[0] == dynamicNULL);

    callback = [](vector<folly::dynamic> args) {};
    int numBlocks = 8;
    int numOperations = 1024;

    for (int i = 0; i < numBlocks; i++) {
      vector<tuple<string, string>> setArgs;
      for (int j = 0; j < numOperations; j++) {
        std::string postFix(std::to_string(i * numOperations + j));
        std::string key = SAMPLE_KEY_1 + postFix;
        std::string val = SAMPLE_VAL_1 + postFix;
        setArgs.push_back(make_tuple(key, val));
      }
      folly::dynamic jsSetArgs = folly::dynamic::array;
      jsSetArgs.push_back(
          FollyDynamicConverter::tupleStringVectorAsRetVal(setArgs));
      kvManager.executeKVOperation(
          AsyncStorageManager::AsyncStorageOperation::multiSet,
          jsSetArgs,
          callback);
    }

    callback = storeCallbackArgAndNotify;

    // Do one last set with the notify callback so we can wait until all sets
    // have been executed.
    vector<tuple<string, string>> setArgs = {
        make_tuple(SAMPLE_KEY_2, SAMPLE_VAL_2)};
    folly::dynamic jsLastSetArg = folly::dynamic::array;
    jsLastSetArg.push_back(
        FollyDynamicConverter::tupleStringVectorAsRetVal(setArgs));
    kvManager.executeKVOperation(
        AsyncStorageManager::AsyncStorageOperation::multiSet,
        jsLastSetArg,
        callback);
    cv.wait(lock);

    Assert::IsTrue(returnedValues[0] == dynamicNULL);

    // Verify all keys were written.
    kvManager.executeKVOperation(
        AsyncStorageManager::AsyncStorageOperation::getAllKeys,
        FollyDynamicConverter::stringVectorAsRetVal(emptyStringVector),
        callback);
    folly::dynamic jsRetValues = folly::dynamic::array;
    jsRetValues.push_back(returnedValues[1]);
    size_t returnedSize =
        FollyDynamicConverter::jsArgAsStringVector(jsRetValues).size();
    Assert::IsTrue(returnedValues[0] == dynamicNULL);
    Assert::AreEqual(
        returnedSize, static_cast<size_t>(numOperations * numBlocks + 1));

    // Verify random key-value pairing.
    std::srand(static_cast<unsigned int>(std::time(0)));
    int randomInt = (std::rand() % numOperations);

    std::string key = SAMPLE_KEY_1 + std::to_string(randomInt);
    std::string val = SAMPLE_VAL_1 + std::to_string(randomInt);

    vector<string> getArgs = {key};
    folly::dynamic jsGetArgs = folly::dynamic::array;
    jsGetArgs.push_back(FollyDynamicConverter::stringVectorAsRetVal(getArgs));
    kvManager.executeKVOperation(
        AsyncStorageManager::AsyncStorageOperation::multiGet,
        jsGetArgs,
        callback);

    vector<tuple<string, string>> expectedTupleVals = {make_tuple(key, val)};
    folly::dynamic jsRetTupleValues = folly::dynamic::array;
    jsRetTupleValues.push_back(returnedValues[1]);
    vector<tuple<string, string>> retTupleVals =
        FollyDynamicConverter::jsArgAsTupleStringVector(jsRetTupleValues);
    Assert::IsTrue(returnedValues[0] == dynamicNULL);
    Assert::IsTrue(retTupleVals == expectedTupleVals);

    // Clear the storage.
    kvManager.executeKVOperation(
        AsyncStorageManager::AsyncStorageOperation::clear,
        FollyDynamicConverter::stringVectorAsRetVal(emptyStringVector),
        callback);
    cv.wait(lock);

    Assert::IsTrue(returnedValues[0] == dynamicNULL);

    lock.unlock();
  }
};

} // namespace Microsoft::React::Test
