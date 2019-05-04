// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"

#include <OInstance.h>
#include <cxxreact/CxxModule.h>
#include <cxxreact/CxxNativeModule.h>
#include <cxxreact/Instance.h>
#include <cxxreact/JSBigString.h>
#include <cxxreact/JSExecutor.h>
#include <cxxreact/Platform.h>
#include "UnicodeConversion.h"

#include "../Chakra/ChakraExecutor.h"
#include "../Chakra/ChakraUtils.h"

#if (defined(_MSC_VER) && !defined(WINRT))
#include "Sandbox/SandboxJSExecutor.h"
#endif

#if !defined(NOJSC)
#include <cxxreact/JSCExecutor.h>
#endif

#include <cxxreact/ModuleRegistry.h>
#include <cxxreact/MessageQueueThread.h>

#if (defined(_MSC_VER) && !defined(WINRT))
#include <WebSocketModule.h>
#endif
#include <SourceCodeModule.h>
#include <Modules/ExceptionsManagerModule.h>

#if (defined(_MSC_VER) && (defined(WINRT)))
#include <Utils/LocalBundleReader.h>
#endif

#include <DevSettings.h>
#include <IDevSupportManager.h>
#include <IReactRootView.h>
#include <IUIManager.h>
#include <WebSocketJSExecutorFactory.h>
#include <CreateModules.h>
#include <Shlwapi.h>

#include <cxxreact/JSExecutor.h>
#include <jsi/jsi.h>
#include <jsiexecutor/jsireact/JSIExecutor.h>

#include "../Chakra/ChakraRuntime.h"

#if (defined(_MSC_VER) && !defined(WINRT))
#include <jsi/V8Runtime.h>
#endif

namespace {

#if (defined(_MSC_VER) && !defined(WINRT))

std::string GetJSBundleDirectory(const std::string& jsBundleBasePath, const std::string& jsBundleRelativePath) noexcept
{
  // If there is a base path, use that to calculate the absolute path.
  if (jsBundleBasePath.length() > 0)
  {
    std::string jsBundleDirectory = jsBundleBasePath;
    if (jsBundleDirectory.back() != '\\')
      jsBundleDirectory += '\\';

    return jsBundleDirectory += jsBundleRelativePath;
  }
  // Otherwise use the path of the executable file to construct the absolute path.
  else
  {
    wchar_t modulePath[MAX_PATH];

    auto len = GetModuleFileNameW(nullptr, modulePath, _countof(modulePath));

    if (len == 0 || (len == _countof(modulePath) && GetLastError() == ERROR_INSUFFICIENT_BUFFER))
      return jsBundleRelativePath;

    // remove the trailing filename as we are interested in only the path
    auto succeeded = PathRemoveFileSpecW(modulePath);
    if (!succeeded)
      return jsBundleRelativePath;

    std::string jsBundlePath = facebook::react::UnicodeConversion::Utf16ToUtf8(modulePath, wcslen(modulePath));
    if (!jsBundlePath.empty() && jsBundlePath.back() != '\\')
      jsBundlePath += '\\';

    return jsBundlePath += jsBundleRelativePath;
  }
}

std::string GetJSBundleFilePath(const std::string& jsBundleBasePath, const std::string& jsBundleRelativePath)
{
  auto jsBundleFilePath = GetJSBundleDirectory(jsBundleBasePath, jsBundleRelativePath);

  // Usually, the module name: "module name" + "." + "platform name", for example "lpc.win32".
  // If we can not find the the bundle.js file under the normal folder,
  // we are trying to find it under the folder without the dot (e.g. "lpcwin32").
  // VSO:1997035 remove this code after we have better name convension
  if (PathFileExistsA((jsBundleFilePath + "\\bundle.js").c_str()))
  {
    jsBundleFilePath += "\\bundle.js";
  }
  else
  {
    // remove the dot only if is belongs to the bundle file name.
    size_t lastDotPosition = jsBundleFilePath.find_last_of('.');
    size_t bundleFilePosition = jsBundleFilePath.find_last_of('\\');
    if (lastDotPosition != std::string::npos && (bundleFilePosition == std::string::npos || lastDotPosition > bundleFilePosition))
    {
      jsBundleFilePath.erase(lastDotPosition, 1);
    }

    jsBundleFilePath += "\\bundle.js";

    // Back before we have base path plumbed through, we made win32 force a seperate folder for each bundle by appending bundle.js
    // Now that we have base path, we can handle multiple SDXs with the same index name so we should switch to using the same scheme
    // as all the other platforms (and the bundle server)
    // TODO: We should remove all the previous logic and use the same names as the other platforms...
    if (!PathFileExistsA(jsBundleFilePath.c_str()))
    {
      jsBundleFilePath = GetJSBundleDirectory(jsBundleBasePath, jsBundleRelativePath) + ".bundle";
    }
  }

  return jsBundleFilePath;
}

bool GetLastWriteTime(const std::string& fileName, uint64_t& result) noexcept
{
  std::wstring fileNameUtf16 = facebook::react::UnicodeConversion::Utf8ToUtf16(fileName);

  std::unique_ptr<void, decltype(&CloseHandle)> handle { CreateFileW(
    static_cast<LPCWSTR>(fileNameUtf16.c_str()), GENERIC_READ, FILE_SHARE_READ,
    nullptr /* lpSecurityAttributes */, OPEN_EXISTING,
    FILE_ATTRIBUTE_NORMAL, nullptr /* hTemplateFile */), &CloseHandle };

  if (handle.get() == INVALID_HANDLE_VALUE)
  {
    return false;
  }

  FILETIME lastWriteTime;
  if (!GetFileTime(handle.get(), nullptr, nullptr, &lastWriteTime))
  {
    return false;
  }

  result = static_cast<uint64_t>(lastWriteTime.dwHighDateTime) << 32 | static_cast<uint64_t>(lastWriteTime.dwLowDateTime);
  return true;
}
#endif

}

