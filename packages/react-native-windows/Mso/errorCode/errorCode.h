// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

#pragma once
#ifndef MSO_ERRORCODE_ERRORCODE_H
#define MSO_ERRORCODE_ERRORCODE_H

#include <guiddef.h>
#include <exception>
#include <type_traits>
#include <utility>
#include "comUtil/IUnknownShim.h"
#include "compilerAdapters/cppMacrosDebug.h"
#include "compilerAdapters/declspecDefinitions.h"
#include "compilerAdapters/functionDecorations.h"
#include "guid/msoGuid.h"
#include "smartPtr/cntPtr.h"

#if MS_TARGET_APPLE // VSO#2909557
#pragma pack(push, 8)
#else
#pragma pack(push, _CRT_PACKING)
#endif

namespace Mso {

struct IErrorProvider;
struct ErrorCodeState;

/*
  ErrorCode provides error facilities similar to C++11 error_code adopted to Office environment
  Office has a lot of components all of which can produce errors. We cannot rely on a fixed list
  of categories defined in a common place when defining ErrorCode. However we still want a lightweight
  error object which can carry rich information about an error.

  Each component that wants to use ErrorCode will define one or more IErrorProvider objects.
  It is recommended to specialize the ErrorProvider template rather than implementing this interface directly.
  It typically involves defining a GUID for an error type and providing a function that returns the error
  provider singleton:

    MSO_CLASS_GUID(ExceptionErrorProviderGuid, "abd53c35-105d-4e67-8747-8473d9c80507")
    LIBLET_PUBLICAPI const ErrorProvider<std::exception_ptr, ExceptionErrorProviderGuid>& ExceptionErrorProvider()
  noexcept;

  The ErrorProvider template is a final class. Instead of inheriting from it you should specialize methods
  you want to override:

    template <>
    LIBLET_PUBLICAPI void ErrorProvider<std::exception_ptr, ExceptionErrorProviderGuid>::Throw(const ErrorCode&
  errorCode, bool shouldHandle) const;

    template <>
    LIBLET_PUBLICAPI Mso::CntPtr<Mso::IErrorString> ErrorProvider<std::exception_ptr,
  ExceptionErrorProviderGuid>::ToString(const ErrorCode& errorCode) const
    {
      return Mso::MakeErrorString("Detailed Error Message");
    }

  The ErrorCode contains Mso::CntPtr<ErrorCodeState>. The ref counted ErrorCodeState has a flag
  indicating whether the error was handled or not. The ErrorProvider has OnUnhandledError method
  that is run if error is not handled before the ErrorCodeState destruction.
  By default we are going to show ShipAssert, but each ErrorProvider instantiation may provide
  a different implementation for that method:

    // Override OnUnhandledError to avoid ShipAssert
    template <>
    void ErrorProvider<MyData, MyDataGuid>::OnUnhandledError(const ErrorCodeState& state) const noexcept
    {
      UNREFERENCED_OACR(state); UNREFERENCED_OACR(this);
    }

  ErrorCode provides a number of methods to handle errors:
  - Handle() - it handles the method regardless of its type.
  - HandleAs(errorProvider) - it handles error corresponding to the provided errorProvider instance.
    it returns a pointer to the provider specific error info when succeeded, or nullptr otherwise.
    this method allows to probe ErrorCode for known errors.
  - HandleAndThrow() - handles any type of error, and throws an exception associated with the error.
    By default ErrorProvider thrown ErrorCodeException, but the template specializations can provide
    custom Throw() method implementation.
  - HandleAndThrowAs(errorProvider) - handles specific error type and throws exception if type matches.

  ErrorCode has also methods that do not handle the error:
  - IsHandled() - returns value of handled flag.
  - Throw() - throws exception associated with the ErrorCode. If ErrorCode state is empty, no exception is thrown.
  - As(errorProvider) - returns error info for the specific errorProvider if error type matches,
    otherwise it returns nullptr.
  - ThrowAs(errorProvider) - throws exception associated with the ErrorCode if it matches the errorProvider.

  This is how calls to the HandleAs method can be chained to process known error types:

    Mso::ErrorCode error = AMethodThatReturnsErrorCode();

    if (const std::exception_ptr* exceptionError = error.HandleAs(Mso::ExceptionErrorProvider()))
    {
      // Handle exceptionError
    }
    else if (const HRESULT* hResultError = error.HandleAs(Mso::HResultErrorProvider()))
    {
      // Handle hResultError
    }
    else if (const MyData* myError = error.HandleAs(MyErrorProvider()))
    {
      // Handle hResultError
    }
    else if (!error)
    {
      // Error has no state - it may indicate that there is no error
    }
    else
    {
      // Unknown error type. We can either
      // - do nothing and mark it as handled by calling Handle() method.
      // - return it,
      // - throw associated exception using HandleAndThrow() or Throw() methods.
      // - crash the app using VerifyElseCrash() macro.
    }
*/
class ErrorCode final {
 public:
  //! Creates new ErrorCode with empty state.
  ErrorCode() = default;

  //! Creates new ErrorCode with empty state.
  ErrorCode(std::nullptr_t) noexcept;

