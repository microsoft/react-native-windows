// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"

#include "UwpReactInstance.h"

// ReactUWP
#include <ReactUWP/IXamlRootView.h>
#include <ReactUWP/Threading/BatchingUIMessageQueueThread.h>

// ReactWindowsCore
#include <CreateModules.h>
#include <CxxMessageQueue.h>
#include <DevServerHelper.h>
#include <DevSettings.h>
#include <InstanceManager.h>
#include <NativeModuleProvider.h>

#include "Unicode.h"

// Standard View Managers
#include <Views/ActivityIndicatorViewManager.h>
#include <Views/CheckboxViewManager.h>
#include <Views/DatePickerViewManager.h>
#include <Views/FlyoutViewManager.h>
#include <Views/Image/ImageViewManager.h>
#include <Views/PickerViewManager.h>
#include <Views/PopupViewManager.h>
#include <Views/RawTextViewManager.h>
#include <Views/RootViewManager.h>
#include <Views/ScrollContentViewManager.h>
#include <Views/ScrollViewManager.h>
#include <Views/SwitchViewManager.h>
#include <Views/TextInputViewManager.h>
#include <Views/TextViewManager.h>
#include <Views/ViewViewManager.h>
#include <Views/VirtualTextViewManager.h>
#include <Views/WebViewManager.h>

// Polyester View Managers // TODO: Move Polyester implementations out of this
// library and depot
#include <Polyester/ButtonContentViewManager.h>
#include <Polyester/ButtonViewManager.h>
#include <Polyester/HyperlinkViewManager.h>
#include <Polyester/IconViewManager.h>

// Modules
#include <AsyncStorageModule.h>
#include <Modules/AlertModuleUwp.h>
#include <Modules/Animated/NativeAnimatedModule.h>
#include <Modules/AppStateModuleUwp.h>
#include <Modules/AppThemeModuleUwp.h>
#include <Modules/ClipboardModule.h>
#include <Modules/ImageViewManagerModule.h>
#include <Modules/LinkingManagerModule.h>
#include <Modules/LocationObserverModule.h>
#include <Modules/NativeUIManager.h>
#include <Modules/NetworkingModule.h>
#include <Modules/StatusBarModule.h>
#include <Modules/UIManagerModule.h>
#include <Modules/WebSocketModuleUwp.h>
#include <ReactUWP/Modules/I18nModule.h>
#include <ReactWindowsCore/IUIManager.h>
#include <Threading/JSQueueThread.h>
#include <Threading/UIMessageQueueThread.h>
#include <Threading/WorkerMessageQueueThread.h>

#include <cxxreact/CxxNativeModule.h>
#include <cxxreact/Instance.h>

#if !defined(OSS_RN)
#include <Utils/UwpPreparedScriptStore.h>
#include <Utils/UwpScriptStore.h>
#endif

#if !defined(OSS_RN)
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

// TODO: This function is just a stand-in for a system that allows an individual
// host to provide a
//  different set of view managers and native ui manager. Having all of this in
//  one place will simply make it easier to slot in that system when ready.

REACTWINDOWS_API_(std::shared_ptr<facebook::react::IUIManager>)
CreateUIManager(
    std::shared_ptr<IReactInstance> instance,
    const std::shared_ptr<ViewManagerProvider> &viewManagerProvider) {
  std::vector<std::unique_ptr<facebook::react::IViewManager>> viewManagers;

  // Custom view managers
  if (viewManagerProvider) {
    viewManagers = viewManagerProvider->GetViewManagers(instance);
  }

  // Standard view managers
  viewManagers.push_back(
      std::make_unique<ActivityIndicatorViewManager>(instance));
  viewManagers.push_back(std::make_unique<CheckBoxViewManager>(instance));
  viewManagers.push_back(std::make_unique<DatePickerViewManager>(instance));
  viewManagers.push_back(std::make_unique<FlyoutViewManager>(instance));
  viewManagers.push_back(std::make_unique<ImageViewManager>(instance));
  viewManagers.push_back(std::make_unique<PickerViewManager>(instance));
  viewManagers.push_back(std::make_unique<PopupViewManager>(instance));
  viewManagers.push_back(std::make_unique<RawTextViewManager>(instance));
  viewManagers.push_back(std::make_unique<RootViewManager>(instance));
  viewManagers.push_back(std::make_unique<ScrollContentViewManager>(instance));
  viewManagers.push_back(std::make_unique<ScrollViewManager>(instance));
  viewManagers.push_back(std::make_unique<SwitchViewManager>(instance));
  viewManagers.push_back(std::make_unique<TextViewManager>(instance));
  viewManagers.push_back(std::make_unique<TextInputViewManager>(instance));
  viewManagers.push_back(std::make_unique<ViewViewManager>(instance));
  viewManagers.push_back(std::make_unique<VirtualTextViewManager>(instance));
  viewManagers.push_back(std::make_unique<WebViewManager>(instance));

  // Polyester view managers
  viewManagers.push_back(
      std::make_unique<polyester::ButtonViewManager>(instance));
  viewManagers.push_back(
      std::make_unique<polyester::ButtonContentViewManager>(instance));
  viewManagers.push_back(
      std::make_unique<polyester::HyperlinkViewManager>(instance));
  viewManagers.push_back(
      std::make_unique<polyester::IconViewManager>(instance));

  // Create UIManager, passing in ViewManagers
  return createIUIManager(std::move(viewManagers), new NativeUIManager());
}

