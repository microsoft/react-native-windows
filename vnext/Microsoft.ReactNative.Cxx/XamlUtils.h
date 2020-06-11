// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.
#pragma once

//
// Common XAML utilities.
//

#include "CppWinRTIncludes.h"

namespace XAML_CPPWINRT_NAMESPACE {

// Return Application::Current() when it is present or nullptr otherwise.
// It does not throw exception as Application::Current() does.
inline Application TryGetCurrentApplication() noexcept {
  auto applicationStatics = get_activation_factory<IApplicationStatics>(name_of<Application>());
  auto abiApplicationStatics = static_cast<impl::abi_t<IApplicationStatics> *>(get_abi(applicationStatics));
  void *value{};
  abiApplicationStatics->get_Current(&value);
  return Application{value, take_ownership_from_abi};
}

} // namespace XAML_CPPWINRT_NAMESPACE
