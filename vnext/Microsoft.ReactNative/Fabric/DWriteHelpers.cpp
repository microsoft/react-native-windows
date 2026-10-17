// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "DWriteHelpers.h"

#include <dwrite_3.h>
#include <windows.h>
#include <cstdint>
#include <string>
#include <vector>

namespace Microsoft::ReactNative {

winrt::com_ptr<::IDWriteFactory> DWriteFactory() noexcept {
  // Function-local static with a dynamic initializer: initialized exactly once,
  // and concurrent first callers wait for that initialization rather than racing
  // it ([stmt.dcl]/4, on by default in MSVC as /Zc:threadSafeInit). The previous
  // `if (!s_dwriteFactory) { ...assign... }` pattern was a first-use data race:
  // two threads could both observe the empty pointer and both create/assign a
  // factory. DWriteAppFontCollection() below is reachable from more than one
  // thread on first use, which makes that race live rather than theoretical.
  static const winrt::com_ptr<::IDWriteFactory> s_dwriteFactory = [] {
    winrt::com_ptr<::IDWriteFactory> factory;
    winrt::check_hresult(::DWriteCreateFactory(
        DWRITE_FACTORY_TYPE_SHARED, __uuidof(factory), reinterpret_cast<::IUnknown **>(factory.put())));
    return factory;
  }();
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

// Appends every file matching <directory> + <pattern> to `paths`. Pure file-system
// enumeration - no DirectWrite objects are created here, so the result is cacheable
// independently of any factory or collection lifetime.
void AppendFontFiles(std::vector<std::wstring> &paths, const std::wstring &directory, const wchar_t *pattern) noexcept {
  const std::wstring searchPattern = directory + pattern;
  WIN32_FIND_DATAW findData{};
  const HANDLE findHandle = ::FindFirstFileW(searchPattern.c_str(), &findData);
  if (findHandle == INVALID_HANDLE_VALUE) {
    return;
  }
  do {
    if (!(findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
      paths.emplace_back(directory + findData.cFileName);
    }
  } while (::FindNextFileW(findHandle, &findData));
  ::FindClose(findHandle);
}

// The cached list of bundled font files. The directory searches run exactly once per
// process, on whichever thread gets here first (thread-safe static initialization);
// every later caller - including any future path that rebuilds a collection - reads
// this list and never touches the file system again. Bundled assets cannot change
// while the process runs, so the list can never go stale.
const std::vector<std::wstring> &AppFontFilePaths() noexcept {
  static const std::vector<std::wstring> s_paths = [] {
    std::vector<std::wstring> paths;
    const std::wstring appDirectory = AppDirectory();
    if (!appDirectory.empty()) {
      for (const auto *subdirectory : {L"Assets\\", L"Assets\\Fonts\\"}) {
        for (const auto *pattern : {L"*.ttf", L"*.otf"}) {
          AppendFontFiles(paths, appDirectory + subdirectory, pattern);
        }
      }
    }
    return paths;
  }();
  return s_paths;
}

// Builds the merged collection from the cached file list. Contains no directory
// enumeration by construction - see AppFontFilePaths().
winrt::com_ptr<::IDWriteFontCollection> CreateAppFontCollection() noexcept {
  try {
    const auto &fontFiles = AppFontFilePaths();
    if (fontFiles.empty()) {
      // Nothing bundled: report "no app collection" so callers pass nullptr to DirectWrite
      // and keep using DirectWrite's own (cached, updatable) system font collection.
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

    // Per-file failures are skipped so that one bad font file cannot break font
    // resolution for the rest of the app.
    uint32_t fontFileCount = 0;
    for (const auto &fontPath : fontFiles) {
      winrt::com_ptr<::IDWriteFontFile> fontFile;
      if (SUCCEEDED(factory5->CreateFontFileReference(fontPath.c_str(), nullptr, fontFile.put())) &&
          SUCCEEDED(builder->AddFontFile(fontFile.get()))) {
        ++fontFileCount;
      }
    }
    if (fontFileCount == 0) {
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

::IDWriteFontCollection *DWriteAppFontCollection() noexcept {
  // One-time initialization, thread-safe by construction (same mechanism as the
  // statics above): concurrent first callers wait rather than race or repeat. The
  // underlying directory searches are cached separately in AppFontFilePaths(), so
  // even a future change that rebuilds the collection can never re-run them.
  //
  // Held by value for the lifetime of the process and handed out as a non-owning
  // raw pointer: GetTextLayout() calls this on every text measure, and returning a
  // com_ptr by value would add an AddRef/Release pair to that path for no benefit.
  static const winrt::com_ptr<::IDWriteFontCollection> s_appFontCollection = CreateAppFontCollection();
  return s_appFontCollection.get();
}

} // namespace Microsoft::ReactNative
