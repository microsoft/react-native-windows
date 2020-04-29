// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include <ReactWindowsCore/II18nModule.h>
#include <cxxreact/CxxModule.h>
#include <folly/dynamic.h>

namespace react {
namespace uwp {

class I18nHelper;

class I18nModule final : public react::windows::II18nModule {
 public:
  using I18nInfo = std::pair<std::string, bool>;
  static I18nInfo GetI18nInfo(); // Must be called from a UI thread

  // II18nModule
  I18nModule();

  std::string getLocaleIdentifier() const override;
  bool getIsRTL() const override;
  void setAllowRTL(bool allowRTL) override;
  void setForceRTL(bool forceRTL) override;

 private:
  I18nHelper &m_helper;
};

class I18nHelper {
 public:
  static I18nHelper &Instance();

  I18nHelper();

  void setInfo(I18nModule::I18nInfo &&i18nInfo);
  std::string getLocaleIdentifier() const;
  bool getIsRTL() const;
  void setAllowRTL(bool allowRTL);
  void setForceRTL(bool forceRTL);

 private:
  I18nModule::I18nInfo m_i18nInfo;
  bool m_allowRTL{true};
  bool m_forceRTL{false};
};

} // namespace uwp
} // namespace react
