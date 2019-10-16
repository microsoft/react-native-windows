// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include <pch.h>

#include "HStringHelper.h"
#include "Unicode.h"

#include <winrt/base.h>

namespace winrt {

std::string HSTRINGToString(HSTRING hstring) {
  std::wstring wstr = WindowsGetStringRawBuffer(hstring, nullptr /*length*/);
  std::string converted_str = Microsoft::Common::Unicode::Utf16ToUtf8(wstr);
  return converted_str;
}

HSTRING StringToHSTRING(const std::string &str) {
  HSTRING hstring;

  std::wstring wString = Microsoft::Common::Unicode::Utf8ToUtf16(str);

  HRESULT hr = WindowsCreateString(
      wString.c_str(), static_cast<UINT32>(wString.length()), &hstring);
  return hstring;
}

} // namespace winrt
