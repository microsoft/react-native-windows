// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.
#pragma once
#ifdef DEBUG
#include <folly/dynamic.h>
#include <string>
#include <vector>

struct TestHook {
  static void NotifyUnimplementedProperty(
      const std::string &viewManager,
      const std::string &reactClassName,
      const std::string &propertyName,
      const folly::dynamic &propertyValue);
  static std::vector<std::string> layoutProperties;
};

#endif // DEBUG
