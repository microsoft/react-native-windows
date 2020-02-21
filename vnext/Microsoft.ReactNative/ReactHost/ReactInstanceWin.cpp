// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "ReactInstanceWin.h"
#include "MoveOnCopy.h"
#include "MsoUtils.h"

#include <Base/CoreNativeModules.h>
#include <ReactUWP/CreateUwpModules.h>
#include <ReactUWP/Modules/I18nModule.h>
#include <Threading/MessageDispatchQueue.h>
#include "ReactErrorProvider.h"

#include "Microsoft.ReactNative/Threading/MessageQueueThreadFactory.h"

#include "Unicode.h"

#include "JSExceptionCallbackFactory.h"

#include <ReactWindowsCore/ViewManager.h>
#include <dispatchQueue/dispatchQueue.h>
#include "Modules/AppStateData.h"

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

namespace react::uwp {

void AddStandardViewManagers(
    std::vector<std::unique_ptr<facebook::react::IViewManager>> &viewManagers,
    std::shared_ptr<IReactInstance> const &instance) noexcept;

void AddPolyesterViewManagers(
    std::vector<std::unique_ptr<facebook::react::IViewManager>> &viewManagers,
    std::shared_ptr<IReactInstance> const &instance) noexcept;

std::shared_ptr<facebook::react::IUIManager> CreateUIManager2(
    std::vector<react::uwp::NativeViewManager> &&viewManagers) noexcept;

} // namespace react::uwp

namespace Mso::React {

//=============================================================================================
// ReactContext implementation
//=============================================================================================

ReactContext::ReactContext(Mso::WeakPtr<ReactInstanceWin> &&reactInstance) noexcept
    : m_reactInstance{std::move(reactInstance)} {}

void ReactContext::CallJSFunction(std::string &&module, std::string &&method, folly::dynamic &&params) noexcept {
  if (auto instance = m_reactInstance.GetStrongPtr()) {
    if (instance->State() == ReactInstanceState::Loaded) {
      if (auto fbInstance = instance->GetInnerInstance()) {
        fbInstance->callJSFunction(std::move(module), std::move(method), std::move(params));
      }
    }
  }
}

void ReactContext::DispatchEvent(int64_t viewTag, std::string &&eventName, folly::dynamic &&eventData) noexcept {
  if (auto instance = m_reactInstance.GetStrongPtr()) {
    if (instance->State() == ReactInstanceState::Loaded) {
      instance->DispatchEvent(viewTag, std::move(eventName), std::move(eventData));
    }
  }
}

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
    ReactOptions &&options,
    Mso::Promise<void> &&whenCreated,
    Mso::Promise<void> &&whenLoaded,
    Mso::VoidFunctor &&updateUI) noexcept
    : Super{reactHost.NativeQueue()},
      m_weakReactHost{&reactHost},
      m_options{std::move(options)},
      m_whenCreated{std::move(whenCreated)},
      m_whenLoaded{std::move(whenLoaded)},
      m_updateUI{std::move(updateUI)},
      m_reactContext{Mso::Make<ReactContext>(this)},
      m_legacyInstance{std::make_shared<react::uwp::UwpReactInstanceProxy>(
          Mso::WeakPtr<Mso::React::IReactInstance>{this},
          Mso::Copy(options.LegacySettings))} {
  m_whenCreated.SetValue();
}

ReactInstanceWin::~ReactInstanceWin() noexcept {}