  //! Creates new ErrorCode with provided state.
  explicit ErrorCode(Mso::CntPtr<ErrorCodeState> &&state) noexcept;

  //! Creates new ErrorCode with the same state as the other ErrorCode.
  ErrorCode(const ErrorCode &other) = default;

  //! Creates new ErrorCode with the state taken from the other ErrorCode. The other ErrorCode state becomes empty.
  ErrorCode(ErrorCode &&other) = default;

  //! Assigns the same state as the other ErrorCode.
  ErrorCode &operator=(const ErrorCode &other) = default;

  //! Assigns the same state as the other ErrorCode. The other ErrorCode state becomes empty.
  ErrorCode &operator=(ErrorCode &&other) = default;

  //! Swaps states with the other ErrorCode.
  void Swap(ErrorCode &other) noexcept;

  //! True if state is not empty.
  explicit operator bool() const noexcept;

  //! True if error was handled
  bool IsHandled() const noexcept;

  //! Handle error by setting internal IsHandled state to true.
  void Handle() const noexcept;

  //! Handle error code when it contains error created by errorProvider.
  //! It returns the pointer to the TErrorInfo if succeeded, otherwise it returns null.
  template <class TErrorProvider, class TErrorInfo = typename TErrorProvider::ErrorInfoType>
  const TErrorInfo *HandleAs(const TErrorProvider &errorProvider) const noexcept;

  //! Handle the error and throw an exception associated with the error code.
  void HandleAndThrow() const;

  //! Handle the error and throw an exception associated with the error code created by errorProvider.
  template <class TErrorProvider>
  void HandleAndThrowAs(const TErrorProvider &errorProvider) const;

  //! Gets TErrorInfo data associated with the error code created by errorProvider.
  //! It returns the pointer to the TErrorInfo if succeeded, otherwise it returns null.
  template <class TErrorProvider, class TErrorInfo = typename TErrorProvider::ErrorInfoType>
  const TErrorInfo *As(const TErrorProvider &errorProvider) const noexcept;

  //! Throw an exception associated with the error code.
  void Throw() const;

  //! Throw an exception associated with the error code created by errorProvider.
  template <class TErrorProvider>
  void ThrowAs(const TErrorProvider &errorProvider) const;

  friend ErrorCodeState *GetErrorCodeState(const ErrorCode &errorCode) noexcept;

  LIBLET_PUBLICAPI const char *ToString() const noexcept;

 private:
  Mso::CntPtr<ErrorCodeState> m_state;
};

//! std::swap specializations. Swaps states between ErrorCode instances.
void swap(ErrorCode &left, ErrorCode &right) noexcept;

//! True if two ErrorCode have the same state instance.
bool operator==(const ErrorCode &left, const ErrorCode &right) noexcept;

//! True if two ErrorCode have different state instance.
bool operator!=(const ErrorCode &left, const ErrorCode &right) noexcept;

//! True if left ErrorCode is empty.
bool operator==(const ErrorCode &left, std::nullptr_t) noexcept;

//! True if left ErrorCode is not empty.
bool operator!=(const ErrorCode &left, std::nullptr_t) noexcept;

//! True is right ErrorCode is empty.
bool operator==(std::nullptr_t, const ErrorCode &right) noexcept;

//! True is right ErrorCode is not empty.
bool operator!=(std::nullptr_t, const ErrorCode &right) noexcept;

// Interface for strings with following properties:
// * Abi compatible
// * Ref-counted
// * Immutable
// * Utf-8 Encoded
struct DECLSPEC_NOVTABLE IErrorString : Mso::IRefCounted {
  virtual const char *Data() noexcept = 0;
};

LIBLET_PUBLICAPI Mso::CntPtr<Mso::IErrorString> MakeErrorString(const char *value) noexcept;

namespace Internal {

// Holds atomic IErrorString pointer and calls Release on it in destructor.
struct ErrorStringHolder {
  ~ErrorStringHolder() noexcept {
    Mso::IErrorString *instance = m_errorString.exchange(nullptr);
    OACR_USE_PTR(instance);
    if (instance) {
      instance->Release();
    }
  }

  std::atomic<Mso::IErrorString *> *operator->() const noexcept {
    return &m_errorString;
  }

 private:
  mutable std::atomic<Mso::IErrorString *> m_errorString{nullptr};
};

} // namespace Internal

// Ref-counted state of error code.
// It has a flag indicating whether the error was handled or not
// In future we want to extend it with a pointer to parent ErrorCode and error creation context.
struct ErrorCodeState {
  ErrorCodeState(const IErrorProvider &errorProvider) noexcept;
  const IErrorProvider &ErrorProvider() const noexcept;
  bool IsHandled() const noexcept;
  void Handle() const noexcept;
  LIBLET_PUBLICAPI void AddRef() const noexcept;
  LIBLET_PUBLICAPI void Release() const noexcept;

