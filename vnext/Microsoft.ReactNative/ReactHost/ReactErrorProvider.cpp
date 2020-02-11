// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include "ReactErrorProvider.h"

namespace Mso::React {

const Mso::ErrorProvider<ReactError, ReactErrorProviderGuid> s_reactErrorProvider{};

const Mso::ErrorProvider<ReactError, ReactErrorProviderGuid> &ReactErrorProvider() noexcept {
  return s_reactErrorProvider;
}

} // namespace Mso::React

namespace Mso {

// Template specialization for ErrorProvider must be defined inside of Mso namespace
template <>
Mso::CntPtr<Mso::IErrorString> ErrorProvider<Mso::React::ReactError, Mso::React::ReactErrorProviderGuid>::ToString(
    const Mso::ErrorCode &errorCode) const noexcept {
  return Mso::MakeErrorString(GetErrorInfo(errorCode).GetSerializedError());
}

} // namespace Mso
