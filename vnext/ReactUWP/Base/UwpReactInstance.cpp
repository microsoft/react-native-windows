// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"

#include "CoreNativeModules.h"
#include "UwpReactInstance.h"

// ReactUWP
#include <ReactUWP/CreateUwpModules.h>
#include <ReactUWP/IXamlRootView.h>

// Modules
#include <Modules/AppStateModuleUwp.h>
#include <Modules/AppThemeModuleUwp.h>
#include <Modules/NativeUIManager.h>

// ReactWindowsCore
#include <CreateModules.h>
#include <CxxMessageQueue.h>
#include <DevServerHelper.h>
#include <DevSettings.h>
#include <IUIManager.h>
#include <InstanceManager.h>
#include <NativeModuleProvider.h>

#include <Threading/MessageQueueThreadFactory.h>
#include "Unicode.h"

#include <Modules/DeviceInfoModule.h>
#include <cxxreact/CxxNativeModule.h>
#include <cxxreact/Instance.h>

#include <winrt/Windows.ApplicationModel.h>

#ifdef PATCH_RN
#include <Utils/UwpPreparedScriptStore.h>
#include <Utils/UwpScriptStore.h>
#endif

#ifdef PATCH_RN
#if defined(USE_HERMES)
#include "HermesRuntimeHolder.h"
#endif // USE_HERMES
#if defined(USE_V8)
#include "BaseScriptStoreImpl.h"
#include "V8JSIRuntimeHolder.h"

#include <winrt/Windows.Storage.h>

#include <codecvt>
#include <locale>
#else
#include "ChakraRuntimeHolder.h"
#endif
#endif

#include <tuple>

