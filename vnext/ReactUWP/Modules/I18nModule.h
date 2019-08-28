// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include <ReactWindowsCore/II18nModule.h>
#include <cxxreact/CxxModule.h>
#include <folly/dynamic.h>

namespace react {
namespace uwp {

class I18nModule final : public react::windows::II18nModule {
 public:
  // II18nModule
  I18nModule();

  std::string getLocaleIdentifier() override;
  bool getIsRTL() override;

 private:
  bool m_isRtl;
  std::string m_locale;
};
} // namespace uwp
} // namespace react
