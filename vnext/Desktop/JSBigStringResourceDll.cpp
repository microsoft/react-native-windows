// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"

#include "JSBigStringResourceDll.h"

#include <exception>

namespace facebook {
namespace react {

JSBigStringResourceDll::JSBigStringResourceDll(HMODULE dll, HRSRC resource) {
  HGLOBAL loadedResource{LoadResource(dll, resource)};
  if (loadedResource == nullptr) {
    throw std::invalid_argument("Could not load resource");
  }
  this->resource = static_cast<char *>(LockResource(loadedResource));
  if (this->resource == nullptr) {
    throw std::invalid_argument("Could not lock resource");
  }

  this->resourceSize = SizeofResource(dll, resource);
  if (this->resourceSize == 0) {
    throw std::invalid_argument("Could not retrieve size of resource");
  }

  if (this->c_str()[this->size()] != '\0') {
    throw std::invalid_argument("Resource was not null-terminated");
  }
}

std::unique_ptr<const JSBigStringResourceDll> JSBigStringResourceDll::Make(HMODULE dll, HRSRC resource) {
  return std::make_unique<const JSBigStringResourceDll>(dll, resource);
}

} // namespace react
} // namespace facebook
