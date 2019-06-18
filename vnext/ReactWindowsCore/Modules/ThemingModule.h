// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include <cxxreact/CxxModule.h>
#include <folly/dynamic.h>
#include <map>
#include <vector>

namespace facebook {
  namespace react {

    class Theming
    {
    public:
      Theming();
      virtual ~Theming();

      virtual const char* getTheme();

      virtual const char* getHighContrast();

      virtual const char* highContrastChanged();
    };

    class ThemingModule : public facebook::xplat::module::CxxModule
    {
    public:
      ThemingModule(std::shared_ptr<Theming> && theme);

      static const char* name;

      // CxxModule
      std::string getName() override;
      std::map<std::string, folly::dynamic> getConstants() override;
      auto getMethods()->std::vector<Method> override;

    private:
      std::shared_ptr<Theming> m_theme;
      std::shared_ptr<Theming> m_highContrast;
    };

  }
} // namespace facebook::react
