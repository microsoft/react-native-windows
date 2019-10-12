// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "ChakraObjectRef.h"

#include "Unicode.h"

#include "jsi//jsi.h"

#include <sstream>
#include <utility>

namespace Microsoft::JSI {

namespace {

inline void CheckedJsAddRef(JsRef ref) {
  ThrowUponChakraError(JsAddRef(ref, nullptr), "JsAddRef");
}

inline void CheckedJsRelease(JsRef ref) {
  ThrowUponChakraError(JsRelease(ref, nullptr), "JsRelease");
}

} // namespace

void ThrowUponChakraError(JsErrorCode error, const char *const chakraApiName) {
  if (error != JsNoError) {
    std::ostringstream errorString;
    errorString << "A call to "
                << (chakraApiName ? chakraApiName : "Chakra(Core) API")
                << " returned error code 0x" << std::hex << error << '.';
    throw facebook::jsi::JSINativeException(errorString.str());
  }
}

ChakraObjectRef::ChakraObjectRef(const ChakraObjectRef &other) noexcept
    : m_ref{other.m_ref}, m_state{other.m_state} {
  if (m_state == State::Initialized) {
    assert(m_ref);
    CheckedJsAddRef(m_ref);
  } else {
    assert(!m_ref);
  }
}

ChakraObjectRef::ChakraObjectRef(ChakraObjectRef &&other) noexcept {
  std::swap(*this, other);
}

ChakraObjectRef &ChakraObjectRef::operator=(
    const ChakraObjectRef &rhs) noexcept {
  ChakraObjectRef rhsCopy(rhs);
  std::swap(*this, rhsCopy);
  return *this;
}

ChakraObjectRef &ChakraObjectRef::operator=(ChakraObjectRef &&rhs) noexcept {
  std::swap(*this, rhs);
  return *this;
}

ChakraObjectRef::~ChakraObjectRef() noexcept {
  if (m_state == State::Initialized) {
    assert(m_ref);
    CheckedJsRelease(m_ref);
  } else {
    assert(!m_ref);
  }
}

void ChakraObjectRef::Initialize(JsRef ref) {
  assert(!m_ref);

  if (m_state != State::Uninitialized) {
    throw facebook::jsi::JSINativeException(
        "A ChakraObjectRef can only be initialzed once.");
  }

  if (!ref) {
    throw facebook::jsi::JSINativeException(
        "Cannot initialize a ChakraObjectRef with a null reference.");
  }

  CheckedJsAddRef(ref);
  m_ref = ref;
  m_state = State::Initialized;
}

void ChakraObjectRef::Invalidate() {
  switch (m_state) {
    case State::Uninitialized: {
      assert(!m_ref);
      throw facebook::jsi::JSINativeException(
          "Cannot invalidate a ChakraObjectRef that has not been initialized.");
      break;
    }
    case State::Initialized: {
      assert(m_ref);
      CheckedJsRelease(m_ref);
      m_ref = nullptr;
      m_state = State::Invalidated;
      break;
    }
    case State::Invalidated: {
      assert(!m_ref);
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

JsValueType getValueType(const ChakraObjectRef &jsValue) {
  JsValueType type;
  ThrowUponChakraError(JsGetValueType(jsValue, &type), "JsGetValueType");
  return type;
}

JsPropertyIdType getPropertyIdType(const ChakraObjectRef &jsPropId) {
  JsPropertyIdType type;
  ThrowUponChakraError(
      JsGetPropertyIdType(jsPropId, &type), "JsGetPropertyIdType");
  return type;
}

std::wstring getPropertyName(const ChakraObjectRef &id) {
  if (getPropertyIdType(id) != JsPropertyIdTypeString) {
    throw facebook::jsi::JSINativeException(
        "It is llegal to retrieve the name of a property symbol.");
  }
  const wchar_t *propertyName;
  ThrowUponChakraError(
      JsGetPropertyNameFromId(id, &propertyName), "JsGetPropertyNameFromId");
  return std::wstring{propertyName};
}

ChakraObjectRef getPropertySymbol(const ChakraObjectRef &id) {
  if (getPropertyIdType(id) != JsPropertyIdTypeSymbol) {
    throw facebook::jsi::JSINativeException(
        "It is llegal to retrieve the symbol associated with a property name.");
  }
  JsValueRef symbol = nullptr;
  ThrowUponChakraError(
      JsGetSymbolFromPropertyId(id, &symbol), "JsGetSymbolFromPropertyId");
  return ChakraObjectRef{symbol};
}

ChakraObjectRef getPropertyId(const wchar_t *const name) {
  JsPropertyIdRef id = nullptr;
  ThrowUponChakraError(
      JsGetPropertyIdFromName(name, &id), "JsGetPropertyIdFromName");
  return ChakraObjectRef(id);
}

std::string toStdString(const ChakraObjectRef &jsString) {
  // TODO (yicyao)
  return "";
}

std::wstring toStdWstring(const ChakraObjectRef &jsString) {
  assert(getValueType(jsString) == JsString);

  const wchar_t *utf16;
  size_t length;
  ThrowUponChakraError(
      JsStringToPointer(jsString, &utf16, &length), "JsStringToPointer");

  return std::wstring(utf16, length);
}

ChakraObjectRef toJsString(const char *utf8, size_t length) {
  JsValueRef result;
  // We use a #ifdef here because we can avoid a UTF-8 to UTF-16 conversion
  // using ChakraCore's JsCreateString API.
#ifdef CHAKRACORE
  ThrowUponChakraError(JsCreateString(utf8, length, &result), "JsCreateString");
#else
  std::wstring utf16 = Common::Unicode::Utf8ToUtf16(utf8, length);
  ThrowUponChakraError(
      JsPointerToString(utf16.c_str(), utf16.length(), &result),
      "JsPointerToString");
#endif
  return ChakraObjectRef(result);
}

ChakraObjectRef toJsString(const wchar_t *const utf16, size_t length) {
  // TODO (yicyao)
  return ChakraObjectRef{};
}

ChakraObjectRef toJsString(const ChakraObjectRef &ref) {
  JsValueRef str = nullptr;
  ThrowUponChakraError(
      JsConvertValueToString(ref, &str), "JsConvertValueToString");
  return ChakraObjectRef(str);
}

ChakraObjectRef toJsExternalArrayBuffer(
    const std::shared_ptr<const facebook::jsi::Buffer> &buffer) {
  // TODO (yicyao)
  return ChakraObjectRef{};
}

bool compareJsValues(
    const ChakraObjectRef &jsValue1,
    const ChakraObjectRef &jsValue2) {
  bool result = false;
  // Note that JsStrictEquals should only be used for JsValueRefs and not for
  // other types of JsRefs (e.g. JsPropertyIdRef, etc.).
  ThrowUponChakraError(
      JsStrictEquals(jsValue1, jsValue2, &result), "JsStrictEquals");
  return result;
}

bool compareJsPropertyIds(
    const ChakraObjectRef &jsPropId1,
    const ChakraObjectRef &jsPropId2) {
  JsPropertyIdType type1 = getPropertyIdType(jsPropId1);
  JsPropertyIdType type2 = getPropertyIdType(jsPropId2);

  if (type1 != type2) {
    return false;
  }

  if (type1 == JsPropertyIdTypeString) {
    assert(type2 == JsPropertyIdTypeString);
    return getPropertyName(jsPropId1) == getPropertyName(jsPropId2);
  }

  if (type1 == JsPropertyIdTypeSymbol) {
    assert(type2 == JsPropertyIdTypeSymbol);
    return compareJsValues(
        getPropertySymbol(jsPropId1), getPropertySymbol(jsPropId2));
  }

  // Control should never reach here.
  std::terminate();
}

} // namespace Microsoft::JSI
