// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include <ReactWindowsCore/II18nModule.h>
#include <folly/dynamic.h>
#include <cxxreact/CxxModule.h>

namespace react { namespace uwp {

class I18nModule final : public react::windows::II18nModule
{
public:
  using I18nInfo = std::pair<std::string, bool>;
  static I18nInfo GetI18nInfo(); // Must be called from a UI thread

  // II18nModule
  I18nModule(I18nInfo&& i18nInfo);

  std::string getLocaleIdentifier() override;
  bool getIsRTL() override;

private:
  I18nInfo m_i18nInfo;
};
} } // namespace react::uwp
