// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once
#include <cxxreact/JSBigString.h>
#include <future>
#include <string>

namespace react {
namespace uwp {

class LocalBundleReader {
 public:
  static std::future<std::string> LoadBundleAsync(
      const std::string &bundlePath);
  static std::string LoadBundle(const std::string &bundlePath);
};

class StorageFileBigString : public facebook::react::JSBigString {
 public:
  StorageFileBigString(const std::string &path);
  bool isAscii() const override;
  const char *c_str() const override;
  size_t size() const override;

  void ensure() const;

 private:
  mutable std::future<std::string> m_futureBuffer;
  mutable std::string m_string;
};

} // namespace uwp
} // namespace react