UwpReactInstance::UwpReactInstance(
    const std::shared_ptr<facebook::react::NativeModuleProvider>
        &moduleProvider,
    const std::shared_ptr<ViewManagerProvider> &viewManagerProvider)
    : m_moduleProvider(moduleProvider),
      m_viewManagerProvider(viewManagerProvider) {}

std::vector<facebook::react::NativeModuleDescription> GetModules(
    std::shared_ptr<facebook::react::IUIManager> uiManager,
    const std::shared_ptr<facebook::react::MessageQueueThread> &messageQueue,
    std::shared_ptr<DeviceInfo> deviceInfo,
    std::shared_ptr<facebook::react::DevSettings> devSettings,
    const I18nModule::I18nInfo &&i18nInfo,
    std::shared_ptr<facebook::react::AppState> appstate,
    std::shared_ptr<react::windows::AppTheme> appTheme,
    std::weak_ptr<IReactInstance> uwpInstance) {
  // Modules
  std::vector<facebook::react::NativeModuleDescription> modules;

  modules.emplace_back(
      "UIManager",
      [uiManager = std::move(uiManager)]() {
        return facebook::react::createUIManagerModule(uiManager);
      },
      messageQueue);

  modules.emplace_back(
      react::uwp::WebSocketModule::name,
      []() { return std::make_unique<react::uwp::WebSocketModule>(); },
      std::make_shared<WorkerMessageQueueThread>());

  modules.emplace_back(
      NetworkingModule::name,
      []() { return std::make_unique<NetworkingModule>(); },
      std::make_shared<WorkerMessageQueueThread>());

  modules.emplace_back(
      "Timing",
      [messageQueue]() {
        return facebook::react::CreateTimingModule(messageQueue);
      },
      messageQueue);

  modules.emplace_back(
      DeviceInfoModule::name,
      [deviceInfo]() { return std::make_unique<DeviceInfoModule>(deviceInfo); },
      messageQueue);

  modules.emplace_back(
      LinkingManagerModule::name,
      []() { return std::make_unique<LinkingManagerModule>(); },
      messageQueue);

  modules.emplace_back(
      ImageViewManagerModule::name,
      [messageQueue]() {
        return std::make_unique<ImageViewManagerModule>(messageQueue);
      },
      messageQueue);

  modules.emplace_back(
      LocationObserverModule::name,
      [messageQueue]() {
        return std::make_unique<LocationObserverModule>(messageQueue);
      },
      std::make_shared<WorkerMessageQueueThread>()); // TODO: figure out
                                                     // threading

  modules.emplace_back(
      facebook::react::AppStateModule::name,
      [appstate = std::move(appstate)]() mutable {
        return std::make_unique<facebook::react::AppStateModule>(
            std::move(appstate));
      },
      std::make_shared<WorkerMessageQueueThread>());

  modules.emplace_back(
      react::windows::AppThemeModule::name,
      [appTheme = std::move(appTheme)]() mutable {
        return std::make_unique<react::windows::AppThemeModule>(
            std::move(appTheme));
      },
      messageQueue);

  modules.emplace_back(
      AlertModule::name,
      []() { return std::make_unique<AlertModule>(); },
      messageQueue);

  modules.emplace_back(
      ClipboardModule::name,
      []() { return std::make_unique<ClipboardModule>(); },
      messageQueue);

  modules.emplace_back(
      StatusBarModule::name,
      []() { return std::make_unique<StatusBarModule>(); },
      messageQueue);

  modules.emplace_back(
      NativeAnimatedModule::name,
      [uwpInstance = std::move(uwpInstance)]() mutable {
        return std::make_unique<NativeAnimatedModule>(std::move(uwpInstance));
      },
      messageQueue);

  modules.emplace_back(
      "I18nManager",
      [i18nInfo = std::move(i18nInfo)]() mutable {
        return createI18nModule(
            std::make_unique<I18nModule>(std::move(i18nInfo)));
      },
      messageQueue);

  modules.emplace_back(
      "AsyncLocalStorage",
      []() {
        return std::make_unique<facebook::react::AsyncStorageModule>(
            L"asyncStorage");
      },
      std::make_shared<WorkerMessageQueueThread>());

  return modules;
}

