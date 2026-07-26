// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include "DWriteHelpers.h"

#include <dwrite_3.h>
#include <windows.h>
#include <cstdint>
#include <string>

namespace Microsoft::ReactNative {

winrt::com_ptr<::IDWriteFactory> DWriteFactory() noexcept {
  static winrt::com_ptr<::IDWriteFactory> s_dwriteFactory;
  if (!s_dwriteFactory) {
    winrt::check_hresult(::DWriteCreateFactory(
        DWRITE_FACTORY_TYPE_SHARED, __uuidof(s_dwriteFactory), reinterpret_cast<::IUnknown **>(s_dwriteFactory.put())));
  }
  return s_dwriteFactory;
}

namespace {

// Directory that contains the running module, including the trailing separator: the
// package root for packaged (MSIX) apps and the directory next to the .exe for
// unpackaged apps. Bundled font assets are deployed below this directory.
std::wstring AppDirectory() noexcept {
  wchar_t modulePath[MAX_PATH]{};
  const DWORD length = ::GetModuleFileNameW(nullptr, modulePath, MAX_PATH);
  if (length == 0 || length >= MAX_PATH) {
    return {};
  }
  std::wstring path(modulePath, length);
  const auto lastSeparator = path.find_last_of(L"\\/");
  if (lastSeparator == std::wstring::npos) {
    return {};
  }
  path.resize(lastSeparator + 1);
  return path;
}

// Adds every file matching <directory> + <pattern> to the font-set builder and returns
// the number of files added. Per-file failures are skipped so that one bad font file
// cannot break font resolution for the rest of the app.
uint32_t AddFontFiles(
    ::IDWriteFactory5 *factory,
    ::IDWriteFontSetBuilder1 *builder,
    const std::wstring &directory,
    const wchar_t *pattern) noexcept {
  uint32_t count = 0;
  const std::wstring searchPattern = directory + pattern;
  WIN32_FIND_DATAW findData{};
  const HANDLE findHandle = ::FindFirstFileW(searchPattern.c_str(), &findData);
  if (findHandle == INVALID_HANDLE_VALUE) {
    return count;
  }
  do {
    if (!(findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
      const std::wstring fontPath = directory + findData.cFileName;
      winrt::com_ptr<::IDWriteFontFile> fontFile;
      if (SUCCEEDED(factory->CreateFontFileReference(fontPath.c_str(), nullptr, fontFile.put())) &&
          SUCCEEDED(builder->AddFontFile(fontFile.get()))) {
        ++count;
      }
    }
  } while (::FindNextFileW(findHandle, &findData));
  ::FindClose(findHandle);
  return count;
}

winrt::com_ptr<::IDWriteFontCollection> CreateAppFontCollection() noexcept {
  try {
    const std::wstring appDirectory = AppDirectory();
    if (appDirectory.empty()) {
      return nullptr;
    }

    const auto factory5 = DWriteFactory().as<::IDWriteFactory5>();

    winrt::com_ptr<::IDWriteFontSetBuilder1> builder;
    winrt::check_hresult(factory5->CreateFontSetBuilder(builder.put()));

    // Include the system font set so that system families keep resolving when this
    // collection is used in place of the system collection.
    winrt::com_ptr<::IDWriteFontSet> systemFontSet;
    winrt::check_hresult(factory5->GetSystemFontSet(systemFontSet.put()));
    winrt::check_hresult(builder->AddFontSet(systemFontSet.get()));

    uint32_t fontFileCount = 0;
    for (const auto *subdirectory : {L"Assets\\", L"Assets\\Fonts\\"}) {
      for (const auto *pattern : {L"*.ttf", L"*.otf"}) {
        fontFileCount += AddFontFiles(factory5.get(), builder.get(), appDirectory + subdirectory, pattern);
      }
    }
    if (fontFileCount == 0) {
      // Nothing bundled: report "no app collection" so callers pass nullptr to DirectWrite
      // and keep using DirectWrite's own (cached, updatable) system font collection.
      return nullptr;
    }

    winrt::com_ptr<::IDWriteFontSet> fontSet;
    winrt::check_hresult(builder->CreateFontSet(fontSet.put()));
    winrt::com_ptr<::IDWriteFontCollection1> collection;
    winrt::check_hresult(factory5->CreateFontCollectionFromFontSet(fontSet.get(), collection.put()));
    return collection.as<::IDWriteFontCollection>();
  } catch (...) {
    // Fail closed: callers fall back to the system font collection (previous behavior).
    return nullptr;
  }
}

} // namespace

winrt::com_ptr<::IDWriteFontCollection> DWriteAppFontCollection() noexcept {
  // Thread-safe (magic static) one-time initialization. Bundled font assets cannot
  // change for the lifetime of the process, so the collection never needs rebuilding.
  static const winrt::com_ptr<::IDWriteFontCollection> s_appFontCollection = CreateAppFontCollection();
  return s_appFontCollection;
}

} // namespace Microsoft::ReactNative
