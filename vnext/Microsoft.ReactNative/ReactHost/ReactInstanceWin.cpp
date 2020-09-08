// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "ReactInstanceWin.h"
#include "MoveOnCopy.h"
#include "MsoUtils.h"

#include <Base/CoreNativeModules.h>
#include <IUIManager.h>
#include <Threading/MessageDispatchQueue.h>
#include <XamlUIService.h>
#include "ReactErrorProvider.h"

#include "Microsoft.ReactNative/IReactNotificationService.h"
#include "Microsoft.ReactNative/Threading/MessageQueueThreadFactory.h"

#include "../../codegen/NativeAppStateSpec.g.h"
#include "../../codegen/NativeClipboardSpec.g.h"
#include "../../codegen/NativeDevSettingsSpec.g.h"
#include "../../codegen/NativeDeviceInfoSpec.g.h"
#include "../../codegen/NativeI18nManagerSpec.g.h"
#include "../../codegen/NativeLogBoxSpec.g.h"
#include "NativeModules.h"
#include "NativeModulesProvider.h"
#include "Unicode.h"

#include <Shared/DevServerHelper.h>
#include <Shared/ViewManager.h>
#include <dispatchQueue/dispatchQueue.h>
#include "DevMenu.h"
#include "IReactContext.h"
#include "IReactDispatcher.h"
#include "Modules/AlertModule.h"
#include "Modules/AppStateModule.h"
#include "Modules/ClipboardModule.h"
#include "Modules/DevSettingsModule.h"
#include "Modules/DeviceInfoModule.h"
#include "Modules/I18nManagerModule.h"
#include "Modules/LogBoxModule.h"

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

#include "RedBox.h"

#include <tuple>
#include "ChakraRuntimeHolder.h"

namespace react::uwp {

void AddStandardViewManagers(
    std::vector<std::unique_ptr<facebook::react::IViewManager>> &viewManagers,
    std::shared_ptr<IReactInstance> const &instance) noexcept;

void AddPolyesterViewManagers(
    std::vector<std::unique_ptr<facebook::react::IViewManager>> &viewManagers,
    std::shared_ptr<IReactInstance> const &instance) noexcept;

std::shared_ptr<facebook::react::IUIManager> CreateUIManager2(
    Mso::React::IReactContext *context,
    std::vector<react::uwp::NativeViewManager> &&viewManagers) noexcept;

} // namespace react::uwp

using namespace winrt::Microsoft::ReactNative;

namespace Mso::React {

//=============================================================================================
// LoadedCallbackGuard ensures that the OnReactInstanceLoaded is always called.
// It calls OnReactInstanceLoaded in destructor with a cancellation error.
// If loading was previously succeeded this call with an error code is ignored.
//=============================================================================================

struct LoadedCallbackGuard {
  LoadedCallbackGuard(ReactInstanceWin &reactInstance) noexcept : m_reactInstance{&reactInstance} {}

  LoadedCallbackGuard(const LoadedCallbackGuard &other) = delete;
  LoadedCallbackGuard &operator=(const LoadedCallbackGuard &other) = delete;

  LoadedCallbackGuard(LoadedCallbackGuard &&other) = default;
  LoadedCallbackGuard &operator=(LoadedCallbackGuard &&other) = default;

  ~LoadedCallbackGuard() noexcept {
    if (m_reactInstance) {
      m_reactInstance->OnReactInstanceLoaded(Mso::CancellationErrorProvider().MakeErrorCode(true));
    }
  }

