// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "ChakraObjectRef.h"

#include "Unicode.h"
#include "Utilities.h"

#include "jsi//jsi.h"

#include <memory>
#include <sstream>
#include <utility>

namespace Microsoft::JSI {

void VerifyChakraErrorElseThrow(JsErrorCode error) {
  if (error != JsNoError) {
    std::ostringstream errorString;
    errorString << "A call to Chakra(Core) API returned error code 0x" << std::hex << error << '.';
    throw facebook::jsi::JSINativeException(errorString.str());
  }
}

ChakraObjectRef::ChakraObjectRef(const ChakraObjectRef &original) noexcept
    : m_ref{original.m_ref}, m_state{original.m_state} {
  if (m_state == State::Initialized) {
    VerifyChakraErrorElseThrow(JsAddRef(m_ref, nullptr));
  }
}

ChakraObjectRef::ChakraObjectRef(ChakraObjectRef &&original) noexcept {
  Swap(original);
}

ChakraObjectRef &ChakraObjectRef::operator=(const ChakraObjectRef &rhs) noexcept {
  ChakraObjectRef rhsCopy(rhs);
  Swap(rhsCopy);
  return *this;
}

ChakraObjectRef &ChakraObjectRef::operator=(ChakraObjectRef &&rhs) noexcept {
  Swap(rhs);
  return *this;
}

ChakraObjectRef::~ChakraObjectRef() noexcept {
  if (m_state == State::Initialized) {
    VerifyChakraErrorElseThrow(JsRelease(m_ref, nullptr));
  }
}

void ChakraObjectRef::Initialize(JsRef ref) {
  if (m_state != State::Uninitialized) {
    throw facebook::jsi::JSINativeException("A ChakraObjectRef can only be initialzed once.");
  }

  if (!ref) {
    throw facebook::jsi::JSINativeException("Cannot initialize a ChakraObjectRef with a null reference.");
  }

  VerifyChakraErrorElseThrow(JsAddRef(ref, nullptr));
  m_ref = ref;
  m_state = State::Initialized;
}

void ChakraObjectRef::Invalidate() {
  switch (m_state) {
    case State::Uninitialized: {
      throw facebook::jsi::JSINativeException("Cannot invalidate a ChakraObjectRef that has not been initialized.");
      break;
    }
    case State::Initialized: {
      VerifyChakraErrorElseThrow(JsRelease(m_ref, nullptr));
      m_ref = JS_INVALID_REFERENCE;
      m_state = State::Invalidated;
      break;
    }
    case State::Invalidated: {
      throw facebook::jsi::JSINativeException(
          "Cannot invalidate a ChakraObjectRef that has already been "
          "invalidated.");
      break;
    }
    default: {
      // Control flow should never reach here.
      std::terminate();
      break;
    }
  }
}

void ChakraObjectRef::Swap(ChakraObjectRef &other) {
  std::swap(m_ref, other.m_ref);
  std::swap(m_state, other.m_state);
}

JsValueType GetValueType(const ChakraObjectRef &jsValue) {
  JsValueType type;
  VerifyChakraErrorElseThrow(JsGetValueType(jsValue, &type));
  return type;
}

JsPropertyIdType GetPropertyIdType(const ChakraObjectRef &jsPropId) {
  JsPropertyIdType type;
  VerifyChakraErrorElseThrow(JsGetPropertyIdType(jsPropId, &type));
  return type;
}

std::wstring GetPropertyName(const ChakraObjectRef &id) {
  if (GetPropertyIdType(id) != JsPropertyIdTypeString) {
    throw facebook::jsi::JSINativeException("It is llegal to retrieve the name of a property symbol.");
  }
  const wchar_t *propertyName = nullptr;
  VerifyChakraErrorElseThrow(JsGetPropertyNameFromId(id, &propertyName));
  return std::wstring{propertyName};
}

ChakraObjectRef GetPropertySymbol(const ChakraObjectRef &id) {
  if (GetPropertyIdType(id) != JsPropertyIdTypeSymbol) {
    throw facebook::jsi::JSINativeException("It is llegal to retrieve the symbol associated with a property name.");
  }
  JsValueRef symbol = JS_INVALID_REFERENCE;
  VerifyChakraErrorElseThrow(JsGetSymbolFromPropertyId(id, &symbol));
  return ChakraObjectRef{symbol};
}

ChakraObjectRef GetPropertyId(const std::string_view &utf8) {
  if (!utf8.data()) {
    throw facebook::jsi::JSINativeException("Property name cannot be a nullptr.");
  }

  // We use a #ifdef here because we can avoid a UTF-8 to UTF-16 conversion
  // using ChakraCore's JsCreatePropertyId API.
#ifdef CHAKRACORE
  JsPropertyIdRef id = JS_INVALID_REFERENCE;
  VerifyChakraErrorElseThrow(JsCreatePropertyId(utf8.data(), utf8.length(), &id));
  return ChakraObjectRef(id);

#else
  std::wstring utf16 = Common::Unicode::Utf8ToUtf16(utf8.data(), utf8.length());
  return GetPropertyId(utf16);
#endif
}

ChakraObjectRef GetPropertyId(const std::wstring &utf16) {
  JsPropertyIdRef id = JS_INVALID_REFERENCE;
  VerifyChakraErrorElseThrow(JsGetPropertyIdFromName(utf16.c_str(), &id));
  return ChakraObjectRef(id);
}

std::string ToStdString(const ChakraObjectRef &jsString) {
  if (GetValueType(jsString) != JsString) {
    throw facebook::jsi::JSINativeException("Cannot convert a non JS string ChakraObjectRef to a std::string.");
  }

  // We use a #ifdef here because we can avoid a UTF-8 to UTF-16 conversion
  // using ChakraCore's JsCopyString API.
#ifdef CHAKRACORE
  size_t length = 0;
  VerifyChakraErrorElseThrow(JsCopyString(jsString, nullptr, 0, &length));

  std::string result(length, 'a');
  VerifyChakraErrorElseThrow(JsCopyString(jsString, result.data(), result.length(), &length));

  if (length != result.length()) {
    throw facebook::jsi::JSINativeException("Failed to convert a JS string to a std::string.");
  }
  return result;

#else
  return Common::Unicode::Utf16ToUtf8(ToStdWstring(jsString));
#endif
}

std::wstring ToStdWstring(const ChakraObjectRef &jsString) {
  if (GetValueType(jsString) != JsString) {
    throw facebook::jsi::JSINativeException("Cannot convert a non JS string ChakraObjectRef to a std::wstring.");
  }

  const wchar_t *utf16 = nullptr;
  size_t length = 0;
  VerifyChakraErrorElseThrow(JsStringToPointer(jsString, &utf16, &length));

  return std::wstring(utf16, length);
}

ChakraObjectRef ToJsString(const std::string_view &utf8) {
  if (!utf8.data()) {
    throw facebook::jsi::JSINativeException("Cannot convert a nullptr to a JS string.");
  }

  // We use a #ifdef here because we can avoid a UTF-8 to UTF-16 conversion
  // using ChakraCore's JsCreateString API.
#ifdef CHAKRACORE
  JsValueRef result = JS_INVALID_REFERENCE;
  VerifyChakraErrorElseThrow(JsCreateString(utf8.data(), utf8.length(), &result));
  return ChakraObjectRef(result);

#else
  std::wstring utf16 = Common::Unicode::Utf8ToUtf16(utf8.data(), utf8.length());
  return ToJsString(std::wstring_view{utf16.c_str(), utf16.length()});
#endif
}

ChakraObjectRef ToJsString(const std::wstring_view &utf16) {
  if (!utf16.data()) {
    throw facebook::jsi::JSINativeException("Cannot convert a nullptr to a JS string.");
  }

  JsValueRef result = JS_INVALID_REFERENCE;
  VerifyChakraErrorElseThrow(JsPointerToString(utf16.data(), utf16.length(), &result));

  return ChakraObjectRef(result);
}

ChakraObjectRef ToJsString(const ChakraObjectRef &ref) {
  JsValueRef str = JS_INVALID_REFERENCE;
  VerifyChakraErrorElseThrow(JsConvertValueToString(ref, &str));
  return ChakraObjectRef(str);
}

ChakraObjectRef ToJsNumber(int num) {
  JsValueRef result = JS_INVALID_REFERENCE;
  VerifyChakraErrorElseThrow(JsIntToNumber(num, &result));
  return ChakraObjectRef(result);
}

ChakraObjectRef ToJsArrayBuffer(const std::shared_ptr<const facebook::jsi::Buffer> &buffer) {
  if (!buffer) {
    throw facebook::jsi::JSINativeException("Cannot create an external JS ArrayBuffer without backing buffer.");
  }

  size_t size = buffer->size();

  if (size > UINT_MAX) {
    throw facebook::jsi::JSINativeException("The external backing buffer for a JS ArrayBuffer is too large.");
  }

  JsValueRef arrayBuffer = JS_INVALID_REFERENCE;
  auto bufferWrapper = std::make_unique<std::shared_ptr<const facebook::jsi::Buffer>>(buffer);

  // We allocate a copy of buffer on the heap, a shared_ptr which is deleted
  // when the JavaScript garbage collecotr releases the created external array
  // buffer. This ensures that buffer stays alive while the JavaScript engine is
  // using it.
  VerifyChakraErrorElseThrow(JsCreateExternalArrayBuffer(
      Common::Utilities::CheckedReinterpretCast<char *>(const_cast<uint8_t *>(buffer->data())),
      static_cast<unsigned int>(size),
      [](void *bufferToDestroy) {
        // We wrap bufferToDestroy in a unique_ptr to avoid calling delete
        // explicitly.
        std::unique_ptr<std::shared_ptr<const facebook::jsi::Buffer>> wrapper{
            static_cast<std::shared_ptr<const facebook::jsi::Buffer> *>(bufferToDestroy)};
      },
      bufferWrapper.get(),
      &arrayBuffer));

  // We only call bufferWrapper.release() after JsCreateExternalObject succeeds.
  // Otherwise, when JsCreateExternalObject fails and an exception is thrown,
  // the shared_ptr that bufferWrapper used to own will be leaked.
  bufferWrapper.release();
  return ChakraObjectRef(arrayBuffer);
}

bool CompareJsValues(const ChakraObjectRef &jsValue1, const ChakraObjectRef &jsValue2) {
  bool result = false;
  // Note that JsStrictEquals should only be used for JsValueRefs and not for
  // other types of JsRefs (e.g. JsPropertyIdRef, etc.).
  VerifyChakraErrorElseThrow(JsStrictEquals(jsValue1, jsValue2, &result));
  return result;
}

bool CompareJsPropertyIds(const ChakraObjectRef &jsPropId1, const ChakraObjectRef &jsPropId2) {
  JsPropertyIdType type1 = GetPropertyIdType(jsPropId1);
  JsPropertyIdType type2 = GetPropertyIdType(jsPropId2);

  if (type1 != type2) {
    return false;
  }

  if (type1 == JsPropertyIdTypeString) {
    return GetPropertyName(jsPropId1) == GetPropertyName(jsPropId2);
  }

  if (type1 == JsPropertyIdTypeSymbol) {
    return CompareJsValues(GetPropertySymbol(jsPropId1), GetPropertySymbol(jsPropId2));
  }

  // Control should never reach here.
  std::terminate();
}

void ThrowJsException(const std::string_view &message) {
  JsValueRef error = JS_INVALID_REFERENCE;
  VerifyChakraErrorElseThrow(JsCreateError(ToJsString(message), &error));
  VerifyChakraErrorElseThrow(JsSetException(error));
}

} // namespace Microsoft::JSI
