// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include "ChakraBytecodeStore.h"
#include "ChakraHelpers.h"
#include "ChakraUtils.h"
#include "Crypto.h"

#pragma warning(push)
#pragma warning(disable : 4244)
#pragma warning(disable : 4245)
#include <boost/crc.hpp>
#pragma warning(pop)

#include <boost/algorithm/hex.hpp>
#include <cstdio>

namespace facebook::react {

///
/// Metadata about a bytecode file that is persisted on disk
///
#pragma pack(push, 1)
struct BytecodeFileHeader {
  uint64_t fileFormatVersion;
  HashedScript::Sha256Hash scriptHash;
  ChakraVersionInfo chakraVersion;
};
#pragma pack(pop)

constexpr uint32_t CurrentBytecodeFileFormatVersion = 3;
static_assert(sizeof(BytecodeFileHeader) == 56, "CurrentBytecodeFileFormatVersion out of date");

const wchar_t *BytecodeScratchSubdirectory = L"ChakraBytecode";
const wchar_t *BytecodeFileExtension = L".bytecode";

ChakraBytecodeStore::ChakraBytecodeStore(const std::filesystem::path &instanceScratchPath) {
  m_bytecodeDirectory = instanceScratchPath / BytecodeScratchSubdirectory;
}

std::unique_ptr<JSBigString> ChakraBytecodeStore::tryObtainCachedBytecode(const HashedScript &script) {
  auto bytecodePath = bytecodePathForScript(script);
  auto bytecodeBodyStr = std::make_unique<FileMappingBigString>(bytecodePath.string(), sizeof(BytecodeFileHeader));
  if (!bytecodeBodyStr->file_data()) {
    return nullptr;
  }

  // file_data will go backwards to the beginning of the file instead of going to the specified offset
  auto header = reinterpret_cast<BytecodeFileHeader *>(bytecodeBodyStr->file_data());

  ChakraVersionInfo curChakraVersion;
  if (!curChakraVersion.initialize() || header->chakraVersion != curChakraVersion) {
    return nullptr;
  }

  if (header->scriptHash != script.GetHash()) {
    return nullptr;
  }

  return bytecodeBodyStr;
}

void ChakraBytecodeStore::persistBytecode(const HashedScript &script, const JSBigString &bytecode) {
  ChakraVersionInfo versionInfo;
  if (!versionInfo.initialize()) {
    return;
  }

  auto bytecodePath = bytecodePathForScript(script);

  // bytecode is a binary representation, so we need to pass in the "b" flag to fopen_s
  FILE *bytecodeFilePtr;
  if (_wfopen_s(&bytecodeFilePtr, bytecodePath.c_str(), L"wb")) {
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
  BytecodeFileHeader header{CurrentBytecodeFileFormatVersion, script.GetHash(), versionInfo};

  fseek(bytecodeFilePtrWrapper.get(), 0, SEEK_SET);
  if (!fwrite(&header, sizeof(BytecodeFileHeader), 1, bytecodeFilePtrWrapper.get()) ||
      fflush(bytecodeFilePtrWrapper.get())) {
    return;
  }
}

std::filesystem::path ChakraBytecodeStore::bytecodePathForScript(const HashedScript &script) {
  std::string filename;
  boost::algorithm::hex_lower(script.GetHash().begin(), script.GetHash().end(), std::back_inserter(filename));

  return (m_bytecodeDirectory / filename).concat(BytecodeFileExtension);
}

} // namespace facebook::react