 private:
  Mso::CntPtr<ReactInstanceWin> m_reactInstance;
};

//=============================================================================================
// ReactInstanceWin implementation
//=============================================================================================

ReactInstanceWin::ReactInstanceWin(
    IReactHost &reactHost,
    ReactOptions const &options,
    Mso::Promise<void> &&whenCreated,
    Mso::Promise<void> &&whenLoaded,
    Mso::VoidFunctor &&updateUI) noexcept
    : Super{reactHost.NativeQueue()},
      m_weakReactHost{&reactHost},
      m_options{options},
      m_whenCreated{std::move(whenCreated)},
      m_whenLoaded{std::move(whenLoaded)},
      m_isFastReloadEnabled(options.UseFastRefresh()),
      m_isLiveReloadEnabled(options.UseLiveReload()),
      m_updateUI{std::move(updateUI)},
      m_useWebDebugger(options.UseWebDebugger()),
      m_useDirectDebugger(options.UseDirectDebugger()),
      m_debuggerBreakOnNextLine(options.DebuggerBreakOnNextLine()),
      m_reactContext{Mso::Make<ReactContext>(
          this,
          options.Properties,
          winrt::make<implementation::ReactNotificationService>(options.Notifications))},
      m_legacyInstance{
          std::make_shared<react::uwp::UwpReactInstanceProxy>(Mso::WeakPtr<Mso::React::IReactInstance>{this})} {
  m_whenCreated.SetValue();
}

ReactInstanceWin::~ReactInstanceWin() noexcept {}

void ReactInstanceWin::LoadModules(
    const std::shared_ptr<winrt::Microsoft::ReactNative::NativeModulesProvider> &nativeModulesProvider,
    const std::shared_ptr<winrt::Microsoft::ReactNative::TurboModulesProvider> &turboModulesProvider) noexcept {
  auto registerNativeModule = [&nativeModulesProvider](
      const wchar_t *name, const ReactModuleProvider &provider) noexcept {
    nativeModulesProvider->AddModuleProvider(name, provider);
  };

  auto registerTurboModule = [ this, &nativeModulesProvider, &turboModulesProvider ](
      const wchar_t *name, const ReactModuleProvider &provider) noexcept {
    if (m_options.UseWebDebugger()) {
      nativeModulesProvider->AddModuleProvider(name, provider);
    } else {
      turboModulesProvider->AddModuleProvider(name, provider);
    }
  };

  registerTurboModule(L"Alert", winrt::Microsoft::ReactNative::MakeModuleProvider<::Microsoft::ReactNative::Alert>());

  registerTurboModule(
      L"AppState",
      winrt::Microsoft::ReactNative::
          MakeTurboModuleProvider<::Microsoft::ReactNative::AppState, ::Microsoft::ReactNativeSpecs::AppStateSpec>());

  registerTurboModule(
      L"LogBox",
      winrt::Microsoft::ReactNative::
          MakeTurboModuleProvider<::Microsoft::ReactNative::LogBox, ::Microsoft::ReactNativeSpecs::LogBoxSpec>());

  registerTurboModule(
      L"Clipboard",
      winrt::Microsoft::ReactNative::
          MakeTurboModuleProvider<::Microsoft::ReactNative::Clipboard, ::Microsoft::ReactNativeSpecs::ClipboardSpec>());

  registerTurboModule(
      L"DeviceInfo",
      winrt::Microsoft::ReactNative::MakeTurboModuleProvider<
          ::Microsoft::ReactNative::DeviceInfo,
          ::Microsoft::ReactNativeSpecs::DeviceInfoSpec>());

  registerTurboModule(
      L"DevSettings",
      winrt::Microsoft::ReactNative::MakeTurboModuleProvider<
          ::Microsoft::ReactNative::DevSettings,
          ::Microsoft::ReactNativeSpecs::DevSettingsSpec>());

  registerTurboModule(
      L"I18nManager",
      winrt::Microsoft::ReactNative::MakeTurboModuleProvider<
          ::Microsoft::ReactNative::I18nManager,
          ::Microsoft::ReactNativeSpecs::I18nManagerSpec>());
}

//! Initialize() is called from the native queue.
void ReactInstanceWin::Initialize() noexcept {
  InitJSMessageThread();
  InitNativeMessageThread();
  InitUIMessageThread();

  m_legacyReactInstance = std::make_shared<react::uwp::UwpReactInstanceProxy>(this);

  // InitUIManager uses m_legacyReactInstance
  InitUIManager();

  Microsoft::ReactNative::DevMenuManager::InitDevMenu(m_reactContext);

  Mso::PostFuture(
      m_uiQueue,
      [weakThis = Mso::WeakPtr{this}]() noexcept {
        // Objects that must be created on the UI thread
        if (auto strongThis = weakThis.GetStrongPtr()) {
          auto const &legacyInstance = strongThis->m_legacyReactInstance;
          strongThis->m_appTheme =
              std::make_shared<react::uwp::AppTheme>(legacyInstance, strongThis->m_uiMessageThread.LoadWithLock());
          Microsoft::ReactNative::I18nManager::InitI18nInfo(
              winrt::Microsoft::ReactNative::ReactPropertyBag(strongThis->Options().Properties));
          strongThis->m_appearanceListener =
              Mso::Make<react::uwp::AppearanceChangeListener>(legacyInstance, strongThis->m_uiQueue);
          Microsoft::ReactNative::DeviceInfoHolder::InitDeviceInfoHolder(
              winrt::Microsoft::ReactNative::ReactPropertyBag(strongThis->Options().Properties));
        }
      })
      .Then(Queue(), [ this, weakThis = Mso::WeakPtr{this} ]() noexcept {
        if (auto strongThis = weakThis.GetStrongPtr()) {
          // auto cxxModulesProviders = GetCxxModuleProviders();

          auto devSettings = std::make_shared<facebook::react::DevSettings>();
          devSettings->useJITCompilation = m_options.EnableJITCompilation;
          devSettings->sourceBundleHost = m_options.DeveloperSettings.SourceBundleHost.empty()
              ? facebook::react::DevServerHelper::DefaultPackagerHost
              : m_options.DeveloperSettings.SourceBundleHost;
          devSettings->sourceBundlePort = m_options.DeveloperSettings.SourceBundlePort
              ? m_options.DeveloperSettings.SourceBundlePort
              : facebook::react::DevServerHelper::DefaultPackagerPort;
          devSettings->debugBundlePath = m_options.DeveloperSettings.SourceBundleName.empty()
              ? m_options.Identity
              : m_options.DeveloperSettings.SourceBundleName;
          devSettings->liveReloadCallback = GetLiveReloadCallback();
          devSettings->errorCallback = GetErrorCallback();
          devSettings->loggingCallback = GetLoggingCallback();
          m_redboxHandler = devSettings->redboxHandler = std::move(GetRedBoxHandler());
          devSettings->useDirectDebugger = m_useDirectDebugger;
          devSettings->debuggerBreakOnNextLine = m_debuggerBreakOnNextLine;
          devSettings->debuggerPort = m_options.DeveloperSettings.DebuggerPort;
          devSettings->debuggerRuntimeName = m_options.DeveloperSettings.DebuggerRuntimeName;
          devSettings->useWebDebugger = m_useWebDebugger;
          devSettings->useFastRefresh = m_isFastReloadEnabled;
          // devSettings->memoryTracker = GetMemoryTracker();
          devSettings->bundleRootPath =
              m_options.BundleRootPath.empty() ? "ms-appx:///Bundle/" : m_options.BundleRootPath;
          m_bundleRootPath = devSettings->bundleRootPath;

          devSettings->waitingForDebuggerCallback = GetWaitingForDebuggerCallback();
          devSettings->debuggerAttachCallback = GetDebuggerAttachCallback();
          devSettings->showDevMenuCallback = [weakThis]() noexcept {
            if (auto strongThis = weakThis.GetStrongPtr()) {
              strongThis->m_uiQueue.Post([context = strongThis->m_reactContext]() {
                Microsoft::ReactNative::DevMenuManager::Show(context->Properties());
              });
            }
          };

          // Now that ReactNativeWindows is building outside devmain, it is missing
          // fix given by PR https://github.com/microsoft/react-native-windows/pull/2624 causing
          // regression. We're turning off console redirection till the fix is available in devmain.
          // Bug https://office.visualstudio.com/DefaultCollection/OC/_workitems/edit/3441551 is tracking this
          devSettings->debuggerConsoleRedirection =
              false; // JSHost::ChangeGate::ChakraCoreDebuggerConsoleRedirection();

          // Acquire default modules and then populate with custom modules
          std::vector<facebook::react::NativeModuleDescription> cxxModules = react::uwp::GetCoreModules(
              m_uiManager.Load(),
              m_batchingUIThread,
              m_uiMessageThread.Load(),
              std::move(m_appTheme),
              std::move(m_appearanceListener),
              m_legacyReactInstance);

          auto nmp = std::make_shared<winrt::Microsoft::ReactNative::NativeModulesProvider>();

          ::Microsoft::ReactNative::DevSettings::SetReload(
              strongThis->Options(), [weakReactHost = m_weakReactHost]() noexcept {
                if (auto reactHost = weakReactHost.GetStrongPtr()) {
                  reactHost->ReloadInstance();
                }
              });
          LoadModules(nmp, m_options.TurboModuleProvider);

          auto modules = nmp->GetModules(m_reactContext, m_jsMessageThread.Load());
          cxxModules.insert(
              cxxModules.end(), std::make_move_iterator(modules.begin()), std::make_move_iterator(modules.end()));

          if (m_options.ModuleProvider != nullptr) {
            std::vector<facebook::react::NativeModuleDescription> customCxxModules =
                m_options.ModuleProvider->GetModules(m_reactContext, m_batchingUIThread);
            cxxModules.insert(std::end(cxxModules), std::begin(customCxxModules), std::end(customCxxModules));
          }

          if (m_options.UseJsi) {
            std::unique_ptr<facebook::jsi::ScriptStore> scriptStore = nullptr;
            std::unique_ptr<facebook::jsi::PreparedScriptStore> preparedScriptStore = nullptr;

            switch (m_options.JsiEngine) {
              case react::uwp::JSIEngine::Hermes:
#if defined(USE_HERMES)
                devSettings->jsiRuntimeHolder = std::make_shared<facebook::react::HermesRuntimeHolder>();
                devSettings->inlineSourceMap = false;
                break;
#endif
              case react::uwp::JSIEngine::V8:
#if defined(USE_V8)
                preparedScriptStore =
                    std::make_unique<facebook::react::BasePreparedScriptStoreImpl>(getApplicationLocalFolder());

                devSettings->jsiRuntimeHolder = std::make_shared<facebook::react::V8JSIRuntimeHolder>(
                    devSettings, m_jsMessageThread.Load(), std::move(scriptStore), std::move(preparedScriptStore));
                break;
#endif
              case react::uwp::JSIEngine::Chakra:
                if (m_options.EnableByteCodeCaching || !m_options.ByteCodeFileUri.empty()) {
                  scriptStore = std::make_unique<react::uwp::UwpScriptStore>();
                  preparedScriptStore = std::make_unique<react::uwp::UwpPreparedScriptStore>(
                      winrt::to_hstring(m_options.ByteCodeFileUri));
                }
                devSettings->jsiRuntimeHolder = std::make_shared<Microsoft::JSI::ChakraRuntimeHolder>(
                    devSettings, m_jsMessageThread.Load(), std::move(scriptStore), std::move(preparedScriptStore));
                break;
            }
          }

          try {
            // We need to keep the instance wrapper alive as its destruction shuts down the native queue.
            m_options.TurboModuleProvider->SetReactContext(
                winrt::make<implementation::ReactContext>(Mso::Copy(m_reactContext)));
            auto instanceWrapper = facebook::react::CreateReactInstance(
                std::string(), // bundleRootPath
                std::move(cxxModules),
                m_options.TurboModuleProvider,
                m_uiManager.Load(),
                m_jsMessageThread.Load(),
                Mso::Copy(m_batchingUIThread),
                std::move(devSettings));

            m_instance.Exchange(Mso::Copy(instanceWrapper->GetInstance()));
            m_instanceWrapper.Exchange(std::move(instanceWrapper));

            if (auto onCreated = m_options.OnInstanceCreated.Get()) {
              onCreated->Invoke(*this);
            }

            LoadJSBundles();

            if (m_options.UseDeveloperSupport() && State() != ReactInstanceState::HasError) {
              folly::dynamic params = folly::dynamic::array(
                  STRING(RN_PLATFORM),
                  m_options.DeveloperSettings.SourceBundleName.empty() ? m_options.Identity
                                                                       : m_options.DeveloperSettings.SourceBundleName,
                  m_options.DeveloperSettings.SourceBundleHost.empty()
                      ? facebook::react::DevServerHelper::DefaultPackagerHost
                      : m_options.DeveloperSettings.SourceBundleHost,
                  m_options.DeveloperSettings.SourceBundlePort ? m_options.DeveloperSettings.SourceBundlePort
                                                               : facebook::react::DevServerHelper::DefaultPackagerPort,
                  m_isFastReloadEnabled);
              m_instance.Load()->callJSFunction("HMRClient", "setup", std::move(params));
            }

          } catch (std::exception &e) {
            OnErrorWithMessage(e.what());
            OnErrorWithMessage("UwpReactInstance: Failed to create React Instance.");
          } catch (winrt::hresult_error const &e) {
            OnErrorWithMessage(Microsoft::Common::Unicode::Utf16ToUtf8(e.message().c_str(), e.message().size()));
            OnErrorWithMessage("UwpReactInstance: Failed to create React Instance.");
          } catch (...) {
            OnErrorWithMessage("UwpReactInstance: Failed to create React Instance.");
          }
        }
      });
}

void ReactInstanceWin::LoadJSBundles() noexcept {
  //
  // We use m_jsMessageThread to load JS bundles synchronously. In that case we only load
  // them if the m_jsMessageThread is not shut down (quitSynchronous() is not called).
  // After the load we call OnReactInstanceLoaded callback on native queue.
  //
  // Note that the instance could be destroyed while we are loading JS Bundles.
  // Though, the JS engine is not destroyed until this work item is not finished.
  // Thus, we check the m_isDestroyed flag to see if we should do an early exit.
  // Also, since we have to guarantee that the OnReactInstanceLoaded callback is called before
  // the OnReactInstanceDestroyed callback, the OnReactInstanceLoaded is called right before the
  // OnReactInstanceDestroyed callback in the Destroy() method. In that case any OnReactInstanceLoaded
  // calls after we finish this JS message queue work item is ignored.
  //
  // The LoadedCallbackGuard is used for the case when runOnQueue does not execute the lambda
  // before destroying it. It may happen if the m_jsMessageThread is already shutdown.
  // In that case, the LoadedCallbackGuard notifies about cancellation by calling OnReactInstanceLoaded.
  // The OnReactInstanceLoaded internally only accepts the first call and ignores others.
  //

  if (m_useWebDebugger || m_isFastReloadEnabled) {
    // Getting bundle from the packager, so do everything async.
    auto instanceWrapper = m_instanceWrapper.LoadWithLock();
    instanceWrapper->loadBundle(Mso::Copy(m_options.Identity));

    m_jsMessageThread.Load()->runOnQueue([
      weakThis = Mso::WeakPtr{this},
      loadCallbackGuard = Mso::MakeMoveOnCopyWrapper(LoadedCallbackGuard{*this})
    ]() noexcept {
      if (auto strongThis = weakThis.GetStrongPtr()) {
        if (strongThis->State() != ReactInstanceState::HasError) {
          strongThis->OnReactInstanceLoaded(Mso::ErrorCode{});
        }
      }
    });
  } else {
    m_jsMessageThread.Load()->runOnQueue([
      weakThis = Mso::WeakPtr{this},
      loadCallbackGuard = Mso::MakeMoveOnCopyWrapper(LoadedCallbackGuard{*this})
    ]() noexcept {
      if (auto strongThis = weakThis.GetStrongPtr()) {
        auto instance = strongThis->m_instance.LoadWithLock();
        auto instanceWrapper = strongThis->m_instanceWrapper.LoadWithLock();
        if (!instance || !instanceWrapper) {
          return;
        }

        auto &options = strongThis->m_options;

        try {
          instanceWrapper->loadBundleSync(Mso::Copy(options.Identity));
        } catch (...) {
          strongThis->m_state = ReactInstanceState::HasError;
          strongThis->AbandonJSCallQueue();
          strongThis->OnReactInstanceLoaded(Mso::ExceptionErrorProvider().MakeErrorCode(std::current_exception()));
          return;
        }

        // All JS bundles successfully loaded.
        strongThis->OnReactInstanceLoaded(Mso::ErrorCode{});
      }
    });
  }
}

void ReactInstanceWin::OnReactInstanceLoaded(const Mso::ErrorCode &errorCode) noexcept {
  if (!m_isLoaded) {
    Queue().InvokeElsePost([ weakThis = Mso::WeakPtr{this}, errorCode ]() noexcept {
      if (auto strongThis = weakThis.GetStrongPtr()) {
        if (!strongThis->m_isLoaded) {
          strongThis->m_isLoaded = true;
          if (!errorCode) {
            strongThis->m_state = ReactInstanceState::Loaded;
            strongThis->DrainJSCallQueue();
          } else {
            strongThis->m_state = ReactInstanceState::HasError;
            strongThis->AbandonJSCallQueue();
          }

          if (auto onLoaded = strongThis->m_options.OnInstanceLoaded.Get()) {
            onLoaded->Invoke(*strongThis, errorCode);
          }

          strongThis->m_whenLoaded.SetValue();
        }
      }
    });
  }
}

Mso::Future<void> ReactInstanceWin::Destroy() noexcept {
  // This method must be called from the native queue.
  VerifyIsInQueueElseCrash();

  if (m_isDestroyed) {
    return m_whenDestroyed.AsFuture();
  }

  m_isDestroyed = true;
  m_state = ReactInstanceState::Unloaded;
  AbandonJSCallQueue();

  if (!m_isLoaded) {
    OnReactInstanceLoaded(Mso::CancellationErrorProvider().MakeErrorCode(true));
  }

  if (auto onDestroyed = m_options.OnInstanceDestroyed.Get()) {
    onDestroyed->Invoke(*this);
  }

  // Make sure that the instance is not destroyed yet
  if (auto instance = m_instance.Exchange(nullptr)) {
    // Release the message queues before the ui manager and instance.
    m_nativeMessageThread.Exchange(nullptr);
    m_jsMessageThread.Exchange(nullptr);
    m_uiManager.Exchange(nullptr);
    m_instanceWrapper.Exchange(nullptr);
    m_jsDispatchQueue.Exchange(nullptr);
  }

  return m_whenDestroyed.AsFuture();
}

const ReactOptions &ReactInstanceWin::Options() const noexcept {
  return m_options;
}

ReactInstanceState ReactInstanceWin::State() const noexcept {
  return m_state;
}

void ReactInstanceWin::InitJSMessageThread() noexcept {
  auto jsDispatchQueue = Mso::DispatchQueue::MakeLooperQueue();

  // Create MessageQueueThread for the DispatchQueue
  VerifyElseCrashSz(jsDispatchQueue, "m_jsDispatchQueue must not be null");

  auto jsDispatcher =
      winrt::make<winrt::Microsoft::ReactNative::implementation::ReactDispatcher>(Mso::Copy(jsDispatchQueue));
  m_options.Properties.Set(ReactDispatcherHelper::JSDispatcherProperty(), jsDispatcher);

  m_jsMessageThread.Exchange(std::make_shared<MessageDispatchQueue>(
      jsDispatchQueue, Mso::MakeWeakMemberFunctor(this, &ReactInstanceWin::OnError), Mso::Copy(m_whenDestroyed)));
  m_jsDispatchQueue.Exchange(std::move(jsDispatchQueue));
}

void ReactInstanceWin::InitNativeMessageThread() noexcept {
  // Native queue was already given us in constructor.
  m_nativeMessageThread.Exchange(
      std::make_shared<MessageDispatchQueue>(Queue(), Mso::MakeWeakMemberFunctor(this, &ReactInstanceWin::OnError)));
}

void ReactInstanceWin::InitUIMessageThread() noexcept {
  // Native queue was already given us in constructor.
  m_uiQueue = winrt::Microsoft::ReactNative::implementation::ReactDispatcher::GetUIDispatchQueue(m_options.Properties);
  VerifyElseCrashSz(m_uiQueue, "No UI Dispatcher provided");
  m_uiMessageThread.Exchange(
      std::make_shared<MessageDispatchQueue>(m_uiQueue, Mso::MakeWeakMemberFunctor(this, &ReactInstanceWin::OnError)));

  m_batchingUIThread = react::uwp::MakeBatchingQueueThread(m_uiMessageThread.Load());
}

void ReactInstanceWin::InitUIManager() noexcept {
  std::vector<std::unique_ptr<facebook::react::IViewManager>> viewManagers;

  // Custom view managers
  if (m_options.ViewManagerProvider) {
    viewManagers = m_options.ViewManagerProvider->GetViewManagers(m_reactContext, m_legacyReactInstance);
  }

  react::uwp::AddStandardViewManagers(viewManagers, m_legacyReactInstance);
  react::uwp::AddPolyesterViewManagers(viewManagers, m_legacyReactInstance);

  auto uiManager = react::uwp::CreateUIManager2(m_reactContext.Get(), std::move(viewManagers));
  auto wkUIManger = std::weak_ptr<facebook::react::IUIManager>(uiManager);
  m_reactContext->Properties().Set(
      implementation::XamlUIService::XamlUIServiceProperty().Handle(),
      winrt::make<implementation::XamlUIService>(std::move(wkUIManger), m_reactContext));
  m_uiManager.Exchange(std::move(uiManager));
}

facebook::react::NativeLoggingHook ReactInstanceWin::GetLoggingCallback() noexcept {
  if (m_options.OnLogging) {
    return [logCallback = m_options.OnLogging](facebook::react::RCTLogLevel logLevel, const char *message) {
      logCallback(static_cast<LogLevel>(logLevel), message);
    };
  } else {
    // When no logging callback was specified, use a default one in DEBUG builds
#if DEBUG
    return [telemetryTag{m_options.Identity}](facebook::react::RCTLogLevel logLevel, const char *message) {
      std::ostringstream ss;
      ss << "ReactNative ['" << telemetryTag << "'] (";
      switch (logLevel) {
        case facebook::react::RCTLogLevel::Trace:
          ss << "trace";
          break;
        case facebook::react::RCTLogLevel::Info:
          ss << "info";
          break;
        case facebook::react::RCTLogLevel::Warning:
          ss << "warning";
          break;
        case facebook::react::RCTLogLevel::Error:
          ss << "error";
          break;
        case facebook::react::RCTLogLevel::Fatal:
          ss << "fatal";
          break;
      }
      ss << "): '" << message << "'\n";
      OutputDebugStringA(ss.str().c_str());
    };
#else
    return facebook::react::NativeLoggingHook{};
#endif
  }
}

std::shared_ptr<IRedBoxHandler> ReactInstanceWin::GetRedBoxHandler() noexcept {
  if (m_options.RedBoxHandler) {
    return m_options.RedBoxHandler;
  } else if (m_options.UseDeveloperSupport()) {
    auto localWkReactHost = m_weakReactHost;
    return CreateDefaultRedBoxHandler(std::move(localWkReactHost), Mso::Copy(m_uiQueue));
  } else {
    return {};
  }
}

std::function<void()> ReactInstanceWin::GetLiveReloadCallback() noexcept {
  // Live reload is enabled if we provide a callback function.
  if (m_isLiveReloadEnabled || m_isFastReloadEnabled) {
    return Mso::MakeWeakMemberStdFunction(this, &ReactInstanceWin::OnLiveReload);
  }
  return std::function<void()>{};
}

std::string ReactInstanceWin::GetBytecodeFileName() noexcept {
  // use bytecode caching if enabled and not debugging
  // (ChakraCore debugging does not work when bytecode caching is enabled)
  // TODO: implement
  // bool useByteCode = Mso::React::BytecodeOptimizationEnabled() && !m_options.DeveloperSettings.UseDirectDebugger;
  // return useByteCode ? Mso::React::GetBytecodeFilePath(m_options.Identity) : "";
  return "";
}

std::function<void(std::string)> ReactInstanceWin::GetErrorCallback() noexcept {
  return Mso::MakeWeakMemberStdFunction(this, &ReactInstanceWin::OnErrorWithMessage);
}

void ReactInstanceWin::OnErrorWithMessage(const std::string &errorMessage) noexcept {
  m_state = ReactInstanceState::HasError;
  AbandonJSCallQueue();

  if (m_redboxHandler && m_redboxHandler->isDevSupportEnabled()) {
    ErrorInfo errorInfo;
    errorInfo.Message = errorMessage;
    errorInfo.Id = 0;
    m_redboxHandler->showNewError(std::move(errorInfo), ErrorType::Native);
  }

  OnError(Mso::React::ReactErrorProvider().MakeErrorCode(Mso::React::ReactError{errorMessage.c_str()}));
  m_updateUI();
}

void ReactInstanceWin::OnError(const Mso::ErrorCode &errorCode) noexcept {
  InvokeInQueue([ this, errorCode ]() noexcept { m_options.OnError(errorCode); });
}

void ReactInstanceWin::OnLiveReload() noexcept {
  if (auto reactHost = m_weakReactHost.GetStrongPtr()) {
    reactHost->ReloadInstance();
  }
}

std::function<void()> ReactInstanceWin::GetWaitingForDebuggerCallback() noexcept {
  if (m_useWebDebugger) {
    return Mso::MakeWeakMemberStdFunction(this, &ReactInstanceWin::OnWaitingForDebugger);
  }

  return {};
}

void ReactInstanceWin::OnWaitingForDebugger() noexcept {
  auto state = m_state.load();
  while (state == ReactInstanceState::Loading) {
    if (m_state.compare_exchange_weak(state, ReactInstanceState::WaitingForDebugger)) {
      break;
    }
  }

  m_updateUI();
}

std::function<void()> ReactInstanceWin::GetDebuggerAttachCallback() noexcept {
  if (m_useWebDebugger) {
    return Mso::MakeWeakMemberStdFunction(this, &ReactInstanceWin::OnDebuggerAttach);
  }

  return {};
}

void ReactInstanceWin::OnDebuggerAttach() noexcept {
  m_updateUI();
}

void ReactInstanceWin::DrainJSCallQueue() noexcept {
  // Handle all items in the queue one by one.
  for (;;) {
    JSCallEntry entry; // To avoid callJSFunction under the lock
    {
      std::scoped_lock lock{m_mutex};
      if (m_state == ReactInstanceState::Loaded && !m_jsCallQueue.empty()) {
        entry = std::move(m_jsCallQueue.front());
        m_jsCallQueue.pop_front();
      } else {
        break;
      }
    }

    if (auto instance = m_instance.LoadWithLock()) {
      instance->callJSFunction(std::move(entry.ModuleName), std::move(entry.MethodName), std::move(entry.Args));
    }
  }
}

void ReactInstanceWin::AbandonJSCallQueue() noexcept {
  std::deque<JSCallEntry> jsCallQueue; // To avoid destruction under the lock
  {
    std::scoped_lock lock{m_mutex};
    if (m_state == ReactInstanceState::HasError || m_state == ReactInstanceState::Unloaded) {
      jsCallQueue = std::move(m_jsCallQueue);
    }
  }
}

void ReactInstanceWin::CallJsFunction(
    std::string &&moduleName,
    std::string &&method,
    folly::dynamic &&params) noexcept {
  bool shouldCall{false}; // To call callJSFunction outside of lock
  {
    std::scoped_lock lock{m_mutex};
    if (m_state == ReactInstanceState::Loaded && m_jsCallQueue.empty()) {
      shouldCall = true;
    } else if (
        m_state == ReactInstanceState::Loading || m_state == ReactInstanceState::WaitingForDebugger ||
        (m_state == ReactInstanceState::Loaded && !m_jsCallQueue.empty())) {
      m_jsCallQueue.push_back(JSCallEntry{std::move(moduleName), std::move(method), std::move(params)});
    }
    // otherwise ignore the call
  }

  if (shouldCall) {
    if (auto instance = m_instance.LoadWithLock()) {
      instance->callJSFunction(std::move(moduleName), std::move(method), std::move(params));
    }
  }
}

void ReactInstanceWin::DispatchEvent(int64_t viewTag, std::string &&eventName, folly::dynamic &&eventData) noexcept {
  folly::dynamic params = folly::dynamic::array(viewTag, std::move(eventName), std::move(eventData));
  CallJsFunction("RCTEventEmitter", "receiveEvent", std::move(params));
}

facebook::react::INativeUIManager *ReactInstanceWin::NativeUIManager() noexcept {
  if (auto uimanager = m_uiManager.LoadWithLock()) {
    return uimanager->getNativeUIManager();
  }
  return nullptr;
}

std::shared_ptr<facebook::react::Instance> ReactInstanceWin::GetInnerInstance() noexcept {
  return m_instance.LoadWithLock();
}

std::string ReactInstanceWin::GetBundleRootPath() noexcept {
  return m_bundleRootPath.empty() ? m_options.BundleRootPath : m_bundleRootPath;
}

std::shared_ptr<react::uwp::IReactInstance> ReactInstanceWin::UwpReactInstance() noexcept {
  return m_legacyInstance;
}

bool ReactInstanceWin::IsLoaded() const noexcept {
  return m_state == ReactInstanceState::Loaded;
}

void ReactInstanceWin::AttachMeasuredRootView(
    facebook::react::IReactRootView *rootView,
    folly::dynamic &&initialProps) noexcept {
  if (auto instanceWrapper = m_instanceWrapper.LoadWithLock()) {
    instanceWrapper->AttachMeasuredRootView(rootView, std::move(initialProps));
  }
}

void ReactInstanceWin::DetachRootView(facebook::react::IReactRootView *rootView) noexcept {
  if (auto instanceWrapper = m_instanceWrapper.LoadWithLock()) {
    instanceWrapper->DetachRootView(rootView);
  }
}

Mso::CntPtr<IReactInstanceInternal> MakeReactInstance(
    IReactHost &reactHost,
    ReactOptions &&options,
    Mso::Promise<void> &&whenCreated,
    Mso::Promise<void> &&whenLoaded,
    Mso::VoidFunctor &&updateUI) noexcept {
  return Mso::Make<ReactInstanceWin, IReactInstanceInternal>(
      reactHost, std::move(options), std::move(whenCreated), std::move(whenLoaded), std::move(updateUI));
}

#if defined(USE_V8)
std::string ReactInstanceWin::getApplicationLocalFolder() {
  auto local = winrt::Windows::Storage::ApplicationData::Current().LocalFolder().Path();

  return Microsoft::Common::Unicode::Utf16ToUtf8(local.c_str(), local.size()) + "\\";
}
#endif

} // namespace Mso::React
