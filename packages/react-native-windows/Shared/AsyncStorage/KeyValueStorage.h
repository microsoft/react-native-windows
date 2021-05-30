// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include <future>
#include <map>
#include <memory>
#include <vector>

#include <AsyncStorage/StorageFileIO.h>

namespace facebook {
namespace react {
class KeyValueStorage {
 public:
  KeyValueStorage(const WCHAR *storageFileName);

  std::vector<std::tuple<std::string, std::string>> multiGet(const std::vector<std::string> &keys);
  void multiSet(const std::vector<std::tuple<std::string, std::string>> &keyValuePairs);
  void multiRemove(const std::vector<std::string> &keys);
  void multiMerge(const std::vector<std::tuple<std::string, std::string>> &keyValuePairs);
  void clear();
  std::vector<std::string> getAllKeys();

 private:
  static const uint32_t EstimatedKeySize = 100; // in chars
  static const uint32_t EstimatedValueSize = 200;
  static const char KeyPrefix = '$';
  static const char ValuePrefix = '%';
  static const char RemovePrefix = 'R'; // Keep RemovePrefix to be backward compatible for the storage file format

 private:
  std::map<std::string, std::string> m_kvMap;
  std::unique_ptr<StorageFileIO> m_fileIOHelper;
  HANDLE m_storageFileLoaded;
  std::future<void> m_storageFileLoader;

 private:
  static void escapeString(std::string &unescapedString);
  static void unescapeString(std::string &escapedString);

 private:
  void load();
  void waitForStorageLoadComplete();
  void setStorageLoadedEvent();
  void saveTable();
};
} // namespace react
} // namespace facebook
