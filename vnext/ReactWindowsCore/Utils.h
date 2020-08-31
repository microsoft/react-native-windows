// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

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

} // namespace Microsoft::React
