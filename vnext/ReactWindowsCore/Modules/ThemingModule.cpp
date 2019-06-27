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
    const std::string PlatformTheme::getCurrentTheme()
    {
      return "light";
    }

    // TODO: real implementation
    bool PlatformTheme::getIsHighContrast()
    {
      return false;
    }

    folly::dynamic PlatformTheme::getHighContrastRGBValues()
    {
      return folly::dynamic::object("highContrastRGBValues", "None");
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

    std::map<std::string, folly::dynamic> ThemingModule::getConstants()
    {
      return {
        { "currentTheme", folly::dynamic { m_theme->getCurrentTheme() } },
        { "isHighContrast", folly::dynamic { m_theme->getIsHighContrast() }},
        { "highContrastRGBValues", folly::dynamic {m_theme->getHighContrastRGBValues()}}
      };
    }

    auto ThemingModule::getMethods() -> std::vector<facebook::xplat::module::CxxModule::Method>
    {
      return {};
    }

  }
} // namespace react::windows
