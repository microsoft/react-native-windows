// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include <future>
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

std::future<std::string> getApplicationDataPath(const wchar_t *childfolder);

} // namespace Microsoft::React
