// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include "jsi/jsi.h"

#ifdef CHAKRACORE
#include "ChakraCore.h"
#else
#ifndef USE_EDGEMODE_JSRT
#define USE_EDGEMODE_JSRT
#endif
#include <jsrt.h>
#endif

#include <exception>
#include <string>
#include <string_view>

namespace Microsoft::JSI {

inline void VerifyChakraErrorElseCrash(JsErrorCode error) {
  if (error != JsNoError) {
    std::terminate();
  }
}

void VerifyChakraErrorElseThrow(JsErrorCode error);

/**
 * @brief An shared_ptr like RAII Wrapper for JsRefs.
 *
 * JsRefs are references to objects owned by the garbage collector and include
 * JsContextRef, JsValueRef, and JsPropertyIdRef, etc. ChakraObjectRef ensures
 * that JsAddRef and JsRelease are called upon initialization and invalidation,
 * respectively. It also allows users to implicitly convert it into a JsRef. A
 * ChakraObjectRef must only be initialized once and invalidated once.
 */
class ChakraObjectRef {
 public:
  ChakraObjectRef() noexcept {}
  inline explicit ChakraObjectRef(JsRef ref) {
    Initialize(ref);
  }

  ChakraObjectRef(const ChakraObjectRef &original) noexcept;
  ChakraObjectRef(ChakraObjectRef &&original) noexcept;

  ChakraObjectRef &operator=(const ChakraObjectRef &rhs) noexcept;
  ChakraObjectRef &operator=(ChakraObjectRef &&rhs) noexcept;

  ~ChakraObjectRef() noexcept;

  void Initialize(JsRef ref);
  void Invalidate();

  inline operator JsRef() const noexcept {
    return m_ref;
  }

 private:
  void Swap(ChakraObjectRef &other);

  enum class State { Uninitialized, Initialized, Invalidated };

  JsRef m_ref = nullptr;
  State m_state = State::Uninitialized;
};

/**
 * @param jsValue A ChakraObjectRef managing a JsValueRef.
 */
JsValueType GetValueType(const ChakraObjectRef &jsValue);

/**
 * @param jsPropId A ChakraObjectRef managing a JsPropertyIdRef.
 */
JsPropertyIdType GetPropertyIdType(const ChakraObjectRef &jsPropId);

/**
 * @param jsPropId A ChakraObjectRef managing a JsPropertyIdRef of type
 * JsPropertyIdTypeString.
 */
std::wstring GetPropertyName(const ChakraObjectRef &jsPropId);

/**
 * @param jsPropId A ChakraObjectRef managing a JsPropertyIdRef of type
 * JsPropertyIdTypeSymbol.
 *
 * @returns A ChakraObjectRef managing a JS Symbol.
 */
ChakraObjectRef GetPropertySymbol(const ChakraObjectRef &jsPropId);

/**
 * @param utf8 A std::string_view to a UTF-8 encoded char array.
 *
 * @returns A ChakraObjectRef managing a JsPropertyIdRef.
 */
ChakraObjectRef GetPropertyId(const std::string_view &utf8);

/**
 * @param utf16 A UTF-16 encoded std::wstring.
 *
 * @returns A ChakraObjectRef managing a JsPropertyIdRef.
 */
ChakraObjectRef GetPropertyId(const std::wstring &utf16);

/**
 * @param jsString A ChakraObjectRef managing a JS string.
 *
 * @returns A std::string that is UTF-8 encoded.
 *
 * @remarks This function copies the JS string buffer into the returned
 * std::string. When using Chakra instead of ChakraCore, this function incurs
 * a UTF-16 to UTF-8 conversion.
 */
std::string ToStdString(const ChakraObjectRef &jsString);

/**
 * @param jsString A ChakraObjectRef managing a JS string.
 *
 * @returns A std::wstring that is UTF-16 encoded.
 *
 * @remarks This functions copies the JS string buffer into the returned
 * std::wstring.
 */
std::wstring ToStdWstring(const ChakraObjectRef &jsString);

/**
 * @param utf8 A std::string_view to a UTF-8 encoded char array.
 *
 * @returns A ChakraObjectRef managing a JS string.
 *
 * @remarks The content of utf8 is copied into JS engine owned memory. When
 * using Chakra instead of ChakraCore, this function incurs a UTF-8 to UTF-16
 * conversion.
 */
ChakraObjectRef ToJsString(const std::string_view &utf8);

/**
 * @param utf16 A std::wstring_view to a UTF-16 encoded wchar_t array.
 *
 * @returns A ChakraObjectRef managing a JS string.
 *
 * @remarks The content of utf16 is copied into JS engine owned memory.
 */
ChakraObjectRef ToJsString(const std::wstring_view &utf16);

/**
 * @param jsValue A ChakraObjectRef mananing a JsValueRef.
 *
 * @returns A ChakraObjectRef managing the return value of the JS .toString
 * function.
 */
ChakraObjectRef ToJsString(const ChakraObjectRef &jsValue);

/**
 * @returns A ChakraObjectRef managing a JS number.
 */
ChakraObjectRef ToJsNumber(int num);

/**
 * @returns A ChakraObjectRef managing a JS Object.
 *
 * @remarks The returned Object is backed by data and keeps data alive till the
 * garbage collector finalizes it.
 */
template <typename T>
ChakraObjectRef ToJsObject(std::unique_ptr<T> &&data) {
  if (!data) {
    throw facebook::jsi::JSINativeException("Cannot create an external JS Object without backing data.");
  }

  JsValueRef obj = nullptr;
  VerifyChakraErrorElseThrow(JsCreateExternalObject(
      data.get(),
      [](void *dataToDestroy) {
        // We wrap dataToDestroy in a unique_ptr to avoid calling delete
        // explicitly.
        std::unique_ptr<T> wrapper{static_cast<T *>(dataToDestroy)};
      },
      &obj));

  // We only call data.release() after JsCreateExternalObject succeeds.
  // Otherwise, when JsCreateExternalObject fails and an exception is thrown,
  // the buffer that data used to own will be leaked.
  data.release();
  return ChakraObjectRef(obj);
}

/**
 * @returns A ChakraObjectRef managing a JS ArrayBuffer.
 *
 * @remarks The returned ArrayBuffer is backed by buffer and keeps buffer alive
 * till the garbage collector finalizes it.
 */
ChakraObjectRef ToJsArrayBuffer(const std::shared_ptr<const facebook::jsi::Buffer> &buffer);

/**
 * @param jsValue1 A ChakraObjectRef managing a JsValueRef.
 * @param jsValue2 A ChakraObjectRef managing a JsValueRef.
 *
 * @returns A boolean indicating whether jsValue1 and jsValue2 are strictly
 * equal.
 */
bool CompareJsValues(const ChakraObjectRef &jsValue1, const ChakraObjectRef &jsValue2);

/**
 * @param jsPropId1 A ChakraObjectRef managing a JsPropertyIdRef.
 * @param jsPropId2 A ChakraObjectRef managing a JsPropertyIdRef.
 *
 * @returns A boolean indicating whether jsPropId1 and jsPropId2 are strictly
 * equal.
 */
bool CompareJsPropertyIds(const ChakraObjectRef &jsPropId1, const ChakraObjectRef &jsPropId2);

} // namespace Microsoft::JSI
