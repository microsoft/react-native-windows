// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"

#include <cxxreact/CxxModule.h>
#include <cxxreact/CxxNativeModule.h>
#include <cxxreact/Instance.h>
#include <cxxreact/JSBigString.h>
#include <cxxreact/JSExecutor.h>
#include <cxxreact/JsBundleType.h>
#include <cxxreact/ReactMarker.h>
#include <folly/Bits.h>
#include <folly/json.h>
#include <jsi/jsi.h>
#include <jsiexecutor/jsireact/JSIExecutor.h>
#include <filesystem>
#include "OInstance.h"
#include "Unicode.h"

#include "Chakra/ChakraHelpers.h"
#include "Chakra/ChakraUtils.h"
#include "JSI/RuntimeHolder.h"

#include <cxxreact/MessageQueueThread.h>
#include <cxxreact/ModuleRegistry.h>

#include <Modules/ExceptionsManagerModule.h>
#include <Modules/HttpModule.h>
#include <Modules/PlatformConstantsModule.h>
#include <Modules/SourceCodeModule.h>
#include <Modules/StatusBarManagerModule.h>

#if (defined(_MSC_VER) && (defined(WINRT)))
#include <Utils/LocalBundleReader.h>
#endif

#include <BatchingMessageQueueThread.h>
#include <CppRuntimeOptions.h>
#include <CreateModules.h>
#include <DevSettings.h>
#include <DevSupportManager.h>
#include <IReactRootView.h>
#include <Shlwapi.h>
#include <WebSocketJSExecutorFactory.h>
#include <safeint.h>
#include "PackagerConnection.h"

#if defined(USE_HERMES) && defined(ENABLE_DEVSERVER_HBCBUNDLES)
#include <hermes/BytecodeVersion.h>
#endif
#include "HermesRuntimeHolder.h"

#if defined(USE_V8)
#include <JSI/NapiJsiV8RuntimeHolder.h>

#include "BaseScriptStoreImpl.h"
#include "V8JSIRuntimeHolder.h"
#endif
#include <ReactCommon/CallInvoker.h>
#include <ReactCommon/TurboModuleBinding.h>
#include "ChakraRuntimeHolder.h"

#include <tracing/tracing.h>
namespace fs = std::filesystem;

using namespace facebook;
using namespace Microsoft::JSI;

using std::make_shared;
using winrt::Microsoft::ReactNative::ReactPropertyBagHelper;

namespace Microsoft::React {

/*extern*/ std::unique_ptr<facebook::xplat::module::CxxModule> CreateHttpModule(
    winrt::Windows::Foundation::IInspectable const &inspectableProperties) noexcept {
  return std::make_unique<HttpModule>(inspectableProperties);
}

} // namespace Microsoft::React

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
    // [vmoroz] Note, that we must not use the RN TurboCxxModule.h code because it uses global LongLivedObjectCollection
    // instance that prevents us from using multiple RN instance in the same process.
    auto binding = [turboModuleManager](const std::string &name) -> std::shared_ptr<TurboModule> {
      return turboModuleManager->getModule(name);
    };

    TurboModuleBinding::install(
        *runtimeHolder_->getRuntime(),
        std::function(binding),
        TurboModuleBindingMode::HostObject,
        longLivedObjectCollection_);

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
      std::shared_ptr<Microsoft::JSI::RuntimeHolderLazyInit> runtimeHolder,
      NativeLoggingHook loggingHook,
      std::shared_ptr<TurboModuleRegistry> turboModuleRegistry,
      std::shared_ptr<LongLivedObjectCollection> longLivedObjectCollection,
      bool isProfilingEnabled,
      std::shared_ptr<CallInvoker> jsCallInvoker) noexcept
      : runtimeHolder_{std::move(runtimeHolder)},
        loggingHook_{std::move(loggingHook)},
        turboModuleRegistry_{std::move(turboModuleRegistry)},
        longLivedObjectCollection_{std::move(longLivedObjectCollection)},
        jsCallInvoker_{std::move(jsCallInvoker)},
        isProfilingEnabled_{isProfilingEnabled} {}

 private:
  std::shared_ptr<Microsoft::JSI::RuntimeHolderLazyInit> runtimeHolder_;
  std::shared_ptr<TurboModuleRegistry> turboModuleRegistry_;
  std::shared_ptr<LongLivedObjectCollection> longLivedObjectCollection_;
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
    std::shared_ptr<facebook::react::LongLivedObjectCollection> longLivedObjectCollection,
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
      std::move(longLivedObjectCollection),
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
      nullptr,
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

