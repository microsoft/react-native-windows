// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"

#include "LocalBundleReader.h"
#include "unicode.h"
#include <winrt/Windows.Storage.h>

#if _MSC_VER <= 1913
// VC 19 (2015-2017.6) cannot optimize co_await/cppwinrt usage
#pragma optimize( "", off )
#endif

namespace react { namespace uwp {

std::future<std::string> LocalBundleReader::LoadBundleAsync(const std::string& bundleUri)
{
  winrt::hstring str(facebook::react::unicode::Utf8ToUtf16(bundleUri));
  winrt::Windows::Foundation::Uri uri(str);

  co_await winrt::resume_background();

  auto file = co_await winrt::Windows::Storage::StorageFile::GetFileFromApplicationUriAsync(uri);
  auto hdata = co_await winrt::Windows::Storage::FileIO::ReadTextAsync(file);

  co_return facebook::react::unicode::Utf16ToUtf8(std::wstring(hdata.data()));
}

std::string LocalBundleReader::LoadBundle(const std::string& bundlePath)
{
  return LoadBundleAsync(bundlePath).get();
}

StorageFileBigString::StorageFileBigString(const std::string& path) {
  m_futureBuffer = LocalBundleReader::LoadBundleAsync(path);
}

bool StorageFileBigString::isAscii() const {
  return false;
}

const char* StorageFileBigString::c_str() const {
  ensure();
  return m_string.c_str();
}

size_t StorageFileBigString::size() const {
  ensure();
  return m_string.size();
}

void StorageFileBigString::ensure() const {
  if (m_string.empty()) {
    m_string = m_futureBuffer.get();
  }
}

}}
