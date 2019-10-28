#pragma once

#include <jsi/ScriptStore.h>
#include <jsi/jsi.h>

#include <algorithm>
#include <fstream>
#include <tuple>
#include <vector>

namespace facebook {
namespace react {

struct BufferStore {
  virtual std::unique_ptr<const facebook::jsi::Buffer> getBuffer(const std::string &bufferId) noexcept = 0;
  virtual bool persistBuffer(const std::string &bufferId, std::unique_ptr<const facebook::jsi::Buffer>) noexcept = 0;
};

class LocalFileSimpleBufferStore : public BufferStore {
 public:
  LocalFileSimpleBufferStore(const std::string &storeDirectory) : storeDirectory_(storeDirectory) {}

  std::unique_ptr<const facebook::jsi::Buffer> getBuffer(const std::string &bufferId) noexcept override;
  bool persistBuffer(const std::string &bufferId, std::unique_ptr<const facebook::jsi::Buffer>) noexcept override;

 private:
  std::string storeDirectory_;
};

struct ScriptVersionProvider {
  virtual facebook::jsi::ScriptVersion_t getVersion(const std::string &url) noexcept = 0;
};

class LocalFileSimpleScriptVersionProvider : public ScriptVersionProvider {
 public:
  facebook::jsi::ScriptVersion_t getVersion(const std::string &url) noexcept override;
};

struct PreparedScriptStoreNameGenerator {
  virtual std::string getStoreName(const std::string &url) noexcept = 0;
};

// Dead simple implementation with local filesystem storage using standard c++
// fileio but with optional extension point with custom bufferStore.
class BasePreparedScriptStoreImpl : public facebook::jsi::PreparedScriptStore {
 public:
  std::shared_ptr<const facebook::jsi::Buffer> tryGetPreparedScript(
      const facebook::jsi::ScriptSignature &scriptSignature,
      const facebook::jsi::JSRuntimeSignature &runtimeSignature,
      const char *prepareTag) noexcept override;

  void persistPreparedScript(
      std::shared_ptr<const facebook::jsi::Buffer> preparedScript,
      const facebook::jsi::ScriptSignature &scriptSignature,
      const facebook::jsi::JSRuntimeSignature &runtimeSignature,
      const char *prepareTag) noexcept override;

  BasePreparedScriptStoreImpl(const std::string &storeDirectory)
      : bufferStore_(std::make_shared<LocalFileSimpleBufferStore>(storeDirectory)) {}

  BasePreparedScriptStoreImpl(std::shared_ptr<BufferStore> bufferStore) : bufferStore_(std::move(bufferStore)) {}

 private:
  std::string getPreparedScriptFileName(
      const facebook::jsi::ScriptSignature &scriptMetadata,
      const facebook::jsi::JSRuntimeSignature &runtimeMetadata,
      const char *prepareTag);

  std::shared_ptr<BufferStore> bufferStore_;
};

// Dead simple script store implementation assuming that the script url is a
// local filesystam path and assuming the script version is the script size, but
// with extension point to provide custom version provider.
class BaseScriptStoreImpl : public facebook::jsi::ScriptStore {
 public:
  facebook::jsi::VersionedBuffer getVersionedScript(const std::string &url) noexcept override;
  facebook::jsi::ScriptVersion_t getScriptVersion(const std::string &url) noexcept override;

  BaseScriptStoreImpl(std::shared_ptr<ScriptVersionProvider> versionProvider)
      : versionProvider_{std::move(versionProvider)} {}

  BaseScriptStoreImpl() {}

 private:
  std::shared_ptr<ScriptVersionProvider> versionProvider_;
};

} // namespace react
} // namespace facebook
