// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"
#include "I18nManagerModule.h"
#include <IReactDispatcher.h>
#include <XamlUtils.h>
#include <utils/Helpers.h>
#include <winrt/Windows.ApplicationModel.Resources.Core.h>
#include <winrt/Windows.Globalization.h>
#include "Unicode.h"

namespace Microsoft::ReactNative {

static const React::ReactPropertyId<bool> &SystemIsRTLPropertyId() noexcept {
  static const React::ReactPropertyId<bool> prop{L"ReactNative.I18n", L"SystemIsRTL"};
  return prop;
}

static const React::ReactPropertyId<bool> &AllowRTLPropertyId() noexcept {
  static const React::ReactPropertyId<bool> prop{L"ReactNative.I18n", L"AllowRTL"};
  return prop;
}

static const React::ReactPropertyId<bool> &ForceRTLPropertyId() noexcept {
  static const React::ReactPropertyId<bool> prop{L"ReactNative.I18n", L"ForceRTL"};
  return prop;
}

void I18nManager::InitI18nInfo(const winrt::Microsoft::ReactNative::ReactPropertyBag &propertyBag) noexcept {
  if (xaml::TryGetCurrentApplication() && !react::uwp::IsXamlIsland()) {
    // TODO: Figure out packaged win32 app story for WinUI 3
    auto layoutDirection = winrt::Windows::ApplicationModel::Resources::Core::ResourceContext()
                               .GetForCurrentView()
                               .QualifierValues()
                               .Lookup(L"LayoutDirection");

    propertyBag.Set(SystemIsRTLPropertyId(), layoutDirection != L"LTR");
  }
}

/*static*/ bool I18nManager::IsRTL(const React::ReactPropertyBag &propertyBag) noexcept {
  if (propertyBag.Get(ForceRTLPropertyId()).value_or(false))
    return true;

  if (!propertyBag.Get(AllowRTLPropertyId()).value_or(false))
    return false;

  return propertyBag.Get(SystemIsRTLPropertyId()).value_or(false);
}

void I18nManager::AllowRTL(bool allowRTL) noexcept {
  m_context.Properties().Set(AllowRTLPropertyId(), allowRTL);
}

void I18nManager::ForceRTL(bool forceRTL) noexcept {
  m_context.Properties().Set(ForceRTLPropertyId(), forceRTL);
}

void I18nManager::SwapLeftAndRightInRTL(bool /*flipStyles*/) noexcept {
  // TODO - https://github.com/microsoft/react-native-windows/issues/4662
}

void I18nManager::GetConstants(React::ReactConstantProvider &provider) noexcept {
  std::string locale = "en-us";

  auto langs = winrt::Windows::Globalization::ApplicationLanguages::Languages();
  if (langs.Size() > 0) {
    locale = Microsoft::Common::Unicode::Utf16ToUtf8(langs.GetAt(0));
  }

  provider.Add(L"localeIdentifier", locale);
  provider.Add(L"doLeftAndRightSwapInRTL", false);
  provider.Add(L"isRTL", IsRTL(m_context.Properties()));
}

void I18nManager::Initialize(React::ReactContext const &reactContext) noexcept {
  m_context = reactContext;
}

} // namespace Microsoft::ReactNative
