// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"
#include "ReactNativeHost.h"
#include "ReactNativeHost.g.cpp"

#include "Inspector/ReactInspectorThread.h"
#include "ReactPackageBuilder.h"
#include "RedBox.h"
#include "TurboModulesProvider.h"

#include <future/futureWinRT.h>
#include <jsinspector-modern/InspectorFlags.h>
#include <winrt/Windows.Foundation.Collections.h>
#include "IReactContext.h"
#include "ReactHost/DebuggerNotifications.h"
#include "ReactInstanceSettings.h"

#ifdef USE_FABRIC
#include <Fabric/Composition/Modal/WindowsModalHostViewComponentView.h>
#include <Fabric/WindowsComponentDescriptorRegistry.h>
#include <ReactPackageBuilder.h>
#include <react/renderer/componentregistry/ComponentDescriptorProviderRegistry.h>
#endif

using namespace winrt;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;

#if !defined(CORE_ABI) && !defined(USE_FABRIC)
using namespace xaml;
using namespace xaml::Controls;
#endif

namespace winrt::Microsoft::ReactNative::implementation {

class ModernInspectorHostTargetDelegate : public facebook::react::jsinspector_modern::HostTargetDelegate,
                                          public std::enable_shared_from_this<ModernInspectorHostTargetDelegate> {
 public:
  ModernInspectorHostTargetDelegate(winrt::weak_ref<ReactNative::ReactNativeHost> &&reactNativeHost) noexcept
      : m_reactNativeHost(std::move(reactNativeHost)) {}

  facebook::react::jsinspector_modern::HostTargetMetadata getMetadata() override {
    return {
        .integrationName = "React Native Windows (Host)",
    };
  }

  void onReload(facebook::react::jsinspector_modern::HostTargetDelegate::PageReloadRequest const &request) override {
    if (auto reactNativeHost = m_reactNativeHost.get()) {
      reactNativeHost.ReloadInstance();
    }
  }

  void onSetPausedInDebuggerMessage(
      facebook::react::jsinspector_modern::HostTargetDelegate::OverlaySetPausedInDebuggerMessageRequest const &request)
      override {
    if (auto reactNativeHost = m_reactNativeHost.get()) {
      const auto instanceSettings = reactNativeHost.InstanceSettings();
      if (instanceSettings) {
        if (request.message.has_value()) {
          ::Microsoft::ReactNative::DebuggerNotifications::OnShowDebuggerPausedOverlay(
              instanceSettings.Notifications(), request.message.value(), [weakThis = weak_from_this()]() {
                if (auto strongThis = weakThis.lock()) {
                  if (auto reactNativeHost = strongThis->m_reactNativeHost.get()) {
                    winrt::get_self<ReactNativeHost>(reactNativeHost)->OnDebuggerResume();
                  }
                }
              });
        } else {
          ::Microsoft::ReactNative::DebuggerNotifications::OnHideDebuggerPausedOverlay(
              instanceSettings.Notifications());
        }
      }
    }
  }

 private:
  winrt::weak_ref<ReactNative::ReactNativeHost> m_reactNativeHost;
};

ReactNativeHost::ReactNativeHost() noexcept : m_reactHost{Mso::React::MakeReactHost()} {
#if _DEBUG
  facebook::react::InitializeLogging([](facebook::react::RCTLogLevel /*logLevel*/, const char *message) {
    std::string str = std::string("ReactNative:") + message;
    OutputDebugStringA(str.c_str());
  });
#endif

  auto &inspectorFlags = facebook::react::jsinspector_modern::InspectorFlags::getInstance();
  
  // Debug fusebox flag
  if (inspectorFlags.getFuseboxEnabled()) {
    OutputDebugStringA("ReactNativeHost: Fusebox is ENABLED\n");
  } else {
    OutputDebugStringA("ReactNativeHost: Fusebox is DISABLED - InspectorTarget will be null!\n");
  }
  
  // Debug inspector page ID
  if (!m_inspectorPageId.has_value()) {
    OutputDebugStringA("ReactNativeHost: No existing inspector page ID\n");
  } else {
    OutputDebugStringA("ReactNativeHost: Inspector page ID already exists\n");
  }
  
  if (inspectorFlags.getFuseboxEnabled() && !m_inspectorPageId.has_value()) {
    OutputDebugStringA("ReactNativeHost: Creating inspector target...\n");
    m_inspectorHostDelegate = std::make_shared<ModernInspectorHostTargetDelegate>(*this);
    m_inspectorTarget = facebook::react::jsinspector_modern::HostTarget::create(
        *m_inspectorHostDelegate, [](std::function<void()> &&callback) {
          ::Microsoft::ReactNative::ReactInspectorThread::Instance().Post(
              [callback = std::move(callback)]() { callback(); });
        });

    if (m_inspectorTarget) {
      OutputDebugStringA("ReactNativeHost: Inspector target created successfully\n");
    } else {
      OutputDebugStringA("ReactNativeHost: Failed to create inspector target!\n");
    }

    std::weak_ptr<facebook::react::jsinspector_modern::HostTarget> weakInspectorTarget = m_inspectorTarget;
    facebook::react::jsinspector_modern::InspectorTargetCapabilities capabilities;
    capabilities.nativePageReloads = true;
    capabilities.prefersFuseboxFrontend = true;
    m_inspectorPageId = facebook::react::jsinspector_modern::getInspectorInstance().addPage(
        "React Native Windows (Experimental)",
        /* vm */ "",
        [weakInspectorTarget](std::unique_ptr<facebook::react::jsinspector_modern::IRemoteConnection> remote)
            -> std::unique_ptr<facebook::react::jsinspector_modern::ILocalConnection> {
          if (const auto inspectorTarget = weakInspectorTarget.lock()) {
            // facebook::react::jsinspector_modern::HostTarget::SessionMetadata sessionMetadata;
            // sessionMetadata.integrationName = "React Native Windows (Host)";
            // return inspectorTarget->connect(std::move(remote), sessionMetadata);
            return inspectorTarget->connect(std::move(remote));
          }

          // This can happen if we're about to be dealloc'd. Reject the connection.
          return nullptr;
        },
        capabilities);
        
    if (m_inspectorPageId.has_value()) {
      OutputDebugStringA("ReactNativeHost: Inspector page added successfully\n");
    } else {
      OutputDebugStringA("ReactNativeHost: Failed to add inspector page\n");
    }
  } else {
    OutputDebugStringA("ReactNativeHost: Skipping inspector creation - conditions not met\n");
  }
}

ReactNativeHost::~ReactNativeHost() noexcept {
  if (m_inspectorPageId.has_value()) {
    facebook::react::jsinspector_modern::getInspectorInstance().removePage(*m_inspectorPageId);
    m_inspectorPageId.reset();
    m_inspectorTarget.reset();
  }
}

/*static*/ ReactNative::ReactNativeHost ReactNativeHost::FromContext(
    ReactNative::IReactContext const &reactContext) noexcept {
  return GetReactNativeHost(ReactPropertyBag{reactContext.Properties()});
}

IVector<IReactPackageProvider> ReactNativeHost::PackageProviders() noexcept {
  return InstanceSettings().PackageProviders();
}

ReactNative::ReactInstanceSettings ReactNativeHost::InstanceSettings() noexcept {
  if (!m_instanceSettings) {
    m_instanceSettings = make<ReactInstanceSettings>();
  }

  return m_instanceSettings;
}

void ReactNativeHost::InstanceSettings(ReactNative::ReactInstanceSettings const &value) noexcept {
  m_instanceSettings = value;
}

IAsyncAction ReactNativeHost::LoadInstance() noexcept {
  return ReloadInstance();
}

winrt::Microsoft::ReactNative::ReactPropertyId<
    winrt::Microsoft::ReactNative::ReactNonAbiValue<winrt::weak_ref<ReactNativeHost>>>
ReactNativeHostProperty() noexcept {
  static winrt::Microsoft::ReactNative::ReactPropertyId<
      winrt::Microsoft::ReactNative::ReactNonAbiValue<winrt::weak_ref<ReactNativeHost>>>
      propId{L"ReactNative.ReactNativeHost", L"ReactNativeHost"};
  return propId;
}

/*static*/ winrt::Microsoft::ReactNative::ReactNativeHost ReactNativeHost::GetReactNativeHost(
    ReactPropertyBag const &properties) noexcept {
  if (auto wkHost = properties.Get(ReactNativeHostProperty()).Value()) {
    if (auto abiHost = wkHost.get()) {
      return abiHost.as<winrt::Microsoft::ReactNative::ReactNativeHost>();
    }
  }
  return nullptr;
}

IAsyncAction ReactNativeHost::ReloadInstance() noexcept {
  // Unload existing instance first to ensure clean state
  OutputDebugStringA("[HOST] ReloadInstance: Unloading previous instance...\n");
  co_await make<Mso::AsyncActionFutureAdapter>(m_reactHost->UnloadInstance());
  OutputDebugStringA("[HOST] ReloadInstance: Previous instance unloaded\n");

  auto modulesProvider = std::make_shared<NativeModulesProvider>();

#if !defined(CORE_ABI) && !defined(USE_FABRIC)
  auto viewManagersProvider = std::make_shared<ViewManagersProvider>();
#endif

  auto turboModulesProvider = std::make_shared<TurboModulesProvider>();

#ifdef USE_FABRIC
  auto uriImageManager =
      std::make_shared<winrt::Microsoft::ReactNative::Composition::implementation::UriImageManager>();
  auto componentregistry = std::make_shared<::Microsoft::ReactNative::WindowsComponentDescriptorRegistry>();
  auto componentDescriptorRegistry = std::make_shared<facebook::react::ComponentDescriptorProviderRegistry>();

  ::Microsoft::ReactNative::WindowsComponentDescriptorRegistry::AddToProperties(
      ReactPropertyBag(m_instanceSettings.Properties()), componentregistry);
#endif

  m_packageBuilder = make<ReactPackageBuilder>(
      modulesProvider,
#if !defined(CORE_ABI) && !defined(USE_FABRIC)
      viewManagersProvider,
#endif
      turboModulesProvider,
#ifdef USE_FABRIC
      componentregistry,
      uriImageManager,
#endif
      m_instanceSettings.UseWebDebugger());

#ifdef USE_FABRIC
  winrt::Microsoft::ReactNative::Composition::implementation::RegisterWindowsModalHostNativeComponent(m_packageBuilder);
#endif

  if (auto packageProviders = InstanceSettings().PackageProviders()) {
    for (auto const &packageProvider : packageProviders) {
      packageProvider.CreatePackage(m_packageBuilder);
    }
  }

  ReactPropertyBag(m_instanceSettings.Properties()).Set(ReactNativeHostProperty(), get_weak());

  Mso::React::ReactOptions reactOptions{};
  reactOptions.Properties = m_instanceSettings.Properties();
  reactOptions.Notifications = m_instanceSettings.Notifications();
  reactOptions.SetUseDeveloperSupport(m_instanceSettings.UseDeveloperSupport());
  reactOptions.DeveloperSettings.SourceBundleName = to_string(m_instanceSettings.DebugBundlePath());
  reactOptions.SetUseWebDebugger(m_instanceSettings.UseWebDebugger());
  reactOptions.SetUseDirectDebugger(m_instanceSettings.UseDirectDebugger());
  reactOptions.SetDebuggerBreakOnNextLine(m_instanceSettings.DebuggerBreakOnNextLine());
  reactOptions.SetUseFastRefresh(m_instanceSettings.UseFastRefresh());
  reactOptions.SetUseLiveReload(m_instanceSettings.UseLiveReload());
  reactOptions.EnableJITCompilation = m_instanceSettings.EnableJITCompilation();
  reactOptions.BundleRootPath = to_string(m_instanceSettings.BundleRootPath());
  reactOptions.DeveloperSettings.DebuggerPort = m_instanceSettings.DebuggerPort();
  reactOptions.DeveloperSettings.DebuggerRuntimeName = to_string(m_instanceSettings.DebuggerRuntimeName());
  if (m_instanceSettings.RedBoxHandler()) {
    reactOptions.RedBoxHandler = Mso::React::CreateRedBoxHandler(m_instanceSettings.RedBoxHandler());
  }
  if (m_instanceSettings.NativeLogger()) {
    reactOptions.OnLogging = [handler = m_instanceSettings.NativeLogger()](
                                 Mso::React::LogLevel logLevel, const char *message) {
      static_assert(LogLevel::Error == static_cast<LogLevel>(Mso::React::LogLevel::Error));
      static_assert(LogLevel::Fatal == static_cast<LogLevel>(Mso::React::LogLevel::Fatal));
      static_assert(LogLevel::Info == static_cast<LogLevel>(Mso::React::LogLevel::Info));
      static_assert(LogLevel::Trace == static_cast<LogLevel>(Mso::React::LogLevel::Trace));
      static_assert(LogLevel::Warning == static_cast<LogLevel>(Mso::React::LogLevel::Warning));
      handler(static_cast<LogLevel>(logLevel), winrt::to_hstring(message));
    };
  }
  reactOptions.DeveloperSettings.SourceBundleHost = to_string(m_instanceSettings.SourceBundleHost());
  reactOptions.DeveloperSettings.SourceBundlePort = m_instanceSettings.SourceBundlePort();
  reactOptions.DeveloperSettings.RequestInlineSourceMap = m_instanceSettings.RequestInlineSourceMap();
  reactOptions.DeveloperSettings.BundleAppId = to_string(m_instanceSettings.BundleAppId());
  reactOptions.DeveloperSettings.DevBundle = m_instanceSettings.RequestDevBundle();

  reactOptions.ByteCodeFileUri = to_string(m_instanceSettings.ByteCodeFileUri());
  reactOptions.EnableByteCodeCaching = m_instanceSettings.EnableByteCodeCaching();
  reactOptions.SetEnableDefaultCrashHandler(m_instanceSettings.EnableDefaultCrashHandler());
  reactOptions.SetJsiEngine(static_cast<Mso::React::JSIEngine>(m_instanceSettings.JSIEngineOverride()));

  reactOptions.ModuleProvider = modulesProvider;
#if !defined(CORE_ABI) && !defined(USE_FABRIC)
  reactOptions.ViewManagerProvider = viewManagersProvider;
#endif
  reactOptions.TurboModuleProvider = turboModulesProvider;

#ifdef USE_FABRIC
  reactOptions.UriImageManager = uriImageManager;
#endif

  reactOptions.OnInstanceCreated = [](Mso::CntPtr<Mso::React::IReactContext> &&context) {
    auto notifications = context->Notifications();
    ReactInstanceSettings::RaiseInstanceCreated(
        notifications, winrt::make<InstanceCreatedEventArgs>(std::move(context)));
  };
  reactOptions.OnInstanceLoaded = [](Mso::CntPtr<Mso::React::IReactContext> &&context, const Mso::ErrorCode &err) {
    auto notifications = context->Notifications();
    ReactInstanceSettings::RaiseInstanceLoaded(
        notifications, winrt::make<InstanceLoadedEventArgs>(std::move(context), !!err));
  };
  reactOptions.OnInstanceDestroyed = [](Mso::CntPtr<Mso::React::IReactContext> &&context) {
    auto notifications = context->Notifications();
    ReactInstanceSettings::RaiseInstanceDestroyed(
        notifications, winrt::make<InstanceDestroyedEventArgs>(std::move(context)));
  };

  std::string jsBundleFile = to_string(m_instanceSettings.JavaScriptBundleFile());
  if (jsBundleFile.empty()) {
    jsBundleFile = "index.windows";
  }

  reactOptions.Identity = jsBundleFile;
  reactOptions.InspectorTarget = m_inspectorTarget.get();
  
  OutputDebugStringA("[HOST] ReloadInstance: Creating new instance...\n");
  co_await make<Mso::AsyncActionFutureAdapter>(m_reactHost->ReloadInstanceWithOptions(std::move(reactOptions)));
  OutputDebugStringA("[HOST] ReloadInstance: New instance created successfully\n");
}

IAsyncAction ReactNativeHost::UnloadInstance() noexcept {
  return make<Mso::AsyncActionFutureAdapter>(m_reactHost->UnloadInstance());
}

Mso::React::IReactHost *ReactNativeHost::ReactHost() noexcept {
  return m_reactHost.Get();
}

void ReactNativeHost::OnDebuggerResume() noexcept {
  ::Microsoft::ReactNative::ReactInspectorThread::Instance().Post(
      [weakInspectorTarget = std::weak_ptr(m_inspectorTarget)]() {
        if (const auto inspectorTarget = weakInspectorTarget.lock()) {
          inspectorTarget->sendCommand(facebook::react::jsinspector_modern::HostCommand::DebuggerResume);
        }
      });
}

} // namespace winrt::Microsoft::ReactNative::implementation
