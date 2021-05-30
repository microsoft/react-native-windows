// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "errorCode/exceptionErrorProvider.h"
#include "errorCode/hresultErrorProvider.h"
#include "motifCpp/testCheck.h"

// Simple Unit Test error structure
// UtSimpleError uses default .ToString()
struct UtSimpleError {};

MSO_CLASS_GUID(UtSimpleErrorProviderGuid, "4dfc7563-9fbc-4628-9bcf-307475f0d54b")
const Mso::ErrorProvider<UtSimpleError, UtSimpleErrorProviderGuid> s_utSimpleErrorProvider{};
const Mso::ErrorProvider<UtSimpleError, UtSimpleErrorProviderGuid> &UtSimpleErrorProvider() noexcept {
  return s_utSimpleErrorProvider;
}

// Unit Test error example with overriden.ToString()
struct UtError {
  int Value;
};

MSO_CLASS_GUID(UtErrorProviderGuid, "4dfc7563-9fbc-4628-9bcf-307475f0d54b")
const Mso::ErrorProvider<UtError, UtErrorProviderGuid> s_utErrorProvider{};
const Mso::ErrorProvider<UtError, UtErrorProviderGuid> &UtErrorProvider() noexcept {
  return s_utErrorProvider;
}

template <>
Mso::CntPtr<Mso::IErrorString> Mso::ErrorProvider<UtError, UtErrorProviderGuid>::ToString(
    const ::Mso::ErrorCode &errorCode) const noexcept {
  OACR_NOEXCEPT_MAYTERMINATE_IGNORE_STL;
  std::string resultString = "UtError Value: " + std::to_string(GetErrorInfo(errorCode).Value);
  return Mso::MakeErrorString(resultString.c_str());
}

TEST_CLASS (ErrorProviderTest) {
  TEST_METHOD(ToStringApiTest) {
    // UtSimpleError
    // Default Error Message
    Mso::ErrorCode utSimpleErrorCode = UtSimpleErrorProvider().MakeErrorCode(UtSimpleError{});
    TestCheckEqual("Error", utSimpleErrorCode.ToString()); // Default error message

    // Swap() Api
    Mso::ErrorCode utSimpleErrorCode2;
    utSimpleErrorCode.Swap(utSimpleErrorCode2);
    TestCheckEqual("Error", utSimpleErrorCode2.ToString()); // Default error message
    TestCheckEqual("Empty ErrorCode", utSimpleErrorCode.ToString()); // Default error message

    // UtError
    Mso::ErrorCode utErrorCode = UtErrorProvider().MakeErrorCode(UtError{15});
    TestCheckEqual("UtError Value: 15", utErrorCode.ToString());

    // HRESULT
    Mso::ErrorCode hresultErrorCode = Mso::HResultErrorProvider().MakeErrorCode(E_UNEXPECTED);
    TestCheckEqual("HRESULT: 0x8000FFFF", hresultErrorCode.ToString());

    // std::exception_ptr with arbitrary exception
    Mso::ErrorCode arbitraryExceptionErrorCode =
        Mso::ExceptionErrorProvider().MakeErrorCode(std::make_exception_ptr(std::make_exception_ptr(12)));
    TestCheckEqual("Exception", arbitraryExceptionErrorCode.ToString());

    // std::exception_ptr with std exception
    Mso::ErrorCode stdExceptionErrorCode =
        Mso::ExceptionErrorProvider().MakeErrorCode(std::make_exception_ptr(std::logic_error("LogicError")));
    TestCheckEqual("Exception: LogicError", stdExceptionErrorCode.ToString());
  }
};
