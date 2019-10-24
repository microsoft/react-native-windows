// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"

#include "Unicode.h"
#include "Utf8DebugExtensions.h"

#include <malloc.h>
#include <stdlib.h>
#include <cassert>
#include <memory>

namespace facebook {
namespace react {

JsErrorCode JsGetPropertyIdFromNameUtf8(_In_z_ const char *name, _Out_ JsPropertyIdRef *propertyId) {
  if (name == nullptr) {
    return JsErrorNullArgument;
  }

  size_t convertedChars = 0;
  size_t stringLength = strlen(name);
  std::unique_ptr<wchar_t[]> wname(new wchar_t[stringLength + 1]);
  mbstowcs_s(&convertedChars, wname.get(), stringLength + 1, name, stringLength);

  if (!wname.get()) {
    return JsErrorOutOfMemory;
  }

  return JsGetPropertyIdFromName(wname.get(), propertyId);
}

JsErrorCode JsPointerToStringUtf8(
    _In_reads_(stringLength) const char *stringValue,
    _In_ size_t stringLength,
    _Out_ JsValueRef *string) noexcept {
  if (stringValue == nullptr) {
    return JsErrorNullArgument;
  }
#if defined(USE_EDGEMODE_JSRT)
  // JsCreateString is not supported in universal chakra engine
  // So we convert the utf8 string to utf16 first and call JsPointerToString to
  // convert the string to JsValueRef
  std::wstring wstr = Microsoft::Common::Unicode::Utf8ToUtf16(stringValue, stringLength);
  return JsPointerToString(wstr.c_str(), wstr.size(), string);

#else
  return JsCreateString(stringValue, stringLength, string);
#endif
}

JsErrorCode JsStringToStdStringUtf8(_In_ JsValueRef stringValue, std::string &string) {
  const wchar_t *wstr;
  size_t wstrLen;
  JsErrorCode err = JsStringToPointer(stringValue, &wstr, &wstrLen);

  if (err == JsNoError)
    string = Microsoft::Common::Unicode::Utf16ToUtf8(wstr, wstrLen);
  else
    string.clear();

  return err;
}

} // namespace react
} // namespace facebook
