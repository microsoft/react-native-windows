// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"
#include "MemoryMappedBuffer.h"

#include <werapi.h>
#include <windows.h>

namespace {

class MemoryMappedBuffer : public facebook::jsi::Buffer {
 public:
  MemoryMappedBuffer(const wchar_t *const filename, uint32_t offset);

  size_t size() const override;
  const uint8_t *data() const override;

 private:
  static void FileDataDeleter(void *p) {
    WerUnregisterMemoryBlock(p);
    UnmapViewOfFile(p);
  }

  std::unique_ptr<void, decltype(&CloseHandle)> m_fileMapping;
  std::unique_ptr<void, decltype(&FileDataDeleter)> m_fileData;
  uint32_t m_fileSize = 0;
  uint32_t m_offset = 0;
};

MemoryMappedBuffer::MemoryMappedBuffer(const wchar_t *const filename, uint32_t offset)
    : m_fileMapping{nullptr, &CloseHandle}, m_fileData{nullptr, &FileDataDeleter}, m_offset{offset} {
  if (!filename) {
    throw facebook::jsi::JSINativeException("MemoryMappedBuffer constructor is called with nullptr filename.");
  }

  std::unique_ptr<void, decltype(&CloseHandle)> fileHandle{
      CreateFile2(filename, GENERIC_READ, FILE_SHARE_READ, OPEN_EXISTING, nullptr /* pCreateExParams */), &CloseHandle};

  if (fileHandle.get() == INVALID_HANDLE_VALUE) {
    throw facebook::jsi::JSINativeException(
        "CreateFileW/CreateFile2 failed with last error " + std::to_string(GetLastError()));
  }

  LARGE_INTEGER fileSize;
  if (!GetFileSizeEx(fileHandle.get(), &fileSize)) {
    throw facebook::jsi::JSINativeException("GetFileSizeEx failed with last error " + std::to_string(GetLastError()));
  }

  if (fileSize.LowPart == 0) {
    throw facebook::jsi::JSINativeException("Cannot memory map an empty file.");
  }

  if (fileSize.HighPart != 0) {
    throw facebook::jsi::JSINativeException(
        "MemoryMappedBuffer only supports files whose size can fit within an "
        "uint32_t.");
  }

  m_fileSize = fileSize.LowPart;
  if (m_offset > m_fileSize) {
    throw facebook::jsi::JSINativeException("Invalid offset.");
  }

  m_fileMapping.reset(CreateFileMappingFromApp(
      fileHandle.get(), nullptr /* SecurityAttributes */, PAGE_READONLY, m_fileSize, nullptr /* Name */));

  if (!m_fileMapping) {
    throw facebook::jsi::JSINativeException(
        "CreateFileMapping/CreateFileMappingFromApp failed with last error " + std::to_string(GetLastError()));
  }

  m_fileData.reset(
      MapViewOfFileFromApp(m_fileMapping.get(), FILE_MAP_READ, 0 /* FileOffset */, 0 /* NumberOfBytesToMap */));

  if (!m_fileData) {
    throw facebook::jsi::JSINativeException(
        "MapViewOfFile/MapViewOfFileFromApp failed with last error " + std::to_string(GetLastError()));
  }

  WerRegisterMemoryBlock(m_fileData.get(), m_fileSize);
}

size_t MemoryMappedBuffer::size() const {
  return m_fileSize - m_offset;
}

const uint8_t *MemoryMappedBuffer::data() const {
  return static_cast<const uint8_t *>(m_fileData.get()) + m_offset;
}

} // anonymous namespace

namespace Microsoft::JSI {

std::unique_ptr<facebook::jsi::Buffer> MakeMemoryMappedBuffer(const wchar_t *const filename, uint32_t offset) {
  return std::make_unique<MemoryMappedBuffer>(filename, offset);
}

} // namespace Microsoft::JSI
