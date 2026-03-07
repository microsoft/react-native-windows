// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#ifndef JSR_FUNC
#define JSR_FUNC(func)
#endif

#ifndef JSR_JSI_FUNC
#define JSR_JSI_FUNC(func)
#endif

#ifndef JSR_PREPARED_SCRIPT
#define JSR_PREPARED_SCRIPT(func)
#endif

// The JS runtime functions sorted alphabetically.
JSR_FUNC(jsr_collect_garbage)
JSR_FUNC(jsr_config_enable_gc_api)
JSR_FUNC(jsr_config_enable_inspector)
JSR_FUNC(jsr_config_set_explicit_microtasks)
JSR_FUNC(jsr_config_set_inspector_break_on_start)
JSR_FUNC(jsr_config_set_inspector_port)
JSR_FUNC(jsr_config_set_inspector_runtime_name)
JSR_FUNC(jsr_config_set_script_cache)
JSR_FUNC(jsr_config_set_task_runner)
JSR_FUNC(jsr_create_config)
JSR_FUNC(jsr_create_runtime)
JSR_FUNC(jsr_delete_config)
JSR_FUNC(jsr_delete_runtime)
JSR_FUNC(jsr_get_and_clear_last_unhandled_promise_rejection)
JSR_FUNC(jsr_has_unhandled_promise_rejection)
JSR_FUNC(jsr_run_script)
JSR_FUNC(jsr_runtime_get_node_api_env)

// The JS runtime functions needed for JSI.
JSR_JSI_FUNC(jsr_close_napi_env_scope)
JSR_JSI_FUNC(jsr_queue_microtask)
JSR_JSI_FUNC(jsr_drain_microtasks)
JSR_JSI_FUNC(jsr_get_description)
JSR_JSI_FUNC(jsr_is_inspectable)
JSR_JSI_FUNC(jsr_open_napi_env_scope)

// The JS runtime functions needed for prepared script.
JSR_PREPARED_SCRIPT(jsr_create_prepared_script)
JSR_PREPARED_SCRIPT(jsr_delete_prepared_script)
JSR_PREPARED_SCRIPT(jsr_prepared_script_run)

#undef JSR_FUNC
#undef JSR_JSI_FUNC
#undef JSR_PREPARED_SCRIPT