// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include "Crypto.h"

#include <cxxreact/JSBigString.h>
#include <folly/Optional.h>
#include <filesystem>
#include <string>

namespace facebook::react {

///
/// Script string combined with a crptographics hash. Useful to pass around in
/// cases where we want to avoid the overhead of reh-hashing a script.
///
class HashedScript {
 public:
  using Sha256Hash = Microsoft::Common::Crypto::Sha256Hash;

  static HashedScript compute(const std::shared_ptr<const JSBigString> &script) {
    HashedScript hashedScript;
    hashedScript.m_script = script;
    hashedScript.m_hash = Microsoft::Common::Crypto::calculateSha256Hash(script->c_str(), script->size());
    return hashedScript;
  }

  std::shared_ptr<const JSBigString> GetString() const {
    return m_script;
  };

  const Sha256Hash &GetHash() const {
    return m_hash;
  };

 private:
  HashedScript() = default;

  std::shared_ptr<const JSBigString> m_script;
  Sha256Hash m_hash;
};

///
/// Used to store and retrive bytecode. Superseded in JSI by PreparedScriptStore.
///
class ChakraBytecodeStore {
 public:
  ChakraBytecodeStore(const std::filesystem::path &instanceScratchPath);

  ///
  /// Wil return bytecode for the Url if it may be reused
  ///
  std::unique_ptr<JSBigString> tryObtainCachedBytecode(const HashedScript &script);

  ///
  /// Attempt to store bytecode for the given script (not guranteed to succeed)
  ///
  void persistBytecode(const HashedScript &script, const JSBigString &bytecode);

 private:
  std::filesystem::path bytecodePathForScript(const HashedScript &script);

  std::filesystem::path m_bytecodeDirectory;
};

} // namespace facebook::react
