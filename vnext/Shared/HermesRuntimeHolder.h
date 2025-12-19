// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once
#include <DevSettings.h>
#include <JSI/RuntimeHolder.h>
#include <JSI/ScriptStore.h>
#include <ReactPropertyBag.h>
#include <cxxreact/MessageQueueThread.h>
#include <hermes/hermes_api.h>
#include <react/runtime/JSRuntimeFactory.h>

namespace Microsoft::ReactNative {

template <typename TLambda, typename TFunctor>
class FunctorAdapter {
  static_assert(sizeof(TLambda) == -1, "Unsupported signature");
};

template <typename TLambda, typename TResult, typename... TArgs>
class FunctorAdapter<TLambda, TResult(NAPI_CDECL)(void *, TArgs...)> {
 public:
  static TResult NAPI_CDECL Invoke(void *data, TArgs... args) {
    return reinterpret_cast<TLambda *>(data)->operator()(args...);
  }
};

template <typename TFunctor, typename TLambda>
inline TFunctor AsFunctor(TLambda &&lambda) {
  using TLambdaType = std::remove_reference_t<TLambda>;
  using TAdapter =
      FunctorAdapter<TLambdaType, std::remove_pointer_t<decltype(std::remove_reference_t<TFunctor>::invoke)>>;
  return TFunctor{
      static_cast<void *>(new TLambdaType(std::forward<TLambdaType>(lambda))), &TAdapter::Invoke, [](void *data) {
        delete static_cast<TLambdaType *>(data);
      }};
}

template <typename TFunctor, typename TInvoke>
class FunctorWrapperBase {
  static_assert(sizeof(TInvoke) == -1, "Unsupported signature");
};

template <typename TFunctor, typename TResult, typename... TArgs>
class FunctorWrapperBase<TFunctor, TResult(NAPI_CDECL *)(void *, TArgs...)> {
 public:
  FunctorWrapperBase(TFunctor functor) : functor_(functor) {}

  ~FunctorWrapperBase() {
    if (functor_.release != nullptr) {
      functor_.release(functor_.data);
    }
  }

  TResult operator()(TArgs... args) {
    return functor_.invoke(functor_.data, args...);
  }

 private:
  TFunctor functor_;
};

template <typename TFunctor>
class FunctorWrapper : public FunctorWrapperBase<TFunctor, decltype(std::declval<TFunctor>().invoke)> {
 public:
  using FunctorWrapperBase<TFunctor, decltype(std::declval<TFunctor>().invoke)>::FunctorWrapperBase;
};

class HermesCdpDebugApiDeleter {
 public:
  void operator()(hermes_cdp_debug_api cdp_debug_api);
};

class HermesCdpAgentDeleter {
 public:
  void operator()(hermes_cdp_agent cdp_agent);
};

class HermesCdpStateDeleter {
 public:
  void operator()(hermes_cdp_state cdp_state);
};

class HermesStackTraceDeleter {
 public:
  void operator()(hermes_stack_trace stack_trace);
};

class HermesSamplingProfileDeleter {
 public:
  void operator()(hermes_sampling_profile sampling_profile);
};

using HermesUniqueCdpDebugApi = std::unique_ptr<hermes_cdp_debug_api_s, HermesCdpDebugApiDeleter>;
using HermesUniqueCdpAgent = std::unique_ptr<hermes_cdp_agent_s, HermesCdpAgentDeleter>;
using HermesUniqueCdpState = std::unique_ptr<hermes_cdp_state_s, HermesCdpStateDeleter>;
using HermesUniqueStackTrace = std::unique_ptr<hermes_stack_trace_s, HermesStackTraceDeleter>;
using HermesUniqueSamplingProfile = std::unique_ptr<hermes_sampling_profile_s, HermesSamplingProfileDeleter>;

class HermesInspectorApi {
 public:
  HermesInspectorApi() = delete;

  static void checkStatus(hermes_status status) {
    if (status != hermes_status_ok) {
      throw std::runtime_error("Hermes API call failed");
    }
  }

  static HermesUniqueCdpDebugApi createCdpDebugApi(hermes_runtime runtime) {
    hermes_cdp_debug_api cdp_debug_api{};
    checkStatus(vtable->create_cdp_debug_api(runtime, &cdp_debug_api));
    return HermesUniqueCdpDebugApi{cdp_debug_api};
  }

  static void addConsoleMessage(
      hermes_cdp_debug_api cdpDebugApi,
      double timestamp,
      hermes_console_api_type type,
      const char *argsPropertyName,
      hermes_stack_trace stackTrace) {
    checkStatus(vtable->add_console_message(cdpDebugApi, timestamp, type, argsPropertyName, stackTrace));
  }

  static HermesUniqueCdpAgent createCdpAgent(
      hermes_cdp_debug_api cdpDebugApi,
      int32_t execitionContextId,
      hermes_enqueue_runtime_task_functor enqueueRuntimeTaskCallback,
      hermes_enqueue_frontend_message_functor enqueueFrontendMessageCallback,
      hermes_cdp_state cdp_state) {
    hermes_cdp_agent cdp_agent{};
    checkStatus(vtable->create_cdp_agent(
        cdpDebugApi,
        execitionContextId,
        enqueueRuntimeTaskCallback,
        enqueueFrontendMessageCallback,
        cdp_state,
        &cdp_agent));
    return HermesUniqueCdpAgent{cdp_agent};
  }

