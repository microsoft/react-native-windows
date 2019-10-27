// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"

#include "ChakraUtils.h"
#include "Unicode.h"

#include <memoryapi.h>
#include <werapi.h>
#include <windows.h>
#include <cassert>
#include <cstdint>

namespace {

uint32_t getPageSize() noexcept {
  SYSTEM_INFO systemInfo;
  GetSystemInfo(&systemInfo);
  return systemInfo.dwPageSize;
}

} // namespace

namespace facebook {
namespace react {

JsValueRefUniquePtr MakeJsValueRefUniquePtr(JsValueRef value) {
  JsAddRef(value, nullptr);
  return JsValueRefUniquePtr(value, [](JsValueRef value) { JsRelease(value, nullptr); });
}

ChakraString jsStringFromBigString(const JSBigString &bigstr) {
  if (bigstr.isAscii()) {
    return ChakraString::createExpectingAscii(bigstr.c_str(), bigstr.size());
  } else {
    return ChakraString(bigstr.c_str());
  }
}

JsValueRefUniquePtr jsArrayBufferFromBigString(std::unique_ptr<const JSBigString> &&bigstr) {
  JsValueRef arrayBuffer = nullptr;
  auto size = bigstr->size();
  assert(size < UINT_MAX);
  JsErrorCode result = JsCreateExternalArrayBuffer(
      const_cast<char *>(bigstr->c_str()),
      static_cast<unsigned int>(size),
      [](void *bigstrToDestroy) {
        // Wrap bigstrToDestroy in unique_ptr to avoid calling delete explicitly
        std::unique_ptr<JSBigString> wrapper{static_cast<JSBigString *>(bigstrToDestroy)};
      },
      const_cast<JSBigString *>(bigstr.get()),
      &arrayBuffer);

  if (result == JsNoError) {
    bigstr.release();
  } else {
    assert(false && "JsCreateExternalArrayBuffer() failed.");
    std::terminate();
  }

  return MakeJsValueRefUniquePtr(arrayBuffer);
}

JsValueRefUniquePtr jsArrayBufferFromBigString(const std::shared_ptr<const JSBigString> &bigstr) {
  JsValueRef arrayBuffer = nullptr;
  auto size = bigstr->size();
  assert(size < UINT_MAX);
  auto bigstrRef = std::make_unique<std::shared_ptr<const JSBigString>>(bigstr);
  JsErrorCode result = JsCreateExternalArrayBuffer(
      const_cast<char *>(bigstr->c_str()),
      static_cast<unsigned int>(size),
      [](void *bigstrToDestroy) {
        // Wrap bigstrToDestroy in unique_ptr to avoid calling delete explicitly
        std::unique_ptr<std::shared_ptr<const JSBigString>> wrapper{
            static_cast<std::shared_ptr<const JSBigString> *>(bigstrToDestroy)};
      },
      bigstrRef.get(),
      &arrayBuffer);

  if (result == JsNoError) {
    bigstrRef.release();
  } else {
    assert(false && "JsCreateExternalArrayBuffer() failed.");
    std::terminate();
  }

  return MakeJsValueRefUniquePtr(arrayBuffer);
}

FileMappingBigString::FileMappingBigString(const std::string &filenameUtf8, uint32_t offset)
    : JSBigString{},
      m_fileMapping{nullptr, &CloseHandle},
      m_fileData{nullptr, &fileDataDeleter},
      m_data{nullptr, nullptr},
      m_size{0},
      m_fileSize{0} {
#if (defined(WINRT))
  std::unique_ptr<void, decltype(&CloseHandle)> fileHandle{
      CreateFile2(
          Microsoft::Common::Unicode::Utf8ToUtf16(filenameUtf8).c_str(),
          GENERIC_READ,
          FILE_SHARE_READ,
          OPEN_EXISTING,
          nullptr /* pCreateExParams */),
      &CloseHandle};
#else
  std::unique_ptr<void, decltype(&CloseHandle)> fileHandle{
      CreateFileW(
          Microsoft::Common::Unicode::Utf8ToUtf16(filenameUtf8).c_str(),
          GENERIC_READ,
          FILE_SHARE_READ,
          nullptr /* lpSecurityAttributes */,
          OPEN_EXISTING,
          FILE_ATTRIBUTE_NORMAL,
          nullptr /* hTemplateFile */),
      &CloseHandle};
#endif

  if (fileHandle.get() == INVALID_HANDLE_VALUE) {
    return;
  }

  LARGE_INTEGER size;
  if (!GetFileSizeEx(fileHandle.get(), &size)) {
    return;
  }

  if (size.LowPart == 0 || size.HighPart != 0) {
    return;
  }

  m_fileSize = size.LowPart;
  if (offset > m_fileSize) {
    offset = m_fileSize;
  }
  m_size = m_fileSize - offset;

#if (defined(WINRT))
  m_fileMapping.reset(CreateFileMappingFromApp(
      fileHandle.get(), nullptr /* SecurityAttributes */, PAGE_READONLY, m_fileSize, nullptr /* Name */));
#else
  m_fileMapping.reset(CreateFileMapping(
      fileHandle.get(),
      nullptr /* lpAttributes */,
      PAGE_READONLY,
      0 /* dwMaximumSizeHigh */,
      m_fileSize,
      nullptr /* lpName */));
#endif

  if (!m_fileMapping) {
    m_size = 0;
    return;
  }

#if (defined(WINRT))
  m_fileData.reset(
      MapViewOfFileFromApp(m_fileMapping.get(), FILE_MAP_READ, 0 /* FileOffset */, 0 /* NumberOfBytesToMap */));
#else
  m_fileData.reset(MapViewOfFile(
      m_fileMapping.get(),
      FILE_MAP_READ,
      0 /* dwFileOffsetHigh */,
      0 /* dwFileOffsetLow */,
      0 /* dwNumberOfBytesToMap */));
#endif

  if (!m_fileData) {
    m_size = 0;
    return;
  }

  WerRegisterMemoryBlock(m_fileData.get(), m_fileSize);

  static const uint32_t s_pageSize = getPageSize();
  if (m_fileSize % s_pageSize != 0) {
    // Data are owned by m_fileData, deleter is no-op
    m_data = decltype(m_data){static_cast<char *>(m_fileData.get()) + offset, [](void *) {}};
  } else {
    // Ensure m_data is null-terminated
    m_data = decltype(m_data){new char[m_size + 1],
                              [](void *pv) // Can't just say &opreator delete[] because of calling
                                           // convention mismatches
                              { delete[] static_cast<char *>(pv); }};
    memcpy(m_data.get(), static_cast<char *>(m_fileData.get()) + offset, m_size);
    m_data.get()[m_size] = '\0';
  }
}

std::unique_ptr<const FileMappingBigString> FileMappingBigString::fromPath(const std::string &filenameUtf8) {
  return std::make_unique<FileMappingBigString>(filenameUtf8);
}

} // namespace react
} // namespace facebook
