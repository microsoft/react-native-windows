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
#include <jsireact/JSIExecutor.h>
#include <filesystem>
#include "OInstance.h"
#include "Unicode.h"

#include "Chakra/ChakraHelpers.h"
#include "Chakra/ChakraUtils.h"
#include "JSI/RuntimeHolder.h"

#include <cxxreact/MessageQueueThread.h>
#include <cxxreact/ModuleRegistry.h>

#if (defined(_MSC_VER) && (defined(WINRT)))
#include <Utils/LocalBundleReader.h>
#endif

#include <BatchingMessageQueueThread.h>
#include <CppRuntimeOptions.h>
#include <CreateModules.h>
#include <DevSettings.h>
#include <DevSupportManager.h>
#include <IReactRootView.h>
#include <ReactPropertyBag.h>
#include <SchedulerSettings.h>
#include <Shlwapi.h>
#include <safeint.h>
#include "Inspector/ReactInspectorThread.h"
#include "PackagerConnection.h"
#include "Threading/MessageDispatchQueue.h"

#if defined(USE_HERMES) && defined(ENABLE_DEVSERVER_HBCBUNDLES)
#include <hermes/BytecodeVersion.h>
#endif
#include "HermesRuntimeHolder.h"

#if defined(USE_V8)
#include <JSI/V8RuntimeHolder.h>
#include "V8JSIRuntimeHolder.h"
#endif
#include <ReactCommon/CallInvoker.h>
#include <ReactCommon/TurboModuleBinding.h>
#include <react/renderer/runtimescheduler/RuntimeScheduler.h>
#include <react/renderer/runtimescheduler/RuntimeSchedulerBinding.h>
#include <react/renderer/runtimescheduler/RuntimeSchedulerCallInvoker.h>
#include "BaseScriptStoreImpl.h"
#include "ChakraRuntimeHolder.h"

#include <tracing/tracing.h>
namespace fs = std::filesystem;

using namespace facebook;
using namespace Microsoft::JSI;

using std::make_shared;
using winrt::Microsoft::ReactNative::ReactPropertyBagHelper;

namespace Microsoft::ReactNative {

// Note: Based on
// https://github.com/facebook/react-native/blob/24d91268b64c7abbd4b26547ffcc663dc90ec5e7/ReactCommon/cxxreact/Instance.cpp#L112
bool isHBCBundle(const std::string &bundle) {
  static uint32_t constexpr HBCBundleMagicNumber = 0xffe7c3c3;

  // Note:: Directly access the pointer to avoid copy/length-check. It matters as this string contains the bundle which
  // can be potentially huge.
  // https://herbsutter.com/2008/04/07/cringe-not-vectors-are-guaranteed-to-be-contiguous/#comment-483
  auto header = reinterpret_cast<const facebook::react::BundleHeader *>(&bundle[0]);
  if (HBCBundleMagicNumber == header->magic32.value) {
    return true;
  } else {
    return false;
  }
}

void LoadRemoteUrlScript(
    std::shared_ptr<facebook::react::DevSettings> devSettings,
    std::shared_ptr<facebook::react::IDevSupportManager> devManager,
    std::string &&jsBundleRelativePath,
    std::function<void(std::unique_ptr<const facebook::react::JSBigStdString> script, const std::string &sourceURL)>
        fnLoadScriptCallback) noexcept {
  // First attempt to get download the Js locally, to catch any bundling
  // errors before attempting to load the actual script.

  uint32_t hermesBytecodeVersion = 0;
#if defined(USE_HERMES) && defined(ENABLE_DEVSERVER_HBCBUNDLES)
  hermesBytecodeVersion = ::hermes::hbc::BYTECODE_VERSION;
#endif

  auto [jsBundleString, success] = GetJavaScriptFromServer(
      devSettings->sourceBundleHost,
      devSettings->sourceBundlePort,
      devSettings->debugBundlePath.empty() ? jsBundleRelativePath : devSettings->debugBundlePath,
      devSettings->platformName,
      devSettings->bundleAppId,
      devSettings->devBundle,
      devSettings->useFastRefresh,
      devSettings->inlineSourceMap,
      hermesBytecodeVersion);

  if (!success) {
    devSettings->errorCallback(jsBundleString);
    return;
  }

  int64_t currentTimeInMilliSeconds =
      std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch())
          .count();

