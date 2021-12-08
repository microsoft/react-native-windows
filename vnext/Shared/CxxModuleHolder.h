// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include <cxxreact/CxxModule.h>

namespace Microsoft::React {

struct CxxModuleHolder {
  facebook::xplat::module::CxxModule *Module{nullptr};
};

} // namespace Microsoft::React
