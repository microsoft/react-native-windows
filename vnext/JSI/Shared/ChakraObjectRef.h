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
JsValueType getValueType(const ChakraObjectRef &jsValue);

// jsPropId must be managing a JsPropertyIdRef.
JsPropertyIdType getPropertyIdType(const ChakraObjectRef &jsPropId);

// jsPropId must be managing a JsPropertyIdRef of type JsPropertyIdTypeString.
std::wstring getPropertyName(const ChakraObjectRef &jsPropId);

// jsPropId must be managing a JsPropertyIdRef of type JsPropertyIdTypeSymbol.
// Returns a ChakraObjectRef managing a JsValueRef pointing to a JS Symbol.
ChakraObjectRef getPropertySymbol(const ChakraObjectRef &jsPropId);

// name must be null terminated.
ChakraObjectRef getPropertyId(const wchar_t *const name);

// jsString must be managing a JsValueRef pointing to a JS string. The returned
// std::string/std::wstring is UTF-8/UTF-16 encoded. These functions copy the JS
// string buffer into the returned std::string/std::wstring.
std::string toStdString(const ChakraObjectRef &jsString);
std::wstring toStdWstring(const ChakraObjectRef &jsString);

// Returns a ChakraObjectRef managing a JsValueRef pointing to a JS string.
// utf8/utf16 must point to a UTF-8/UTF-16 encoded buffer. These buffers do not
// have to be null terminated and are copied to JS engine owned memory.
ChakraObjectRef toJsString(const char *const utf8, size_t length);
ChakraObjectRef toJsString(const wchar_t *const utf16, size_t length);

// jsValue must be mananing a JsValueRef. Returns a ChakraObjectRef managing a
// JsValueRef that points the return value of the JS .toString function.
ChakraObjectRef toJsString(const ChakraObjectRef &jsValue);

// Returns a ChakraObjectRef managing a JsValueRef pointing to a JS ArrayBuffer.
// This ArrayBuffer is backed by buffer and keeps buffer alive till the garbage
// collector finalizes it.
ChakraObjectRef toJsExternalArrayBuffer(
    const std::shared_ptr<const facebook::jsi::Buffer> &buffer);

// Both jsValue1 and jsValue2 must be managing a JsValueRef. Returns whether
// jsValue1 anf jsValue2 are strictly equal.
bool compareJsValues(
    const ChakraObjectRef &jsValue1,
    const ChakraObjectRef &jsValue2);

// Both jsPropId1 and jsPropId2 must be managing a JsPropertyIdRef.
bool compareJsPropertyIds(
    const ChakraObjectRef &jsPropId1,
    const ChakraObjectRef &jsPropId2);

} // namespace Microsoft::JSI
