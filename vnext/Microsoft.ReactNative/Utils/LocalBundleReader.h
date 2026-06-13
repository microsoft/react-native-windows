// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once
#include <cxxreact/JSBigString.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Storage.Streams.h>
#include <string>

namespace Microsoft::ReactNative {

class LocalBundleReader {
 public:
  static winrt::Windows::Foundation::IAsyncOperation<winrt::Windows::Storage::Streams::IBuffer> LoadBundleAsync(
      const std::wstring bundlePath);
  static std::string LoadBundle(const std::wstring &bundlePath);
};

class StorageFileBigString : public facebook::react::JSBigString {
 public:
  StorageFileBigString(const std::wstring &path);
  bool isAscii() const override;
  const char *c_str() const override;
  size_t size() const override;

  void ensure() const;

 private:
  mutable winrt::Windows::Foundation::IAsyncOperation<winrt::Windows::Storage::Streams::IBuffer> m_pendingLoad;
  mutable std::string m_string;
};

} // namespace Microsoft::ReactNative
