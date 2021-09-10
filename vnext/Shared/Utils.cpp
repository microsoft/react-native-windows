// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "Utils.h"
#include <regex>

#include <winrt/Windows.Storage.h>
#include <appmodel.h>
#include <ShlObj.h>
#include <Shlwapi.h>
#include <DesktopWindowBridge.h>

using std::string;

namespace Microsoft::React {

std::future<std::string> getApplicationDataPath(const wchar_t *childFolder) {
  uint32_t length{0};
  bool isPackagedApp = GetCurrentPackageFullName(&length, nullptr) != APPMODEL_ERROR_NO_PACKAGE;
  std::string appDataPath;
  if (isPackagedApp) {
    if (!childFolder) {
      co_return winrt::to_string(winrt::Windows::Storage::ApplicationData::Current().LocalFolder().Path());
    } else {
      auto subfolder = co_await winrt::Windows::Storage::ApplicationData::Current().LocalFolder().CreateFolderAsync(
          childFolder, winrt::Windows::Storage::CreationCollisionOption::OpenIfExists);
      co_return winrt::to_string(subfolder.Path());
    }
  } else {
    wchar_t wzAppData[MAX_PATH]{};
    if (CALL_INDIRECT(
            L"shell32.dll", SHGetFolderPathW, nullptr, CSIDL_LOCAL_APPDATA, nullptr, SHGFP_TYPE_CURRENT, wzAppData) !=
        S_OK) {
      std::abort(); // We don't expect this to happen ever ..
    }
    appDataPath = winrt::to_string(wzAppData);
    if (!childFolder) {
      co_return appDataPath;
    } else {
      std::ostringstream os;
      os << appDataPath << "\\" << winrt::to_string(childFolder);
      auto childFolderPath = os.str();
      if (!CreateDirectoryA(childFolderPath.c_str(), NULL))
        std::abort();
      co_return childFolderPath;
    }
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
