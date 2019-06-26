// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include <cxxreact/CxxModule.h>
#include <folly/dynamic.h>
#include <map>

namespace react { namespace windows {

class AppTheme
{
public:
  AppTheme();
  virtual ~AppTheme();

  virtual const std::string getCurrentTheme();
};

class AppThemeModule : public facebook::xplat::module::CxxModule
{
public:
  AppThemeModule(std::shared_ptr<AppTheme> && appTheme);

  static const std::string name;

  // CxxModule
  std::string getName() override;
  std::map<std::string, folly::dynamic> getConstants() override;
  auto getMethods()->std::vector<Method> override;

private:
  std::shared_ptr<AppTheme> m_appTheme;
};

} } // namespace react::windows
