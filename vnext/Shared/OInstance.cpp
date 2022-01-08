// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"

#include <cxxreact/CxxModule.h>
#include <cxxreact/CxxNativeModule.h>
#include <cxxreact/Instance.h>
#include <cxxreact/JSBigString.h>
#include <cxxreact/JSExecutor.h>
#include <cxxreact/ReactMarker.h>
#include <jsi/jsi.h>
#include <jsiexecutor/jsireact/JSIExecutor.h>
#include <filesystem>
#include "OInstance.h"
#include "Unicode.h"

#include "Chakra/ChakraExecutor.h"
#include "Chakra/ChakraHelpers.h"
#include "Chakra/ChakraUtils.h"
#include "JSI/Shared/RuntimeHolder.h"

#include <cxxreact/MessageQueueThread.h>
#include <cxxreact/ModuleRegistry.h>

#if (defined(_MSC_VER) && !defined(WINRT))
#include <Modules/WebSocketModule.h>
#endif
#include <Modules/ExceptionsManagerModule.h>
#include <Modules/PlatformConstantsModule.h>
#include <Modules/SourceCodeModule.h>
#include <Modules/StatusBarManagerModule.h>

#if (defined(_MSC_VER) && (defined(WINRT)))
#include <Utils/LocalBundleReader.h>
#endif

#include <BatchingMessageQueueThread.h>
#include <CreateModules.h>
#include <DevSettings.h>
#include <DevSupportManager.h>
#include <IReactRootView.h>
#include <RuntimeOptions.h>
#include <Shlwapi.h>
#include <WebSocketJSExecutorFactory.h>
#include "PackagerConnection.h"

#if defined(USE_HERMES)
#include "HermesRuntimeHolder.h"
#endif
#if defined(USE_V8)
#include <NapiJsiV8RuntimeHolder.h>

#include "BaseScriptStoreImpl.h"
#include "V8JSIRuntimeHolder.h"
#endif
#include <ReactCommon/CallInvoker.h>
#include <ReactCommon/TurboModuleBinding.h>
#include "ChakraRuntimeHolder.h"

#if (defined(_MSC_VER) && !defined(WINRT))
// Type only available in Desktop.
using Microsoft::React::WebSocketModule;
#endif

namespace fs = std::filesystem;

// forward declaration.
namespace facebook::react::tracing {
void initializeETW();
void initializeJSHooks(facebook::jsi::Runtime &runtime, bool isProfiling);
} // namespace facebook::react::tracing

using namespace facebook;
using namespace Microsoft::JSI;

using std::make_shared;

namespace facebook {
namespace react {

namespace {

class OJSIExecutorFactory : public JSExecutorFactory {
 public:
  std::unique_ptr<JSExecutor> createJSExecutor(
      std::shared_ptr<ExecutorDelegate> delegate,
      std::shared_ptr<MessageQueueThread> jsQueue) override {
    Logger logger;
    if (loggingHook_) {
      // TODO :: Ensure the logLevels are mapped properly.
      logger = [loggingHook = std::move(loggingHook_)](const std::string &message, unsigned int logLevel) {
        loggingHook(static_cast<RCTLogLevel>(logLevel), message.c_str());
      };
    } else {
      logger = [loggingHook = std::move(loggingHook_)](const std::string & /*message*/, unsigned int /*logLevel*/) {};
    }
    bindNativeLogger(*runtimeHolder_->getRuntime(), logger);

    auto turboModuleManager = std::make_shared<TurboModuleManager>(turboModuleRegistry_, jsCallInvoker_);

    // TODO: The binding here should also add the proxys that convert cxxmodules into turbomodules
    auto binding = [turboModuleManager](
                       const std::string &name, const jsi::Value *
                       /*schema*/) -> std::shared_ptr<TurboModule> { return turboModuleManager->getModule(name); };

    TurboModuleBinding::install(*runtimeHolder_->getRuntime(), std::function(binding));

    // init TurboModule
    for (const auto &moduleName : turboModuleManager->getEagerInitModuleNames()) {
      turboModuleManager->getModule(moduleName);
    }

    return std::make_unique<JSIExecutor>(
        runtimeHolder_->getRuntime(),
        std::move(delegate),
        JSIExecutor::defaultTimeoutInvoker,
        [isProfiling = isProfilingEnabled_]([[maybe_unused]] jsi::Runtime &runtime) {
#ifdef ENABLE_JS_SYSTRACE_TO_ETW
          facebook::react::tracing::initializeJSHooks(runtime, isProfiling);
#endif
        });
  }

