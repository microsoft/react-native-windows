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

// TODO: real implementation
const std::string AppTheme::getCurrentTheme()
{
  return "light";
}

//
// AppThemeModule
//

/* static */ const std::string AppThemeModule::name = "RTCAppTheme";

AppThemeModule::AppThemeModule(std::shared_ptr<AppTheme>&& appTheme)
  : m_appTheme(std::move(appTheme))
{
}

std::string AppThemeModule::getName()
{
  return name;
}

std::map<std::string, folly::dynamic> AppThemeModule::getConstants()
{
  return {
    { "initialAppTheme", folly::dynamic { m_appTheme->getCurrentTheme() } }
  };
}

auto AppThemeModule::getMethods() -> std::vector<facebook::xplat::module::CxxModule::Method>
{
  return { };
}

} } // namespace react::windows
