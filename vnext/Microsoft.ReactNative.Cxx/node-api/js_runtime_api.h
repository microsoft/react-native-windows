// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#ifndef SRC_JS_RUNTIME_API_H_
#define SRC_JS_RUNTIME_API_H_

#include "node_api.h"

//
// Node-API extensions required for JavaScript engine hosting.
//
// It is a very early version of the APIs which we consider to be experimental.
// These APIs are not stable yet and are subject to change while we continue
// their development. After some time we will stabilize the APIs and make them
// "officially stable".
//

#define JSR_API NAPI_EXTERN napi_status NAPI_CDECL

EXTERN_C_START

typedef struct jsr_runtime_s* jsr_runtime;
typedef struct jsr_config_s* jsr_config;
typedef struct jsr_prepared_script_s* jsr_prepared_script;
typedef struct jsr_napi_env_scope_s* jsr_napi_env_scope;

typedef void(NAPI_CDECL* jsr_data_delete_cb)(void* data, void* deleter_data);

//=============================================================================
// jsr_runtime
//=============================================================================

JSR_API jsr_create_runtime(jsr_config config, jsr_runtime* runtime);
JSR_API jsr_delete_runtime(jsr_runtime runtime);
JSR_API jsr_runtime_get_node_api_env(jsr_runtime runtime, napi_env* env);

//=============================================================================
// jsr_config
//=============================================================================

JSR_API jsr_create_config(jsr_config* config);
JSR_API jsr_delete_config(jsr_config config);

JSR_API jsr_config_enable_inspector(jsr_config config, bool value);
JSR_API jsr_config_set_inspector_runtime_name(jsr_config config,
                                              const char* name);
JSR_API jsr_config_set_inspector_port(jsr_config config, uint16_t port);
JSR_API jsr_config_set_inspector_break_on_start(jsr_config config, bool value);

JSR_API jsr_config_enable_gc_api(jsr_config config, bool value);

JSR_API jsr_config_set_explicit_microtasks(jsr_config config, bool value);

// A callback to process unhandled JS error
typedef void(NAPI_CDECL* jsr_unhandled_error_cb)(void* cb_data,
                                                 napi_env env,
                                                 napi_value error);

JSR_API jsr_config_on_unhandled_error(
    jsr_config config,
    void* cb_data,
    jsr_unhandled_error_cb unhandled_error_cb);

//=============================================================================
// jsr_config task runner
//=============================================================================

// A callback to run task
typedef void(NAPI_CDECL* jsr_task_run_cb)(void* task_data);

// A callback to post task to the task runner
typedef void(NAPI_CDECL* jsr_task_runner_post_task_cb)(
    void* task_runner_data,
    void* task_data,
    jsr_task_run_cb task_run_cb,
    jsr_data_delete_cb task_data_delete_cb,
    void* deleter_data);

JSR_API jsr_config_set_task_runner(
    jsr_config config,
    void* task_runner_data,
    jsr_task_runner_post_task_cb task_runner_post_task_cb,
    jsr_data_delete_cb task_runner_data_delete_cb,
    void* deleter_data);

//=============================================================================
// jsr_config script cache
//=============================================================================

typedef void(NAPI_CDECL* jsr_script_cache_load_cb)(
    void* script_cache_data,
    const char* source_url,
    uint64_t source_hash,
    const char* runtime_name,
    uint64_t runtime_version,
    const char* cache_tag,
    const uint8_t** buffer,
    size_t* buffer_size,
    jsr_data_delete_cb* buffer_delete_cb,
    void** deleter_data);

typedef void(NAPI_CDECL* jsr_script_cache_store_cb)(
    void* script_cache_data,
    const char* source_url,
    uint64_t source_hash,
    const char* runtime_name,
    uint64_t runtime_version,
    const char* cache_tag,
    const uint8_t* buffer,
    size_t buffer_size,
    jsr_data_delete_cb buffer_delete_cb,
    void* deleter_data);

JSR_API jsr_config_set_script_cache(
    jsr_config config,
    void* script_cache_data,
    jsr_script_cache_load_cb script_cache_load_cb,
    jsr_script_cache_store_cb script_cache_store_cb,
    jsr_data_delete_cb script_cache_data_delete_cb,
    void* deleter_data);

//=============================================================================
// napi_env scope
//=============================================================================

// Opens the napi_env scope in the current thread.
// Calling Node-API functions without the opened scope may cause a failure.
// The scope must be closed by the jsr_close_napi_env_scope call.
JSR_API jsr_open_napi_env_scope(napi_env env, jsr_napi_env_scope* scope);

// Closes the napi_env scope in the current thread. It must match to the
// jsr_open_napi_env_scope call.
JSR_API jsr_close_napi_env_scope(napi_env env, jsr_napi_env_scope scope);

//=============================================================================
// Additional functions to implement JSI
//=============================================================================

// To implement JSI description()
JSR_API jsr_get_description(napi_env env, const char** result);

// To implement JSI queueMicrotask()
JSR_API jsr_queue_microtask(napi_env env, napi_value callback);

// To implement JSI drainMicrotasks()
JSR_API
jsr_drain_microtasks(napi_env env, int32_t max_count_hint, bool* result);

// To implement JSI isInspectable()
JSR_API jsr_is_inspectable(napi_env env, bool* result);

//=============================================================================
// Script preparing and running.
//
// Script is usually converted to byte code, or in other words - prepared - for
// execution. Then, we can run the prepared script.
//=============================================================================

// Run script with source URL.
JSR_API jsr_run_script(napi_env env,
                       napi_value source,
                       const char* source_url,
                       napi_value* result);

// Prepare the script for running.
JSR_API jsr_create_prepared_script(napi_env env,
                                   const uint8_t* script_data,
                                   size_t script_length,
                                   jsr_data_delete_cb script_delete_cb,
                                   void* deleter_data,
                                   const char* source_url,
                                   jsr_prepared_script* result);

// Delete the prepared script.
JSR_API jsr_delete_prepared_script(napi_env env,
                                   jsr_prepared_script prepared_script);

// Run the prepared script.
JSR_API jsr_prepared_script_run(napi_env env,
                                jsr_prepared_script prepared_script,
                                napi_value* result);

//=============================================================================
// Functions to support unit tests.
//=============================================================================

// Provides a hint to run garbage collection.
// It is typically used for unit tests.
// It requires enabling GC by calling jsr_config_enable_gc_api.
JSR_API jsr_collect_garbage(napi_env env);

// Checks if the environment has an unhandled promise rejection.
JSR_API jsr_has_unhandled_promise_rejection(napi_env env, bool* result);

// Gets and clears the last unhandled promise rejection.
JSR_API jsr_get_and_clear_last_unhandled_promise_rejection(napi_env env,
                                                           napi_value* result);

// Create new napi_env for the runtime.
JSR_API
jsr_create_node_api_env(napi_env root_env, int32_t api_version, napi_env* env);

// Run task in the environment context.
JSR_API jsr_run_task(napi_env env, jsr_task_run_cb task_cb, void* data);

// Initializes native module.
JSR_API jsr_initialize_native_module(napi_env env,
                                     napi_addon_register_func register_module,
                                     int32_t api_version,
                                     napi_value* exports);

EXTERN_C_END

#endif  // !SRC_JS_RUNTIME_API_H_
