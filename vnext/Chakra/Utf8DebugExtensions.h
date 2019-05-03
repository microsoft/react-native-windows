// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#if defined(USE_EDGEMODE_JSRT)
#include <jsrt.h>
#else
#include <cstdint>  //for uint16_t in <ChakraCore.h>
#include <ChakraCore.h>
#endif

#include <string>
#if _HAS_CXX17
#include <string_view>
#endif

// unit test classes for testing static private function
class StringConversionTest_Desktop;
class StringConversionTest_Universal;

namespace facebook { namespace react {

JsErrorCode JsGetPropertyIdFromNameUtf8(_In_z_ const char *name, _Out_ JsPropertyIdRef *propertyId);

JsErrorCode JsPointerToStringUtf8(_In_reads_(stringLength) const char *stringValue, _In_ size_t stringLength, _Out_ JsValueRef *string) noexcept;

JsErrorCode JsStringToStdStringUtf8(
  _In_ JsValueRef stringValue,
  std::string& string);

} } // facebook::react