namespace facebook { namespace react {

#if (defined(_MSC_VER) && !defined(WINRT)  && !defined(_M_ARM))
class ChakraJSIExecutorFactory : public JSExecutorFactory {
public:
  std::unique_ptr<JSExecutor> createJSExecutor(
    std::shared_ptr<ExecutorDelegate> delegate,
    std::shared_ptr<MessageQueueThread> jsQueue) override {

    auto logger = [](const std::string& message, unsigned int logLevel) {};

    return std::make_unique<JSIExecutor>(facebook::react::chakra::makeChakraRuntime(std::move(args_), std::move(jsQueue), logger),
      std::move(delegate),
      logger,
      JSIExecutor::defaultTimeoutInvoker,
      nullptr);
  }

  ChakraJSIExecutorFactory(facebook::react::ChakraInstanceArgs&& args)
    : args_(std::move(args)) {}
private:
  facebook::react::ChakraInstanceArgs args_;
};

#if defined(USE_V8)
class V8JSIExecutorFactory : public JSExecutorFactory {
public:
  std::unique_ptr<JSExecutor> createJSExecutor(
    std::shared_ptr<ExecutorDelegate> delegate,
    std::shared_ptr<MessageQueueThread> jsQueue) override {

    auto cacheProvider = std::make_shared<facebook::v8runtime::CacheProvider>([](const std::string& sourceUrl) {return nullptr; });
    auto logger = std::make_shared<facebook::v8runtime::Logger>([](const std::string& message, unsigned int logLevel) { });

    return std::make_unique<JSIExecutor>(
      facebook::v8runtime::makeV8Runtime(nullptr, std::move(logger), std::move(jsQueue), std::move(cacheProvider), nullptr, nullptr, nullptr, nullptr),
      delegate,
      *logger.get(),
      JSIExecutor::defaultTimeoutInvoker,
      nullptr);
  }
};
#endif // USE_V8

#else

class ChakraJSIExecutorFactory : public JSExecutorFactory {
public:
  std::unique_ptr<JSExecutor> createJSExecutor(
    std::shared_ptr<ExecutorDelegate> delegate,
    std::shared_ptr<MessageQueueThread> jsQueue) override {
      std::abort();
  }

