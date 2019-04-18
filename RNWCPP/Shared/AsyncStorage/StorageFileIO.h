// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include <string>
#include <memory>
#include <vector>
#include <sstream>
#include <functional>
#include <Windows.h>

namespace facebook {
  namespace react {
    class StorageFileIO
    {
    public:
      StorageFileIO(const WCHAR* storageFileName);
      virtual ~StorageFileIO();

      void clear();
      void append(const std::string& fileContent);
      void resetLine();
      bool getLine(std::string& line);

      static void throwLastErrorMessage();

    private:
      HANDLE m_storageFileHandle;
      std::unique_ptr<FILE, std::function<void(FILE *)>> m_storageFile;

    private:
      static const size_t IOHelperBufferSize = 1024;
      char m_fileBuffer[IOHelperBufferSize + 1] = { 0 };
      size_t m_fileBufferIdx = 0;
      size_t m_fileBufferSize = IOHelperBufferSize;
      bool m_fileBufferInited = false;
    };
  }
}
