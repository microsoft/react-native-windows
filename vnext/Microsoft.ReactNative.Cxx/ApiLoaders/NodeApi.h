// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once
#ifndef APILOADERS_NODEAPI_H_
#define APILOADERS_NODEAPI_H_

#include <js_native_api.h>

namespace Microsoft::NodeApiJsi {

using LibHandle = struct LibHandle_t *;
using FuncPtr = struct FuncPtr_t *;

class LibLoader {
 public:
  static LibHandle loadLib(const char *libName);
  static FuncPtr getFuncPtr(LibHandle libHandle, const char *funcName);
};

template <typename TApi, typename T, const char *funcName, size_t offset>
struct ApiFuncResolver;

template <typename TApi, typename TResult, const char *funcName, size_t offset, typename... TArgs>
struct ApiFuncResolver<TApi, TResult(NAPI_CDECL *)(TArgs...), funcName, offset> {
  // Stub is a special function that loads the targeting function on the first call,
  // replaces function pointer field with the loaded function, and executes the function.
  // After this all future calls to the loaded function are going to be directly from
  // the field and not use this Stub.
  static TResult NAPI_CDECL stub(TArgs... args) {
    using TFunc = TResult(NAPI_CDECL *)(TArgs...);
    TApi *current = TApi::current();
    TFunc func = reinterpret_cast<TFunc>(current->getFuncPtr(funcName));
    *reinterpret_cast<TFunc *>(reinterpret_cast<char *>(current) + offset) = func;
    return (*func)(args...);
  }

  // Optional stub tries to load target function on the first call and set it
  // as the function pointer. If loading fails, then it uses provided default
  // function implementation. The default function can either do the required
  // work or return a failure.
  template <TResult(NAPI_CDECL *defaultFunc)(TArgs...)>
  static TResult NAPI_CDECL optionalStub(TArgs... args) {
    using TFunc = TResult(NAPI_CDECL *)(TArgs...);
    TApi *current = TApi::current();
    TFunc func = reinterpret_cast<TFunc>(current->getFuncPtr(funcName));
    if (func == nullptr) {
      func = defaultFunc;
    }
    *reinterpret_cast<TFunc *>(reinterpret_cast<char *>(current) + offset) = func;
    return (*func)(args...);
  }

  template <void (*preloadFunc)()>
  static TResult NAPI_CDECL preloadStub(TArgs... args) {
    using TFunc = TResult(NAPI_CDECL *)(TArgs...);
    preloadFunc();
    TApi *current = TApi::current();
    TFunc func = *reinterpret_cast<TFunc *>(reinterpret_cast<char *>(current) + offset);
    return (*func)(args...);
  }
};

struct IFuncResolver {
  virtual FuncPtr getFuncPtr(const char *funcName) = 0;
};

class LibFuncResolver : public IFuncResolver {
 public:
  LibFuncResolver(const char *libName);
  FuncPtr getFuncPtr(const char *funcName) override;

 private:
  LibHandle libHandle_;
};

class DelayLoadedApi {
 public:
  DelayLoadedApi(IFuncResolver *funcResolver);
  ~DelayLoadedApi();

  FuncPtr getFuncPtr(const char *funcName);

  DelayLoadedApi(const DelayLoadedApi &) = delete;
  DelayLoadedApi &operator=(const DelayLoadedApi &) = delete;

 private:
  IFuncResolver *funcResolver_;
};

class NodeApi : public DelayLoadedApi {
 public:
  NodeApi(IFuncResolver *funcResolver);

  static NodeApi *current() {
    return current_;
  }

  static void setCurrent(NodeApi *current) {
    current_ = current;
  }

  class Scope {
   public:
    Scope(NodeApi *nodeApi) : prevNodeApi_(NodeApi::current_) {
      NodeApi::current_ = nodeApi;
    }

    ~Scope() {
      NodeApi::current_ = prevNodeApi_;
    }

   private:
    NodeApi *prevNodeApi_;
  };

 public:
#define NODE_API_FUNC(func) decltype(::func) *const func;
#include "NodeApi.inc"

 private:
  static thread_local NodeApi *current_;
};

} // namespace Microsoft::NodeApiJsi

#endif // !APILOADERS_NODEAPI_H_
