// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include "ChakraBytecodeStore.h"
#include "ChakraHelpers.h"
#include "ChakraUtils.h"
#include "Unicode.h"

#pragma warning(push)
#pragma warning(disable : 4244)
#pragma warning(disable : 4245)
#include <boost/crc.hpp>
#pragma warning(pop)

#include <cstdio>

namespace facebook::react {

///
/// Metadata about a bytecode file that is persisted on disk
///
#pragma pack(push, 1)
struct BytecodeFileHeader {
  uint64_t fileFormatVersion;
  uint64_t scriptChecksum;
  ChakraVersionInfo chakraVersion;
};

constexpr uint32_t CurrentBytecodeFileFormatVersion = 3;
static_assert(sizeof(BytecodeFileHeader) == 32, "CurrentBytecodeFileFormatVersion out of date");
#pragma pack(pop)

const wchar_t *BytecodeScratchSubdirectory = L"Bytecode";
const wchar_t *BytecodeFileExtension = L".bytecode";

ChakraBytecodeStore::ChakraBytecodeStore(const std::filesystem::path &executorScratchDirectory) {
  m_bytecodeDirectory = executorScratchDirectory / BytecodeScratchSubdirectory;
}

std::unique_ptr<JSBigString> ChakraBytecodeStore::tryObtainCachedBytecode(const std::string &scriptUrl) {
  auto bytecodePath = bytecodePathFromScriptUrl(scriptUrl);
  if (!bytecodePath) {
    return nullptr;
  }

  auto bytecodeBodyStr = std::make_unique<FileMappingBigString>(bytecodePath->string(), sizeof(BytecodeFileHeader));
  if (!bytecodeBodyStr->file_data()) {
    return nullptr;
  }

  // file_data will go backwards to the beginning of the file instead of going to the specified offset
  auto header = reinterpret_cast<BytecodeFileHeader *>(bytecodeBodyStr->file_data());

  ChakraVersionInfo curChakraVersion;
  if (!curChakraVersion.initialize() || header->chakraVersion != curChakraVersion) {
    return nullptr;
  }

  folly::Optional<uint64_t> scriptChecksum = calculateScriptChecksum(scriptUrl);
  if (!scriptChecksum || header->scriptChecksum != scriptChecksum) {
    return nullptr;
  }

  return bytecodeBodyStr;
}

void ChakraBytecodeStore::persistBytecode(const std::string &scriptUrl, const JSBigString &bytecode) {
  ChakraVersionInfo versionInfo;
  if (!versionInfo.initialize()) {
    return;
  }

  auto bytecodePath = bytecodePathFromScriptUrl(scriptUrl);
  if (!bytecodePath) {
    return;
  }

  folly::Optional<uint64_t> scriptChecksum = calculateScriptChecksum(scriptUrl);
  if (!scriptChecksum) {
    return;
  }

  // bytecode is a binary representation, so we need to pass in the "b" flag to fopen_s
  FILE *bytecodeFilePtr;
  if (_wfopen_s(&bytecodeFilePtr, bytecodePath->c_str(), L"wb")) {
    return;
  }
  std::unique_ptr<FILE, decltype(&fclose)> bytecodeFilePtrWrapper(bytecodeFilePtr, fclose);

  // Write a junk header at first in case we fail
  BytecodeFileHeader junkHeader{};

  if (fwrite(&junkHeader, sizeof(BytecodeFileHeader), 1, bytecodeFilePtrWrapper.get()) != sizeof(BytecodeFileHeader) ||
      fwrite(bytecode.c_str(), bytecode.size(), 1, bytecodeFilePtrWrapper.get()) != bytecode.size() ||
      fflush(bytecodeFilePtrWrapper.get())) {
    return;
  }

  // Write the real header now
  BytecodeFileHeader header{CurrentBytecodeFileFormatVersion, *scriptChecksum, versionInfo};

  fseek(bytecodeFilePtrWrapper.get(), 0, SEEK_SET);
  if (!fwrite(&header, sizeof(BytecodeFileHeader), 1, bytecodeFilePtrWrapper.get()) ||
      fflush(bytecodeFilePtrWrapper.get())) {
    return;
  }
}

folly::Optional<std::filesystem::path> ChakraBytecodeStore::bytecodePathFromScriptUrl(const std::string &scriptUrl) {
  std::filesystem::path filename = std::filesystem::path(scriptUrl).filename();
  if (filename.empty()) {
    return folly::none;
  }

  return m_bytecodeDirectory / filename.concat(BytecodeFileExtension);
}

folly::Optional<uint64_t> ChakraBytecodeStore::calculateScriptChecksum(const std::string &scriptUrl) {
  FileMappingBigString scriptBuffer(scriptUrl);
  if (!scriptBuffer.file_data()) {
    return folly::none;
  }

  boost::crc_32_type crc32;
  crc32.process_bytes(scriptBuffer.file_data(), scriptBuffer.file_size());
  return crc32.checksum();
}

} // namespace facebook::react
