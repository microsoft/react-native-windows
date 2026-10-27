#pragma once

// Resolves the upstream umbrella include without adding this module directory
// to the compiler include path. Those directories also contain loose headers
// whose names collide across modules.
#include <react/nativemodule/core/React/NativeModuleCore.h>
