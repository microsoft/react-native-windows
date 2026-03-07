// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "NodeApi.h"

namespace Microsoft::NodeApiJsi {

namespace {

struct NodeApiNames {
#define NODE_API_FUNC(func) static constexpr const char func[] = #func;
#include "NodeApi.inc"
};

} // namespace

LibFuncResolver::LibFuncResolver(const char *libName) : libHandle_(LibLoader::loadLib(libName)) {}

FuncPtr LibFuncResolver::getFuncPtr(const char *funcName) {
  return LibLoader::getFuncPtr(libHandle_, funcName);
}

DelayLoadedApi::DelayLoadedApi(IFuncResolver *funcResolver) : funcResolver_(funcResolver) {}

DelayLoadedApi::~DelayLoadedApi() = default;

FuncPtr DelayLoadedApi::getFuncPtr(const char *funcName) {
  return funcResolver_->getFuncPtr(funcName);
}

thread_local NodeApi *NodeApi::current_{};

NodeApi::NodeApi(IFuncResolver *funcResolver)
    : DelayLoadedApi(funcResolver)
#define NODE_API_FUNC(func) \
  , func(&ApiFuncResolver<NodeApi, decltype(::func) *, NodeApiNames::func, offsetof(NodeApi, func)>::stub)
#include "NodeApi.inc"
{
}

} // namespace Microsoft::NodeApiJsi