namespace react {
namespace uwp {

UwpReactInstance::UwpReactInstance(
    const std::shared_ptr<facebook::react::NativeModuleProvider> &moduleProvider,
    const std::shared_ptr<ViewManagerProvider> &viewManagerProvider)
    : m_moduleProvider(moduleProvider), m_viewManagerProvider(viewManagerProvider) {}

void UwpReactInstance::Start(const std::shared_ptr<IReactInstance> &spThis, const ReactInstanceSettings &settings) {
  if (m_started)
    return;

  m_reactInstanceSettings = settings;

  assert(
      m_uiDispatcher == nullptr && m_defaultNativeThread == nullptr && m_batchingNativeThread == nullptr &&
      m_jsThread == nullptr && m_initThread == nullptr && m_instanceWrapper == nullptr);

  m_started = true;
  m_defaultNativeThread = MakeUIQueueThread();
  m_batchingNativeThread = MakeBatchingQueueThread(m_defaultNativeThread);

  // Objects that must be created on the UI thread
  auto deviceInfo(std::make_shared<DeviceInfo>(spThis));
  std::shared_ptr<facebook::react::AppState> appstate = std::make_shared<react::uwp::AppState>(spThis);
  std::shared_ptr<react::windows::AppTheme> appTheme =
      std::make_shared<react::uwp::AppTheme>(spThis, m_defaultNativeThread);
  std::pair<std::string, bool> i18nInfo = I18nModule::GetI18nInfo();

  // TODO: Figure out threading. What thread should this really be on?
  m_initThread = MakeSerialQueueThread();
  m_jsThread = std::static_pointer_cast<facebook::react::MessageQueueThread>(m_initThread);
  m_initThread->runOnQueueSync([this,
                                spThis,
                                deviceInfo,
                                settings,
                                i18nInfo = std::move(i18nInfo),
                                appstate = std::move(appstate),
                                appTheme = std::move(appTheme)]() mutable {
    // Setup DevSettings based on our own internal structure
    auto devSettings(std::make_shared<facebook::react::DevSettings>());
    devSettings->debugBundlePath = settings.DebugBundlePath;
    devSettings->useWebDebugger = settings.UseWebDebugger;
    devSettings->useDirectDebugger = settings.UseDirectDebugger;
    devSettings->loggingCallback = std::move(settings.LoggingCallback);
    devSettings->jsExceptionCallback = std::move(settings.JsExceptionCallback);
    devSettings->useJITCompilation = settings.EnableJITCompilation;
    devSettings->debugHost = settings.DebugHost;

    // In most cases, using the hardcoded ms-appx URI works fine, but there are
    // certain scenarios, such as in optional packaging, where the developer
    // might need to modify the path, in which case we should use the custom
    // path instead.
    devSettings->bundleRootPath = settings.BundleRootPath.empty() ? "ms-appx:///Bundle/" : settings.BundleRootPath;
    m_bundleRootPath = devSettings->bundleRootPath;

    if (settings.UseLiveReload) {
      devSettings->liveReloadCallback = [weakThis = std::weak_ptr<IReactInstance>(spThis)]() noexcept {
        auto strongThis = weakThis.lock();
        if (strongThis != nullptr) {
          auto uwpInstance = std::static_pointer_cast<UwpReactInstance>(strongThis);

          // Mark the instance in needs reload state
          uwpInstance->SetAsNeedsReload();

          // Invoke every callback registered
          for (auto const &current : uwpInstance->m_liveReloadCallbacks)
            current.second();
        }
      };
    }

    devSettings->errorCallback = [weakThis = std::weak_ptr<IReactInstance>(spThis)](std::string message) noexcept {
      auto strongThis = weakThis.lock();
      if (strongThis != nullptr) {
        auto uwpInstance = std::static_pointer_cast<UwpReactInstance>(strongThis);
        uwpInstance->OnHitError(message);
      }
    };

    if (settings.UseWebDebugger) {
      devSettings->waitingForDebuggerCallback = [weakThis = std::weak_ptr<IReactInstance>(spThis)]() noexcept {
        auto strongThis = weakThis.lock();
        if (strongThis != nullptr) {
          auto uwpInstance = std::static_pointer_cast<UwpReactInstance>(strongThis);
          uwpInstance->OnWaitingForDebugger();
        }
      };

      devSettings->debuggerAttachCallback = [weakThis = std::weak_ptr<IReactInstance>(spThis)]() noexcept {
        auto strongThis = weakThis.lock();
        if (strongThis != nullptr) {
          auto uwpInstance = std::static_pointer_cast<UwpReactInstance>(strongThis);
          uwpInstance->OnDebuggerAttach();
        }
      };
    }

    // Create NativeUIManager & UIManager
    m_uiManager = CreateUIManager(spThis, m_viewManagerProvider);

    // Acquire default modules and then populate with custom modules
    std::vector<facebook::react::NativeModuleDescription> cxxModules = GetCoreModules(
        m_uiManager,
        m_batchingNativeThread,
        deviceInfo,
        devSettings,
        std::move(i18nInfo),
        std::move(appstate),
        std::move(appTheme),
        std::weak_ptr<IReactInstance>(spThis));

    if (m_moduleProvider != nullptr) {
      std::vector<facebook::react::NativeModuleDescription> customCxxModules =
          m_moduleProvider->GetModules(m_batchingNativeThread);
      cxxModules.insert(std::end(cxxModules), std::begin(customCxxModules), std::end(customCxxModules));
    }

    std::shared_ptr<facebook::react::MessageQueueThread> jsQueue = MakeJSQueueThread();

#ifdef PATCH_RN
    if (settings.UseJsi) {
      std::unique_ptr<facebook::jsi::ScriptStore> scriptStore = nullptr;
      std::unique_ptr<facebook::jsi::PreparedScriptStore> preparedScriptStore = nullptr;

#if defined(USE_HERMES)
      devSettings->jsiRuntimeHolder = std::make_shared<facebook::react::HermesRuntimeHolder>();
#elif defined(USE_V8)
      preparedScriptStore = std::make_unique<facebook::react::BasePreparedScriptStoreImpl>(getApplicationLocalFolder());

      devSettings->jsiRuntimeHolder = std::make_shared<facebook::react::V8JSIRuntimeHolder>(
          devSettings, jsQueue, std::move(scriptStore), std::move(preparedScriptStore));
#else
      if (settings.EnableByteCodeCaching || !settings.ByteCodeFileUri.empty()) {
        scriptStore = std::make_unique<UwpScriptStore>();
        preparedScriptStore = std::make_unique<UwpPreparedScriptStore>(winrt::to_hstring(settings.ByteCodeFileUri));
      }
      devSettings->jsiRuntimeHolder = std::make_shared<Microsoft::JSI::ChakraRuntimeHolder>(
          devSettings, jsQueue, std::move(scriptStore), std::move(preparedScriptStore));
#endif
    }
#endif

    try {
      // Create the react instance
      m_instanceWrapper = facebook::react::CreateReactInstance(
          std::string(), // bundleRootPath
          std::move(cxxModules),
          m_uiManager,
          jsQueue,
          m_batchingNativeThread,
          std::move(devSettings));
    } catch (std::exception &e) {
      OnHitError(e.what());
      OnHitError("UwpReactInstance: Failed to create React Instance.");
    } catch (winrt::hresult_error const &e) {
      OnHitError(Microsoft::Common::Unicode::Utf16ToUtf8(e.message().c_str(), e.message().size()));
      OnHitError("UwpReactInstance: Failed to create React Instance.");
    } catch (...) {
      OnHitError("UwpReactInstance: Failed to create React Instance.");
    }
  });
}

void UwpReactInstance::AttachMeasuredRootView(IXamlRootView *pRootView, folly::dynamic &&initProps) {
  if (!IsInError()) {
    m_instanceWrapper->AttachMeasuredRootView(pRootView, std::move(initProps));
  }
}

void UwpReactInstance::DetachRootView(IXamlRootView *pRootView) {
  m_instanceWrapper->DetachRootView(pRootView);
}

LiveReloadCallbackCookie UwpReactInstance::RegisterLiveReloadCallback(std::function<void()> callback) {
  static LiveReloadCallbackCookie g_nextLiveReloadCallbackCookie(0);

  // Add callback to map with new cookie
  LiveReloadCallbackCookie cookie = ++g_nextLiveReloadCallbackCookie;
  m_liveReloadCallbacks[cookie] = callback;

  // Its possible this instance is already in a reload state, if so
  // trigger immediate reload
  if (NeedsReload())
    callback();

  return cookie;
}

void UwpReactInstance::UnregisterLiveReloadCallback(LiveReloadCallbackCookie &cookie) {
  m_liveReloadCallbacks.erase(cookie);
  cookie = 0;
}

ErrorCallbackCookie UwpReactInstance::RegisterErrorCallback(std::function<void()> callback) {
  static ErrorCallbackCookie g_nextErrorCallbackCookie(0);

  // Add callback to map with new cookie
  ErrorCallbackCookie cookie = ++g_nextErrorCallbackCookie;
  m_errorCallbacks[cookie] = callback;

  return cookie;
}

void UwpReactInstance::UnregisterErrorCallback(ErrorCallbackCookie &cookie) {
  m_errorCallbacks.erase(cookie);
  cookie = 0;
}

DebuggerAttachCallbackCookie UwpReactInstance::RegisterDebuggerAttachCallback(std::function<void()> callback) {
  static DebuggerAttachCallbackCookie g_nextDebuggerAttachCallbackCookie(0);

  // Add callback to map with new cookie
  DebuggerAttachCallbackCookie cookie = ++g_nextDebuggerAttachCallbackCookie;
  m_debuggerAttachCallbacks[cookie] = callback;

  return cookie;
}

void UwpReactInstance::UnRegisterDebuggerAttachCallback(DebuggerAttachCallbackCookie &cookie) {
  m_debuggerAttachCallbacks.erase(cookie);
  cookie = 0;
}

void UwpReactInstance::DispatchEvent(int64_t viewTag, std::string eventName, folly::dynamic &&eventData) {
  if (!IsInError())
    m_instanceWrapper->DispatchEvent(viewTag, eventName, std::move(eventData));
}

void UwpReactInstance::CallJsFunction(
    std::string &&moduleName,
    std::string &&method,
    folly::dynamic &&params) noexcept {
  if (!IsInError())
    m_instanceWrapper->GetInstance()->callJSFunction(std::move(moduleName), std::move(method), std::move(params));
}

std::shared_ptr<facebook::react::MessageQueueThread> UwpReactInstance::GetNewUIMessageQueue() const {
  return MakeUIQueueThread();
}

const std::shared_ptr<facebook::react::MessageQueueThread> &UwpReactInstance::JSMessageQueueThread() const noexcept {
  return m_jsThread;
}

const std::shared_ptr<facebook::react::MessageQueueThread> &UwpReactInstance::DefaultNativeMessageQueueThread() const
    noexcept {
  return m_defaultNativeThread;
}

facebook::react::INativeUIManager *UwpReactInstance::NativeUIManager() const noexcept {
  return m_uiManager->getNativeUIManager();
}

static std::string PrettyError(const std::string &error) noexcept {
  std::string prettyError = error;
  if (prettyError.length() > 0 && prettyError[0] == '{') {
    // if starting with {, assume JSONy

    // Replace escape characters with actuals
    size_t pos = prettyError.find('\\');
    while (pos != std::wstring::npos && pos + 2 <= prettyError.length()) {
      if (prettyError[pos + 1] == 'n') {
        prettyError.replace(pos, 2, "\r\n", 2);
      } else if (prettyError[pos + 1] == 'b') {
        prettyError.replace(pos, 2, "\b", 2);
      } else if (prettyError[pos + 1] == 't') {
        prettyError.replace(pos, 2, "\t", 2);
      } else if (prettyError[pos + 1] == 'u' && pos + 6 <= prettyError.length()) {
        // Convert 4 hex digits
        auto hexVal = [&](int c) -> uint16_t {
          return uint16_t(
              c >= '0' && c <= '9' ? c - '0'
                                   : c >= 'a' && c <= 'f' ? c - 'a' + 10 : c >= 'A' && c <= 'F' ? c - 'A' + 10 : 0);
        };
        wchar_t replWide = 0;
        replWide += hexVal(prettyError[pos + 2]) << 12;
        replWide += hexVal(prettyError[pos + 3]) << 8;
        replWide += hexVal(prettyError[pos + 4]) << 4;
        replWide += hexVal(prettyError[pos + 5]);
        std::string repl = Microsoft::Common::Unicode::Utf16ToUtf8(&replWide, 1);

        prettyError.replace(pos, 6, repl);
      }

      pos = prettyError.find('\\', pos + 2);
    }
  }

  return prettyError;
}

void UwpReactInstance::OnHitError(const std::string &error) noexcept {
  m_isInError = true;

  // Append new error message onto others
  if (!m_errorMessage.empty())
    m_errorMessage += "\n";
  m_errorMessage += " -- ";
  m_errorMessage += PrettyError(error);

  OutputDebugStringA("UwpReactInstance Error Hit ...\n");
  OutputDebugStringA(m_errorMessage.c_str());
  OutputDebugStringA("\n");

  // Invoke every callback registered
  for (auto const &current : m_errorCallbacks)
    current.second();
}

void UwpReactInstance::OnWaitingForDebugger() noexcept {
  m_isWaitingForDebugger = true;
}

void UwpReactInstance::OnDebuggerAttach() noexcept {
  m_isWaitingForDebugger = false;

  // Invoke every callback registered
  for (auto const &current : m_debuggerAttachCallbacks)
    current.second();
}

void UwpReactInstance::SetXamlViewCreatedTestHook(std::function<void(react::uwp::XamlView)> testHook) {
  m_xamlViewCreatedTestHook = testHook;
}

void UwpReactInstance::CallXamlViewCreatedTestHook(react::uwp::XamlView view) {
  if (m_xamlViewCreatedTestHook != nullptr) {
    m_xamlViewCreatedTestHook(view);
  }
}

#if defined(USE_V8)
std::string UwpReactInstance::getApplicationLocalFolder() {
  auto local = winrt::Windows::Storage::ApplicationData::Current().LocalFolder().Path();

  return std::wstring_convert<std::codecvt_utf8<wchar_t>>().to_bytes(std::wstring(local.c_str(), local.size())) + "\\";
}
#endif

} // namespace uwp
} // namespace react
