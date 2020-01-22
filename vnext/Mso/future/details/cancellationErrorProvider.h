// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

#pragma once
#ifndef MSO_FUTURE_DETAILS_CANCELLATIONERRORPROVIDER_H
#define MSO_FUTURE_DETAILS_CANCELLATIONERRORPROVIDER_H

#include "errorCode/errorProvider.h"

namespace Mso {

MSO_GUID(CancellationErrorProviderGuid, "e7d1526a-efcf-4e62-a906-84fdcd4b6371")

LIBLET_PUBLICAPI const ErrorProvider<bool, CancellationErrorProviderGuid> &CancellationErrorProvider() noexcept;

template <>
LIBLET_PUBLICAPI void ErrorProvider<bool, CancellationErrorProviderGuid>::Throw(
    const ErrorCode &errorCode,
    bool shouldHandle) const;

} // namespace Mso

#endif // MSO_FUTURE_DETAILS_CANCELLATIONERRORPROVIDER_H
