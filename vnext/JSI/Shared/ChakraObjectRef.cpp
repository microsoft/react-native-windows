// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "ChakraObjectRef.h"

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

} // namespace Microsoft::JSI
