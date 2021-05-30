// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

#pragma once
#ifndef MSO_FUTURE_DETAILS_TIMEOUTEXCEPTION_H
#define MSO_FUTURE_DETAILS_TIMEOUTEXCEPTION_H

#include <stdexcept>
#include "errorCode/errorProvider.h"

namespace Mso {
namespace Async {

struct TimeoutException : std::runtime_error {
  explicit TimeoutException(const char *message) noexcept : std::runtime_error(message) {}
};

MSO_GUID(TimeoutErrorGuid, "38a62d9f-d24e-4f98-b667-5f4db8b1350f")

LIBLET_PUBLICAPI const Mso::ErrorProvider<int, TimeoutErrorGuid> &TimeoutError() noexcept;

} // namespace Async

template <>
LIBLET_PUBLICAPI void ErrorProvider<int, Async::TimeoutErrorGuid>::Throw(const ErrorCode &errorCode, bool shouldHandle)
    const;

} // namespace Mso

#endif // MSO_FUTURE_DETAILS_TIMEOUTEXCEPTION_H