 private:
  friend ErrorCode; // Creates m_errorString
  const IErrorProvider &m_errorProvider;
  const Internal::ErrorStringHolder m_errorString;
  mutable std::atomic<uint32_t> m_refCount{1};
  mutable std::atomic<bool> m_isHandled{false};
};

// Handles specific error type. Consider to use the default implementation of this interface: ErrorProvider<T,
// GuidType>.
struct IErrorProvider {
  // Returns true if provider is associated with the provided GUID.
  // It can be used by error providers to handle cases where provider code exists in multiple dlls.
  virtual bool IsOfType(const GUID &type) const noexcept = 0;

  virtual void Destroy(const ErrorCodeState &state) const noexcept = 0;
  virtual void Throw(const ErrorCode &errorCode, bool shouldHandle) const = 0;
  virtual Mso::CntPtr<Mso::IErrorString> ToString(const ErrorCode &errorCode) const noexcept = 0;
};

// ErrorCode can be carried as an exception by wrapping into ErrorCodeException class
class ErrorCodeException : public std::exception {
 public:
  ErrorCodeException(const ErrorCode &errorCode) noexcept : m_errorCode(errorCode) {}

  ErrorCodeException(ErrorCode &&errorCode) noexcept : m_errorCode(std::move(errorCode)) {}

  const ErrorCode &Error() const noexcept {
    return m_errorCode;
  }

 private:
  ErrorCode m_errorCode;
};

//============================================================================================
// ErrorCode implementation
//============================================================================================

inline ErrorCode::ErrorCode(std::nullptr_t) noexcept {}

inline ErrorCode::ErrorCode(Mso::CntPtr<ErrorCodeState> &&state) noexcept : m_state(std::move(state)) {}

inline void ErrorCode::Swap(ErrorCode &other) noexcept {
  using std::swap;
  swap(m_state, other.m_state);
}

inline ErrorCode::operator bool() const noexcept {
  return !m_state.IsEmpty();
}

inline bool ErrorCode::IsHandled() const noexcept {
  return m_state && m_state->IsHandled();
}

inline void ErrorCode::Handle() const noexcept {
  if (m_state) {
    m_state->Handle();
  }
}

template <class TErrorProvider, class TErrorInfo>
inline const TErrorInfo *ErrorCode::HandleAs(const TErrorProvider &errorProvider) const noexcept {
  return errorProvider.TryGetErrorInfo(*this, /*shouldHandle:*/ true);
}

inline void ErrorCode::HandleAndThrow() const {
  if (m_state) {
    m_state->ErrorProvider().Throw(*this, /*shouldHandle:*/ true);
  }
}

template <class TErrorProvider>
inline void ErrorCode::HandleAndThrowAs(const TErrorProvider &errorProvider) const {
  errorProvider.Throw(m_state.Get(), /*shouldHandle:*/ true);
}

template <class TErrorProvider, class TErrorInfo>
inline const TErrorInfo *ErrorCode::As(const TErrorProvider &errorProvider) const noexcept {
  return errorProvider.TryGetErrorInfo(*this, /*shouldHandle:*/ false);
}

inline void ErrorCode::Throw() const {
  if (m_state) {
    m_state->ErrorProvider().Throw(*this, /*shouldHandle:*/ false);
  }
}

template <class TErrorProvider>
inline void ErrorCode::ThrowAs(const TErrorProvider &errorProvider) const {
  errorProvider.Throw(m_state.Get(), /*shouldHandle:*/ false);
}

//============================================================================================
// ErrorCode standalone functions implementation
//============================================================================================

inline ErrorCodeState *GetErrorCodeState(const ErrorCode &errorCode) noexcept {
  return errorCode.m_state.Get();
}

inline void swap(ErrorCode &left, ErrorCode &right) noexcept {
  left.Swap(right);
}

inline bool operator==(const ErrorCode &left, const ErrorCode &right) noexcept {
  return GetErrorCodeState(left) == GetErrorCodeState(right);
}

inline bool operator!=(const ErrorCode &left, const ErrorCode &right) noexcept {
  return GetErrorCodeState(left) != GetErrorCodeState(right);
}

inline bool operator==(const ErrorCode &left, std::nullptr_t) noexcept {
  return GetErrorCodeState(left) == nullptr;
}

inline bool operator!=(const ErrorCode &left, std::nullptr_t) noexcept {
  return GetErrorCodeState(left) != nullptr;
}

inline bool operator==(std::nullptr_t, const ErrorCode &right) noexcept {
  return nullptr == GetErrorCodeState(right);
}

inline bool operator!=(std::nullptr_t, const ErrorCode &right) noexcept {
  return nullptr != GetErrorCodeState(right);
}

//============================================================================================
// ErrorCodeState implementation
//============================================================================================

inline ErrorCodeState::ErrorCodeState(const IErrorProvider &errorProvider) noexcept : m_errorProvider(errorProvider) {}

inline const IErrorProvider &ErrorCodeState::ErrorProvider() const noexcept {
  return m_errorProvider;
}

inline bool ErrorCodeState::IsHandled() const noexcept {
  return m_isHandled;
}

inline void ErrorCodeState::Handle() const noexcept {
  m_isHandled = true;
}

} // namespace Mso

#pragma pack(pop)

#endif // MSO_ERRORCODE_ERRORCODE_H
