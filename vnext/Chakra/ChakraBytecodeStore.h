// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include <cxxreact/JSBigString.h>
#include <folly/Optional.h>
#include <filesystem>
#include <string>

namespace facebook {
namespace react {

///
/// Used to store and retrive bytecode. Superseded in JSI by PreparedScriptStore.
///
class ChakraBytecodeStore {
 public:
  ChakraBytecodeStore(const std::filesystem::path &executorScratchDirectory);

  ///
  /// Wil return bytecode for the Url if it exists may be reused
  ///
  std::unique_ptr<JSBigString> tryObtainCachedBytecode(const std::string &scriptUrl);

  ///
  /// Attempt to store bytecode for the given script (not guranteed to succeed)
  ///
  void persistBytecode(const std::string &scriptUrl, const JSBigString &bytecode);

 private:
  folly::Optional<std::filesystem::path> bytecodePathFromScriptUrl(const std::string &scriptUrl);
  folly::Optional<uint64_t> calculateScriptChecksum(const std::string &scriptUrl);

  std::filesystem::path m_bytecodeDirectory;
};

} // namespace react
} // namespace facebook
