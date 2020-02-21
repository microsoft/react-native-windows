// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#pragma warning(push)
#pragma warning(disable : 4146)
#include <cxxreact/CxxModule.h>
#pragma warning(pop)

#include <folly/dynamic.h>

namespace facebook {
namespace react {

class SourceCodeModule : public facebook::xplat::module::CxxModule {
 public:
  SourceCodeModule(const std::string &bundleUrl);
  std::string getName();
  virtual auto getConstants() -> std::map<std::string, folly::dynamic>;
  virtual auto getMethods() -> std::vector<Method>;

  static const char *Name;

 private:
  std::string m_bundleUrl;
};

} // namespace react
} // namespace facebook
