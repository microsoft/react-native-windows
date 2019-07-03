// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"

#include "JSBigStringResourceDll.h"

#include <exception>
#if defined(USE_EDGEMODE_JSRT)
#include <jsrt.h>
#else
#include <ChakraCore.h>
#endif

namespace facebook {
namespace react {

JSBigStringResourceDll::JSBigStringResourceDll(HMODULE dll, HRSRC resource) {
  HGLOBAL loadedResource{LoadResource(dll, resource)};
  if (loadedResource == nullptr) {
    throw new std::invalid_argument("Could not load resource");
  }
  this->resource = static_cast<char *>(LockResource(loadedResource));
  if (this->resource == nullptr) {
    throw new std::invalid_argument("Could not lock resource");
  }

  this->resourceSize = SizeofResource(dll, resource);
  if (this->resourceSize == 0) {
    throw new std::invalid_argument("Could not retrieve size of resource");
  }

  if (this->c_str()[this->size()] != '\0') {
    throw new std::invalid_argument("Resource was not null-terminated");
  }
}

std::unique_ptr<const JSBigStringResourceDll> JSBigStringResourceDll::Make(
    HMODULE dll,
    HRSRC resource) {
  return std::make_unique<const JSBigStringResourceDll>(dll, resource);
}

} // namespace react
} // namespace facebook