  ChakraJSIExecutorFactory(facebook::react::ChakraInstanceArgs&& args)
    : args_(std::move(args)) { std::abort(); }
private:
  facebook::react::ChakraInstanceArgs args_;
};

#if defined(USE_V8)
class V8JSIExecutorFactory : public JSExecutorFactory {
public:
  std::unique_ptr<JSExecutor> createJSExecutor(
    std::shared_ptr<ExecutorDelegate> delegate,
    std::shared_ptr<MessageQueueThread> jsQueue) override {
      std::abort();
  }

  V8JSIExecutorFactory() { std::abort(); }
};
#endif // USE_V8


#endif

void logMarker(const facebook::react::ReactMarker::ReactMarkerId /*id*/, const char* /*tag*/) {
}

struct BridgeUIBatchInstanceCallback : public InstanceCallback {
  BridgeUIBatchInstanceCallback(std::weak_ptr<Instance> instance, std::weak_ptr<IUIManager> uimanager, std::weak_ptr<MessageQueueThread> uithread)
    : m_instance(instance), m_weakUiManager(std::move(uimanager)), m_uiThread(std::move(uithread)) {}
  virtual ~BridgeUIBatchInstanceCallback() = default;
  void onBatchComplete() override {
    if (auto uithread = m_uiThread.lock()) {
      std::weak_ptr<IUIManager> weakUiManager(m_weakUiManager);
      uithread->runOnQueue([weakUiManager]() {
        auto uiManager = weakUiManager.lock();
        if (uiManager != nullptr)
          uiManager->onBatchComplete();
      });
    }
  }
  void incrementPendingJSCalls() override {}
  void decrementPendingJSCalls() override {}