void UwpReactInstance::Start(
    const std::shared_ptr<IReactInstance> &spThis,
    const ReactInstanceSettings &settings) {
  if (m_started)
    return;

  m_reactInstanceSettings = settings;

  assert(
      m_uiDispatcher == nullptr && m_defaultNativeThread == nullptr &&
      m_batchingNativeThread == nullptr && m_jsThread == nullptr &&
      m_initThread == nullptr && m_instanceWrapper == nullptr);

  m_started = true;
  m_uiDispatcher = winrt::CoreWindow::GetForCurrentThread().Dispatcher();
  m_defaultNativeThread =
      std::make_shared<react::uwp::UIMessageQueueThread>(m_uiDispatcher);
  m_batchingNativeThread =
      std::make_shared<react::uwp::BatchingUIMessageQueueThread>(
          m_uiDispatcher);

  // Objects that must be created on the UI thread
  m_deviceInfo = std::make_shared<DeviceInfo>(spThis);
  std::shared_ptr<facebook::react::AppState> appstate =
      std::make_shared<react::uwp::AppState>(spThis);
  std::shared_ptr<react::windows::AppTheme> appTheme =
      std::make_shared<react::uwp::AppTheme>(spThis, m_defaultNativeThread);
  std::pair<std::string, bool> i18nInfo = I18nModule::GetI18nInfo();

  // TODO: Figure out threading. What thread should this really be on?
  m_initThread = std::make_unique<react::uwp::WorkerMessageQueueThread>();
  m_jsThread = std::static_pointer_cast<facebook::react::MessageQueueThread>(
      m_initThread);
  m_initThread->runOnQueueSync([this,
                                spThis,
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
    devSettings->bundleRootPath = settings.BundleRootPath.empty()
        ? "ms-appx:///Bundle/"
        : settings.BundleRootPath;
    m_bundleRootPath = devSettings->bundleRootPath;

    if (settings.UseLiveReload) {
      devSettings->liveReloadCallback =
          [weakThis = std::weak_ptr<IReactInstance>(spThis)]() noexcept {
        auto strongThis = weakThis.lock();
        if (strongThis != nullptr) {
          auto uwpInstance =
              std::static_pointer_cast<UwpReactInstance>(strongThis);

          // Mark the instance in needs reload state
          uwpInstance->SetAsNeedsReload();

          // Invoke every callback registered
          for (auto const &current : uwpInstance->m_liveReloadCallbacks)
            current.second();
        }
      };
    }

    devSettings
        ->errorCallback = [weakThis = std::weak_ptr<IReactInstance>(spThis)](
        std::string message) noexcept {
      auto strongThis = weakThis.lock();
      if (strongThis != nullptr) {
        auto uwpInstance =
            std::static_pointer_cast<UwpReactInstance>(strongThis);
        uwpInstance->OnHitError(message);
      }
    };

    // Create NativeUIManager & UIManager
    m_uiManager = CreateUIManager(spThis, m_viewManagerProvider);

    // Acquire default modules and then populate with custom modules
    std::vector<facebook::react::NativeModuleDescription> cxxModules =
        GetModules(
            m_uiManager,
            m_batchingNativeThread,
            m_deviceInfo,
            devSettings,
            std::move(i18nInfo),
            std::move(appstate),
            std::move(appTheme),
            std::weak_ptr<IReactInstance>(spThis));

    if (m_moduleProvider != nullptr) {
      std::vector<facebook::react::NativeModuleDescription> customCxxModules =
          m_moduleProvider->GetModules(m_batchingNativeThread);
      cxxModules.insert(
          std::end(cxxModules),
          std::begin(customCxxModules),
          std::end(customCxxModules));
    }

    std::shared_ptr<facebook::react::CxxMessageQueue> jsQueue =
        CreateAndStartJSQueueThread();

#if !defined(OSS_RN)
    if (settings.UseJsi) {
      std::unique_ptr<facebook::jsi::ScriptStore> scriptStore = nullptr;
      std::unique_ptr<facebook::jsi::PreparedScriptStore> preparedScriptStore =
          nullptr;

#if defined(USE_HERMES)
      devSettings->jsiRuntimeHolder =
          std::make_shared<facebook::react::HermesRuntimeHolder>();
#elif defined(USE_V8)
      preparedScriptStore =
          std::make_unique<facebook::react::BasePreparedScriptStoreImpl>(
              getApplicationLocalFolder());

      devSettings->jsiRuntimeHolder =
          std::make_shared<facebook::react::V8JSIRuntimeHolder>(
              devSettings,
              jsQueue,
              std::move(scriptStore),
              std::move(preparedScriptStore));
#else
      if (settings.EnableByteCodeCacheing ||
          !settings.ByteCodeFileUri.empty()) {
        scriptStore = std::make_unique<UwpScriptStore>();
        preparedScriptStore = std::make_unique<UwpPreparedScriptStore>(
            winrt::to_hstring(settings.ByteCodeFileUri));
      }
      devSettings->jsiRuntimeHolder =
          std::make_shared<Microsoft::JSI::ChakraRuntimeHolder>(
              devSettings,
              jsQueue,
              std::move(scriptStore),
              std::move(preparedScriptStore));
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
      OnHitError(Microsoft::Common::Unicode::Utf16ToUtf8(
          e.message().c_str(), e.message().size()));
      OnHitError("UwpReactInstance: Failed to create React Instance.");
    } catch (...) {
      OnHitError("UwpReactInstance: Failed to create React Instance.");
    }
  });
}

