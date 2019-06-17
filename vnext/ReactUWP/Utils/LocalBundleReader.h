// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once
#include <cxxreact/JSBigString.h>
#include <string>
#include <future>
#include <winrt/Windows.Storage.h>

namespace react { namespace uwp {

class LocalBundleReader
{
public:
  static std::future<std::string> LoadBundleAsync(const std::string& bundlePath);
  static std::string LoadBundle(const std::string& bundlePath);

  // Get app bundle created date to compare with byte code file created date
  static std::future<winrt::Windows::Foundation::DateTime> LoadBundleCreatedDateAsync(const std::string& bundlePath);
  static winrt::Windows::Foundation::DateTime LoadBundleCreatedDate(const std::string& bundlePath);
};

class StorageFileBigString : public facebook::react::JSBigString
{
public:
  StorageFileBigString(const std::string& path);
  bool isAscii() const override;
  const char* c_str() const override;
  size_t size() const override;

  void ensure() const;

private:
  mutable std::future<std::string> m_futureBuffer;
  mutable std::string m_string;
};

}}
