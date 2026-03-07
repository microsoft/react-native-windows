// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "NodeApi.h"
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>

namespace Microsoft::NodeApiJsi {

LibHandle LibLoader::loadLib(const char *libName) {
  return reinterpret_cast<LibHandle>(LoadLibraryA(libName));
}

FuncPtr LibLoader::getFuncPtr(LibHandle libHandle, const char *funcName) {
  return reinterpret_cast<FuncPtr>(GetProcAddress(reinterpret_cast<HMODULE>(libHandle), funcName));
}

} // namespace Microsoft::NodeApiJsi