  auto bundleUrl = facebook::react::DevServerHelper::get_BundleUrl(
      devSettings->sourceBundleHost,
      devSettings->sourceBundlePort,
      devSettings->debugBundlePath.empty() ? jsBundleRelativePath : devSettings->debugBundlePath,
      devSettings->platformName,
      devSettings->bundleAppId,
      devSettings->devBundle,
      devSettings->useFastRefresh,
      devSettings->inlineSourceMap,
      hermesBytecodeVersion);

  // This code is based on the HBC Bundle integration on Android
  // Ref:
  // https://github.com/facebook/react-native/blob/24d91268b64c7abbd4b26547ffcc663dc90ec5e7/ReactAndroid/src/main/jni/react/jni/CatalystInstanceImpl.cpp#L231
  if (isHBCBundle(jsBundleString)) {
    auto script = std::make_unique<facebook::react::JSBigStdString>(jsBundleString, false);
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

      fnLoadScriptCallback(
          std::make_unique<const facebook::react::JSBigStdString>(std::string(buffer + segment, buffer + segmentEnd)),
          bundleUrl);

      // Aligned at 4 byte boundary.
      offset += ((moduleLength + 3) & ~3) + 4;
    }
#undef __SAFEADD__
  } else {
    // Remote debug executor loads script from a Uri, rather than taking the actual bundle string
    fnLoadScriptCallback(std::make_unique<const facebook::react::JSBigStdString>(jsBundleString), bundleUrl);
  }
}

std::unique_ptr<const facebook::react::JSBigString> JsBigStringFromPath(
    std::shared_ptr<facebook::react::DevSettings> devSettings,
    const std::string &jsBundleRelativePath) noexcept {
#if (defined(_MSC_VER) && !defined(WINRT))
  std::wstring bundlePath = (fs::u8path(devSettings->bundleRootPath) / jsBundleRelativePath).wstring();
  return facebook::react::FileMappingBigString::fromPath(bundlePath);
#else
  std::wstring bundlePath;
  if (devSettings->bundleRootPath.starts_with("resource://")) {
    auto uri = winrt::Windows::Foundation::Uri(
        winrt::to_hstring(devSettings->bundleRootPath), winrt::to_hstring(jsBundleRelativePath));
    bundlePath = uri.ToString();
  } else {
    bundlePath = (fs::u8path(devSettings->bundleRootPath) / (jsBundleRelativePath + ".bundle")).wstring();
  }

  return std::make_unique<::Microsoft::ReactNative::StorageFileBigString>(bundlePath);
#endif
}

} // namespace Microsoft::ReactNative

namespace facebook::react {

namespace {

// OJSIExecutor is needed to override getRuntimeTargetDelegate to support the modern JSI inspector.
class OJSIExecutor : public JSIExecutor {
 public:
  OJSIExecutor(
      std::shared_ptr<jsi::Runtime> runtime,
      std::shared_ptr<ExecutorDelegate> delegate,
      const JSIScopedTimeoutInvoker &timeoutInvoker,
      RuntimeInstaller runtimeInstaller,
      std::shared_ptr<facebook::react::jsinspector_modern::RuntimeTargetDelegate> &&targetDelegate) noexcept
      : JSIExecutor(std::move(runtime), std::move(delegate), timeoutInvoker, std::move(runtimeInstaller)),
        targetDelegate_(std::move(targetDelegate)) {}
  jsinspector_modern::RuntimeTargetDelegate &getRuntimeTargetDelegate() override {
    if (!targetDelegate_) {
      // Use the fallback implementation from JSIExecutor.
      return JSIExecutor::getRuntimeTargetDelegate();
    }
    return *targetDelegate_;
  }

 private:
  std::shared_ptr<facebook::react::jsinspector_modern::RuntimeTargetDelegate> targetDelegate_;
};

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

    return std::make_unique<OJSIExecutor>(
        runtimeHolder_->getRuntime(),
        std::move(delegate),
        JSIExecutor::defaultTimeoutInvoker,
        [isProfiling = isProfilingEnabled_]([[maybe_unused]] jsi::Runtime &runtime) {
#ifdef ENABLE_JS_SYSTRACE_TO_ETW
          facebook::react::tracing::initializeJSHooks(runtime, isProfiling);
#endif
        },
        runtimeHolder_->createRuntimeTargetDelegate());
  }

  OJSIExecutorFactory(
      std::shared_ptr<Microsoft::JSI::RuntimeHolderLazyInit> runtimeHolder,
      NativeLoggingHook loggingHook,
      bool isProfilingEnabled) noexcept
      : runtimeHolder_{std::move(runtimeHolder)},
        loggingHook_{std::move(loggingHook)},
        isProfilingEnabled_{isProfilingEnabled} {}

