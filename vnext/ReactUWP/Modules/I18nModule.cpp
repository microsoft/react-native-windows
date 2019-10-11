// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"

#include "I18nModule.h"

#include <winrt/Windows.ApplicationModel.Resources.Core.h>
#include <winrt/Windows.Globalization.h>
#include "Unicode.h"

namespace react {
namespace uwp {

I18nModule::I18nModule() {
  std::string locale = "en-us";
  bool isRTL = false;

  auto langs = winrt::Windows::Globalization::ApplicationLanguages::Languages();
  if (langs.Size() > 0) {
    auto localHstring = langs.GetAt(0);
    locale = Microsoft::Common::Unicode::Utf16ToUtf8(localHstring);

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

  m_isRtl = isRTL;
  m_locale = locale;
}

std::string I18nModule::getLocaleIdentifier() {
  return m_locale;
}

bool I18nModule::getIsRTL() {
  return m_isRtl;
}

} // namespace uwp
} // namespace react
