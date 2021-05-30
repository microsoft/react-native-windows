// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include <cxxreact/CxxModule.h>

namespace facebook::react {

class PlatformConstantsModule : public facebook::xplat::module::CxxModule {
 public:
  static const char *Name;

  std::string getName() override;
  std::map<std::string, folly::dynamic> getConstants() override;
  std::vector<Method> getMethods() override;

 private:
  static folly::dynamic StringOrNull(std::string_view str) noexcept;
  static int32_t OsVersion() noexcept;
};

} // namespace facebook::react
