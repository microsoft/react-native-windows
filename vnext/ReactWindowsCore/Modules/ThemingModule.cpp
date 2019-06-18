// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include "ThemingModule.h"

namespace facebook {
  namespace react {

    //
    // Theming and High Contrast
    //

    Theming::Theming() = default;
    Theming::~Theming() = default;

    // TODO: real implementation
    const char* Theming::getTheme()
    {
      return "light";
    }

    const char* Theming::getHighContrast()
    {
      return "false";
    }

    // Fires event or something? Returns string to bridge indicating event?
    const char* Theming::highContrastChanged()
    {
      return "highContrastChanged";
    }

    //
    // ThemingModule
    //
    const char* ThemingModule::name = "Theming";

    ThemingModule::ThemingModule(std::shared_ptr<Theming>&& theme)
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
      return { /*{"initialContrastState", m_highContrast->getHighContrast()}*/ };
    }

    auto ThemingModule::getMethods() -> std::vector<facebook::xplat::module::CxxModule::Method>
    {
      return {
        Method("getCurrentTheme", [this](folly::dynamic args, Callback cbSuccess, Callback /*cbFailure*/)
        {
          cbSuccess({folly::dynamic::object("theme", m_theme->getTheme())});
        }, AsyncTag),
        Method("isHighContrast", [this](folly::dynamic args, Callback cbSuccess, Callback /*cbFailure*/)
        {
          cbSuccess({folly::dynamic::object(boolean, m_highContrast->getHighContrast())});
        }, AsyncTag),
          Method("highContrastChanged", [this](folly::dynamic args, Callback cbSuccess, Callback /*cbFailure*/)
        {
          cbSuccess({folly::dynamic::object("highContrast", m_highContrast->highContrastChanged())});
        }, AsyncTag),
      };
    }

  }
} // namespace facebook::react
