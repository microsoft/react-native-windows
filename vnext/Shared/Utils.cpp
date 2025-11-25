// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "Utils.h"
#include <regex>

#include <AppModelHelpers.h>
#include <DesktopWindowBridge.h>
#include <ShlObj.h>
#include <Shlwapi.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Storage.h>

#include <cstdarg>
#include <sstream>

using std::string;
using winrt::Windows::Foundation::IAsyncOperation;

namespace Microsoft::React {

std::string FormatString(const char *format, ...) {
  if (format == nullptr) {
    return "";
  }

  va_list args;
  va_start(args, format);

  // Get required size
  va_list args_copy;
  va_copy(args_copy, args);
  int size = vsnprintf(nullptr, 0, format, args_copy);
  va_end(args_copy);

  if (size < 0) {
    va_end(args);
    return "";
  }

  std::string result(size, '\0');
  vsnprintf(result.data(), size + 1, format, args);
  va_end(args);
  return result;
}

namespace {
IAsyncOperation<winrt::hstring> getPackagedApplicationDataPath(const wchar_t *childFolder) {
  auto localFolder = winrt::Windows::Storage::ApplicationData::Current().LocalFolder();
  if (!childFolder) {
    co_return localFolder.Path();
  } else {
    auto subfolder = co_await localFolder.CreateFolderAsync(
        childFolder, winrt::Windows::Storage::CreationCollisionOption::OpenIfExists);
    co_return subfolder.Path();
  }
}

IAsyncOperation<winrt::hstring> getUnPackagedApplicationDataPath(const wchar_t *childFolder) {
  wchar_t *pwzAppDataPath = NULL;
  if (CALL_INDIRECT(
          L"shell32.dll",
          SHGetKnownFolderPath,
          FOLDERID_AppDataProgramData,
          KF_FLAG_CREATE,
          static_cast<HANDLE>(NULL),
          &pwzAppDataPath) != S_OK)
    std::abort();

  winrt::hstring appDataPath(pwzAppDataPath);
  CoTaskMemFree(pwzAppDataPath);

  if (!childFolder) {
    co_return appDataPath;
  } else {
    std::wostringstream os;
    os << appDataPath.c_str() << "\\" << childFolder;
    auto childFolderPath = os.str();
    if (!CreateDirectoryW(childFolderPath.c_str(), NULL) && GetLastError() != ERROR_ALREADY_EXISTS)
      std::abort();
    co_return winrt::hstring(childFolderPath);
  }
}
} // namespace

IAsyncOperation<winrt::hstring> getApplicationDataPath(const wchar_t *childFolder) {
  if (Microsoft::ReactNative::HasPackageIdentity()) {
    co_return co_await getPackagedApplicationDataPath(childFolder);
  } else {
    co_return co_await getUnPackagedApplicationDataPath(childFolder);
  }
}

Url::Url(string &&source) {
  //                           ( 1 )              ( 2 )   ( 3 (4) )   ( 5 )    ( 6 (7) )
  std::regex expression("(http|https|ws|wss)://([^:/\\?]+)(:(\\d+))?(/[^\\?]*)?(\\?(.*))?$");
  //                          scheme              host       port      path      query
  constexpr int schemeIdx = 1;
  constexpr int hostIdx = 2;
  constexpr int portIdx = 4;
  constexpr int pathIdx = 5;
  constexpr int queryIdx = 7;

  std::cmatch match;
  if (std::regex_match(source.c_str(), match, expression)) {
    this->scheme = string(match[schemeIdx].first, match[schemeIdx].second);
    this->host = string(match[hostIdx].first, match[hostIdx].second);
    this->port = string(match[portIdx].first, match[portIdx].second);
    this->path = string(match[pathIdx].first, match[pathIdx].second);

    if (1 > path.length() || '/' != path.at(0))
      path.insert(0, "/");

    this->queryString = string(match[queryIdx].first, match[queryIdx].second);
  } else {
    throw std::exception("Could not parse URL.");
  }
}

string Url::Target() {
  if (1 > queryString.length())
    return path;
  else
    return path.append("?").append(queryString);
}

} // namespace Microsoft::React

// Folly/folly/SafeAssert.cpp brings in a bunch of file APIs that we otherwise
// don't need And we probably want to look at some other functionality for
// reporting errors at some point anyway.  For now, just stub them out.
namespace folly::detail {

namespace {
void writeStderr(const char *s, size_t len) {
  // nyi
  std::terminate();
}
void writeStderr(const char *s) {
  // nyi
  std::terminate();
}
} // namespace

// \node_modules\.folly\folly-2020.09.14.00\folly\lang\SafeAssert.h
void assertionFailure(
    const char *expr,
    const char *msg,
    const char *file,
    unsigned int line,
    const char *function,
    int error) {
  // nyi
  std::terminate();
}

} // namespace folly::detail
