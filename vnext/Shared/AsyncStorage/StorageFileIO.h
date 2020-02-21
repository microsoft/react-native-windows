// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include <Windows.h>
#include <functional>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

namespace facebook {
namespace react {
class StorageFileIO {
 public:
  StorageFileIO(const WCHAR *storageFileName);
  virtual ~StorageFileIO();

  void clear();
  void append(const std::string &fileContent);
  void resetLine();
  bool getLine(std::string &line);
  void flush();

  static void throwLastErrorMessage();

 private:
  HANDLE m_storageFileHandle;
  std::unique_ptr<FILE, std::function<void(FILE *)>> m_storageFile;

 private:
  static const size_t IOHelperBufferSize = 1024;
  char m_fileBuffer[IOHelperBufferSize + 1] = {0};
  size_t m_fileBufferIdx = 0;
  size_t m_fileBufferSize = IOHelperBufferSize;
  bool m_fileBufferInited = false;
};
} // namespace react
} // namespace facebook
