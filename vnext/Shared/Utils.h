// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include <winrt/Windows.Foundation.h>
#include <string>

namespace Microsoft::React {

struct Url {
  std::string scheme;
  std::string host;
  std::string port;
  std::string path;
  std::string queryString;

  Url(std::string &&urlString);

  std::string Target();
};

winrt::Windows::Foundation::IAsyncOperation<winrt::hstring> getApplicationDataPath(const wchar_t *childfolder);

//string formatting
std::string string_format(_Printf_format_string_ const char *format, ...);

} // namespace Microsoft::React