 private:
  std::shared_ptr<Microsoft::JSI::RuntimeHolderLazyInit> runtimeHolder_;
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
    const winrt::Microsoft::ReactNative::IReactPropertyBag &propertyBag,
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
      propertyBag,
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
      nullptr, // longLivedObjectCollection
      nullptr, // PropertyBag
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
    std::shared_ptr<facebook::react::LongLivedObjectCollection> longLivedObjectCollection,
    const winrt::Microsoft::ReactNative::IReactPropertyBag &propertyBag,
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
  facebook::react::ReactMarker::logTaggedMarkerImpl = logMarker;

#ifdef ENABLE_ETW_TRACING
  // TODO :: Find a better place to initialize ETW once per process.
  facebook::react::tracing::initializeETW();
#endif

  if (m_devSettings->useDirectDebugger) {
    m_devManager->EnsureInspectorPackagerConnection(
        m_devSettings->sourceBundleHost, m_devSettings->sourceBundlePort, m_devSettings->bundleAppId);
  }

  std::vector<std::unique_ptr<NativeModule>> modules;

  // Add app provided modules.
  for (auto &cxxModule : cxxModules) {
    modules.push_back(std::make_unique<CxxNativeModule>(
        m_innerInstance, move(std::get<0>(cxxModule)), move(std::get<1>(cxxModule)), move(std::get<2>(cxxModule))));
  }
  m_moduleRegistry = std::make_shared<facebook::react::ModuleRegistry>(std::move(modules));

  // Choose JSExecutor
  std::shared_ptr<JSExecutorFactory> jsef;
  if (m_devSettings->useFastRefresh || m_devSettings->liveReloadCallback) {
    Microsoft::ReactNative::PackagerConnection::CreateOrReusePackagerConnection(*m_devSettings);
  }

