// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include <cxxreact/JSBigString.h>
#include "ChakraValue.h"
#include <cxxreact/JSExecutor.h>
#include <folly/Memory.h>
#include "Utf8DebugExtensions.h"
#include <memory>
#include <string>
#include <functional>
#include <werapi.h>

namespace facebook {
namespace react {

ChakraString jsStringFromBigString(const JSBigString& bigstr);

using JsValueRefUniquePtr = std::unique_ptr<void, std::function<void(JsValueRef)>>;
JsValueRefUniquePtr MakeJsValueRefUniquePtr(JsValueRef value);
JsValueRefUniquePtr jsArrayBufferFromBigString(std::unique_ptr<const JSBigString>&& bigstr);
JsValueRefUniquePtr jsArrayBufferFromBigString(const std::shared_ptr<const JSBigString>& bigstr);

// We only support files whose size can fit within an uint32_t. Memory
// mapping an empty or a larger file fails. If the file contents are not
// null-terminated, file is copied into a dynamically  allocated,
// page-file backed,  null-terminated buffer
class FileMappingBigString : public JSBigString
{
public:
  FileMappingBigString(const std::string& filenameUtf8, uint32_t offset = 0);

  bool isAscii() const override {
    return false;
  }

  const char* c_str() const override
  {
    return m_data.get();
  }

  size_t size() const override
  {
    return m_size;
  }

  // Returns nullptr when memory mapping failed
  void* file_data() const
  {
    return m_fileData.get();
  }

  // Returns zero when memory mapping failed
  uint32_t file_size() const
  {
    return m_fileSize;
  }

  static std::unique_ptr<const FileMappingBigString>
    fromPath(const std::string& filenameUtf8);

private:

  static void fileDataDeleter(void* p)
  {
    WerUnregisterMemoryBlock(p);
    UnmapViewOfFile(p);
  }

  std::unique_ptr<void, decltype(&CloseHandle)> m_fileMapping;
  std::unique_ptr<void, decltype(&fileDataDeleter)> m_fileData;
  std::unique_ptr<char, void (*)(void*)> m_data;
  uint32_t m_fileSize;
  uint32_t m_size;
};

} // namespace react
} // namespace facebook