namespace {
bool shouldStartHermesInspector(DevSettings &devSettings) {
  bool isHermes =
      ((devSettings.jsiEngineOverride == JSIEngineOverride::Hermes) ||
       (devSettings.jsiEngineOverride == JSIEngineOverride::Default && devSettings.jsiRuntimeHolder &&
        devSettings.jsiRuntimeHolder->getRuntimeType() == facebook::react::JSIEngineOverride::Hermes));

  if (isHermes && devSettings.useDirectDebugger && !devSettings.useWebDebugger)
    return true;
  else
    return false;
}
} // namespace

InstanceImpl::InstanceImpl(
    std::shared_ptr<Instance> &&instance,
    std::string &&jsBundleBasePath,
    std::vector<
        std::tuple<std::string, facebook::xplat::module::CxxModule::Provider, std::shared_ptr<MessageQueueThread>>>
        &&cxxModules,
    std::shared_ptr<TurboModuleRegistry> turboModuleRegistry,
    std::shared_ptr<facebook::react::LongLivedObjectCollection> longLivedObjectCollection,
    std::unique_ptr<InstanceCallback> &&callback,
    std::shared_ptr<MessageQueueThread> jsQueue,
    std::shared_ptr<MessageQueueThread> nativeQueue,
    std::shared_ptr<DevSettings> devSettings,
    std::shared_ptr<IDevSupportManager> devManager)
    : m_turboModuleRegistry(std::move(turboModuleRegistry)),
      m_longLivedObjectCollection(std::move(longLivedObjectCollection)),
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

  if (shouldStartHermesInspector(*m_devSettings)) {
    m_devManager->EnsureHermesInspector(m_devSettings->sourceBundleHost, m_devSettings->sourceBundlePort);
  }

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
          m_longLivedObjectCollection,
          !m_devSettings->useFastRefresh,
          m_innerInstance->getJSCallInvoker());
    } else {
      assert(m_devSettings->jsiEngineOverride != JSIEngineOverride::Default);
      switch (m_devSettings->jsiEngineOverride) {
        case JSIEngineOverride::Hermes:
          m_devSettings->jsiRuntimeHolder = std::make_shared<HermesRuntimeHolder>(m_devSettings, m_jsThread);
          break;
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
        default: // TODO: Add other engines once supported
          m_devSettings->jsiRuntimeHolder =
              std::make_shared<Microsoft::JSI::ChakraRuntimeHolder>(m_devSettings, m_jsThread, nullptr, nullptr);
          break;
      }
      jsef = std::make_shared<OJSIExecutorFactory>(
          m_devSettings->jsiRuntimeHolder,
          m_devSettings->loggingCallback,
          m_turboModuleRegistry,
          m_longLivedObjectCollection,
          !m_devSettings->useFastRefresh,
          m_innerInstance->getJSCallInvoker());
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

// Note: Based on
// https://github.com/facebook/react-native/blob/24d91268b64c7abbd4b26547ffcc663dc90ec5e7/ReactCommon/cxxreact/Instance.cpp#L112
bool isHBCBundle(const std::string &bundle) {
  static uint32_t constexpr HBCBundleMagicNumber = 0xffe7c3c3;

  // Note:: Directly access the pointer to avoid copy/length-check. It matters as this string contains the bundle which
  // can be potentially huge.
  // https://herbsutter.com/2008/04/07/cringe-not-vectors-are-guaranteed-to-be-contiguous/#comment-483
  auto header = reinterpret_cast<const BundleHeader *>(&bundle[0]);
  if (HBCBundleMagicNumber == header->magic32.value) {
    return true;
  } else {
    return false;
  }
}

void InstanceImpl::loadBundleInternal(std::string &&jsBundleRelativePath, bool synchronously) {
  try {
    if (m_devSettings->useWebDebugger || m_devSettings->liveReloadCallback != nullptr ||
        m_devSettings->useFastRefresh) {
      // First attempt to get download the Js locally, to catch any bundling
      // errors before attempting to load the actual script.

      uint32_t hermesBytecodeVersion = 0;
#if defined(USE_HERMES) && defined(ENABLE_DEVSERVER_HBCBUNDLES)
      hermesBytecodeVersion = ::hermes::hbc::BYTECODE_VERSION;
#endif

      auto [jsBundleString, success] = Microsoft::ReactNative::GetJavaScriptFromServer(
          m_devSettings->sourceBundleHost,
          m_devSettings->sourceBundlePort,
          m_devSettings->debugBundlePath.empty() ? jsBundleRelativePath : m_devSettings->debugBundlePath,
          m_devSettings->platformName,
          true /* dev */,
          m_devSettings->useFastRefresh,
          m_devSettings->inlineSourceMap,
          hermesBytecodeVersion);

      if (!success) {
        m_devManager->UpdateBundleStatus(false, -1);
        m_devSettings->errorCallback(jsBundleString);
        return;
      }

      int64_t currentTimeInMilliSeconds =
          std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch())
              .count();
      m_devManager->UpdateBundleStatus(true, currentTimeInMilliSeconds);

      auto bundleUrl = DevServerHelper::get_BundleUrl(
          m_devSettings->sourceBundleHost,
          m_devSettings->sourceBundlePort,
          m_devSettings->debugBundlePath.empty() ? jsBundleRelativePath : m_devSettings->debugBundlePath,
          m_devSettings->platformName,
          /*dev*/ true,
          /*hot*/ false,
          m_devSettings->inlineSourceMap,
          hermesBytecodeVersion);

      // This code is based on the HBC Bundle integration on Android
      // Ref:
      // https://github.com/facebook/react-native/blob/24d91268b64c7abbd4b26547ffcc663dc90ec5e7/ReactAndroid/src/main/jni/react/jni/CatalystInstanceImpl.cpp#L231
      if (isHBCBundle(jsBundleString)) {
        auto script = std::make_unique<JSBigStdString>(jsBundleString, false);
        const char *buffer = script->c_str();
        uint32_t bufferLength = (uint32_t)script->size();

        // Please refer the code here for details on the file format:
        // https://github.com/facebook/metro/blob/b1bacf52070be62872d6bd3420f37a4405ed34e6/packages/metro/src/lib/bundleToBytecode.js#L29
        // Essentially, there is an 8 byte long file header with 4 bytes of a magic number followed by 4 bytes to encode
        // the number of modules.The module buffers follows, each one starts with 4 byte header which encodes module
        // length.A properly formatted HBCB should have at least 8 bytes..
        uint32_t offset = 8;
#define __SAFEADD__(s1, s2, t)             \
  if (!msl::utilities::SafeAdd(s1, s2, t)) \
    break;
        while (offset < bufferLength) {
          uint32_t segment;
          __SAFEADD__(offset, 4, segment)
          uint32_t moduleLength = (bufferLength < segment) ? 0 : *(((uint32_t *)buffer) + offset / 4);

          // Early break if the module length is computed as 0.. as the segment start may be overflowing the buffer.
          if (moduleLength == 0)
            break;

          uint32_t segmentEnd;
          __SAFEADD__(moduleLength, segment, segmentEnd)
          // Early break if the segment overflows beyond the buffer. This is unlikely for a properly formatted
          // HBCB though.
          if (segmentEnd > bufferLength)
            break;

          m_innerInstance->loadScriptFromString(
              std::make_unique<const JSBigStdString>(std::string(buffer + segment, buffer + segmentEnd)),
              bundleUrl,
              false);

          // Aligned at 4 byte boundary.
          offset += ((moduleLength + 3) & ~3) + 4;
        }
#undef __SAFEADD__
      } else {
        // Remote debug executor loads script from a Uri, rather than taking the actual bundle string
        m_innerInstance->loadScriptFromString(
            std::make_unique<const JSBigStdString>(m_devSettings->useWebDebugger ? bundleUrl : jsBundleString),
            bundleUrl,
            synchronously);
      }
    } else {
#if (defined(_MSC_VER) && !defined(WINRT))
      std::string bundlePath = (fs::u8path(m_devSettings->bundleRootPath) / jsBundleRelativePath).u8string();
      auto bundleString = FileMappingBigString::fromPath(bundlePath);
#else
      std::string bundlePath;
      if (m_devSettings->bundleRootPath._Starts_with("resource://")) {
        auto uri = winrt::Windows::Foundation::Uri(
            winrt::to_hstring(m_devSettings->bundleRootPath), winrt::to_hstring(jsBundleRelativePath));
        bundlePath = winrt::to_string(uri.ToString());
      } else {
        bundlePath = (fs::u8path(m_devSettings->bundleRootPath) / (jsBundleRelativePath + ".bundle")).u8string();
      }

      auto bundleString = std::make_unique<::Microsoft::ReactNative::StorageFileBigString>(bundlePath);
#endif
      m_innerInstance->loadScriptFromString(std::move(bundleString), std::move(jsBundleRelativePath), synchronously);
    }
  } catch (const std::exception &e) {
    m_devSettings->errorCallback(e.what());
  } catch (const winrt::hresult_error &hrerr) {
    auto error = fmt::format("[0x{:0>8x}] {}", static_cast<uint32_t>(hrerr.code()), winrt::to_string(hrerr.message()));

    m_devSettings->errorCallback(std::move(error));
  }
}

