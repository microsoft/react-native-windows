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

namespace Microsoft::JSI {

inline void CrashUponChakraError(JsErrorCode error) {
  if (error != JsNoError) {
    std::terminate();
  }
}

void ThrowUponChakraError(
    JsErrorCode error,
    const char *const chakraApiName = nullptr);

// An shared_ptr like RAII Wrapper for JsRefs, which are references to objects
// owned by the garbage collector. These include JsContextRef, JsValueRef, and
// JsPropertyIdRef, etc. ChakraObjectRef ensures that JsAddRef and JsRelease
// are called upon initialization and invalidation, respectively. It also allows
// users to implicitly convert it into a JsRef. A ChakraObjectRef must only be
// initialized once and invalidated once.
class ChakraObjectRef {
 public:
  ChakraObjectRef() noexcept {}
  inline explicit ChakraObjectRef(JsRef ref) {
    Initialize(ref);
  }

  ChakraObjectRef(const ChakraObjectRef &other) noexcept;
  ChakraObjectRef(ChakraObjectRef &&other) noexcept;

  ChakraObjectRef &operator=(const ChakraObjectRef &rhs) noexcept;
  ChakraObjectRef &operator=(ChakraObjectRef &&rhs) noexcept;

  ~ChakraObjectRef() noexcept;

  void Initialize(JsRef ref);
  void Invalidate();

  inline operator JsRef() const noexcept {
    return m_ref;
  }

 private:
  enum class State { Uninitialized, Initialized, Invalidated };

  JsRef m_ref = nullptr;
  State m_state = State::Uninitialized;
};

// jsValue must be managing a JsValueRef.
JsValueType GetValueType(const ChakraObjectRef &jsValue);

// jsPropId must be managing a JsPropertyIdRef.
JsPropertyIdType GetPropertyIdType(const ChakraObjectRef &jsPropId);

// jsPropId must be managing a JsPropertyIdRef of type JsPropertyIdTypeString.
std::wstring GetPropertyName(const ChakraObjectRef &jsPropId);

// jsPropId must be managing a JsPropertyIdRef of type JsPropertyIdTypeSymbol.
// Returns a ChakraObjectRef managing a JsValueRef pointing to a JS Symbol.
ChakraObjectRef GetPropertySymbol(const ChakraObjectRef &jsPropId);

// utf8 does not have to be null terminated.
ChakraObjectRef GetPropertyId(const char *const utf8, size_t length);

ChakraObjectRef GetPropertyId(const std::wstring &utf16);

// jsString must be managing a JsValueRef pointing to a JS string. The returned
// std::string/std::wstring is UTF-8/UTF-16 encoded. These functions copy the JS
// string buffer into the returned std::string/std::wstring.
std::string ToStdString(const ChakraObjectRef &jsString);
std::wstring ToStdWstring(const ChakraObjectRef &jsString);

// Returns a ChakraObjectRef managing a JsValueRef pointing to a JS string.
// utf8 and utf16 do not have to be null terminated and are copied to JS engine
// owned memory.
ChakraObjectRef ToJsString(const char *const utf8, size_t length);
ChakraObjectRef ToJsString(const wchar_t *const utf16, size_t length);

// jsValue must be mananing a JsValueRef. Returns a ChakraObjectRef managing a
// JsValueRef that points the return value of the JS .toString function.
ChakraObjectRef ToJsString(const ChakraObjectRef &jsValue);

// Returns a ChakraObjectRef managing a JsValueRef pointing to a JS number.
ChakraObjectRef ToJsNumber(int num);

// Returns a ChakraObjectRef managing a JsValueRef pointing to a JS ArrayBuffer.
// This ArrayBuffer is backed by buffer and keeps buffer alive till the garbage
// collector finalizes it.
ChakraObjectRef ToJsExternalArrayBuffer(
    const std::shared_ptr<const facebook::jsi::Buffer> &buffer);

// Both jsValue1 and jsValue2 must be managing a JsValueRef. Returns whether
// jsValue1 anf jsValue2 are strictly equal.
bool CompareJsValues(
    const ChakraObjectRef &jsValue1,
    const ChakraObjectRef &jsValue2);

// Both jsPropId1 and jsPropId2 must be managing a JsPropertyIdRef.
bool CompareJsPropertyIds(
    const ChakraObjectRef &jsPropId1,
    const ChakraObjectRef &jsPropId2);

} // namespace Microsoft::JSI