  // If the consumer gives us a JSI runtime, then  use it.
  if (m_devSettings->jsiRuntimeHolder) {
    assert(m_devSettings->jsiEngineOverride == JSIEngineOverride::Default);
    jsef = std::make_shared<OJSIExecutorFactory>(
        m_devSettings->jsiRuntimeHolder, m_devSettings->loggingCallback, !m_devSettings->useFastRefresh);
  } else if (m_devSettings->jsExecutorFactoryDelegate != nullptr) {
    jsef = m_devSettings->jsExecutorFactoryDelegate(m_innerInstance->getJSCallInvoker());
  } else {
    assert(m_devSettings->jsiEngineOverride != JSIEngineOverride::Default);
    switch (m_devSettings->jsiEngineOverride) {
      case JSIEngineOverride::Hermes: {
        std::shared_ptr<facebook::jsi::PreparedScriptStore> preparedScriptStore;

        wchar_t tempPath[MAX_PATH];
        if (GetTempPathW(MAX_PATH, tempPath)) {
          preparedScriptStore =
              std::make_shared<facebook::react::BasePreparedScriptStoreImpl>(winrt::to_string(tempPath));
        }

        m_devSettings->jsiRuntimeHolder = std::make_shared<Microsoft::ReactNative::HermesRuntimeHolder>(
            m_devSettings, m_jsThread, std::move(preparedScriptStore));
        break;
      }
      case JSIEngineOverride::V8: {
#if defined(USE_V8)
        std::shared_ptr<facebook::jsi::PreparedScriptStore> preparedScriptStore;

        wchar_t tempPath[MAX_PATH];
        if (GetTempPathW(MAX_PATH, tempPath)) {
          preparedScriptStore =
              std::make_shared<facebook::react::BasePreparedScriptStoreImpl>(winrt::to_string(tempPath));
        }

        m_devSettings->jsiRuntimeHolder = std::make_shared<facebook::react::V8JSIRuntimeHolder>(
            m_devSettings, m_jsThread, nullptr, std::move(preparedScriptStore), /*multithreading*/ false);
        break;
#else
        assert(false); // V8 is not available in this build, fallthrough
        [[fallthrough]];
#endif
      }
      case JSIEngineOverride::V8NodeApi: {
#if defined(USE_V8)
        std::shared_ptr<facebook::jsi::PreparedScriptStore> preparedScriptStore;

        wchar_t tempPath[MAX_PATH];
        if (GetTempPathW(MAX_PATH, tempPath)) {
          preparedScriptStore =
              std::make_shared<facebook::react::BasePreparedScriptStoreImpl>(winrt::to_string(tempPath));
        }

        m_devSettings->jsiRuntimeHolder = make_shared<Microsoft::ReactNative::V8RuntimeHolder>(
            m_devSettings, m_jsThread, std::move(preparedScriptStore), false);
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
        m_devSettings->jsiRuntimeHolder, m_devSettings->loggingCallback, !m_devSettings->useFastRefresh);
  }

  m_innerInstance->initializeBridge(
      std::move(callback), jsef, m_jsThread, m_moduleRegistry, m_devSettings->inspectorHostTarget);

  // For RuntimeScheduler to work properly, we need to install TurboModuleManager with RuntimeSchedulerCallbackInvoker.
  // To be able to do that, we need to be able to call m_innerInstance->getRuntimeExecutor(), which we can only do after
  // m_innerInstance->initializeBridge(...) is called.
  const auto runtimeExecutor = m_innerInstance->getRuntimeExecutor();
  Microsoft::ReactNative::SchedulerSettings::SetRuntimeExecutor(
      winrt::Microsoft::ReactNative::ReactPropertyBag(propertyBag), runtimeExecutor);
  if (m_devSettings->useRuntimeScheduler) {
    m_runtimeScheduler = std::make_shared<RuntimeScheduler>(runtimeExecutor);
    Microsoft::ReactNative::SchedulerSettings::SetRuntimeScheduler(
        winrt::Microsoft::ReactNative::ReactPropertyBag(propertyBag), m_runtimeScheduler);
  }

  // Using runOnQueueSync because initializeBridge calls createJSExecutor with runOnQueueSync,
  // so this is an attempt to keep the same semantics for exiting this method with TurboModuleManager
  // initialized.
  m_jsThread->runOnQueueSync([propertyBag,
                              innerInstance = m_innerInstance,
                              runtimeHolder = m_devSettings->jsiRuntimeHolder,
                              runtimeScheduler = m_runtimeScheduler,
                              turboModuleRegistry = m_turboModuleRegistry,
                              longLivedObjectCollection = m_longLivedObjectCollection]() {
    if (runtimeScheduler) {
      RuntimeSchedulerBinding::createAndInstallIfNeeded(*runtimeHolder->getRuntime(), runtimeScheduler);
    }
    auto turboModuleManager = std::make_shared<TurboModuleManager>(
        turboModuleRegistry,
        runtimeScheduler ? std::make_shared<RuntimeSchedulerCallInvoker>(runtimeScheduler)
                         : innerInstance->getJSCallInvoker());

    // TODO: The binding here should also add the proxys that convert cxxmodules into turbomodules
    // [@vmoroz] Note, that we must not use the RN TurboCxxModule.h code because it uses global
    // LongLivedObjectCollection instance that prevents us from using multiple RN instance in the same process.
    auto binding = [turboModuleManager](const std::string &name) -> std::shared_ptr<TurboModule> {
      return turboModuleManager->getModule(name);
    };

    TurboModuleBinding::install(
        *runtimeHolder->getRuntime(), std::function(binding), nullptr, longLivedObjectCollection);

    // init TurboModule
    for (const auto &moduleName : turboModuleManager->getEagerInitModuleNames()) {
      turboModuleManager->getModule(moduleName);
    }
  });

  // All JSI runtimes do support host objects and hence the native modules
  // proxy.
  const bool isNativeModulesProxyAvailable =
      ((m_devSettings->jsiRuntimeHolder != nullptr) ||
       (m_devSettings->jsiEngineOverride != JSIEngineOverride::Default));
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
    if (m_devSettings->liveReloadCallback != nullptr || m_devSettings->useFastRefresh) {
      Microsoft::ReactNative::LoadRemoteUrlScript(
          m_devSettings,
          m_devManager,
          std::move(jsBundleRelativePath),
          [=](std::unique_ptr<const JSBigStdString> script, const std::string &sourceURL) {
            m_innerInstance->loadScriptFromString(std::move(script), sourceURL, false);
          });

    } else {
      auto bundleString = Microsoft::ReactNative::JsBigStringFromPath(m_devSettings, jsBundleRelativePath);
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
  if (m_devSettings->inspectorHostTarget) {
    Mso::React::MessageDispatchQueue messageDispatchQueue{
        ::Microsoft::ReactNative::ReactInspectorThread::Instance(), nullptr};
    messageDispatchQueue.runOnQueueSync([weakInnerInstance = std::weak_ptr(m_innerInstance)]() {
      if (std::shared_ptr<facebook::react::Instance> innerInstance = weakInnerInstance.lock()) {
        innerInstance->unregisterFromInspector();
      }
    });
  }

  m_nativeQueue->quitSynchronous();
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

} // namespace facebook::react
