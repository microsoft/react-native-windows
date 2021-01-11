// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include "IReactInstanceInternal.h"
#include "React.h"
#include "object/refCountedObject.h"

namespace Mso::React {

// This is a factory method to be implemented per platform.
// It creates a platform specific react instance.
Mso::CntPtr<IReactInstanceInternal> MakeReactInstance(
    IReactHost &reactHost,
    ReactOptions &&options,
    Mso::Promise<void> &&whenCreated,
    Mso::Promise<void> &&whenLoaded,
    Mso::VoidFunctor &&updateUI) noexcept;

} // namespace Mso::React
