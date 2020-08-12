// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include <string>
#include "ReactWindowsAPI.h"

namespace react {
namespace windows {

REACTWINDOWS_API_(void) SetAsyncStorageDBPath(std::string &&path);

}
} // namespace react
