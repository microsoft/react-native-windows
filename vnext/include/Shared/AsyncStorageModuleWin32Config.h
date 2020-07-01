// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include <ReactWindowsAPI.h>
#include <string>

namespace react {
namespace windows {

REACTWINDOWS_API_(void) SetAsyncStorageDBPath(std::string &&path);

}
} // namespace react
