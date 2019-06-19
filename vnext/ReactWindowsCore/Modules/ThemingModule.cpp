// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include "ThemingModule.h"

namespace react {
  namespace windows {

    //
    // Theming and High Contrast
    //

    PlatformTheme::PlatformTheme() = default;
    PlatformTheme::~PlatformTheme() = default;

    // TODO: real implementation
    const std::string PlatformTheme::getTheme()
    {
      return "light";
    }

    // TODO: real implementation
    bool PlatformTheme::getIsHighContrast()
    {
      return "false";
    }

    //
    // ThemingModule
    //

    /* static */ const std::string ThemingModule::name = "ThemingModule";

    ThemingModule::ThemingModule(std::shared_ptr<PlatformTheme>&& theme)
      : m_theme(std::move(theme))
    {
    }

    std::string ThemingModule::getName()
    {
      return name;
    }

    // This seems maybe redundant
    std::map<std::string, folly::dynamic> ThemingModule::getConstants()
    {
      return {
        { "isHighContrast", folly::dynamic { m_theme->getIsHighContrast() } }
      };
    }

    auto ThemingModule::getMethods() -> std::vector<facebook::xplat::module::CxxModule::Method>
    {
      return {
        Method("getCurrentTheme", [this](folly::dynamic args, Callback cbSuccess, Callback /*cbFailure*/)
        {
          cbSuccess({folly::dynamic::object("platform_theme", m_theme->getTheme())});
        }, AsyncTag)
      };
    }

  }
} // namespace react::windows
