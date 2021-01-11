// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include <errorCode/errorCode.h>
#include <errorCode/errorProvider.h>

namespace Mso::React {

struct ReactError {
  ReactError(const char *serializedError) noexcept
      : m_serializedError(Mso::MakeErrorString(serializedError)) // Copy string
  {}

  const char *GetSerializedError() const noexcept {
    return m_serializedError->Data();
  }

 private:
  Mso::CntPtr<Mso::IErrorString> m_serializedError;
};

MSO_CLASS_GUID(ReactErrorProviderGuid, "74745895-9fe7-4421-98a9-43ef643a10f0")
LIBLET_PUBLICAPI const Mso::ErrorProvider<ReactError, ReactErrorProviderGuid> &ReactErrorProvider() noexcept;

} // namespace Mso::React

namespace Mso {

// Template specialization for ErrorProvider must be defined inside of Mso namespace
template <>
Mso::CntPtr<Mso::IErrorString> ErrorProvider<Mso::React::ReactError, Mso::React::ReactErrorProviderGuid>::ToString(
    const Mso::ErrorCode &errorCode) const noexcept;

} // namespace Mso
