// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "Utils.h"
#include <regex>

using std::string;

namespace Microsoft::React {

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
