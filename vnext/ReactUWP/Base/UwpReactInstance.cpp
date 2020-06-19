// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"

#include "CoreNativeModules.h"
#include "UwpReactInstance.h"

// ReactUWP
#include <ReactUWP/CreateUwpModules.h>
#include <ReactUWP/IXamlRootView.h>
#include <ReactUWP/Threading/BatchingUIMessageQueueThread.h>

// Modules
#include <Modules/AppStateModuleUwp.h>
#include <Modules/AppThemeModuleUwp.h>
#include <Modules/ClipboardModule.h>
#include <Modules/NativeUIManager.h>
#include <Threading/JSQueueThread.h>
#include <Threading/UIMessageQueueThread.h>
#include <Threading/WorkerMessageQueueThread.h>

// ReactWindowsCore
#include <CreateModules.h>
#include <CxxMessageQueue.h>
#include <DevServerHelper.h>
#include <DevSettings.h>
#include <IUIManager.h>
#include <InstanceManager.h>
#include <NativeModuleProvider.h>

#include "Unicode.h"

#include <Modules/DevSettingsModule.h>
#include <Modules/DeviceInfoModule.h>
#include <cxxreact/CxxNativeModule.h>
#include <cxxreact/Instance.h>

#include <winrt/Windows.ApplicationModel.h>

#include <Utils/UwpPreparedScriptStore.h>
#include <Utils/UwpScriptStore.h>

#if defined(USE_HERMES)
#include "HermesRuntimeHolder.h"
#endif // USE_HERMES

#if defined(USE_V8)
#include <winrt/Windows.Storage.h>
#include "BaseScriptStoreImpl.h"
#include "V8JSIRuntimeHolder.h"
#endif // USE_V8

#include <ReactWindowsCore/IRedBoxHandler.h>
#include <winrt/Windows.UI.Popups.h>
#include "ChakraRuntimeHolder.h"

#include <winrt/Windows.UI.Popups.h>
#include <tuple>

