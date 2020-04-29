#pragma once
#ifdef DEBUG
#include <string>
#include <vector>
#include <folly/dynamic.h>

struct TestHook {
  static void NotifyUnimplementedProperty(
      const std::string &viewManager,
      const std::string &reactClassName,
      const std::string &propertyName,
      const folly::dynamic &propertyValue);
  static std::vector<std::string> layoutProperties;
};

#endif // DEBUG
