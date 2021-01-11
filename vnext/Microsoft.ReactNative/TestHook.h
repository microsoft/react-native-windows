// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.
#pragma once
#include <JSValue.h>
#include <string>
#include <vector>

struct TestHook {
  static void NotifyUnimplementedProperty(
      const std::string &viewManager,
      const std::string &reactClassName,
      const std::string &propertyName,
      const winrt::Microsoft::ReactNative::JSValue &propertyValue);
  static std::vector<std::string> layoutProperties;
};
