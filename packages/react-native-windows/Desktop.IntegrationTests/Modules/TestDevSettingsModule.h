// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include <cxxreact/CxxModule.h>
#include <cxxreact/Instance.h>

namespace Microsoft::React::Test {

class TestDevSettingsModule : public facebook::xplat::module::CxxModule {
 public:
  static std::string name;

#pragma region CxxModule members

  std::string getName() override;
  std::map<std::string, folly::dynamic> getConstants() override;
  std::vector<Method> getMethods() override;

#pragma endregion CxxModule members
};

} // namespace Microsoft::React::Test
