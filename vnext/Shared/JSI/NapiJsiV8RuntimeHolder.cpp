// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"

#include "NapiJsiV8RuntimeHolder.h"

// V8-JSI
#include <NapiJsiRuntime.h>

// Standard Library

using namespace facebook::jsi;
using namespace facebook::react;

using std::shared_ptr;
using std::unique_ptr;

namespace Microsoft::JSI {

NapiJsiV8RuntimeHolder::NapiJsiV8RuntimeHolder(
  shared_ptr<DevSettings> devSettings,
  shared_ptr<MessageQueueThread> jsQueue,
  unique_ptr<ScriptStore> && scriptStore,
  unique_ptr<PreparedScriptStore>&& preparedScritpStore) noexcept
    : m_useDirectDebugger{devSettings->useDirectDebugger},
      m_debuggerBreakOnNextLine{devSettings->debuggerBreakOnNextLine},
      m_debuggerPort{devSettings->debuggerPort},
      m_debuggerRuntimeName{devSettings->debuggerRuntimeName},
      m_jsQueue{std::move(jsQueue)},
      m_scriptStore{std::move(scriptStore)},
      m_preparedScriptStore{std::move(preparedScritpStore)} {}

void NapiJsiV8RuntimeHolder::InitRuntime() noexcept
{
  napi_env env{};
  napi_ext_create_env(napi_ext_env_attribute_enable_gc_api, &env);
  m_runtime = napijsi::MakeNapiJsiRuntime(env);
}

#pragma region facebook::jsi::RuntimeHolderLazyInit

shared_ptr<Runtime> NapiJsiV8RuntimeHolder::getRuntime() noexcept /*override*/
{
  std::call_once(m_onceFlag, [this]() { InitRuntime(); });

  if (!m_runtime)
    std::terminate();

  // V8 NapiJsiRuntime is not known to be thread safe.
  if (m_ownThreadId != std::this_thread::get_id())
    std::terminate();

  return m_runtime;
}

#pragma endregion facebook::jsi::RuntimeHolderLazyInit


} // namespace Microsoft::JSI
