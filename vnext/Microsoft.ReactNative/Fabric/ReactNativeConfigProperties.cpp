// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"

#include "ReactNativeConfigProperties.h"
#include "unicode.h"

namespace Microsoft::ReactNative {

ReactNativeConfigProperties::ReactNativeConfigProperties(
    winrt::Microsoft::ReactNative::ReactContext const &reactContext)
    : m_context(reactContext) {}

ReactNativeConfigProperties::~ReactNativeConfigProperties() {}

bool ReactNativeConfigProperties::getBool(const std::string &param) const {
  auto value = m_context.Properties().Handle().Get(winrt::Microsoft::ReactNative::ReactPropertyBagHelper::GetName(
      winrt::Microsoft::ReactNative::ReactPropertyBagHelper::GetNamespace(winrt::hstring(L"ReactNativeConfig")),
      winrt::hstring(Common::Unicode::Utf8ToUtf16(param))));

  winrt::Windows::Foundation::IReference<bool> result;
  if (value.try_as<winrt::Windows::Foundation::IReference<bool>>(result)) {
    return result.GetBoolean();
  }
  return false;
}

std::string ReactNativeConfigProperties::getString(const std::string &param) const {
  auto value = m_context.Properties().Handle().Get(winrt::Microsoft::ReactNative::ReactPropertyBagHelper::GetName(
      winrt::Microsoft::ReactNative::ReactPropertyBagHelper::GetNamespace(winrt::hstring(L"ReactNativeConfig")),
      winrt::hstring(Common::Unicode::Utf8ToUtf16(param))));

  winrt::Windows::Foundation::IReference<winrt::hstring> result;
  if (value.try_as<winrt::Windows::Foundation::IReference<winrt::hstring>>(result)) {
    return Common::Unicode::Utf16ToUtf8(result.GetString());
  }
  return {};
}

int64_t ReactNativeConfigProperties::getInt64(const std::string &param) const {
  auto value = m_context.Properties().Handle().Get(winrt::Microsoft::ReactNative::ReactPropertyBagHelper::GetName(
      winrt::Microsoft::ReactNative::ReactPropertyBagHelper::GetNamespace(winrt::hstring(L"ReactNativeConfig")),
      winrt::hstring(Common::Unicode::Utf8ToUtf16(param))));

  winrt::Windows::Foundation::IReference<int64_t> result;
  if (value.try_as<winrt::Windows::Foundation::IReference<int64_t>>(result)) {
    return result.GetInt64();
  }
  return false;
}

double ReactNativeConfigProperties::getDouble(const std::string &param) const {
  auto value = m_context.Properties().Handle().Get(winrt::Microsoft::ReactNative::ReactPropertyBagHelper::GetName(
      winrt::Microsoft::ReactNative::ReactPropertyBagHelper::GetNamespace(winrt::hstring(L"ReactNativeConfig")),
      winrt::hstring(Common::Unicode::Utf8ToUtf16(param))));

  winrt::Windows::Foundation::IReference<double> result;
  if (value.try_as<winrt::Windows::Foundation::IReference<double>>(result)) {
    return result.GetDouble();
  }
  return false;
}

} // namespace Microsoft::ReactNative
