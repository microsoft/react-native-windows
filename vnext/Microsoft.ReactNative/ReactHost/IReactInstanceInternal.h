// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include "React.h"
#include "object/queryCast.h"

namespace Mso::React {

//! Internal interface to be implemented by ReactInstance
struct IReactInstanceInternal : Mso::QueryCastList<IReactInstance> {
  virtual Mso::Future<void> Destroy() noexcept = 0;
};

} // namespace Mso::React