  static HermesUniqueCdpState getCdpState(hermes_cdp_agent cdp_agent) {
    hermes_cdp_state cdp_state{};
    checkStatus(vtable->cdp_agent_get_state(cdp_agent, &cdp_state));
    return HermesUniqueCdpState{cdp_state};
  }

  static void handleCommand(hermes_cdp_agent cdpAgent, const char *jsonUtf8, size_t jsonSize) {
    checkStatus(vtable->cdp_agent_handle_command(cdpAgent, jsonUtf8, jsonSize));
  }

  static void enableRuntimeDomain(hermes_cdp_agent cdpAgent) {
    checkStatus(vtable->cdp_agent_enable_runtime_domain(cdpAgent));
  }

  static void enableDebuggerDomain(hermes_cdp_agent cdpAgent) {
    checkStatus(vtable->cdp_agent_enable_debugger_domain(cdpAgent));
  }

  static HermesUniqueStackTrace captureStackTrace(hermes_runtime runtime) {
    hermes_stack_trace stack_trace{};
    checkStatus(vtable->capture_stack_trace(runtime, &stack_trace));
    return HermesUniqueStackTrace{stack_trace};
  }

  static void enableSamplingProfiler(hermes_runtime runtime) {
    checkStatus(vtable->enable_sampling_profiler(runtime));
  }

  static void disableSamplingProfiler(hermes_runtime runtime) {
    checkStatus(vtable->disable_sampling_profiler(runtime));
  }

  static HermesUniqueSamplingProfile collectSamplingProfile(
      hermes_runtime runtime,
      void *cb_data,
      hermes_on_sampling_profile_info_callback on_info_callback,
      hermes_on_sampling_profile_sample_callback on_sample_callback,
      hermes_on_sampling_profile_frame_callback on_frame_callback) {
    hermes_sampling_profile profile{};
    checkStatus(vtable->collect_sampling_profile(
        runtime, cb_data, on_info_callback, on_sample_callback, on_frame_callback, &profile));
    return HermesUniqueSamplingProfile{profile};
  }

 private:
  friend HermesCdpDebugApiDeleter;
  friend HermesCdpAgentDeleter;
  friend HermesCdpStateDeleter;
  friend HermesStackTraceDeleter;
  friend HermesSamplingProfileDeleter;

  friend void setHermesInspectorVTable(const hermes_inspector_vtable *vtable);

  static const hermes_inspector_vtable *vtable;
};

inline void HermesCdpDebugApiDeleter::operator()(hermes_cdp_debug_api cdp_debug_api) {
  HermesInspectorApi::vtable->release_cdp_debug_api(cdp_debug_api);
}

inline void HermesCdpAgentDeleter::operator()(hermes_cdp_agent cdp_agent) {
  HermesInspectorApi::vtable->release_cdp_agent(cdp_agent);
}

inline void HermesCdpStateDeleter::operator()(hermes_cdp_state cdp_state) {
  HermesInspectorApi::vtable->release_cdp_state(cdp_state);
}

inline void HermesStackTraceDeleter::operator()(hermes_stack_trace stack_trace) {
  HermesInspectorApi::vtable->release_stack_trace(stack_trace);
}

inline void HermesSamplingProfileDeleter::operator()(hermes_sampling_profile sampling_profile) {
  HermesInspectorApi::vtable->release_sampling_profile(sampling_profile);
}

class HermesRuntimeHolder : public Microsoft::JSI::RuntimeHolderLazyInit,
                            public std::enable_shared_from_this<HermesRuntimeHolder> {
 public: // RuntimeHolderLazyInit implementation.
  std::shared_ptr<facebook::jsi::Runtime> getRuntime() noexcept override;
  facebook::react::JSIEngineOverride getRuntimeType() noexcept override;
  void crashHandler(int fileDescriptor) noexcept override;

 public:
  HermesRuntimeHolder(
      std::shared_ptr<facebook::react::DevSettings> devSettings,
      std::shared_ptr<facebook::react::MessageQueueThread> jsQueue,
      std::shared_ptr<facebook::jsi::PreparedScriptStore> preparedScriptStore) noexcept;
  ~HermesRuntimeHolder();

  std::shared_ptr<facebook::react::jsinspector_modern::RuntimeTargetDelegate> createRuntimeTargetDelegate() override;

  hermes_runtime getHermesRuntime() noexcept;

 private:
  void initRuntime() noexcept;

 private:
  jsr_runtime m_runtime{};
  std::shared_ptr<facebook::jsi::Runtime> m_jsiRuntime;
  std::once_flag m_onceFlag{};
  std::thread::id m_ownThreadId{};
  std::weak_ptr<facebook::react::DevSettings> m_weakDevSettings;
  std::shared_ptr<facebook::react::MessageQueueThread> m_jsQueue;
  std::shared_ptr<facebook::jsi::PreparedScriptStore> m_preparedScriptStore;
};

class HermesJSRuntime final : public facebook::react::JSRuntime {
 public:
  HermesJSRuntime(std::shared_ptr<Microsoft::JSI::RuntimeHolderLazyInit> hermesRuntimeHolder);

  facebook::jsi::Runtime &getRuntime() noexcept override;

  facebook::react::jsinspector_modern::RuntimeTargetDelegate &getRuntimeTargetDelegate() override;

 private:
  std::shared_ptr<Microsoft::JSI::RuntimeHolderLazyInit> m_holder;
  std::shared_ptr<facebook::react::jsinspector_modern::RuntimeTargetDelegate> m_runtimeTargetDelegate;
};

} // namespace Microsoft::ReactNative
