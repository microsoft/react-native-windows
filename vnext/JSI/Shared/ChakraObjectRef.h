// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#ifdef CHAKRACORE
#include "ChakraCore.h"
#else
#ifndef USE_EDGEMODE_JSRT
#define USE_EDGEMODE_JSRT
#endif
#include <jsrt.h>
#endif

#include <exception>

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

} // namespace Microsoft::JSI
