// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

#pragma once
#ifndef MSO_ERRORCODE_EXCEPTIONERRORPROVIDER_H
#define MSO_ERRORCODE_EXCEPTIONERRORPROVIDER_H

#include "errorCode/errorProvider.h"

namespace Mso {

MSO_CLASS_GUID(ExceptionErrorProviderGuid, "abd53c35-105d-4e67-8747-8473d9c80507")

LIBLET_PUBLICAPI const ErrorProvider<std::exception_ptr, ExceptionErrorProviderGuid> &ExceptionErrorProvider() noexcept;

template <>
LIBLET_PUBLICAPI void ErrorProvider<std::exception_ptr, ExceptionErrorProviderGuid>::Throw(
    const ErrorCode &errorCode,
    bool shouldHandle) const;

template <>
LIBLET_PUBLICAPI Mso::CntPtr<Mso::IErrorString> ErrorProvider<std::exception_ptr, ExceptionErrorProviderGuid>::ToString(
    const ErrorCode &errorCode) const noexcept;

} // namespace Mso

#endif // MSO_ERRORCODE_EXCEPTIONERRORPROVIDER_H