  std::weak_ptr<Instance> m_instance;
  std::weak_ptr<IUIManager> m_weakUiManager;
  std::weak_ptr<MessageQueueThread> m_uiThread;
};

struct BridgeTestInstanceCallback : public InstanceCallback {
  BridgeTestInstanceCallback() {}
  virtual ~BridgeTestInstanceCallback() = default;
  void onBatchComplete() override {}
  void incrementPendingJSCalls() override {}
  void decrementPendingJSCalls() override {}
};

/*static*/ std::shared_ptr<InstanceImpl> InstanceImpl::MakeNoBundle(std::string&& jsBundleBasePath,
      std::vector<std::tuple<std::string, facebook::xplat::module::CxxModule::Provider, std::shared_ptr<MessageQueueThread>>>&& cxxModules,
      std::shared_ptr<IUIManager> uimanager,
      std::shared_ptr<MessageQueueThread> jsQueue,
      std::shared_ptr<MessageQueueThread> nativeQueue,
      std::shared_ptr<DevSettings> devSettings,
      std::shared_ptr<IDevSupportManager> devManager) noexcept {
    auto instance = std::shared_ptr<InstanceImpl>(
      new InstanceImpl(
        std::move(jsBundleBasePath),
        std::move(cxxModules),
        std::move(uimanager),
        std::move(jsQueue),
        std::move(nativeQueue),
        std::move(devSettings),
        std::move(devManager)));

  instance->RegisterForReloadIfNecessary();

  return instance;
}

/*static*/ std::shared_ptr<InstanceImpl> InstanceImpl::MakeAndLoadBundle(std::string&& jsBundleBasePath,
    std::string&& jsBundleRelativePath,
    std::vector<std::tuple<std::string, facebook::xplat::module::CxxModule::Provider, std::shared_ptr<MessageQueueThread>>>&& cxxModules,
    std::shared_ptr<IUIManager> uimanager,
    std::shared_ptr<MessageQueueThread> jsQueue,
    std::shared_ptr<MessageQueueThread> nativeQueue,
    std::shared_ptr<DevSettings> devSettings,
    std::shared_ptr<IDevSupportManager> devManager) noexcept {
  auto instance = std::shared_ptr<InstanceImpl>(
    new InstanceImpl(
      std::move(jsBundleBasePath),
      std::move(cxxModules),
      std::move(uimanager),
      std::move(jsQueue),
      std::move(nativeQueue),
      std::move(devSettings),
      std::move(devManager)));

  instance->loadBundle(std::move(jsBundleRelativePath));
  instance->RegisterForReloadIfNecessary();

  return instance;
}

#if (defined(_MSC_VER) && !defined(WINRT))
#if !defined(OSS_RN)

/*static*/ std::shared_ptr<InstanceImpl> InstanceImpl::MakeSandbox(std::string&& jsString,
    std::string&& configsString,
    std::string&& sourceUrl,
    std::shared_ptr<MessageQueueThread> jsQueue,
    std::shared_ptr<MessageQueueThread> nativeQueue,
    std::shared_ptr<DevSettings> devSettings,
    std::shared_ptr<IDevSupportManager> devManager,
    SendNativeModuleCall&& sendNativeModuleCall) noexcept {
  auto instance = std::shared_ptr<InstanceImpl>(
    new InstanceImpl(
      std::move(jsString),
      std::move(configsString),
      std::move(sourceUrl),
      std::move(jsQueue),
      std::move(nativeQueue),
      std::move(devSettings),
      std::move(devManager),
      std::move(sendNativeModuleCall)));

  instance->RegisterForReloadIfNecessary();

  return instance;
}

/*static*/ std::shared_ptr<InstanceImpl> InstanceImpl::MakeSandbox(std::string&& jsBundleBasePath,
    std::string&& jsBundleRelativePath,
    std::string&& configsString,
    std::string&& sourceUrl,
    std::shared_ptr<MessageQueueThread> jsQueue,
    std::shared_ptr<MessageQueueThread> nativeQueue,
    std::shared_ptr<DevSettings> devSettings,
    std::shared_ptr<IDevSupportManager> devManager,
    SendNativeModuleCall&& sendNativeModuleCall) noexcept {
  auto instance = std::shared_ptr<InstanceImpl>(
    new InstanceImpl(
      std::move(jsBundleBasePath),
      std::move(jsBundleRelativePath),
      std::move(configsString),
      std::move(sourceUrl),
      std::move(jsQueue),
      std::move(nativeQueue),
      std::move(devSettings),
      std::move(devManager),
      std::move(sendNativeModuleCall)));

  instance->RegisterForReloadIfNecessary();

  return instance;
}
#endif
#endif

InstanceImpl::InstanceImpl(std::string&& jsBundleBasePath,
    std::vector<std::tuple<std::string, facebook::xplat::module::CxxModule::Provider, std::shared_ptr<MessageQueueThread>>>&& cxxModules,
    std::shared_ptr<IUIManager> uimanager,
    std::shared_ptr<MessageQueueThread> jsQueue,
    std::shared_ptr<MessageQueueThread> nativeQueue,
    std::shared_ptr<DevSettings> devSettings,
    std::shared_ptr<IDevSupportManager> devManager)
    : m_uimanager(std::move(uimanager))
    , m_jsThread(std::move(jsQueue))
    , m_nativeQueue(nativeQueue)
    , m_jsBundleBasePath(std::move(jsBundleBasePath))
    , m_devSettings(std::move(devSettings))
    , m_devManager(std::move(devManager))
    , m_innerInstance(std::make_shared<Instance>()) {

  // Temp set the logmarker here
  facebook::react::ReactMarker::logTaggedMarker = logMarker;

  // Default (common) NativeModules
  auto modules = GetDefaultNativeModules(nativeQueue);

  // Add app provided modules.
  for (auto& cxxModule : cxxModules) {
    modules.push_back(std::make_unique<CxxNativeModule>(m_innerInstance, move(std::get<0>(cxxModule)), move(std::get<1>(cxxModule)), move(std::get<2>(cxxModule))));
  }
  m_moduleRegistry = std::make_shared<facebook::react::ModuleRegistry>(std::move(modules));

  // choose ExecutorDelegate and JSExecutor
  std::shared_ptr<JSExecutorFactory> jsef;
#if !defined(OSS_RN)
  std::shared_ptr<ExecutorDelegateFactory> edf;
  if (m_devSettings->useSandbox) {
#if (defined(_MSC_VER) && !defined(WINRT))
    edf = std::make_shared<SandboxExecutorDelegateFactory>();
    auto jseFunc = m_devManager->LoadJavaScriptInSandboxMode(*m_devSettings);
    jsef = std::make_shared<SandboxJSExecutorFactory>(std::move(jseFunc));
#endif
  }
  else
#endif
  /* else */ if (m_devSettings->useWebDebugger) {
    try
    {
      auto jseFunc = m_devManager->LoadJavaScriptInProxyMode(*m_devSettings);

      if ((jseFunc == nullptr) || m_devManager->HasException())
      {
        m_devSettings->errorCallback("Failed to create JavaScript Executor.");
        return;
      }

      jsef = std::make_shared<WebSocketJSExecutorFactory>(std::move(jseFunc));
    }
    catch (std::exception& e)
    {
      m_devSettings->errorCallback(e.what());
      return;
    }
  }
  else {
    ChakraInstanceArgs instanceArgs;

    instanceArgs.DebuggerBreakOnNextLine = m_devSettings->debuggerBreakOnNextLine;
    instanceArgs.DebuggerPort = m_devSettings->debuggerPort;
    instanceArgs.DebuggerRuntimeName = m_devSettings->debuggerRuntimeName;

    instanceArgs.EnableDebugging = m_devSettings->useDirectDebugger;
    instanceArgs.LoggingCallback = m_devSettings->loggingCallback;

    instanceArgs.EnableNativePerformanceNow = m_devSettings->enableNativePerformanceNow;

    if (!m_devSettings->useJITCompilation)
    {
#if (defined(_MSC_VER) && !defined(WINRT))
      instanceArgs.RuntimeAttributes = static_cast<JsRuntimeAttributes>(instanceArgs.RuntimeAttributes | JsRuntimeAttributeDisableNativeCodeGeneration | JsRuntimeAttributeDisableExecutablePageAllocation);
#else
      instanceArgs.RuntimeAttributes = static_cast<JsRuntimeAttributes>(instanceArgs.RuntimeAttributes | JsRuntimeAttributeDisableNativeCodeGeneration);
#endif
    }

    instanceArgs.MemoryTracker = m_devSettings->memoryTracker ? m_devSettings->memoryTracker : CreateMemoryTracker(std::shared_ptr<MessageQueueThread>{m_nativeQueue});

    switch (m_devSettings->jsiMode) {
    case JSIMode::None:
      // TODO: What should we use here for the first and second param, cacheDir and jscConfig respectively
      jsef = std::make_shared<ChakraExecutorFactory>(std::move(instanceArgs));
      break;
    case JSIMode::ChakraJSI:
      jsef = std::make_shared<ChakraJSIExecutorFactory>(std::move(instanceArgs));
      break;
#if defined(USE_V8)
    case JSIMode::V8JSI:
      jsef = std::make_shared<V8JSIExecutorFactory>();
      break;
#endif // USE_V8
    }

  }

  m_innerInstance->initializeBridge(std::make_unique<BridgeUIBatchInstanceCallback>(m_innerInstance, m_uimanager, m_nativeQueue),
#if !defined(OSS_RN)
    edf,
#endif
    jsef,
    m_jsThread,
    m_moduleRegistry);

  folly::dynamic configArray = folly::dynamic::array;
  for (auto const& moduleName : m_moduleRegistry->moduleNames()) {
    auto moduleConfig = m_moduleRegistry->getConfig(moduleName);
    if (moduleConfig) {
      configArray.push_back(std::move(moduleConfig->config));
    }
  }
  folly::dynamic configs = folly::dynamic::object("remoteModuleConfig", configArray);
  m_innerInstance->setGlobalVariable("__fbBatchedBridgeConfig", std::make_unique<JSBigStdString>(folly::toJson(configs)));
}

void InstanceImpl::loadBundle(std::string&& jsBundleRelativePath)
{
  loadBundleInternal(std::move(jsBundleRelativePath), /*synchronously:*/false);
}

void InstanceImpl::loadBundleSync(std::string&& jsBundleRelativePath)
{
  loadBundleInternal(std::move(jsBundleRelativePath), /*synchronously:*/true);
}

void InstanceImpl::loadBundleInternal(std::string&& jsBundleRelativePath, bool synchronously)
{
  std::string bytecodeFileNameCopy { m_devSettings->bytecodeFileName };

  // load JS
  if (m_devSettings->useWebDebugger)
  {
    try
    {
      auto bundleUrl = DevServerHelper::get_BundleUrl(
        m_devSettings->debugHost,
        m_devSettings->debugBundlePath.empty() ? jsBundleRelativePath : m_devSettings->debugBundlePath,
        m_devSettings->platformName,
        /*dev*/ "true",
        /*hot*/ "false");

      m_innerInstance->loadScriptFromString(
        std::make_unique<const JSBigStdString>(bundleUrl),
#if !defined(OSS_RN)
        0 /*bundleVersion*/,
#endif
        bundleUrl,
        synchronously
#if !defined(OSS_RN)
        , std::move(bytecodeFileNameCopy)
#endif
      );
    }
    catch (std::exception& e)
    {
      m_devSettings->errorCallback(e.what());
      return;
    }
  }
  else if (m_devSettings->liveReloadCallback != nullptr)
  {
    auto jsBundleString = m_devManager->GetJavaScriptFromServer(
      m_devSettings->debugHost,
      jsBundleRelativePath,
      m_devSettings->platformName);

    if (m_devManager->HasException())
    {
      m_devSettings->errorCallback(jsBundleString);
    }
    else
    {
      m_innerInstance->loadScriptFromString(
        std::make_unique<const JSBigStdString>(jsBundleString),
#if !defined(OSS_RN)
        0 /*bundleVersion*/,
#endif
        jsBundleRelativePath,
        synchronously
#if !defined(OSS_RN)
        , "" /*bytecodeFileName*/ // No bytecode is used during Live Reload
#endif
      );
    }
  }
  else {
    try
    {
#if (defined(_MSC_VER) && !defined(WINRT))
      auto fullBundleFilePath = GetJSBundleFilePath(m_jsBundleBasePath, jsBundleRelativePath);

      // If fullBundleFilePath exists, load User bundle.
      // Otherwise all bundles (User and Platform) are loaded through platformBundles.
      if (PathFileExistsA(fullBundleFilePath.c_str()))
      {
#if defined(_CHAKRACORE_H_)
        auto bundleString = FileMappingBigString::fromPath(fullBundleFilePath);
#else
        auto bundleString = JSBigFileString::fromPath(fullBundleFilePath);
#endif
        uint64_t bundleTimestamp = 0;
        if (GetLastWriteTime(fullBundleFilePath, bundleTimestamp)) {
          m_innerInstance->loadScriptFromString(std::move(bundleString),
#if !defined(OSS_RN)
            bundleTimestamp,
#endif
            std::move(fullBundleFilePath),
            synchronously
#if !defined(OSS_RN)
            , std::move(bytecodeFileNameCopy)
#endif
          );
        }
        else {
          // Opt out of bytecode if we cannot get bundle timestamp.
          m_innerInstance->loadScriptFromString(std::move(bundleString),
#if !defined(OSS_RN)
            /*jsBundleVersion*/ 0,
#endif
            std::move(fullBundleFilePath),
            synchronously
#if !defined(OSS_RN)
            , "" /*bytecodeFileName*/
#endif
          );
        }
      }

#else
    auto bundleString = std::make_unique<::react::uwp::StorageFileBigString>("ms-appx:///Bundle/" + jsBundleRelativePath + ".bundle");
    m_innerInstance->loadScriptFromString(std::move(bundleString),
#if !defined(OSS_RN)
      0 /*bundleVersion*/,
#endif
      jsBundleRelativePath,
      synchronously
#if !defined(OSS_RN)
      , "" /*bytecodeFileName*/
#endif
    );
#endif

    }
    catch (std::exception& e) {
      m_devSettings->errorCallback(e.what());
    }
  }
}

#if (defined(_MSC_VER) && !defined(WINRT))
#if !defined(OSS_RN)

// For sandbox process.
InstanceImpl::InstanceImpl(std::string&& jsString,
    std::string&& configsString,
    std::string&& sourceUrl,
    std::shared_ptr<MessageQueueThread> jsQueue,
    std::shared_ptr<MessageQueueThread> nativeQueue,
    std::shared_ptr<DevSettings> devSettings,
    std::shared_ptr<IDevSupportManager> devManager,
    SendNativeModuleCall&& sendNativeModuleCall)
    : InstanceImpl(
        std::string(),
        std::move(jsString),
        std::move(configsString),
        std::move(sourceUrl),
        std::move(jsQueue),
        nativeQueue,
        std::move(devSettings),
        std::move(devManager),
        std::move(sendNativeModuleCall)) {}

// For sandbox process.
InstanceImpl::InstanceImpl(std::string&& jsBundleBasePath,
    std::string&& jsBundleRelativePath,
    std::string&& configsString,
    std::string&& sourceUrl,
    std::shared_ptr<MessageQueueThread> jsQueue,
    std::shared_ptr<MessageQueueThread> nativeQueue,
    std::shared_ptr<DevSettings> devSettings,
    std::shared_ptr<IDevSupportManager> devManager,
    SendNativeModuleCall&& sendNativeModuleCall)
    : m_jsThread(std::move(jsQueue))
    , m_nativeQueue(nativeQueue)
    , m_devSettings(std::move(devSettings))
    , m_devManager(std::move(devManager))
    , m_innerInstance(std::make_shared<Instance>()) {
    auto modules = GetDefaultNativeModules(nativeQueue);
  m_moduleRegistry = std::make_shared<facebook::react::ModuleRegistry>(std::move(modules));

  // Load ChakraExecutor for sandbox process
  auto edf = std::make_shared<SandboxDelegateFactory>(std::move(sendNativeModuleCall));

  ChakraInstanceArgs instanceArgs;
  instanceArgs.RuntimeAttributes = m_devSettings->useJITCompilation ? JsRuntimeAttributeNone : JsRuntimeAttributeDisableNativeCodeGeneration;

  auto jsef = std::make_shared<ChakraExecutorFactory>(std::move(instanceArgs));
  m_innerInstance->initializeBridge(std::make_unique<BridgeTestInstanceCallback>(),
    edf,
    jsef,
    m_jsThread,
    m_moduleRegistry);

  std::string bytecodeFileNameCopy = m_devSettings->bytecodeFileName;
  m_innerInstance->setGlobalVariable("__fbBatchedBridgeConfig", std::make_unique<JSBigStdString>(std::move(configsString)));

  std::string fullBundleFilePath = GetJSBundleDirectory(m_jsBundleBasePath, jsBundleRelativePath);

  try {
    m_innerInstance->loadScriptFromString(
      std::make_unique<const JSBigStdString>(std::move(fullBundleFilePath)),
      0 /*bundleVersion*/,
      sourceUrl,
      false /*synchronously*/,
      std::move(bytecodeFileNameCopy));
  }
  catch (std::exception& e) {
    m_devSettings->errorCallback(e.what());
  }
}

#endif
#endif

InstanceImpl::~InstanceImpl() {
  m_nativeQueue->quitSynchronous();
}


void InstanceImpl::AttachMeasuredRootView(IReactRootView* rootView, folly::dynamic&& initProps) noexcept
{
  if (m_devManager->HasException())
  {
    return;
  }

  rootView->ResetView();

  auto rootTag = m_uimanager->AddMeasuredRootView(rootView);
  rootView->SetTag(rootTag);

  std::string jsMainModuleName = rootView->JSComponentName();
  folly::dynamic params = folly::dynamic::array(
    std::move(jsMainModuleName),
    folly::dynamic::object("initialProps", std::move(initProps))
    ("rootTag", rootTag));
  m_innerInstance->callJSFunction("AppRegistry", "runApplication", std::move(params));
}

void InstanceImpl::DetachRootView(IReactRootView* rootView) noexcept
{
  if (m_devManager->HasException())
  {
    return;
  }

  auto rootTag = rootView->GetTag();
  folly::dynamic params = folly::dynamic::array(rootTag);
  m_innerInstance->callJSFunction("AppRegistry", "unmountApplicationComponentAtRootTag", std::move(params));

  // Give the JS thread time to finish executing
  m_jsThread->runOnQueueSync([]() { });
}

// TODO: Task #1895397: Host should pass information about to where each native module loaded. (host or sandbox)
std::vector<std::unique_ptr<NativeModule>> InstanceImpl::GetDefaultNativeModules(std::shared_ptr<MessageQueueThread> nativeQueue) {
  std::vector<std::unique_ptr<NativeModule>> modules;
// TODO: This is not included for UWP due to difficulties getting it working. For now, we still use the old module
//  written specifically for UWP. That one gets added later.
#if (defined(_MSC_VER) && !defined(WINRT))
  modules.push_back(std::make_unique<CxxNativeModule>(m_innerInstance, "WebSocketModule", []() -> std::unique_ptr<xplat::module::CxxModule>
  {
    return std::make_unique<WebSocketModule>();
  }, nativeQueue));
#endif
// TODO: This is not included for UWP because we have a different module which is added later. However, this one is designed
//  so that we can base a UWP version on it. We need to do that but is not high priority.
#if (defined(_MSC_VER) && !defined(WINRT))
  modules.push_back(std::make_unique<CxxNativeModule>(m_innerInstance, "Timing", [nativeQueue]() -> std::unique_ptr<xplat::module::CxxModule>
  {
    return react::CreateTimingModule(nativeQueue);
  }, nativeQueue));
#endif

  // TODO - Encapsulate this in a helpers, and make sure callers add it to their list
  std::string bundleUrl = m_devSettings->useWebDebugger ? DevServerHelper::get_BundleUrl(m_devSettings->debugHost, m_devSettings->debugBundlePath, m_devSettings->platformName, "true" /*dev*/, "false" /*hot*/) : std::string();
  modules.push_back(std::make_unique<CxxNativeModule>(m_innerInstance, facebook::react::SourceCodeModule::name, [bundleUrl]() -> std::unique_ptr<xplat::module::CxxModule>
  {
    return std::make_unique<SourceCodeModule>(bundleUrl);
  }, nativeQueue));

 modules.push_back(std::make_unique<CxxNativeModule>(m_innerInstance, "ExceptionsManager", [&]() -> std::unique_ptr<xplat::module::CxxModule>
  {
    return std::make_unique<facebook::react::ExceptionsManagerModule>(std::move(m_devSettings->jsExceptionCallback));
  }, nativeQueue));

  return modules;
}

void InstanceImpl::RegisterForReloadIfNecessary() noexcept {
  // setup polling for live reload
  if (!m_devManager->HasException() && m_devSettings->liveReloadCallback != nullptr) {
    m_devManager->StartPollingLiveReload(m_devSettings->debugHost, m_devSettings->liveReloadCallback);
  }
}

void InstanceImpl::DispatchEvent(int64_t viewTag, std::string eventName, folly::dynamic&& eventData) {
  if (m_devManager->HasException()) {
    return;
  }

  folly::dynamic params = folly::dynamic::array(viewTag, eventName, std::move(eventData));
  m_innerInstance->callJSFunction("RCTEventEmitter", "receiveEvent", std::move(params));
}

void InstanceImpl::invokeCallback(const int64_t callbackId, folly::dynamic&& params) {
  if (m_devManager->HasException()) {
    return;
  }

  m_innerInstance->callJSCallback(callbackId, std::move(params));
}

}}
