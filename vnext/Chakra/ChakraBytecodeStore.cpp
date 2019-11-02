// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include "ChakraBytecodeStore.h"
#include "ChakraUtils.h"

#include "ChakraHelpers.h"
#include "Unicode.h"

#include <cstdio>

namespace facebook {
namespace react {

#pragma pack(push, 1)
struct BytecodeFileHeader {
  // This field must always come first and may not be changed
  uint64_t bytecodeFileFormatVersion;

  uint64_t scriptVersion;
  ChakraVersionInfo chakraVersion;
};
#pragma pack(pop)

constexpr uint32_t CurrentBytecodeFileFormatVersion = 3;
static_assert(sizeof(BytecodeFileHeader) == 32, "CurrentBytecodeFileFormatVersion out of date");

ChakraBytecodeStore::ChakraBytecodeStore(const std::shared_ptr<ScriptPropertyResolver> &resolver)
    : m_resolver(resolver) {}

std::unique_ptr<JSBigString> ChakraBytecodeStore::tryReadExistingBytecode(const std::string &scriptUrl) {
  std::string filename = m_resolver->resolveBytecodeFilename(scriptUrl);
  if (filename.empty()) {
    return nullptr;
  }

  auto bytecodeBodyOffset = static_cast<uint32_t>(sizeof(BytecodeFileHeader));
  auto bytecodeBody = std::make_unique<FileMappingBigString>(filename, bytecodeBodyOffset);
  if (!bytecodeBody->file_data()) {
    return nullptr;
  }

  auto header = reinterpret_cast<BytecodeFileHeader *>(bytecodeBody->file_data());

  ChakraVersionInfo curChakraVersion;
  if (!curChakraVersion.initialize()) {
    return nullptr;
  }

  uint64_t curScriptVersion = m_resolver->resolveScriptVersion(scriptUrl);
  if (header->chakraVersion == curChakraVersion && header->scriptVersion == curScriptVersion) {
    return bytecodeBody;
  } else {
    return nullptr;
  }
}

void ChakraBytecodeStore::persistBytecode(const std::string &scriptUrl, const JSBigString &bytecode) {
  std::string filename = m_resolver->resolveBytecodeFilename(scriptUrl);
  if (filename.empty()) {
    return;
  }

  ChakraVersionInfo versionInfo;
  if (!versionInfo.initialize()) {
    return;
  }

  // bytecode is a binary representation, so we need to pass in the "b" flag to
  // fopen_s
  FILE *bytecodeFilePtr;
  if (_wfopen_s(&bytecodeFilePtr, Microsoft::Common::Unicode::Utf8ToUtf16(filename).c_str(), L"wb")) {
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
  uint64_t scriptVersion = m_resolver->resolveScriptVersion(scriptUrl);
  BytecodeFileHeader header{CurrentBytecodeFileFormatVersion, scriptVersion, versionInfo};

  fseek(bytecodeFilePtrWrapper.get(), 0, SEEK_SET);
  if (!fwrite(&header, sizeof(BytecodeFileHeader), 1, bytecodeFilePtrWrapper.get()) ||
      fflush(bytecodeFilePtrWrapper.get())) {
    return;
  }
}

} // namespace react
} // namespace facebook
