// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "JSRuntimeApi.h"

EXTERN_C_START

// Default JSR function implementations if they are not found in the engine DLL.
extern napi_status NAPI_CDECL default_jsr_open_napi_env_scope(napi_env env, jsr_napi_env_scope *scope);
extern napi_status NAPI_CDECL default_jsr_close_napi_env_scope(napi_env env, jsr_napi_env_scope scope);
extern napi_status NAPI_CDECL default_jsr_get_description(napi_env env, const char **result);
extern napi_status NAPI_CDECL default_jsr_queue_microtask(napi_env env, napi_value callback);
extern napi_status NAPI_CDECL default_jsr_drain_microtasks(napi_env env, int32_t max_count_hint, bool *result);
extern napi_status NAPI_CDECL default_jsr_is_inspectable(napi_env env, bool *result);

extern napi_status NAPI_CDECL default_jsr_create_prepared_script(
    napi_env env,
    const uint8_t *script_utf8,
    size_t script_length,
    jsr_data_delete_cb script_delete_cb,
    void *deleter_data,
    const char *source_url,
    jsr_prepared_script *result);
extern napi_status NAPI_CDECL default_jsr_delete_prepared_script(napi_env env, jsr_prepared_script prepared_script);
extern napi_status NAPI_CDECL
default_jsr_prepared_script_run(napi_env env, jsr_prepared_script prepared_script, napi_value *result);

EXTERN_C_END

namespace Microsoft::NodeApiJsi {

namespace {

struct JSRuntimeApiNames {
#define JSR_FUNC(func) static constexpr const char func[] = #func;
#define JSR_JSI_FUNC JSR_FUNC
#define JSR_PREPARED_SCRIPT JSR_FUNC
#include "JSRuntimeApi.inc"
};

// Prepared script functions either should be all loaded or we use all default functions.
void loadPreparedScriptFuncs() {
  JSRuntimeApi *current = JSRuntimeApi::current();
  bool useDefault = false;
#define JSR_PREPARED_SCRIPT(func)                                                         \
  decltype(::func) *loaded_##func =                                                       \
      reinterpret_cast<decltype(::func) *>(current->getFuncPtr(JSRuntimeApiNames::func)); \
  useDefault = useDefault || loaded_##func == nullptr;
#include "JSRuntimeApi.inc"
#define JSR_PREPARED_SCRIPT(func)                                                             \
  size_t offset_##func = offsetof(JSRuntimeApi, func);                                        \
  *reinterpret_cast<decltype(::func) **>(reinterpret_cast<char *>(current) + offset_##func) = \
      useDefault ? &default_##func : loaded_##func;
#include "JSRuntimeApi.inc"
}

} // namespace

thread_local JSRuntimeApi *JSRuntimeApi::current_{};

JSRuntimeApi::JSRuntimeApi(IFuncResolver *funcResolver)
    : NodeApi(funcResolver)
#define JSR_FUNC(func)                                                                                                 \
  ,                                                                                                                    \
      func(&ApiFuncResolver<JSRuntimeApi, decltype(::func) *, JSRuntimeApiNames::func, offsetof(JSRuntimeApi, func)>:: \
               stub)
#define JSR_JSI_FUNC(func)                                                                                             \
  ,                                                                                                                    \
      func(&ApiFuncResolver<JSRuntimeApi, decltype(::func) *, JSRuntimeApiNames::func, offsetof(JSRuntimeApi, func)>:: \
               optionalStub<&default_##func>)
#define JSR_PREPARED_SCRIPT(func)                                                                                      \
  ,                                                                                                                    \
      func(&ApiFuncResolver<JSRuntimeApi, decltype(::func) *, JSRuntimeApiNames::func, offsetof(JSRuntimeApi, func)>:: \
               preloadStub<&loadPreparedScriptFuncs>)
#include "JSRuntimeApi.inc"
{
}

} // namespace Microsoft::NodeApiJsi
