// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

#include "errorCode/errorCode.h"
#include <iomanip>
#include <sstream>
#include "errorCode/exceptionErrorProvider.h"
#include "errorCode/hresultErrorProvider.h"
#include "errorCode/maybe.h"
#include "object/refCountedObject.h"

namespace Mso {

//============================================================================================
// ErrorCode implementation
//============================================================================================

const char *ErrorCode::ToString() const noexcept {
  auto state = m_state.Get();

  if (state) {
    auto returnValue{state->m_errorString->load(std::memory_order_acquire)};
    if (!returnValue) {
      Mso::CntPtr<Mso::IErrorString> newInstance{state->m_errorProvider.ToString(*this)};
      if (state->m_errorString->compare_exchange_strong(returnValue, newInstance.Get())) {
        // The current thread won the race.
        // Ensure that a pointer with a reference count of 1 is stored.
        returnValue = newInstance.Detach();
      }
    }

    return returnValue->Data();
  }

  return "Empty ErrorCode";
}

//============================================================================================
// String implementation
//============================================================================================

struct ErrorString : Mso::RefCountedObject<IErrorString> {
  const char *Data() noexcept override;

 private:
  friend MakePolicy;
  ErrorString(const char *value) noexcept;

 private:
  std::string m_stringValue;
};

ErrorString::ErrorString(const char *value) noexcept
    : m_stringValue(value) // Copy
{
  OACR_NOEXCEPT_MAYTERMINATE_IGNORE_STL;
}

const char *ErrorString::Data() noexcept {
  return m_stringValue.c_str();
}

Mso::CntPtr<Mso::IErrorString> MakeErrorString(const char *value) noexcept {
  return Mso::Make<ErrorString>(value);
}

//============================================================================================
// ErrorCodeState implementation
//============================================================================================

LIBLET_PUBLICAPI void ErrorCodeState::AddRef() const noexcept {
  uint32_t refCount = ++m_refCount;
  (void)refCount; // To avoid unused variable warning
  Debug(VerifyElseCrashSzTag(refCount != 1, "Ref count must not bounce from zero", 0x0158c55e /* tag_bwmv4 */));
}

LIBLET_PUBLICAPI void ErrorCodeState::Release() const noexcept {
  uint32_t refCount = --m_refCount;
  Debug(VerifyElseCrashSzTag((int32_t)refCount >= 0, "Ref count must not be negative", 0x0158c55f /* tag_bwmv5 */));
  if (refCount == 0) {
    m_errorProvider.Destroy(*this);
  }
}

//============================================================================================
// ExceptionErrorProvider implementation
//============================================================================================

template <>
LIBLET_PUBLICAPI void ErrorProvider<std::exception_ptr, ExceptionErrorProviderGuid>::Throw(
    const ErrorCode &errorCode,
    bool shouldHandle) const {
  const std::exception_ptr *exception = TryGetErrorInfo(errorCode, shouldHandle);
  if (exception) {
    std::rethrow_exception(*exception);
  }
}

const ErrorProvider<std::exception_ptr, ExceptionErrorProviderGuid> s_exceptionErrorProvider{};

LIBLET_PUBLICAPI const ErrorProvider<std::exception_ptr, ExceptionErrorProviderGuid>
    &ExceptionErrorProvider() noexcept {
  return s_exceptionErrorProvider;
}

template <>
LIBLET_PUBLICAPI _Noexcept_mayterminate_ Mso::CntPtr<Mso::IErrorString>
ErrorProvider<std::exception_ptr, ExceptionErrorProviderGuid>::ToString(const ErrorCode &errorCode) const noexcept {
  OACR_WARNING_PUSH;
  OACR_WARNING_DISABLE(
      CATCH_ALL_IS_FORBIDDEN,
      "Rethrow and Catch All is needed to differentiate between C++ Arbitrary and Std exception types");

  try {
    std::rethrow_exception(GetErrorInfo(errorCode));
  } catch (const std::exception &e) {
    OACR_NOEXCEPT_MAYTERMINATE_IGNORE_STL;
    // Std exception type
    return Mso::MakeErrorString((std::string("Exception: ") + e.what()).c_str());
  } catch (...) {
    // C++ Arbitrary exception type
    return Mso::MakeErrorString("Exception");
  }

  OACR_WARNING_POP;
}

//============================================================================================
// HResultErrorProvider implementation
//============================================================================================

const ErrorProvider<HRESULT, HResultErrorProviderGuid> s_hResultErrorProvider{};

LIBLET_PUBLICAPI const ErrorProvider<HRESULT, HResultErrorProviderGuid> &HResultErrorProvider() noexcept {
  return s_hResultErrorProvider;
}

// Returns string of Hex representation for HRESULT value.
// For example: E_OUTOFMEMORY will return "0x8007000E"
template <>
LIBLET_PUBLICAPI Mso::CntPtr<Mso::IErrorString> ErrorProvider<HRESULT, HResultErrorProviderGuid>::ToString(
    const ErrorCode &errorCode) const noexcept {
  OACR_NOEXCEPT_MAYTERMINATE_IGNORE_STL;

  std::stringstream ss;
  ss << "HRESULT: "
     << "0x" << std::uppercase << std::setfill('0') << std::setw(8) << std::hex << GetErrorInfo(errorCode);

  return Mso::MakeErrorString(ss.str().c_str());
}

} // namespace Mso