namespace react {
namespace uwp {

UwpReactInstance::UwpReactInstance(
    const std::shared_ptr<facebook::react::NativeModuleProvider> &moduleProvider,
    const std::shared_ptr<ViewManagerProvider> &viewManagerProvider)
    : m_moduleProvider(moduleProvider), m_viewManagerProvider(viewManagerProvider) {}

UwpReactInstance::UwpReactInstance(
    const std::shared_ptr<facebook::react::TurboModuleRegistry> &turboModuleRegistry,
    const std::shared_ptr<facebook::react::NativeModuleProvider> &moduleProvider,
    const std::shared_ptr<ViewManagerProvider> &viewManagerProvider)
    : m_moduleProvider(moduleProvider),
      m_viewManagerProvider(viewManagerProvider),
      m_turboModuleRegistry(turboModuleRegistry) {}

struct UwpReactRedBoxHandler : Mso::React::IRedBoxHandler {
  // Inherited via IRedBoxHandler
  virtual void showNewError(Mso::React::ErrorInfo &&info, Mso::React::ErrorType) override {
    std::stringstream ss;

    ss << "A better redbox experience is provided by Microsoft.ReactNative - Consider moving off ReactUwp to Microsoft.ReactNative today!\n\n";
    ss << info.Message << "\n\n";
    for (auto frame : info.Callstack) {
      ss << frame.Method << "\n" << frame.File << ":" << frame.Line << ":" << frame.Column << "\n";
    }
    auto dlg = winrt::Windows::UI::Popups::MessageDialog(
        Microsoft::Common::Unicode::Utf8ToUtf16(ss.str().c_str()), L"RedBox Error");
    dlg.ShowAsync();
  }
  virtual bool isDevSupportEnabled() const override {
    return true;
  }
  virtual void updateError(Mso::React::ErrorInfo &&) override {}
  virtual void dismissRedbox() override {}
};

void UwpReactInstance::Start(const std::shared_ptr<IReactInstance> &spThis, const ReactInstanceSettings &settings) {
  if (m_started)
    return;

  m_reactInstanceSettings = settings;

  assert(
      m_uiDispatcher == nullptr && m_defaultNativeThread == nullptr && m_batchingNativeThread == nullptr &&
      m_jsThread == nullptr && m_initThread == nullptr && m_instanceWrapper == nullptr);

  m_started = true;
  m_uiDispatcher = winrt::Windows::UI::Core::CoreWindow::GetForCurrentThread().Dispatcher();
  m_defaultNativeThread = std::make_shared<react::uwp::UIMessageQueueThread>(m_uiDispatcher);
  m_batchingNativeThread = std::make_shared<react::uwp::BatchingUIMessageQueueThread>(m_uiDispatcher);

  // Objects that must be created on the UI thread
  auto deviceInfo(std::make_shared<DeviceInfo>(spThis));
  std::shared_ptr<facebook::react::AppState> appstate = std::make_shared<react::uwp::AppState>(spThis);
  std::shared_ptr<react::uwp::AppTheme> appTheme =
      std::make_shared<react::uwp::AppTheme>(spThis, m_defaultNativeThread);
  I18nHelper::Instance().setInfo(I18nModule::GetI18nInfo());
  auto appearanceListener = Mso::Make<AppearanceChangeListener>(spThis, Mso::DispatchQueue::GetCurrentUIThreadQueue());

  // TODO: Figure out threading. What thread should this really be on?
  m_initThread = std::make_unique<react::uwp::WorkerMessageQueueThread>();
  m_jsThread = std::static_pointer_cast<facebook::react::MessageQueueThread>(m_initThread);
  m_initThread->runOnQueueSync([this,
                                spThis,
                                deviceInfo,
                                settings,
                                appstate = std::move(appstate),
                                appTheme = std::move(appTheme),
                                appearanceListener = std::move(appearanceListener)]() mutable {
    // Setup DevSettings based on our own internal structure
    auto devSettings(std::make_shared<facebook::react::DevSettings>());
    devSettings->debugBundlePath = settings.DebugBundlePath;
    devSettings->useWebDebugger = settings.UseWebDebugger;
    devSettings->useDirectDebugger = settings.UseDirectDebugger;
    devSettings->debuggerBreakOnNextLine = settings.DebuggerBreakOnNextLine;
    devSettings->loggingCallback = std::move(settings.LoggingCallback);
    devSettings->redboxHandler = std::move(settings.RedBoxHandler);
    devSettings->useJITCompilation = settings.EnableJITCompilation;
    devSettings->debugHost = settings.DebugHost;
    devSettings->debuggerPort = settings.DebuggerPort;

    if (!devSettings->redboxHandler &&
        (devSettings->useWebDebugger || devSettings->useDirectDebugger || settings.UseLiveReload)) {
      devSettings->redboxHandler = std::move(std::make_shared<UwpReactRedBoxHandler>());
    }

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
        m_defaultNativeThread,
        std::move(deviceInfo),
        std::move(appstate),
        std::move(appTheme),
        std::move(appearanceListener),
        spThis);

    cxxModules.emplace_back(
        ClipboardModule::name, []() { return std::make_unique<ClipboardModule>(); }, m_batchingNativeThread);

    cxxModules.emplace_back(
        DevSettingsModule::name, []() { return std::make_unique<DevSettingsModule>(); }, m_batchingNativeThread);

    if (m_moduleProvider != nullptr) {
      std::vector<facebook::react::NativeModuleDescription> customCxxModules =
          m_moduleProvider->GetModules(m_batchingNativeThread);
      cxxModules.insert(std::end(cxxModules), std::begin(customCxxModules), std::end(customCxxModules));
    }

    std::shared_ptr<facebook::react::MessageQueueThread> jsQueue = CreateAndStartJSQueueThread();

    if (settings.UseJsi) {
      std::unique_ptr<facebook::jsi::ScriptStore> scriptStore = nullptr;
      std::unique_ptr<facebook::jsi::PreparedScriptStore> preparedScriptStore = nullptr;

      switch (settings.jsiEngine) {
        case JSIEngine::Hermes:
#if defined(USE_HERMES)
          devSettings->jsiRuntimeHolder = std::make_shared<facebook::react::HermesRuntimeHolder>();
          break;
#endif
        case JSIEngine::V8:
#if defined(USE_V8)
          preparedScriptStore =
              std::make_unique<facebook::react::BasePreparedScriptStoreImpl>(getApplicationLocalFolder());

          devSettings->jsiRuntimeHolder = std::make_shared<facebook::react::V8JSIRuntimeHolder>(
              devSettings, jsQueue, std::move(scriptStore), std::move(preparedScriptStore));
          break;
#endif
        case JSIEngine::Chakra:
          if (settings.EnableByteCodeCaching || !settings.ByteCodeFileUri.empty()) {
            scriptStore = std::make_unique<UwpScriptStore>();
            preparedScriptStore = std::make_unique<UwpPreparedScriptStore>(winrt::to_hstring(settings.ByteCodeFileUri));
          }
          devSettings->jsiRuntimeHolder = std::make_shared<Microsoft::JSI::ChakraRuntimeHolder>(
              devSettings, jsQueue, std::move(scriptStore), std::move(preparedScriptStore));
          break;
      }
    }

    try {
      // Create the react instance
      m_instanceWrapper = facebook::react::CreateReactInstance(
          std::string(), // bundleRootPath
          std::move(cxxModules),
          m_turboModuleRegistry,
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
  return std::make_shared<react::uwp::UIMessageQueueThread>(m_uiDispatcher);
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
  try {
    auto local = winrt::Windows::Storage::ApplicationData::Current().LocalFolder().Path();

    return Microsoft::Common::Unicode::Utf16ToUtf8(local.c_str(), local.size()) + "\\";
  } catch (winrt::hresult_error const &ex) {
    winrt::hresult hr = ex.to_abi();
    if (hr == HRESULT_FROM_WIN32(APPMODEL_ERROR_NO_PACKAGE)) {
      // This is a win32 application using UWP APIs, pick a reasonable location for caching bytecode
      char tempPath[MAX_PATH];
      if (GetTempPathA(MAX_PATH, tempPath)) {
        return std::string(tempPath);
      }
    }

    throw ex;
  }
}
#endif

} // namespace uwp
} // namespace react
