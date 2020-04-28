// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"

#include "I18nModule.h"

#include <winrt/Windows.ApplicationModel.Resources.Core.h>
#include <winrt/Windows.Globalization.h>
#include "Unicode.h"

namespace react {
namespace uwp {

/*static*/ I18nModule::I18nInfo I18nModule::GetI18nInfo() {
  std::string locale = "en-us";
  bool isRTL = false;

  auto langs = winrt::Windows::Globalization::ApplicationLanguages::Languages();
  if (langs.Size() > 0) {
    locale = Microsoft::Common::Unicode::Utf16ToUtf8(langs.GetAt(0));

    auto layoutDirection = winrt::Windows::ApplicationModel::Resources::Core::ResourceContext()
                               .GetForCurrentView()
                               .QualifierValues()
                               .Lookup(L"LayoutDirection");
    isRTL = layoutDirection != L"LTR";
  }

  return std::make_pair<std::string, bool>(std::move(locale), std::move(isRTL));
}

I18nModule::I18nModule() : m_helper(I18nHelper::Instance()) {}

std::string I18nModule::getLocaleIdentifier() const {
  return m_helper.getLocaleIdentifier();
}

bool I18nModule::getIsRTL() const {
  return m_helper.getIsRTL();
}

void I18nModule::setAllowRTL(bool allowRTL) {
  m_helper.setAllowRTL(allowRTL);
}

void I18nModule::setForceRTL(bool forceRTL) {
  m_helper.setForceRTL(forceRTL);
}

/*static*/ I18nHelper &I18nHelper::Instance() {
  static I18nHelper theInstance;
  return theInstance;
}

I18nHelper::I18nHelper() {}

void I18nHelper::setInfo(I18nModule::I18nInfo &&i18nInfo) {
  m_i18nInfo = i18nInfo;
}

std::string I18nHelper::getLocaleIdentifier() const {
  return m_i18nInfo.first;
}

bool I18nHelper::getIsRTL() const {
  if (m_forceRTL) {
    // Used for debugging purposes, forces RTL even in LTR locales
    return true;
  }

  // If the app allows RTL (default is true), then we are in RTL if the locale is RTL
  return m_allowRTL && m_i18nInfo.second;
}

void I18nHelper::setAllowRTL(bool allowRTL) {
  m_allowRTL = allowRTL;
}

void I18nHelper::setForceRTL(bool forceRTL) {
  m_forceRTL = forceRTL;
}

} // namespace uwp
} // namespace react
