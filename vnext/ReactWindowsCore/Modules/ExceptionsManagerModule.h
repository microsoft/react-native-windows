// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include <DevSettings.h>
#include <cxxreact/CxxModule.h>
#include <folly/dynamic.h>
#include <map>
#include <vector>

namespace facebook {
namespace react {

class ExceptionsManagerModule : public facebook::xplat::module::CxxModule {
 public:
  ExceptionsManagerModule(
      std::function<void(JSExceptionInfo)> &&jsExceptionCallback);

  static constexpr const char *name = "ExceptionsManager";

  // CxxModule
  std::string getName() override;
  std::map<std::string, folly::dynamic> getConstants() override;
  std::vector<Method> getMethods() override;

 private:
  std::function<void(JSExceptionInfo &&)> m_jsExceptionCallback;
  JSExceptionInfo CreateExceptionInfo(
      const folly::dynamic &args,
      JSExceptionType jsExceptionType) const noexcept;
  std::string RetrieveValueFromMap(
      const folly::dynamic &map,
      const std::string &key,
      folly::dynamic::Type type) const noexcept;
};

} // namespace react
} // namespace facebook
