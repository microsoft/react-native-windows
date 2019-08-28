// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"

#include "I18nModule.h"

#include <winrt/Windows.ApplicationModel.Resources.Core.h>
#include <winrt/Windows.Globalization.h>
#include "unicode.h"

namespace react {
namespace uwp {

/*static*/ I18nModule::I18nInfo I18nModule::GetI18nInfo() {
  std::string locale = "en-us";
  bool isRTL = false;

  auto langs = winrt::Windows::Globalization::ApplicationLanguages::Languages();
  if (langs.Size() > 0) {
    auto localHstring = langs.GetAt(0);
    locale = facebook::react::unicode::utf16ToUtf8(localHstring);

    // Using Win32 api to avoid requiring UI Thread
    unsigned long reading_layout;
    if (GetLocaleInfoEx(
            localHstring.c_str(),
            LOCALE_IREADINGLAYOUT | LOCALE_RETURN_NUMBER,
            reinterpret_cast<LPTSTR>(&reading_layout),
            sizeof(reading_layout) / sizeof(TCHAR))) {
      isRTL = reading_layout == 1;
    }
  }

  return std::make_pair<std::string, bool>(std::move(locale), std::move(isRTL));
}

I18nModule::I18nModule(std::pair<std::string, bool> &&i18nInfo)
    : m_i18nInfo(std::move(i18nInfo)) {}

std::string I18nModule::getLocaleIdentifier() {
  return m_i18nInfo.first;
}

bool I18nModule::getIsRTL() {
  return m_i18nInfo.second;
}

} // namespace uwp
} // namespace react
