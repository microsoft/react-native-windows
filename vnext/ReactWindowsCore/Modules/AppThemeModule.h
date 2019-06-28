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
  static inline const std::string dark = "dark";
  static inline const std::string light = "light";

  AppTheme();
  virtual ~AppTheme();

  virtual const std::string getCurrentTheme();
  virtual bool getIsHighContrast();
  virtual folly::dynamic getHighContrastRGBValues();
};

class AppThemeModule : public facebook::xplat::module::CxxModule
{
public:
  static inline const std::string name = "RTCAppTheme";

  AppThemeModule(std::shared_ptr<AppTheme> && appTheme);

  // CxxModule
  std::string getName() override { return name; };
  auto getConstants() -> std::map<std::string, folly::dynamic> override;
  auto getMethods() -> std::vector<Method> override;

private:
  std::shared_ptr<AppTheme> m_appTheme;
};

} } // namespace react::windows
