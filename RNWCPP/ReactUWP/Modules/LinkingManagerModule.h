// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include <folly/dynamic.h>
#include <cxxreact/CxxModule.h>

namespace react { namespace uwp {

class LinkingManagerModule final : public facebook::xplat::module::CxxModule
{
public:
  LinkingManagerModule();
  virtual ~LinkingManagerModule();

  // CxxModule
  std::string getName() override;
  std::map<std::string, folly::dynamic> getConstants() override;
  auto getMethods() -> std::vector<Method> override;

  static const char* name;
};
} } // namespace react::uwp
