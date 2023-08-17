// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "ApiLoaders/NodeApi.h"

namespace Microsoft::NodeApiJsi {

LibHandle LibLoader::loadLib(const char * /*libName*/) {
  return reinterpret_cast<LibHandle>(0);
}

FuncPtr LibLoader::getFuncPtr(LibHandle /*libHandle*/, const char * /*funcName*/) {
  return reinterpret_cast<FuncPtr>(0);
}

} // namespace Microsoft::NodeApiJsi