//! Initialize() is called from the native queue.
void ReactInstanceWin::Initialize() noexcept {
  InitJSMessageThread();
  InitNativeMessageThread();
  InitUIMessageThread();

  m_legacyReactInstance =
      std::make_shared<react::uwp::UwpReactInstanceProxy>(this, Mso::Copy(m_options.LegacySettings));

  // InitUIManager uses m_legacyReactInstance
  InitUIManager();

  Mso::PostFuture(
      Mso::DispatchQueue::MainUIQueue(),
      [weakThis = Mso::WeakPtr{this}]() noexcept {
        // Objects that must be created on the UI thread
        if (auto strongThis = weakThis.GetStrongPtr()) {
          auto const &legacyInstance = strongThis->m_legacyReactInstance;
          strongThis->m_deviceInfo = std::make_shared<react::uwp::DeviceInfo>(legacyInstance);
          strongThis->m_appTheme =
              std::make_shared<react::uwp::AppTheme>(legacyInstance, strongThis->m_uiMessageThread.LoadWithLock());
          strongThis->m_i18nInfo = react::uwp::I18nModule::GetI18nInfo();
        }
      })
      .Then(Queue(), [ this, weakThis = Mso::WeakPtr{this} ]() noexcept {
        if (auto strongThis = weakThis.GetStrongPtr()) {
          // auto cxxModulesProviders = GetCxxModuleProviders();

          auto devSettings = std::make_shared<facebook::react::DevSettings>();
          devSettings->useJITCompilation = m_options.EnableJITCompilation;
          devSettings->debugHost = GetDebugHost();
          devSettings->debugBundlePath = m_options.DeveloperSettings.SourceBundlePath;
          devSettings->liveReloadCallback = GetLiveReloadCallback();
          devSettings->errorCallback = GetErrorCallback();
          devSettings->loggingCallback = GetLoggingCallback();
          devSettings->jsExceptionCallback = GetJSExceptionCallback();
          devSettings->useDirectDebugger = m_options.DeveloperSettings.UseDirectDebugger;
          devSettings->debuggerBreakOnNextLine = m_options.DeveloperSettings.DebuggerBreakOnNextLine;
          devSettings->debuggerPort = m_options.DeveloperSettings.DebuggerPort;
          devSettings->debuggerRuntimeName = m_options.DeveloperSettings.DebuggerRuntimeName;
          devSettings->useWebDebugger = m_options.DeveloperSettings.UseWebDebugger;
          // devSettings->memoryTracker = GetMemoryTracker();
          devSettings->bundleRootPath =
              m_options.BundleRootPath.empty() ? "ms-appx:///Bundle/" : m_options.BundleRootPath;
          m_bundleRootPath = devSettings->bundleRootPath;

          devSettings->waitingForDebuggerCallback = GetWaitingForDebuggerCallback();
          devSettings->debuggerAttachCallback = GetDebuggerAttachCallback();

          // Now that ReactNativeWindows is building outside devmain, it is missing
          // fix given by PR https://github.com/microsoft/react-native-windows/pull/2624 causing
          // regression. We're turning off console redirection till the fix is available in devmain.
          // Bug https://office.visualstudio.com/DefaultCollection/OC/_workitems/edit/3441551 is tracking this
          devSettings->debuggerConsoleRedirection =
              false; // JSHost::ChangeGate::ChakraCoreDebuggerConsoleRedirection();

          m_appState = std::make_shared<react::uwp::AppState2>(*m_reactContext);

          // Acquire default modules and then populate with custom modules
          std::vector<facebook::react::NativeModuleDescription> cxxModules = react::uwp::GetCoreModules(
              m_uiManager.Load(),
              m_batchingUIThread,
              m_uiMessageThread.Load(),
              m_deviceInfo,
              devSettings,
              std::move(m_i18nInfo),
              std::move(m_appState),
              std::move(m_appTheme),
              std::weak_ptr{m_legacyReactInstance});

          if (m_options.ModuleProvider != nullptr) {
            std::vector<facebook::react::NativeModuleDescription> customCxxModules =
                m_options.ModuleProvider->GetModules(m_reactContext, m_batchingUIThread);
            cxxModules.insert(std::end(cxxModules), std::begin(customCxxModules), std::end(customCxxModules));
          }

#ifdef PATCH_RN
          if (m_options.LegacySettings.UseJsi) {
            std::unique_ptr<facebook::jsi::ScriptStore> scriptStore = nullptr;
            std::unique_ptr<facebook::jsi::PreparedScriptStore> preparedScriptStore = nullptr;

#if defined(USE_HERMES)
            devSettings->jsiRuntimeHolder = std::make_shared<facebook::react::HermesRuntimeHolder>();
#elif defined(USE_V8)
            preparedScriptStore =
                std::make_unique<facebook::react::BasePreparedScriptStoreImpl>(getApplicationLocalFolder());

            devSettings->jsiRuntimeHolder = std::make_shared<facebook::react::V8JSIRuntimeHolder>(
                devSettings, jsQueue, std::move(scriptStore), std::move(preparedScriptStore));
#else
            if (m_options.LegacySettings.EnableByteCodeCaching || !m_options.LegacySettings.ByteCodeFileUri.empty()) {
              scriptStore = std::make_unique<react::uwp::UwpScriptStore>();
              preparedScriptStore = std::make_unique<react::uwp::UwpPreparedScriptStore>(
                  winrt::to_hstring(m_options.LegacySettings.ByteCodeFileUri));
            }
            devSettings->jsiRuntimeHolder = std::make_shared<Microsoft::JSI::ChakraRuntimeHolder>(
                devSettings, m_jsMessageThread.Load(), std::move(scriptStore), std::move(preparedScriptStore));
#endif
          }
#endif

          try {
            // We need to keep the instance wrapper alive as its destruction shuts down the native queue.
            auto instanceWrapper = facebook::react::CreateReactInstance(
                std::string(), // bundleRootPath
                std::move(cxxModules),
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

  if (!m_options.DeveloperSettings.UseWebDebugger) {
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
          strongThis->OnReactInstanceLoaded(Mso::ExceptionErrorProvider().MakeErrorCode(std::current_exception()));
          return;
        }

        // All JS bundles successfully loaded.
        strongThis->OnReactInstanceLoaded(Mso::ErrorCode{});
      }
    });
  } else {
    // Web Debugging
    auto instanceWrapper = m_instanceWrapper.LoadWithLock();
    instanceWrapper->loadBundle(Mso::Copy(m_options.Identity));

    m_jsMessageThread.Load()->runOnQueue([
      weakThis = Mso::WeakPtr{this},
      loadCallbackGuard = Mso::MakeMoveOnCopyWrapper(LoadedCallbackGuard{*this})
    ]() noexcept {
      if (auto strongThis = weakThis.GetStrongPtr()) {
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
          strongThis->m_state = ReactInstanceState::Loaded;
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

std::string ReactInstanceWin::LastErrorMessage() const noexcept {
  std::lock_guard lock{m_mutex};
  return m_errorMessage;
}

void ReactInstanceWin::InitJSMessageThread() noexcept {
  // Use the explicit JSQueue if it is provided.
  const auto &properties = m_options.Properties;
  auto jsDispatchQueue = Mso::DispatchQueue{properties.Get(JSDispatchQueueProperty)};
  if (jsDispatchQueue) {
    VerifyElseCrashSz(jsDispatchQueue.IsSerial(), "JS Queue must be sequential");
  } else {
    // Currently we have to use Looper DispatchQueue because our JS Engine based on Chakra uses thread local storage.
    jsDispatchQueue = Mso::DispatchQueue::MakeLooperQueue();
  }

  // Create MessageQueueThread for the DispatchQueue
  VerifyElseCrashSz(jsDispatchQueue, "m_jsDispatchQueue must not be null");
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
  m_uiMessageThread.Exchange(std::make_shared<MessageDispatchQueue>(
      Mso::DispatchQueue::MainUIQueue(), Mso::MakeWeakMemberFunctor(this, &ReactInstanceWin::OnError)));

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

  auto uiManager = react::uwp::CreateUIManager2(std::move(viewManagers));
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

std::function<void(facebook::react::JSExceptionInfo &&)> ReactInstanceWin::GetJSExceptionCallback() noexcept {
  if (m_options.OnJSException) {
    return CreateExceptionCallback(Mso::Copy(m_options.OnJSException));
  }

  return {};
}

std::function<void()> ReactInstanceWin::GetLiveReloadCallback() noexcept {
  // Live reload is enabled if we provide a callback function.
  if (m_options.DeveloperSettings.UseLiveReload) {
    return Mso::MakeWeakMemberStdFunction(this, &ReactInstanceWin::OnLiveReload);
  }
  return std::function<void()>{};
}

std::string ReactInstanceWin::GetDebugHost() noexcept {
  std::string debugHost;
  const ReactDevOptions &devOptions = m_options.DeveloperSettings;
  if (!devOptions.DebugHost.empty()) {
    debugHost = devOptions.DebugHost;
  } else {
    debugHost = !devOptions.SourceBundleHost.empty() ? devOptions.SourceBundleHost : "localhost";
    debugHost.append(":");
    debugHost.append(!devOptions.SourceBundlePort.empty() ? devOptions.SourceBundlePort : "8081");
  }
  return debugHost;
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

void ReactInstanceWin::OnErrorWithMessage(const std::string &errorMessage) noexcept {
  m_state = ReactInstanceState::HasError;

  // Append new error message onto others
  if (!m_errorMessage.empty())
    m_errorMessage += "\n";
  m_errorMessage += " -- ";
  m_errorMessage += PrettyError(errorMessage);

  OutputDebugStringA("ReactInstance Error Hit ...\n");
  OutputDebugStringA(m_errorMessage.c_str());
  OutputDebugStringA("\n");

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
  if (m_options.DeveloperSettings.UseWebDebugger) {
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
  if (m_options.DeveloperSettings.UseWebDebugger) {
    return Mso::MakeWeakMemberStdFunction(this, &ReactInstanceWin::OnDebuggerAttach);
  }

  return {};
}

void ReactInstanceWin::OnDebuggerAttach() noexcept {
  m_updateUI();
}

void ReactInstanceWin::CallJsFunction(
    std::string &&moduleName,
    std::string &&method,
    folly::dynamic &&params) noexcept {
  // callJSFunction can be called from any thread. The native bridge will post the call to the right queue internally.
  if (m_state == ReactInstanceState::Loaded) {
    if (auto instance = m_instance.LoadWithLock()) {
      instance->callJSFunction(std::move(moduleName), std::move(method), std::move(params));
    }
  }
}

void ReactInstanceWin::DispatchEvent(int64_t viewTag, std::string &&eventName, folly::dynamic &&eventData) noexcept {
  if (m_state == ReactInstanceState::Loaded) {
    if (auto instance = m_instanceWrapper.LoadWithLock()) {
      instance->DispatchEvent(viewTag, eventName, std::move(eventData));
    }
  }
}

facebook::react::INativeUIManager *ReactInstanceWin::NativeUIManager() noexcept {
  return m_uiManager.LoadWithLock()->getNativeUIManager();
}

std::shared_ptr<facebook::react::Instance> ReactInstanceWin::GetInnerInstance() noexcept {
  return m_instance.LoadWithLock();
}

std::string ReactInstanceWin::GetBundleRootPath() noexcept {
  return m_options.LegacySettings.BundleRootPath;
}

std::shared_ptr<react::uwp::IReactInstance> ReactInstanceWin::UwpReactInstance() noexcept {
  return m_legacyInstance;
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

} // namespace Mso::React
