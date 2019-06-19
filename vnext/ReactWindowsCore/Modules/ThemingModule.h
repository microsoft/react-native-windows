// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include <cxxreact/CxxModule.h>
#include <folly/dynamic.h>
#include <map>
#include <vector>

namespace react {
  namespace windows {

    class PlatformTheme
    {
    public:
      PlatformTheme();
      virtual ~PlatformTheme();

      virtual const std::string getTheme();
      virtual bool getIsHighContrast();
    };

    class ThemingModule : public facebook::xplat::module::CxxModule
    {
    public:
      ThemingModule(std::shared_ptr<PlatformTheme> && theme);

      static const std::string name;

      // CxxModule
      std::string getName() override;
      std::map<std::string, folly::dynamic> getConstants() override;
      auto getMethods()->std::vector<Method> override;

    private:
      std::shared_ptr<PlatformTheme> m_theme;
    };

  }
} // namespace react::windows
