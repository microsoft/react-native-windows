// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

#pragma once
#ifndef MSO_ERRORCODE_HRESULTERRORPROVIDER_H
#define MSO_ERRORCODE_HRESULTERRORPROVIDER_H

#include "errorCode/errorProvider.h"

namespace Mso {

MSO_CLASS_GUID(HResultErrorProviderGuid, "770f51aa-07f8-421f-ae15-0206be2bd92b")

LIBLET_PUBLICAPI const ErrorProvider<HRESULT, HResultErrorProviderGuid> &HResultErrorProvider() noexcept;

template <>
LIBLET_PUBLICAPI Mso::CntPtr<Mso::IErrorString> ErrorProvider<HRESULT, HResultErrorProviderGuid>::ToString(
    const ErrorCode &errorCode) const noexcept;

} // namespace Mso

#endif MSO_ERRORCODE_HRESULTERRORPROVIDER_H
