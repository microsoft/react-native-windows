// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once
#include "ReactError.h"
#include "winrt/Microsoft.ReactNative.h"

namespace winrt::Microsoft::ReactNative {

// ReactPromise represents a JavaScript Promise
// which can be passed to the native module as a method parameter.
//
// Methods with REACT_METHOD attribute that use an ReactPromise as the last parameter
// will be marked as "promise" and will return a promise when invoked from JavaScript.

// Base class that does not depend on template parameter T
struct ReactPromiseBase {
  ReactPromiseBase(
      IJSValueWriter const &writer,
      MethodResultCallback const &resolve,
      MethodResultCallback const &reject) noexcept;

  // Report an Error.
  void Reject(ReactError &&error) noexcept;

  void Reject(const char *errorMessage) noexcept;
  void Reject(const wchar_t *errorMessage) noexcept;

 protected:
  void Clear() noexcept;

 protected:
  IJSValueWriter m_writer;
  MethodResultCallback m_resolve;
  MethodResultCallback m_reject;
};

template <class T>
struct ReactPromise : ReactPromiseBase {
  using ReactPromiseBase::ReactPromiseBase;

  // Successfully resolve the IReactPromise with an optional value.
  void Resolve(T const &value) noexcept;
};

template <>
struct ReactPromise<void> : ReactPromiseBase {
  using ReactPromiseBase::ReactPromiseBase;

  // Successfully resolve the IReactPromise with an optional value.
  void Resolve() noexcept;
};

// Successfully resolve the ReactPromise with an optional value.
template <class T>
void ReactPromise<T>::Resolve(T const &value) noexcept {
  if (m_resolve) {
    WriteArgs(m_writer, value);
    m_resolve(m_writer);
    Clear();
  }
}

} // namespace winrt::Microsoft::ReactNative
