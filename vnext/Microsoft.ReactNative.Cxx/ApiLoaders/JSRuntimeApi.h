// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#ifndef APILOADERS_JSRUNTIMEAPI_H_
#define APILOADERS_JSRUNTIMEAPI_H_

#include <js_runtime_api.h>
#include "NodeApi.h"

namespace Microsoft::NodeApiJsi {

class JSRuntimeApi : public NodeApi {
 public:
  JSRuntimeApi(IFuncResolver *funcResolver);

  static JSRuntimeApi *current() {
    return current_;
  }

  static void setCurrent(JSRuntimeApi *current) {
    NodeApi::setCurrent(current);
    current_ = current;
  }

  class Scope : public NodeApi::Scope {
   public:
    Scope(JSRuntimeApi *api) : NodeApi::Scope(api), prevJSRuntimeApi_(JSRuntimeApi::current_) {
      JSRuntimeApi::current_ = api;
    }

    ~Scope() {
      JSRuntimeApi::current_ = prevJSRuntimeApi_;
    }

   private:
    JSRuntimeApi *prevJSRuntimeApi_;
  };

 public:
#define JSR_FUNC(func) decltype(::func) *const func;
#define JSR_JSI_FUNC JSR_FUNC
#define JSR_PREPARED_SCRIPT JSR_FUNC
#include "JSRuntimeApi.inc"

 private:
  static thread_local JSRuntimeApi *current_;
};

} // namespace Microsoft::NodeApiJsi

#endif // !APILOADERS_JSRUNTIMEAPI_H_
