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

ChakraObjectRef::ChakraObjectRef(const ChakraObjectRef &original) noexcept
    : m_ref{original.m_ref}, m_state{original.m_state} {
  if (m_state == State::Initialized) {
    assert(m_ref);
    CheckedJsAddRef(m_ref);
  } else {
    assert(!m_ref);
  }
}

ChakraObjectRef::ChakraObjectRef(ChakraObjectRef &&original) noexcept {
  swap(original);
}

ChakraObjectRef &ChakraObjectRef::operator=(
    const ChakraObjectRef &rhs) noexcept {
  ChakraObjectRef rhsCopy(rhs);
  swap(rhsCopy);
  return *this;
}

ChakraObjectRef &ChakraObjectRef::operator=(ChakraObjectRef &&rhs) noexcept {
  swap(rhs);
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

void ChakraObjectRef::swap(ChakraObjectRef &other) {
  std::swap(m_ref, other.m_ref);
  std::swap(m_state, other.m_state);
}

JsValueType GetValueType(const ChakraObjectRef &jsValue) {
  JsValueType type;
  ThrowUponChakraError(JsGetValueType(jsValue, &type), "JsGetValueType");
  return type;
}

JsPropertyIdType GetPropertyIdType(const ChakraObjectRef &jsPropId) {
  JsPropertyIdType type;
  ThrowUponChakraError(
      JsGetPropertyIdType(jsPropId, &type), "JsGetPropertyIdType");
  return type;
}

std::wstring GetPropertyName(const ChakraObjectRef &id) {
  if (GetPropertyIdType(id) != JsPropertyIdTypeString) {
    throw facebook::jsi::JSINativeException(
        "It is llegal to retrieve the name of a property symbol.");
  }
  const wchar_t *propertyName = nullptr;
  ThrowUponChakraError(
      JsGetPropertyNameFromId(id, &propertyName), "JsGetPropertyNameFromId");
  return std::wstring{propertyName};
}

ChakraObjectRef GetPropertySymbol(const ChakraObjectRef &id) {
  if (GetPropertyIdType(id) != JsPropertyIdTypeSymbol) {
    throw facebook::jsi::JSINativeException(
        "It is llegal to retrieve the symbol associated with a property name.");
  }
  JsValueRef symbol = nullptr;
  ThrowUponChakraError(
      JsGetSymbolFromPropertyId(id, &symbol), "JsGetSymbolFromPropertyId");
  return ChakraObjectRef{symbol};
}

ChakraObjectRef GetPropertyId(const char *const utf8, size_t length) {
  assert(utf8);

  // We use a #ifdef here because we can avoid a UTF-8 to UTF-16 conversion
  // using ChakraCore's JsCreatePropertyId API.
#ifdef CHAKRACORE
  JsPropertyIdRef id = nullptr;
  ThrowUponChakraError(
      JsCreatePropertyId(utf8, length, &id), "JsCreatePropertyId");
  return ChakraObjectRef(id);

#else
  std::wstring utf16 = Common::Unicode::Utf8ToUtf16(utf8, length);
  return GetPropertyId(utf16);
#endif
}

ChakraObjectRef GetPropertyId(const std::wstring &utf16) {
  JsPropertyIdRef id = nullptr;
  ThrowUponChakraError(
      JsGetPropertyIdFromName(utf16.c_str(), &id), "JsGetPropertyIdFromName");
  return ChakraObjectRef(id);
}

std::string ToStdString(const ChakraObjectRef &jsString) {
  // We use a #ifdef here because we can avoid a UTF-8 to UTF-16 conversion
  // using ChakraCore's JsCopyString API.
#ifdef CHAKRACORE
  size_t length = 0;
  ThrowUponChakraError(
      JsCopyString(jsString, nullptr, 0, &length), "JsCopyString");
  std::string result(length, 'a');
  ThrowUponChakraError(
      JsCopyString(jsString, result.data(), result.length(), &length),
      "JsCopyString");
  assert(length == result.length());
  return result;

#else
  return Common::Unicode::Utf16ToUtf8(ToStdWstring(jsString));
#endif
}

std::wstring ToStdWstring(const ChakraObjectRef &jsString) {
  assert(GetValueType(jsString) == JsString);

  const wchar_t *utf16 = nullptr;
  size_t length = 0;
  ThrowUponChakraError(
      JsStringToPointer(jsString, &utf16, &length), "JsStringToPointer");

  return std::wstring(utf16, length);
}

ChakraObjectRef ToJsString(const char *utf8, size_t length) {
  assert(utf8);

  // We use a #ifdef here because we can avoid a UTF-8 to UTF-16 conversion
  // using ChakraCore's JsCreateString API.
#ifdef CHAKRACORE
  JsValueRef result = nullptr;
  ThrowUponChakraError(JsCreateString(utf8, length, &result), "JsCreateString");
  return ChakraObjectRef(result);

#else
  std::wstring utf16 = Common::Unicode::Utf8ToUtf16(utf8, length);
  return ToJsString(utf16.c_str(), utf16.length());
#endif
}

ChakraObjectRef ToJsString(const wchar_t *const utf16, size_t length) {
  assert(utf16);
  JsValueRef result = nullptr;
  ThrowUponChakraError(
      JsPointerToString(utf16, length, &result), "JsPointerToString");
  return ChakraObjectRef(result);
}

ChakraObjectRef ToJsString(const ChakraObjectRef &ref) {
  JsValueRef str = nullptr;
  ThrowUponChakraError(
      JsConvertValueToString(ref, &str), "JsConvertValueToString");
  return ChakraObjectRef(str);
}

ChakraObjectRef ToJsNumber(int num) {
  JsValueRef result = nullptr;
  ThrowUponChakraError(JsIntToNumber(num, &result), "JsIntToNumber");
  return ChakraObjectRef(result);
}

ChakraObjectRef ToJsArrayBuffer(
    const std::shared_ptr<const facebook::jsi::Buffer> &buffer) {
  assert(buffer);

  size_t size = buffer->size();
  assert(size < UINT_MAX);

  JsValueRef arrayBuffer = nullptr;
  auto bufferWrapper =
      std::make_unique<std::shared_ptr<const facebook::jsi::Buffer>>(buffer);

  // We allocate a copy of buffer on the heap, a shared_ptr which is deleted
  // when the JavaScript garbage collecotr releases the created external array
  // buffer. This ensures that buffer stays alive while the JavaScript engine is
  // using it.
  ThrowUponChakraError(
      JsCreateExternalArrayBuffer(
          Common::Utilities::CheckedReinterpretCast<char *>(
              const_cast<uint8_t *>(buffer->data())),
          static_cast<unsigned int>(size),
          [](void *bufferToDestroy) {
            // We wrap bufferToDestroy in a unique_ptr to avoid calling delete
            // explicitly.
            std::unique_ptr<std::shared_ptr<const facebook::jsi::Buffer>>
                wrapper{
                    static_cast<std::shared_ptr<const facebook::jsi::Buffer> *>(
                        bufferToDestroy)};
          },
          bufferWrapper.get(),
          &arrayBuffer),
      "JsCreateExternalArrayBuffer");

  bufferWrapper.release();
  return ChakraObjectRef(arrayBuffer);
}

bool CompareJsValues(
    const ChakraObjectRef &jsValue1,
    const ChakraObjectRef &jsValue2) {
  bool result = false;
  // Note that JsStrictEquals should only be used for JsValueRefs and not for
  // other types of JsRefs (e.g. JsPropertyIdRef, etc.).
  ThrowUponChakraError(
      JsStrictEquals(jsValue1, jsValue2, &result), "JsStrictEquals");
  return result;
}

bool CompareJsPropertyIds(
    const ChakraObjectRef &jsPropId1,
    const ChakraObjectRef &jsPropId2) {
  JsPropertyIdType type1 = GetPropertyIdType(jsPropId1);
  JsPropertyIdType type2 = GetPropertyIdType(jsPropId2);

  if (type1 != type2) {
    return false;
  }

  if (type1 == JsPropertyIdTypeString) {
    assert(type2 == JsPropertyIdTypeString);
    return GetPropertyName(jsPropId1) == GetPropertyName(jsPropId2);
  }

  if (type1 == JsPropertyIdTypeSymbol) {
    assert(type2 == JsPropertyIdTypeSymbol);
    return CompareJsValues(
        GetPropertySymbol(jsPropId1), GetPropertySymbol(jsPropId2));
  }

  // Control should never reach here.
  std::terminate();
}

} // namespace Microsoft::JSI
