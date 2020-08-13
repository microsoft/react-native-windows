// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "Utils.h"
#include <regex>

using namespace std;

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

  ++index;
  this->host = string(match[index].first, match[index].second);

  ++index;
  ++index;
  this->port = string(match[index].first, match[index].second);

  ++index;
  this->path = string(match[index].first, match[index].second);
  if (1 > path.length() || '/' != path.at(0))
    path.insert(0, "/");

  ++index;
  ++index;
  this->queryString = string(match[index].first, match[index].second);
}
else {
  throw std::exception("Could not parse URL.");
}
} // namespace Microsoft::React

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

void assertionFailure(const char *expr, const char *msg, const char *file, unsigned int line, const char *function) {
  // nyi
  std::terminate();
}

} // namespace folly::detail
