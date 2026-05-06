// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "Utils.h"
#include <regex>

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
