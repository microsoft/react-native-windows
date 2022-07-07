// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "Utils.h"
#include <regex>

#include <AppModelHelpers.h>
#include <DesktopWindowBridge.h>
#include <ShlObj.h>
#include <Shlwapi.h>
#include <winrt/Windows.Storage.h>

#include <sstream>

using std::string;

namespace Microsoft::React {

namespace {
std::future<std::string> getPackagedApplicationDataPath(const wchar_t *childFolder) {
  auto localFolder = winrt::Windows::Storage::ApplicationData::Current().LocalFolder();
  if (!childFolder) {
    co_return winrt::to_string(localFolder.Path());
  } else {
    auto subfolder = co_await localFolder.CreateFolderAsync(
        childFolder, winrt::Windows::Storage::CreationCollisionOption::OpenIfExists);
    co_return winrt::to_string(subfolder.Path());
  }
}

std::future<std::string> getUnPackagedApplicationDataPath(const wchar_t *childFolder) {
  wchar_t *pwzAppDataPath = NULL;
  if (CALL_INDIRECT(
          L"shell32.dll",
          SHGetKnownFolderPath,
          FOLDERID_AppDataProgramData,
          KF_FLAG_CREATE,
          static_cast<HANDLE>(NULL),
          &pwzAppDataPath) != S_OK)
    std::abort();

  auto appDataPath = winrt::to_string(pwzAppDataPath);
  CoTaskMemFree(pwzAppDataPath);

  if (!childFolder) {
    co_return appDataPath;
  } else {
    std::ostringstream os;
    os << appDataPath << "\\" << winrt::to_string(childFolder);
    auto childFolderPath = os.str();
    if (!CreateDirectoryA(childFolderPath.c_str(), NULL) && GetLastError() != ERROR_ALREADY_EXISTS)
      std::abort();
    co_return childFolderPath;
  }
}
} // namespace

std::future<std::string> getApplicationDataPath(const wchar_t *childFolder) {
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
// dont need And we probably want to look at some other functionality for
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
