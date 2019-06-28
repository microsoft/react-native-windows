// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include "AppThemeModule.h"

namespace react { namespace windows {

//
// AppTheme
//

AppTheme::AppTheme() = default;
AppTheme::~AppTheme() = default;

const std::string AppTheme::getCurrentTheme()
{
  return AppTheme::light;
}

bool AppTheme::getIsHighContrast()
{
  return false;
}

folly::dynamic AppTheme::getHighContrastRGBValues()
{
  return folly::dynamic::object("highContrastRGBValues", "None");
}

//
// AppThemeModule
//

AppThemeModule::AppThemeModule(std::shared_ptr<AppTheme>&& appTheme)
  : m_appTheme(std::move(appTheme))
{
}

auto AppThemeModule::getConstants() -> std::map<std::string, folly::dynamic>
{
  return {
    { "initialAppTheme", folly::dynamic { m_appTheme->getCurrentTheme() } },
    { "isHighContrast", folly::dynamic { m_appTheme->getIsHighContrast() }},
    { "highContrastRGBValues", folly::dynamic {m_appTheme->getHighContrastRGBValues()}}
  };
}

auto AppThemeModule::getMethods() -> std::vector<facebook::xplat::module::CxxModule::Method>
{
  return { };
}

} } // namespace react::windows