  OJSIExecutorFactory(
      std::shared_ptr<jsi::RuntimeHolderLazyInit> runtimeHolder,
      NativeLoggingHook loggingHook,
      std::shared_ptr<TurboModuleRegistry> turboModuleRegistry,
      bool isProfilingEnabled,
      std::shared_ptr<CallInvoker> jsCallInvoker) noexcept
      : runtimeHolder_{std::move(runtimeHolder)},
        loggingHook_{std::move(loggingHook)},
        turboModuleRegistry_{std::move(turboModuleRegistry)},
        jsCallInvoker_{std::move(jsCallInvoker)},
        isProfilingEnabled_{isProfilingEnabled} {}

 private:
  std::shared_ptr<jsi::RuntimeHolderLazyInit> runtimeHolder_;
  std::shared_ptr<TurboModuleRegistry> turboModuleRegistry_;
  std::shared_ptr<CallInvoker> jsCallInvoker_;
  NativeLoggingHook loggingHook_;
  bool isProfilingEnabled_;
};

} // namespace

void logMarker(const facebook::react::ReactMarker::ReactMarkerId /*id*/, const char * /*tag*/) {}

/*static*/ std::shared_ptr<InstanceImpl> InstanceImpl::MakeNoBundle(
    std::shared_ptr<Instance> &&instance,
    std::string &&jsBundleBasePath,
    std::vector<
        std::tuple<std::string, facebook::xplat::module::CxxModule::Provider, std::shared_ptr<MessageQueueThread>>>
        &&cxxModules,
    std::shared_ptr<TurboModuleRegistry> turboModuleRegistry,
    std::unique_ptr<InstanceCallback> &&callback,
    std::shared_ptr<MessageQueueThread> jsQueue,
    std::shared_ptr<MessageQueueThread> nativeQueue,
    std::shared_ptr<DevSettings> devSettings,
    std::shared_ptr<IDevSupportManager> devManager) noexcept {
  auto inner = std::shared_ptr<InstanceImpl>(new InstanceImpl(
      std::move(instance),
      std::move(jsBundleBasePath),
      std::move(cxxModules),
      std::move(turboModuleRegistry),
      std::move(callback),
      std::move(jsQueue),
      std::move(nativeQueue),
      std::move(devSettings),
      std::move(devManager)));

  inner->RegisterForReloadIfNecessary();

  return inner;
}

/*static*/ std::shared_ptr<InstanceImpl> InstanceImpl::MakeAndLoadBundle(
    std::shared_ptr<Instance> &&instance,
    std::string &&jsBundleBasePath,
    std::string &&jsBundleRelativePath,
    std::vector<
        std::tuple<std::string, facebook::xplat::module::CxxModule::Provider, std::shared_ptr<MessageQueueThread>>>
        &&cxxModules,
    std::shared_ptr<TurboModuleRegistry> turboModuleRegistry,
    std::unique_ptr<InstanceCallback> &&callback,
    std::shared_ptr<MessageQueueThread> jsQueue,
    std::shared_ptr<MessageQueueThread> nativeQueue,
    std::shared_ptr<DevSettings> devSettings,
    std::shared_ptr<IDevSupportManager> devManager) noexcept {
  auto inner = std::shared_ptr<InstanceImpl>(new InstanceImpl(
      std::move(instance),
      std::move(jsBundleBasePath),
      std::move(cxxModules),
      std::move(turboModuleRegistry),
      std::move(callback),
      std::move(jsQueue),
      std::move(nativeQueue),
      std::move(devSettings),
      std::move(devManager)));

  inner->loadBundle(std::move(jsBundleRelativePath));
  inner->RegisterForReloadIfNecessary();

  return inner;
}

void InstanceImpl::SetInError() noexcept {
  m_isInError = true;
}

InstanceImpl::InstanceImpl(
    std::shared_ptr<Instance> &&instance,
    std::string &&jsBundleBasePath,
    std::vector<
        std::tuple<std::string, facebook::xplat::module::CxxModule::Provider, std::shared_ptr<MessageQueueThread>>>
        &&cxxModules,
    std::shared_ptr<TurboModuleRegistry> turboModuleRegistry,
    std::unique_ptr<InstanceCallback> &&callback,
    std::shared_ptr<MessageQueueThread> jsQueue,
    std::shared_ptr<MessageQueueThread> nativeQueue,
    std::shared_ptr<DevSettings> devSettings,
    std::shared_ptr<IDevSupportManager> devManager)
    : m_turboModuleRegistry(std::move(turboModuleRegistry)),
      m_jsThread(std::move(jsQueue)),
      m_nativeQueue(nativeQueue),
      m_jsBundleBasePath(std::move(jsBundleBasePath)),
      m_devSettings(std::move(devSettings)),
      m_devManager(std::move(devManager)),
      m_innerInstance(std::move(instance)) {
  // Temp set the logmarker here
  facebook::react::ReactMarker::logTaggedMarker = logMarker;

#ifdef ENABLE_ETW_TRACING
  // TODO :: Find a better place to initialize ETW once per process.
  facebook::react::tracing::initializeETW();
#endif

  // Default (common) NativeModules
  auto modules = GetDefaultNativeModules(nativeQueue);

  // Add app provided modules.
  for (auto &cxxModule : cxxModules) {
    modules.push_back(std::make_unique<CxxNativeModule>(
        m_innerInstance, move(std::get<0>(cxxModule)), move(std::get<1>(cxxModule)), move(std::get<2>(cxxModule))));
  }
  m_moduleRegistry = std::make_shared<facebook::react::ModuleRegistry>(std::move(modules));

  // Choose JSExecutor
  std::shared_ptr<JSExecutorFactory> jsef;
  if (m_devSettings->useWebDebugger) {
    try {
      auto jseFunc = m_devManager->LoadJavaScriptInProxyMode(*m_devSettings, [weakthis = weak_from_this()]() {
        if (auto strongThis = weakthis.lock()) {
          strongThis->SetInError();
        }
      });

      if ((jseFunc == nullptr) || m_isInError) {
        m_devSettings->errorCallback("Failed to create JavaScript Executor.");
        return;
      }

      jsef = std::make_shared<WebSocketJSExecutorFactory>(std::move(jseFunc));
    } catch (std::exception &e) {
      m_devSettings->errorCallback(e.what());
      return;
    }
  } else {
    if (m_devSettings->useFastRefresh || m_devSettings->liveReloadCallback) {
      Microsoft::ReactNative::PackagerConnection::CreateOrReusePackagerConnection(*m_devSettings);
    }

    // If the consumer gives us a JSI runtime, then  use it.
    if (m_devSettings->jsiRuntimeHolder) {
      assert(m_devSettings->jsiEngineOverride == JSIEngineOverride::Default);
      jsef = std::make_shared<OJSIExecutorFactory>(
          m_devSettings->jsiRuntimeHolder,
          m_devSettings->loggingCallback,
          m_turboModuleRegistry,
          !m_devSettings->useFastRefresh,
          m_innerInstance->getJSCallInvoker());
    } else if (m_devSettings->jsiEngineOverride != JSIEngineOverride::Default) {
      switch (m_devSettings->jsiEngineOverride) {
        case JSIEngineOverride::Hermes:
#if defined(USE_HERMES)
          m_devSettings->jsiRuntimeHolder = std::make_shared<HermesRuntimeHolder>();
          m_devSettings->inlineSourceMap = false;
          break;
#else
          assert(false); // Hermes is not available in this build, fallthrough
          [[fallthrough]];
#endif
        case JSIEngineOverride::V8: {
#if defined(USE_V8)
          std::unique_ptr<facebook::jsi::ScriptStore> scriptStore = nullptr;
          std::unique_ptr<facebook::jsi::PreparedScriptStore> preparedScriptStore = nullptr;

          char tempPath[MAX_PATH];
          if (GetTempPathA(MAX_PATH, tempPath)) {
            preparedScriptStore = std::make_unique<facebook::react::BasePreparedScriptStoreImpl>(tempPath);
          }

          m_devSettings->jsiRuntimeHolder = std::make_shared<facebook::react::V8JSIRuntimeHolder>(
              m_devSettings, m_jsThread, std::move(scriptStore), std::move(preparedScriptStore));
          break;
#else
          assert(false); // V8 is not available in this build, fallthrough
          [[fallthrough]];
#endif
        }
        case JSIEngineOverride::V8NodeApi: {
#if defined(USE_V8)
          std::unique_ptr<facebook::jsi::PreparedScriptStore> preparedScriptStore;

          wchar_t tempPath[MAX_PATH];
          if (GetTempPathW(static_cast<DWORD>(std::size(tempPath)), tempPath)) {
            preparedScriptStore =
                std::make_unique<facebook::react::BasePreparedScriptStoreImpl>(winrt::to_string(tempPath));
          }

          if (!preparedScriptStore) {
            if (m_devSettings->errorCallback)
              m_devSettings->errorCallback("Could not initialize prepared script store");

            break;
          }

          m_devSettings->jsiRuntimeHolder = make_shared<NapiJsiV8RuntimeHolder>(
              m_devSettings, m_jsThread, nullptr /*scriptStore*/, std::move(preparedScriptStore));
          break;
#else
          if (m_devSettings->errorCallback)
            m_devSettings->errorCallback("JSI/V8/NAPI engine is not available in this build");
          assert(false);
          [[fallthrough]];
#endif
        }
        case JSIEngineOverride::Chakra:
        case JSIEngineOverride::ChakraCore:
        default: // TODO: Add other engines once supported
          m_devSettings->jsiRuntimeHolder =
              std::make_shared<Microsoft::JSI::ChakraRuntimeHolder>(m_devSettings, m_jsThread, nullptr, nullptr);
          break;
      }
      jsef = std::make_shared<OJSIExecutorFactory>(
          m_devSettings->jsiRuntimeHolder,
          m_devSettings->loggingCallback,
          m_turboModuleRegistry,
          !m_devSettings->useFastRefresh,
          m_innerInstance->getJSCallInvoker());
    } else {
#if defined(USE_V8)
      std::unique_ptr<facebook::jsi::ScriptStore> scriptStore = nullptr;
      std::unique_ptr<facebook::jsi::PreparedScriptStore> preparedScriptStore = nullptr;

      char tempPath[MAX_PATH];
      if (GetTempPathA(MAX_PATH, tempPath)) {
        preparedScriptStore = std::make_unique<facebook::react::BasePreparedScriptStoreImpl>(tempPath);
      }

      m_devSettings->jsiRuntimeHolder = std::make_shared<facebook::react::V8JSIRuntimeHolder>(
          m_devSettings, m_jsThread, std::move(scriptStore), std::move(preparedScriptStore));

      jsef = std::make_shared<OJSIExecutorFactory>(
          m_devSettings->jsiRuntimeHolder,
          m_devSettings->loggingCallback,
          m_turboModuleRegistry,
          !m_devSettings->useFastRefresh,
          m_innerInstance->getJSCallInvoker());
#else
      // We use the older non-JSI ChakraExecutor pipeline as a fallback as of
      // now. This will go away once we completely move to JSI flow.
      ChakraInstanceArgs instanceArgs;

      instanceArgs.DebuggerBreakOnNextLine = m_devSettings->debuggerBreakOnNextLine;
      instanceArgs.DebuggerPort = m_devSettings->debuggerPort;
      instanceArgs.DebuggerRuntimeName = m_devSettings->debuggerRuntimeName;

      instanceArgs.EnableDebugging = m_devSettings->useDirectDebugger;
      instanceArgs.LoggingCallback = m_devSettings->loggingCallback;

      instanceArgs.EnableNativePerformanceNow = m_devSettings->enableNativePerformanceNow;
      instanceArgs.DebuggerConsoleRedirection = m_devSettings->debuggerConsoleRedirection;

      // Disable bytecode caching with live reload as we don't make guarantees
      // that the bundle version will change with edits
      if (m_devSettings->liveReloadCallback == nullptr) {
        instanceArgs.BundleUrlMetadataMap = m_devSettings->chakraBundleUrlMetadataMap;
      }

      if (!m_devSettings->useJITCompilation) {
#if (defined(_MSC_VER) && !defined(WINRT))
        instanceArgs.RuntimeAttributes = static_cast<JsRuntimeAttributes>(
            instanceArgs.RuntimeAttributes | JsRuntimeAttributeDisableNativeCodeGeneration |
            JsRuntimeAttributeDisableExecutablePageAllocation);
#else
        instanceArgs.RuntimeAttributes = static_cast<JsRuntimeAttributes>(
            instanceArgs.RuntimeAttributes | JsRuntimeAttributeDisableNativeCodeGeneration);
#endif
      }

      instanceArgs.MemoryTracker = m_devSettings->memoryTracker
          ? m_devSettings->memoryTracker
          : CreateMemoryTracker(std::shared_ptr<MessageQueueThread>{m_nativeQueue});

      jsef = std::make_shared<ChakraExecutorFactory>(std::move(instanceArgs));
#endif
    }
  }

  m_innerInstance->initializeBridge(std::move(callback), jsef, m_jsThread, m_moduleRegistry);

  // All JSI runtimes do support host objects and hence the native modules
  // proxy.
  const bool isNativeModulesProxyAvailable = ((m_devSettings->jsiRuntimeHolder != nullptr) ||
                                              (m_devSettings->jsiEngineOverride != JSIEngineOverride::Default)) &&
      !m_devSettings->useWebDebugger;
  if (!isNativeModulesProxyAvailable) {
    folly::dynamic configArray = folly::dynamic::array;
    for (auto const &moduleName : m_moduleRegistry->moduleNames()) {
      auto moduleConfig = m_moduleRegistry->getConfig(moduleName);
      configArray.push_back(moduleConfig ? std::move(moduleConfig->config) : nullptr);
    }

    folly::dynamic configs = folly::dynamic::object("remoteModuleConfig", std::move(configArray));
    m_innerInstance->setGlobalVariable(
        "__fbBatchedBridgeConfig", std::make_unique<JSBigStdString>(folly::toJson(configs)));
  }
}

void InstanceImpl::loadBundle(std::string &&jsBundleRelativePath) {
  loadBundleInternal(std::move(jsBundleRelativePath), /*synchronously:*/ false);
}

void InstanceImpl::loadBundleSync(std::string &&jsBundleRelativePath) {
  loadBundleInternal(std::move(jsBundleRelativePath), /*synchronously:*/ true);
}

void InstanceImpl::loadBundleInternal(std::string &&jsBundleRelativePath, bool synchronously) {
  try {
    if (m_devSettings->useWebDebugger || m_devSettings->liveReloadCallback != nullptr ||
        m_devSettings->useFastRefresh) {
      // First attempt to get download the Js locally, to catch any bundling
      // errors before attempting to load the actual script.

      auto [jsBundleString, success] = Microsoft::ReactNative::GetJavaScriptFromServer(
          m_devSettings->sourceBundleHost,
          m_devSettings->sourceBundlePort,
          m_devSettings->debugBundlePath.empty() ? jsBundleRelativePath : m_devSettings->debugBundlePath,
          m_devSettings->platformName,
          true /* dev */,
          m_devSettings->useFastRefresh,
          m_devSettings->inlineSourceMap);

      if (!success) {
        m_devSettings->errorCallback(jsBundleString);
        return;
      }

      auto bundleUrl = DevServerHelper::get_BundleUrl(
          m_devSettings->sourceBundleHost,
          m_devSettings->sourceBundlePort,
          m_devSettings->debugBundlePath.empty() ? jsBundleRelativePath : m_devSettings->debugBundlePath,
          m_devSettings->platformName,
          /*dev*/ true,
          /*hot*/ false,
          m_devSettings->inlineSourceMap);

      // Remote debug executor loads script from a Uri, rather than taking the actual bundle string
      m_innerInstance->loadScriptFromString(
          std::make_unique<const JSBigStdString>(m_devSettings->useWebDebugger ? bundleUrl : jsBundleString),
          bundleUrl,
          synchronously);
    } else {
#if (defined(_MSC_VER) && !defined(WINRT))
      std::string bundlePath = (fs::path(m_devSettings->bundleRootPath) / jsBundleRelativePath).string();
      auto bundleString = FileMappingBigString::fromPath(bundlePath);
#else
      std::string bundlePath = (fs::path(m_devSettings->bundleRootPath) / (jsBundleRelativePath + ".bundle")).string();
      auto bundleString = std::make_unique<::react::uwp::StorageFileBigString>(bundlePath);
#endif
      m_innerInstance->loadScriptFromString(std::move(bundleString), std::move(jsBundleRelativePath), synchronously);
    }
  } catch (const std::exception &e) {
    m_devSettings->errorCallback(e.what());
  } catch (const winrt::hresult_error &hrerr) {
    std::stringstream ss;
    ss << "[" << std::hex << std::showbase << std::setw(8) << static_cast<uint32_t>(hrerr.code()) << "] "
       << winrt::to_string(hrerr.message());

    m_devSettings->errorCallback(std::move(ss.str()));
  }
}

InstanceImpl::~InstanceImpl() {
  m_nativeQueue->quitSynchronous();
}

std::vector<std::unique_ptr<NativeModule>> InstanceImpl::GetDefaultNativeModules(
    std::shared_ptr<MessageQueueThread> nativeQueue) {
  std::vector<std::unique_ptr<NativeModule>> modules;
// TODO: This is not included for UWP due to difficulties getting it working.
// For now, we still use the old module
//  written specifically for UWP. That one gets added later.
#if (defined(_MSC_VER) && !defined(WINRT))
  modules.push_back(std::make_unique<CxxNativeModule>(
      m_innerInstance,
      "WebSocketModule",
      []() -> std::unique_ptr<xplat::module::CxxModule> { return std::make_unique<WebSocketModule>(); },
      nativeQueue));
#endif
// TODO: This is not included for UWP because we have a different module which
// is added later. However, this one is designed
//  so that we can base a UWP version on it. We need to do that but is not high
//  priority.
#if (defined(_MSC_VER) && !defined(WINRT))
  modules.push_back(std::make_unique<CxxNativeModule>(
      m_innerInstance,
      "Timing",
      [nativeQueue]() -> std::unique_ptr<xplat::module::CxxModule> { return react::CreateTimingModule(nativeQueue); },
      nativeQueue));
#endif

  // TODO - Encapsulate this in a helpers, and make sure callers add it to their
  // list
  std::string bundleUrl = (m_devSettings->useWebDebugger || m_devSettings->liveReloadCallback)
      ? DevServerHelper::get_BundleUrl(
            m_devSettings->sourceBundleHost,
            m_devSettings->sourceBundlePort,
            m_devSettings->debugBundlePath,
            m_devSettings->platformName,
            true /*dev*/,
            m_devSettings->useFastRefresh,
            m_devSettings->inlineSourceMap)
      : std::string();
  modules.push_back(std::make_unique<CxxNativeModule>(
      m_innerInstance,
      facebook::react::SourceCodeModule::Name,
      [bundleUrl]() -> std::unique_ptr<xplat::module::CxxModule> {
        return std::make_unique<SourceCodeModule>(bundleUrl);
      },
      nativeQueue));

  modules.push_back(std::make_unique<CxxNativeModule>(
      m_innerInstance,
      "ExceptionsManager",
      [redboxHandler = m_devSettings->redboxHandler]() mutable {
        return std::make_unique<ExceptionsManagerModule>(redboxHandler);
      },
      nativeQueue));

  modules.push_back(std::make_unique<CxxNativeModule>(
      m_innerInstance,
      PlatformConstantsModule::Name,
      []() { return std::make_unique<PlatformConstantsModule>(); },
      nativeQueue));

  modules.push_back(std::make_unique<CxxNativeModule>(
      m_innerInstance,
      StatusBarManagerModule::Name,
      []() { return std::make_unique<StatusBarManagerModule>(); },
      nativeQueue));

  return modules;
}

void InstanceImpl::RegisterForReloadIfNecessary() noexcept {
  // setup polling for live reload
  if (!m_isInError && !m_devSettings->useFastRefresh && m_devSettings->liveReloadCallback != nullptr) {
    m_devManager->StartPollingLiveReload(
        m_devSettings->sourceBundleHost, m_devSettings->sourceBundlePort, m_devSettings->liveReloadCallback);
  }
}

void InstanceImpl::DispatchEvent(int64_t viewTag, std::string eventName, folly::dynamic &&eventData) {
  if (m_isInError) {
    return;
  }

  folly::dynamic params = folly::dynamic::array(viewTag, eventName, std::move(eventData));
  m_innerInstance->callJSFunction("RCTEventEmitter", "receiveEvent", std::move(params));
}

void InstanceImpl::invokeCallback(const int64_t callbackId, folly::dynamic &&params) {
  if (m_isInError) {
    return;
  }

  m_innerInstance->callJSCallback(callbackId, std::move(params));
}

} // namespace react
} // namespace facebook