InstanceImpl::~InstanceImpl() {
  if (shouldStartHermesInspector(*m_devSettings) && m_devSettings->jsiRuntimeHolder) {
    m_devSettings->jsiRuntimeHolder->teardown();
  }
  m_nativeQueue->quitSynchronous();
}

std::vector<std::unique_ptr<NativeModule>> InstanceImpl::GetDefaultNativeModules(
    std::shared_ptr<MessageQueueThread> nativeQueue) {
  std::vector<std::unique_ptr<NativeModule>> modules;
  auto transitionalProps{ReactPropertyBagHelper::CreatePropertyBag()};

#if (defined(_MSC_VER) && !defined(WINRT))
  modules.push_back(std::make_unique<CxxNativeModule>(
      m_innerInstance,
      Microsoft::React::GetHttpModuleName(),
      [nativeQueue, transitionalProps]() -> std::unique_ptr<xplat::module::CxxModule> {
        return Microsoft::React::CreateHttpModule(transitionalProps);
      },
      nativeQueue));
#endif

  modules.push_back(std::make_unique<CxxNativeModule>(
      m_innerInstance,
      Microsoft::React::GetWebSocketModuleName(),
      [nativeQueue, transitionalProps]() -> std::unique_ptr<xplat::module::CxxModule> {
        return Microsoft::React::CreateWebSocketModule(transitionalProps);
      },
      nativeQueue));

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

  uint32_t hermesBytecodeVersion = 0;
#if defined(USE_HERMES) && defined(ENABLE_DEVSERVER_HBCBUNDLES)
  hermesBytecodeVersion = ::hermes::hbc::BYTECODE_VERSION;
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
            m_devSettings->inlineSourceMap,
            hermesBytecodeVersion)
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

  // These modules are instantiated separately in MSRN (Universal Windows).
  // When there are module name colisions, the last one registered is used.
  // If this code is enabled, we will have unused module instances.
  // Also, MSRN has a different property bag mechanism incompatible with this method's transitionalProps variable.
#if (defined(_MSC_VER) && !defined(WINRT))
  if (Microsoft::React::GetRuntimeOptionBool("Blob.EnableModule")) {
    modules.push_back(std::make_unique<CxxNativeModule>(
        m_innerInstance,
        Microsoft::React::GetBlobModuleName(),
        [transitionalProps]() { return Microsoft::React::CreateBlobModule(transitionalProps); },
        nativeQueue));

    modules.push_back(std::make_unique<CxxNativeModule>(
        m_innerInstance,
        Microsoft::React::GetFileReaderModuleName(),
        [transitionalProps]() { return Microsoft::React::CreateFileReaderModule(transitionalProps); },
        nativeQueue));
  }
#endif

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
