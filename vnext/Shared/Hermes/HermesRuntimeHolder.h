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

namespace facebook::hermes {

// Direct DLL loading implementation that bypasses broken delay loading
inline hermes_api_vtable getHermesApiVTable() {
  static hermes_api_vtable vtable = nullptr;
  static bool initialization_attempted = false;
  
  if (!vtable && !initialization_attempted) {
    initialization_attempted = true;
    // TODO: dont call .dll twice
    HMODULE hermesModule = LoadLibraryW(L"hermes.dll");
    // check where are we calling from
    typedef napi_status (*hermes_get_cdp_vtable_func)(hermes_api_vtable *);
    hermes_get_cdp_vtable_func getCdpVtable = 
        (hermes_get_cdp_vtable_func)GetProcAddress(hermesModule, "hermes_get_cdp_vtable");
    
    if (getCdpVtable) { 
      napi_status status = getCdpVtable(&vtable);
      if (status == napi_ok && vtable) {
        OutputDebugStringA("[RNW] getHermesApiVTable() - CDP vtable loaded successfully!\n");
      } else {
        OutputDebugStringA("[RNW] getHermesApiVTable() - Failed to get CDP vtable\n");
        vtable = nullptr;
      }
    }
  }
  
  return vtable;
}

template <typename TLambda, typename TFunctor>
struct FunctorAdapter {
  static_assert(sizeof(TLambda) == -1, "Unsupported signature");
};

template <typename TLambda, typename TResult, typename... TArgs>
struct FunctorAdapter<TLambda, TResult(void *, TArgs...)> {
  static TResult Invoke(void *data, TArgs... args) {
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
struct FunctorWrapperBase {
  static_assert(sizeof(TInvoke) == -1, "Unsupported signature");
};

template <typename TFunctor, typename TResult, typename... TArgs>
struct FunctorWrapperBase<TFunctor, TResult (*)(void *, TArgs...)> {
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
struct FunctorWrapper : FunctorWrapperBase<TFunctor, decltype(std::declval<TFunctor>().invoke)> {
  using FunctorWrapperBase<TFunctor, decltype(std::declval<TFunctor>().invoke)>::FunctorWrapperBase;
};

struct HermesCdpDebuggerDeleter {
  void operator()(hermes_cdp_debugger cdp_debugger) {
    // Skip deletion for stub/dummy values to prevent crashes
    if (cdp_debugger == nullptr || 
        cdp_debugger == (hermes_cdp_debugger)0x1 || 
        cdp_debugger == (hermes_cdp_debugger)0x2) {
      return;
    }
    
    auto vtable = getHermesApiVTable();
    if (vtable) {
      vtable->release_cdp_debugger(cdp_debugger);
    }
  }
};

struct HermesCdpAgentDeleter {
  void operator()(hermes_cdp_agent cdp_agent) {
    if (cdp_agent == nullptr){
      return;
    }
    
    auto vtable = getHermesApiVTable();
    if (vtable) {
      vtable->release_cdp_agent(cdp_agent);
    }
  }
};

struct HermesCdpStateDeleter {
  void operator()(hermes_cdp_state cdp_state) {
    // Skip deletion for stub/dummy values to prevent crashes
    if (cdp_state == nullptr) {
      return;
    }
    
    auto vtable = getHermesApiVTable();
    if (vtable) {
      vtable->release_cdp_state(cdp_state);
    }
  }
};

struct HermesStackTraceDeleter {
  void operator()(hermes_stack_trace stack_trace) {
    // Skip deletion for stub/dummy values to prevent crashes
    if (stack_trace == nullptr) {
      return;
    }
    
    auto vtable = getHermesApiVTable();
    if (vtable) {
      vtable->release_stack_trace(stack_trace);
    }
  }
};

using HermesUniqueCdpDebugger = std::unique_ptr<hermes_cdp_debugger_s, HermesCdpDebuggerDeleter>;
using HermesUniqueCdpAgent = std::unique_ptr<hermes_cdp_agent_s, HermesCdpAgentDeleter>;
using HermesUniqueCdpState = std::unique_ptr<hermes_cdp_state_s, HermesCdpStateDeleter>;
using HermesUniqueStackTrace = std::unique_ptr<hermes_stack_trace_s, HermesStackTraceDeleter>;

struct HermesApi2 {
  HermesApi2() = default;

  void checkStatus(hermes_status status) {
    if (status != hermes_status_ok) {
      throw std::runtime_error("Hermes API call failed");
    }
  }

  HermesUniqueCdpDebugger createCdpDebugger(hermes_runtime runtime) {
    hermes_cdp_debugger cdp_debugger{};
    checkStatus(vtable->create_cdp_debugger(runtime, &cdp_debugger));
    return HermesUniqueCdpDebugger{cdp_debugger};
  }

  HermesUniqueCdpAgent createCdpAgent(
      hermes_cdp_debugger cdpDebugger,
      int32_t execitionContextId,
      hermes_enqueue_runtime_task_functor enqueueRuntimeTaskCallback,
      hermes_enqueue_frontend_message_functor enqueueFrontendMessageCallback,
      hermes_cdp_state cdp_state) {
    hermes_cdp_agent cdp_agent{};
    checkStatus(vtable->create_cdp_agent(
        cdpDebugger,
        execitionContextId,
        enqueueRuntimeTaskCallback,
        enqueueFrontendMessageCallback,
        cdp_state,
        &cdp_agent));
    return HermesUniqueCdpAgent{cdp_agent};
  }

  HermesUniqueCdpState getCdpState(hermes_cdp_agent cdp_agent) {
    hermes_cdp_state cdp_state{};
    checkStatus(vtable->get_cdp_state(cdp_agent, &cdp_state));
    return HermesUniqueCdpState{cdp_state};
  }

  HermesUniqueStackTrace captureStackTrace(hermes_runtime runtime) {
    hermes_stack_trace stack_trace{};
    checkStatus(vtable->capture_stack_trace(runtime, &stack_trace));
    return HermesUniqueStackTrace{stack_trace};
  }

  void handleCommand(hermes_cdp_agent cdpAgent, const char *jsonUtf8, size_t jsonSize) {
    checkStatus(vtable->cdp_agent_handle_command(cdpAgent, jsonUtf8, jsonSize));
  }

  void enableRuntimeDomain(hermes_cdp_agent cdpAgent) {
    checkStatus(vtable->cdp_agent_enable_runtime_domain(cdpAgent));
  }

  void enableDebuggerDomain(hermes_cdp_agent cdpAgent) {
    checkStatus(vtable->cdp_agent_enable_debugger_domain(cdpAgent));
  }

 private:
  hermes_api_vtable vtable{getHermesApiVTable()};
  
};

} // namespace facebook::hermes

namespace Microsoft::ReactNative {

class HermesRuntimeHolder : public Microsoft::JSI::RuntimeHolderLazyInit, public std::enable_shared_from_this<HermesRuntimeHolder> {
 public: // RuntimeHolderLazyInit implementation.
  std::shared_ptr<facebook::jsi::Runtime> getRuntime() noexcept override;
  facebook::react::JSIEngineOverride getRuntimeType() noexcept override;
  void crashHandler(int fileDescriptor) noexcept override;
  void teardown() noexcept override;

 public:
  HermesRuntimeHolder(
      std::shared_ptr<facebook::react::DevSettings> devSettings,
      std::shared_ptr<facebook::react::MessageQueueThread> jsQueue,
      std::shared_ptr<facebook::jsi::PreparedScriptStore> preparedScriptStore) noexcept;
  ~HermesRuntimeHolder();

  const std::shared_ptr<facebook::react::jsinspector_modern::RuntimeTargetDelegate> &getSharedRuntimeTargetDelegate()
      override;

  static std::shared_ptr<HermesRuntimeHolder> loadFrom(
      winrt::Microsoft::ReactNative::ReactPropertyBag const &propertyBag) noexcept;

  static void storeTo(
      winrt::Microsoft::ReactNative::ReactPropertyBag const &propertyBag,
      std::shared_ptr<HermesRuntimeHolder> const &holder) noexcept;

  void addToProfiling() const noexcept;
  void removeFromProfiling() const noexcept;

  static void enableSamplingProfiler() noexcept;
  static void disableSamplingProfiler() noexcept;
  static void dumpSampledTraceToFile(const std::string &fileName) noexcept;

  hermes_runtime getHermesRuntime() noexcept;

 private:
  void initRuntime() noexcept;

 private:
  jsr_runtime m_runtime{};
  std::shared_ptr<facebook::jsi::Runtime> m_jsiRuntime;
  std::shared_ptr<facebook::react::jsinspector_modern::RuntimeTargetDelegate> m_targetDelegate;
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
  void addConsoleMessage(facebook::jsi::Runtime &runtime, facebook::react::jsinspector_modern::ConsoleMessage message)
    override;
  bool supportsConsole() const override;
  std::unique_ptr<facebook::react::jsinspector_modern::StackTrace> captureStackTrace(
      facebook::jsi::Runtime &runtime,
      size_t framesToSkip = 0) override;
  
  /**
   * Start sampling profiler.
   */
  void enableSamplingProfiler() override;

  /**
   * Stop sampling profiler.
   */
  void disableSamplingProfiler() override;

  /**
   * Return recorded sampling profile for the previous sampling session.
   */
  facebook::react::jsinspector_modern::tracing::RuntimeSamplingProfile collectSamplingProfile() override;

  std::unique_ptr<facebook::react::jsinspector_modern::RuntimeAgentDelegate> createAgentDelegate(
      facebook::react::jsinspector_modern::FrontendChannel frontendChannel,
      facebook::react::jsinspector_modern::SessionState &sessionState,
      std::unique_ptr<facebook::react::jsinspector_modern::RuntimeAgentDelegate::ExportedState> previouslyExportedState,
      const facebook::react::jsinspector_modern::ExecutionContextDescription &executionContextDescription,
      facebook::react::RuntimeExecutor runtimeExecutor) override;

  facebook::react::jsinspector_modern::RuntimeTargetDelegate &getRuntimeTargetDelegate() override;

 private:
  std::shared_ptr<Microsoft::JSI::RuntimeHolderLazyInit> m_holder;
};

} // namespace Microsoft::ReactNative