void UwpReactInstance::AttachMeasuredRootView(
    IXamlRootView *pRootView,
    folly::dynamic &&initProps) {
  if (!IsInError()) {
    m_instanceWrapper->AttachMeasuredRootView(pRootView, std::move(initProps));
    auto rootView = pRootView->GetXamlView().try_as<winrt::FrameworkElement>();
    if (rootView) {
      m_deviceInfo->attachRoot(rootView);
    }
  }
}

void UwpReactInstance::DetachRootView(IXamlRootView *pRootView) {
  m_deviceInfo->detachRoot();
  m_instanceWrapper->DetachRootView(pRootView);
}

LiveReloadCallbackCookie UwpReactInstance::RegisterLiveReloadCallback(
    std::function<void()> callback) {
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

void UwpReactInstance::UnregisterLiveReloadCallback(
    LiveReloadCallbackCookie &cookie) {
  m_liveReloadCallbacks.erase(cookie);
  cookie = 0;
}

ErrorCallbackCookie UwpReactInstance::RegisterErrorCallback(
    std::function<void()> callback) {
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

void UwpReactInstance::DispatchEvent(
    int64_t viewTag,
    std::string eventName,
    folly::dynamic &&eventData) {
  if (!IsInError())
    m_instanceWrapper->DispatchEvent(viewTag, eventName, std::move(eventData));
}

void UwpReactInstance::CallJsFunction(
    std::string &&moduleName,
    std::string &&method,
    folly::dynamic &&params) noexcept {
  if (!IsInError())
    m_instanceWrapper->GetInstance()->callJSFunction(
        std::move(moduleName), std::move(method), std::move(params));
}

std::shared_ptr<facebook::react::MessageQueueThread>
UwpReactInstance::GetNewUIMessageQueue() const {
  return std::make_shared<react::uwp::UIMessageQueueThread>(m_uiDispatcher);
}

const std::shared_ptr<facebook::react::MessageQueueThread>
    &UwpReactInstance::JSMessageQueueThread() const noexcept {
  return m_jsThread;
}

const std::shared_ptr<facebook::react::MessageQueueThread>
    &UwpReactInstance::DefaultNativeMessageQueueThread() const noexcept {
  return m_defaultNativeThread;
}

facebook::react::INativeUIManager *UwpReactInstance::NativeUIManager() const
    noexcept {
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
      } else if (
          prettyError[pos + 1] == 'u' && pos + 6 <= prettyError.length()) {
        // Convert 4 hex digits
        auto hexVal = [&](int c) -> uint16_t {
          return uint16_t(
              c >= '0' && c <= '9' ? c - '0'
                                   : c >= 'a' && c <= 'f'
                      ? c - 'a' + 10
                      : c >= 'A' && c <= 'F' ? c - 'A' + 10 : 0);
        };
        wchar_t replWide = 0;
        replWide += hexVal(prettyError[pos + 2]) << 12;
        replWide += hexVal(prettyError[pos + 3]) << 8;
        replWide += hexVal(prettyError[pos + 4]) << 4;
        replWide += hexVal(prettyError[pos + 5]);
        std::string repl =
            Microsoft::Common::Unicode::Utf16ToUtf8(&replWide, 1);

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

void UwpReactInstance::SetXamlViewCreatedTestHook(
    std::function<void(react::uwp::XamlView)> testHook) {
  m_xamlViewCreatedTestHook = testHook;
}

void UwpReactInstance::CallXamlViewCreatedTestHook(react::uwp::XamlView view) {
  if (m_xamlViewCreatedTestHook != nullptr) {
    m_xamlViewCreatedTestHook(view);
  }
}

#if defined(USE_V8)
std::string UwpReactInstance::getApplicationLocalFolder() {
  auto local =
      winrt::Windows::Storage::ApplicationData::Current().LocalFolder().Path();

  return std::wstring_convert<std::codecvt_utf8<wchar_t>>().to_bytes(
             std::wstring(local.c_str(), local.size())) +
      "\\";
}
#endif

} // namespace uwp
} // namespace react
