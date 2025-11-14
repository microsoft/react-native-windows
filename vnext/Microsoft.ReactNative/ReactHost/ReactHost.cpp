// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "ReactHost.h"
#include <Future/FutureWait.h>
#include <ReactPropertyBag.h>
#include <winrt/Windows.Foundation.h>

#include <CppRuntimeOptions.h>

#include <jsinspector-modern/HostCommand.h>
#include <jsinspector-modern/InspectorFlags.h>
#include <react/featureflags/ReactNativeFeatureFlags.h>
#include <react/featureflags/ReactNativeFeatureFlagsDefaults.h>

#include "Inspector/ReactInspectorThread.h"
#include "ReactHost/DebuggerNotifications.h"

using namespace facebook::react;

namespace Mso::React {

//=============================================================================================
// ReactOptions internal properties
//=============================================================================================

winrt::Microsoft::ReactNative::IReactPropertyName FastRefreshEnabledProperty() noexcept {
  static winrt::Microsoft::ReactNative::IReactPropertyName propName =
      winrt::Microsoft::ReactNative::ReactPropertyBagHelper::GetName(
          winrt::Microsoft::ReactNative::ReactPropertyBagHelper::GetNamespace(L"ReactNative.ReactOptions"),
          L"FastRefreshEnabled");
  return propName;
}

winrt::Microsoft::ReactNative::IReactPropertyName UseDeveloperSupportProperty() noexcept {
  static winrt::Microsoft::ReactNative::IReactPropertyName propName =
      winrt::Microsoft::ReactNative::ReactPropertyBagHelper::GetName(
          winrt::Microsoft::ReactNative::ReactPropertyBagHelper::GetNamespace(L"ReactNative.ReactOptions"),
          L"UseDeveloperSupport");
  return propName;
}

winrt::Microsoft::ReactNative::IReactPropertyName JSIEngineProperty() noexcept {
  static winrt::Microsoft::ReactNative::IReactPropertyName propName =
      winrt::Microsoft::ReactNative::ReactPropertyBagHelper::GetName(
          winrt::Microsoft::ReactNative::ReactPropertyBagHelper::GetNamespace(L"ReactNative.ReactOptions"),
          L"JSIEngine");
  return propName;
}

winrt::Microsoft::ReactNative::ReactPropertyId<bool> JSIEngineV8NodeApiProperty() noexcept {
  static winrt::Microsoft::ReactNative::ReactPropertyId<bool> propId{
      L"ReactNative.ReactOptions", L"JSIEngineV8NodeApi"};
  return propId;
}

winrt::Microsoft::ReactNative::IReactPropertyName LiveReloadEnabledProperty() noexcept {
  static winrt::Microsoft::ReactNative::IReactPropertyName propName =
      winrt::Microsoft::ReactNative::ReactPropertyBagHelper::GetName(
          winrt::Microsoft::ReactNative::ReactPropertyBagHelper::GetNamespace(L"ReactNative.ReactOptions"),
          L"LiveReloadEnabled");
  return propName;
}

winrt::Microsoft::ReactNative::IReactPropertyName DebuggerBreakOnNextLineProperty() noexcept {
  static winrt::Microsoft::ReactNative::IReactPropertyName propName =
      winrt::Microsoft::ReactNative::ReactPropertyBagHelper::GetName(
          winrt::Microsoft::ReactNative::ReactPropertyBagHelper::GetNamespace(L"ReactNative.ReactOptions"),
          L"DebuggerBreakOnNextLine");
  return propName;
}

winrt::Microsoft::ReactNative::IReactPropertyName UseDirectDebuggerProperty() noexcept {
  static winrt::Microsoft::ReactNative::IReactPropertyName propName =
      winrt::Microsoft::ReactNative::ReactPropertyBagHelper::GetName(
          winrt::Microsoft::ReactNative::ReactPropertyBagHelper::GetNamespace(L"ReactNative.ReactOptions"),
          L"UseDirectDebugger");
  return propName;
}

winrt::Microsoft::ReactNative::IReactPropertyName EnableFabricProperty() noexcept {
  static winrt::Microsoft::ReactNative::IReactPropertyName propName =
      winrt::Microsoft::ReactNative::ReactPropertyBagHelper::GetName(
          winrt::Microsoft::ReactNative::ReactPropertyBagHelper::GetNamespace(L"ReactNative.ReactOptions"),
          L"EnableFabric");
  return propName;
}

winrt::Microsoft::ReactNative::IReactPropertyName EnableDefaultCrashHandlerProperty() noexcept {
  static winrt::Microsoft::ReactNative::IReactPropertyName propName =
      winrt::Microsoft::ReactNative::ReactPropertyBagHelper::GetName(
          winrt::Microsoft::ReactNative::ReactPropertyBagHelper::GetNamespace(L"ReactNative.ReactOptions"),
          L"EnableDefaultCrashHandler");
  return propName;
}

//=============================================================================================
// ReactOptions implementation
//=============================================================================================

LIBLET_PUBLICAPI ReactOptions &ReactOptions::AddRegisteredJSBundle(std::string_view jsBundleId) noexcept {
  this->JSBundles.push_back(MakeRegisteredJSBundle(jsBundleId));
  return *this;
}

//! Adds JS bundle based on a file name to JSBundles.
LIBLET_PUBLICAPI ReactOptions &ReactOptions::AddFileJSBundle(
    std::string_view jsBundleId,
    std::string_view fileName) noexcept {
  this->JSBundles.push_back(MakeFileJSBundle(jsBundleId.data(), fileName.data()));
  return *this;
}

void ReactOptions::SetUseDeveloperSupport(bool enabled) noexcept {
  Properties.Set(UseDeveloperSupportProperty(), winrt::box_value(enabled));
}

bool ReactOptions::UseDeveloperSupport() const noexcept {
  return UseDeveloperSupport(Properties);
}

/*static*/ void ReactOptions::SetUseDeveloperSupport(
    winrt::Microsoft::ReactNative::IReactPropertyBag const &properties,
    bool value) noexcept {
  properties.Set(UseDeveloperSupportProperty(), winrt::box_value(value));
}

/*static*/ bool ReactOptions::UseDeveloperSupport(
    winrt::Microsoft::ReactNative::IReactPropertyBag const &properties) noexcept {
  return winrt::unbox_value_or<bool>(properties.Get(UseDeveloperSupportProperty()), false);
}

/*static*/ JSIEngine ReactOptions::JsiEngine(
    winrt::Microsoft::ReactNative::IReactPropertyBag const &properties) noexcept {
  return (Mso::React::JSIEngine)winrt::unbox_value_or<uint32_t>(properties.Get(JSIEngineProperty()), 0);
}

JSIEngine ReactOptions::JsiEngine() const noexcept {
  return static_cast<JSIEngine>(JsiEngine(Properties));
}

void ReactOptions::SetJsiEngine(JSIEngine value) noexcept {
  Properties.Set(JSIEngineProperty(), winrt::box_value(static_cast<uint32_t>(value)));
}
/*static*/ void ReactOptions::SetJsiEngine(
    winrt::Microsoft::ReactNative::IReactPropertyBag const &properties,
    JSIEngine value) noexcept {
  properties.Set(JSIEngineProperty(), winrt::box_value(static_cast<uint32_t>(value)));
}

/*static*/ bool ReactOptions::JsiEngineV8NodeApi(
    winrt::Microsoft::ReactNative::IReactPropertyBag const &properties) noexcept {
  return winrt::Microsoft::ReactNative::ReactPropertyBag(properties).Get(JSIEngineV8NodeApiProperty()).value_or(false);
}

bool ReactOptions::JsiEngineV8NodeApi() const noexcept {
  return JsiEngineV8NodeApi(Properties);
}

/*static*/ void ReactOptions::SetUseFastRefresh(
    winrt::Microsoft::ReactNative::IReactPropertyBag const &properties,
    bool value) noexcept {
  properties.Set(FastRefreshEnabledProperty(), winrt::box_value(value));
}

/*static*/ bool ReactOptions::UseFastRefresh(
    winrt::Microsoft::ReactNative::IReactPropertyBag const &properties) noexcept {
  return winrt::unbox_value_or<bool>(properties.Get(FastRefreshEnabledProperty()), false);
}

void ReactOptions::SetUseFastRefresh(bool enabled) noexcept {
  Properties.Set(FastRefreshEnabledProperty(), winrt::box_value(enabled));
}

bool ReactOptions::UseFastRefresh() const noexcept {
  return UseFastRefresh(Properties);
}

void ReactOptions::SetUseLiveReload(bool enabled) noexcept {
  Properties.Set(LiveReloadEnabledProperty(), winrt::box_value(enabled));
}

/*static*/ void ReactOptions::SetUseLiveReload(
    winrt::Microsoft::ReactNative::IReactPropertyBag const &properties,
    bool value) noexcept {
  properties.Set(LiveReloadEnabledProperty(), winrt::box_value(value));
}

/*static*/ bool ReactOptions::UseLiveReload(
    winrt::Microsoft::ReactNative::IReactPropertyBag const &properties) noexcept {
  return winrt::unbox_value_or<bool>(properties.Get(LiveReloadEnabledProperty()), false);
}

bool ReactOptions::UseLiveReload() const noexcept {
  return UseLiveReload(Properties);
}

void ReactOptions::SetDebuggerBreakOnNextLine(bool enabled) noexcept {
  Properties.Set(DebuggerBreakOnNextLineProperty(), winrt::box_value(enabled));
}

bool ReactOptions::DebuggerBreakOnNextLine() const noexcept {
  return DebuggerBreakOnNextLine(Properties);
}

/*static*/ void ReactOptions::SetDebuggerBreakOnNextLine(
    winrt::Microsoft::ReactNative::IReactPropertyBag const &properties,
    bool value) noexcept {
  properties.Set(DebuggerBreakOnNextLineProperty(), winrt::box_value(value));
}

/*static*/ bool ReactOptions::DebuggerBreakOnNextLine(
    winrt::Microsoft::ReactNative::IReactPropertyBag const &properties) noexcept {
  return winrt::unbox_value_or<bool>(properties.Get(DebuggerBreakOnNextLineProperty()), false);
}

void ReactOptions::SetUseDirectDebugger(bool enabled) noexcept {
  Properties.Set(UseDirectDebuggerProperty(), winrt::box_value(enabled));
}

bool ReactOptions::UseDirectDebugger() const noexcept {
  return UseDirectDebugger(Properties);
}

/*static*/ void ReactOptions::SetUseDirectDebugger(
    winrt::Microsoft::ReactNative::IReactPropertyBag const &properties,
    bool value) noexcept {
  properties.Set(UseDirectDebuggerProperty(), winrt::box_value(value));
}

/*static*/ bool ReactOptions::UseDirectDebugger(
    winrt::Microsoft::ReactNative::IReactPropertyBag const &properties) noexcept {
  return winrt::unbox_value_or<bool>(properties.Get(UseDirectDebuggerProperty()), false);
}

void ReactOptions::SetEnableDefaultCrashHandler(bool enabled) noexcept {
  Properties.Set(EnableDefaultCrashHandlerProperty(), winrt::box_value(enabled));
}

bool ReactOptions::EnableDefaultCrashHandler() const noexcept {
  return EnableDefaultCrashHandler(Properties);
}

/*static*/ void ReactOptions::SetEnableDefaultCrashHandler(
    winrt::Microsoft::ReactNative::IReactPropertyBag const &properties,
    bool value) noexcept {
  properties.Set(EnableDefaultCrashHandlerProperty(), winrt::box_value(value));
}

/*static*/ bool ReactOptions::EnableDefaultCrashHandler(
    winrt::Microsoft::ReactNative::IReactPropertyBag const &properties) noexcept {
  return winrt::unbox_value_or<bool>(properties.Get(EnableDefaultCrashHandlerProperty()), false);
}

//=============================================================================================
// ReactNativeWindowsFeatureFlags implementation
//=============================================================================================

class ReactNativeWindowsFeatureFlags : public facebook::react::ReactNativeFeatureFlagsDefaults {
 public:
  bool enableBridgelessArchitecture() override {
    return true;
  }

  bool enableCppPropsIteratorSetter() override {
    return true;
  }

  bool fuseboxEnabledRelease() override {
    return true; // Enable Fusebox (modern CDP backend) by default for React Native Windows
  }

  bool fuseboxNetworkInspectionEnabled() override {
    return true; // Enable network inspection support in Fusebox
  }
};

//=============================================================================================
// ReactInspectorHostTargetDelegate implementation
//=============================================================================================

class ReactInspectorHostTargetDelegate : public jsinspector_modern::HostTargetDelegate,
                                         public std::enable_shared_from_this<ReactInspectorHostTargetDelegate> {
 public:
  ReactInspectorHostTargetDelegate(Mso::WeakPtr<ReactHost> &&reactHost) noexcept : m_reactHost(std::move(reactHost)) {}

  jsinspector_modern::HostTargetMetadata getMetadata() override {
    jsinspector_modern::HostTargetMetadata metadata;
    metadata.integrationName = "React Native Windows (Host)";
    metadata.platform = "windows";
    
    if (Mso::CntPtr<ReactHost> reactHost = m_reactHost.GetStrongPtr()) {
      auto options = reactHost->Options();
      if (!options.Identity.empty()) {
        std::string identity = options.Identity;
        size_t lastSlash = identity.find_last_of("\\/");
        metadata.appDisplayName = (lastSlash != std::string::npos && lastSlash + 1 < identity.length())
            ? identity.substr(lastSlash + 1)
            : identity;
      }
    }
    
    wchar_t computerName[MAX_COMPUTERNAME_LENGTH + 1];
    DWORD size = MAX_COMPUTERNAME_LENGTH + 1;
    if (GetComputerNameW(computerName, &size)) {
      metadata.deviceName = winrt::to_string(computerName);
    }
    
    return metadata;
  }

  void onReload(jsinspector_modern::HostTargetDelegate::PageReloadRequest const &request) override {
    if (Mso::CntPtr<ReactHost> reactHost = m_reactHost.GetStrongPtr()) {
      reactHost->ReloadInstance();
    }
  }

  void onSetPausedInDebuggerMessage(
      jsinspector_modern::HostTargetDelegate::OverlaySetPausedInDebuggerMessageRequest const &request) override {
    if (Mso::CntPtr<ReactHost> reactHost = m_reactHost.GetStrongPtr()) {
      auto notifications = reactHost->Options().Notifications;
      if (request.message.has_value()) {
        ::Microsoft::ReactNative::DebuggerNotifications::OnShowDebuggerPausedOverlay(
            notifications, request.message.value(), [weakReactHost = m_reactHost]() {
              if (Mso::CntPtr<ReactHost> strongReactHost = weakReactHost.GetStrongPtr()) {
                strongReactHost->OnDebuggerResume();
              }
            });
      } else {
        ::Microsoft::ReactNative::DebuggerNotifications::OnHideDebuggerPausedOverlay(notifications);
      }
    }
  }

 private:
  Mso::WeakPtr<ReactHost> m_reactHost;
};

//=============================================================================================
// ReactHost implementation
//=============================================================================================

ReactHost::ReactHost(Mso::DispatchQueue const &queue) noexcept
    : Super{EnsureSerialQueue(queue)},
      m_options{Queue(), m_mutex},
      m_notifyWhenClosed{ReactHostRegistry::Register(*this), Queue(), m_mutex},
      m_inspectorHostTargetDelegate{std::make_shared<ReactInspectorHostTargetDelegate>(this)},
      m_inspectorHostTarget{
          jsinspector_modern::HostTarget::create(*m_inspectorHostTargetDelegate, [](std::function<void()> &&callback) {
            ::Microsoft::ReactNative::ReactInspectorThread::Instance().Post(
                [callback = std::move(callback)]() { callback(); });
          })} {
  static std::once_flag initFeatureFlagsOnce;
  std::call_once(initFeatureFlagsOnce, []() noexcept {
    ReactNativeFeatureFlags::override(std::make_unique<ReactNativeWindowsFeatureFlags>());
  });
}

ReactHost::~ReactHost() noexcept {}

// Finalize is always called from the Native queue
void ReactHost::Finalize() noexcept {
  // If we are here, then it means that the current ref count is zero.
  // Since each AsyncAction has a strong ref count to ReactHost, the AsyncActionQueue must be empty.
  // Thus, we only need to call UnloadInQueue to unload ReactInstance if the ReactHost is not closed yet.
  if (Mso::Promise<void> notifyWhenClosed = m_notifyWhenClosed.Exchange(nullptr)) {
    UnloadInQueue(UnloadReason::CloseHost, 0)
        .Then<Mso::Executors::Inline>(
            [notifyWhenClosed = std::move(notifyWhenClosed)]() noexcept { notifyWhenClosed.TrySetValue(); });
  }
}

void ReactHost::Close() noexcept {
  InvokeInQueue([this]() noexcept {
    // Put the ReactHost to the closed state, unload ReactInstance, and notify the closing Promise.
    auto whenClosed = m_actionQueue.Load()->PostAction(MakeUnloadInstanceAction(UnloadReason::CloseHost));

    // After we set the m_notifyWhenClosed to null, the ReactHost is considered to be closed.
    Mso::SetPromiseValue(m_notifyWhenClosed.Exchange(nullptr), std::move(whenClosed));
  });
}

bool ReactHost::IsClosed() const noexcept {
  return !static_cast<bool>(m_notifyWhenClosed.Load());
}

Mso::CntPtr<AsyncActionQueue> ReactHost::ActionQueue() const noexcept {
  return m_actionQueue.LoadWithLock();
}

size_t ReactHost::PendingUnloadActionId() const noexcept {
  Mso::Internal::VerifyIsInQueueElseCrash(Queue());
  return m_pendingUnloadActionId;
}

bool ReactHost::IsInstanceUnloading() const noexcept {
  return m_isInstanceUnloading.Load();
}

/*static*/ Mso::DispatchQueue ReactHost::EnsureSerialQueue(Mso::DispatchQueue const &queue) noexcept {
  if (queue) {
    VerifyElseCrashSz(queue.IsSerial(), "NativeQueue must be sequential");
    return queue;
  }

  // Create a new serial queue if it was provided
  return Mso::DispatchQueue::MakeSerialQueue();
}

ReactOptions ReactHost::Options() const noexcept {
  return m_options.LoadWithLock();
}

Mso::CntPtr<IReactInstance> ReactHost::Instance() const noexcept {
  return m_reactInstance.LoadWithLock();
}

Mso::DispatchQueue const &ReactHost::NativeQueue() const noexcept {
  return Queue();
}

Mso::Future<void> ReactHost::ReloadInstance() noexcept {
  return ReloadInstanceWithOptions(m_options.LoadWithLock());
}

Mso::Future<void> ReactHost::ReloadInstanceWithOptions(ReactOptions &&options) noexcept {
  return PostInQueue([this, options = std::move(options)]() mutable noexcept {
    return m_actionQueue.Load()->PostActions(
        {MakeUnloadInstanceAction(UnloadReason::Unload), MakeLoadInstanceAction(std::move(options))});
  });
}

Mso::Future<void> ReactHost::UnloadInstance() noexcept {
  return PostInQueue(
      [this]() noexcept { return m_actionQueue.Load()->PostAction(MakeUnloadInstanceAction(UnloadReason::Unload)); });
}

AsyncAction ReactHost::MakeLoadInstanceAction(ReactOptions &&options) noexcept {
  return [spThis = Mso::CntPtr{this}, options = std::move(options)]() mutable noexcept {
    return spThis->LoadInQueue(std::move(options));
  };
}

AsyncAction ReactHost::MakeUnloadInstanceAction(UnloadReason reason) noexcept {
  Mso::Internal::VerifyIsInQueueElseCrash(Queue());
  size_t unloadActionId = ++m_nextUnloadActionId;
  m_pendingUnloadActionId = unloadActionId;
  return [spThis = Mso::CntPtr{this}, reason, unloadActionId]() noexcept {
    return spThis->UnloadInQueue(reason, unloadActionId);
  };
}

Mso::CntPtr<IReactViewHost> ReactHost::MakeViewHost(ReactViewOptions &&options) noexcept {
  return Mso::Make<ReactViewHost, IReactViewHost>(*this, std::move(options));
}

Mso::Future<std::vector<Mso::CntPtr<IReactViewHost>>> ReactHost::GetViewHostList() noexcept {
  Mso::Promise<std::vector<Mso::CntPtr<IReactViewHost>>> result;
  InvokeInQueue([this, result]() noexcept {
    std::vector<Mso::CntPtr<IReactViewHost>> viewHosts;
    ForEachViewHost([&viewHosts](auto &viewHost) noexcept { viewHosts.push_back(&viewHost); });
    result.SetValue(std::move(viewHosts));
  });
  return result.AsFuture();
}

Mso::Future<void> ReactHost::LoadInQueue(ReactOptions &&options) noexcept {
  // If the ReactHost is already closed then we cancel loading ReactInstance.
  if (IsClosed()) {
    return Mso::MakeCanceledFuture();
  }

  // Make sure that we set new options even if we do not load due to the pending unload.
  m_options.Exchange(Mso::Copy(options));

  // If there is a pending unload action, then we cancel loading ReactInstance.
  if (PendingUnloadActionId()) {
    return Mso::MakeCanceledFuture();
  }

  // Start or stop inspector page if needed.
  // Make sure to update the both copies of options.
  if (IsInspectable()) {
    AddInspectorPage();
    options.InspectorHostTarget = m_inspectorHostTarget.get();
    m_options.Load().InspectorHostTarget = m_inspectorHostTarget.get();
  } else {
    RemoveInspectorPage();
    options.InspectorHostTarget = nullptr;
    m_options.Load().InspectorHostTarget = nullptr;
  }

  Mso::Promise<void> whenCreated;
  Mso::Promise<void> whenLoaded;

  // Requires MakeReactInstance which incurs platform-specific dependencies.
  m_reactInstance.Exchange(
      MakeReactInstance(*this, std::move(options), Mso::Copy(whenCreated), Mso::Copy(whenLoaded), [this]() noexcept {
        InvokeInQueue([this]() noexcept {
          ForEachViewHost([](auto &viewHost) noexcept { viewHost.UpdateViewInstanceInQueue(); });
        });
      }));

  return whenCreated.AsFuture().Then(Mso::Executors::Inline{}, [this, whenLoaded]() noexcept {
    std::vector<Mso::Future<void>> initCompletionList;
    for (const auto &entry : m_viewHosts.Load()) {
      if (auto viewHost = entry.second.GetStrongPtr()) {
        viewHost->InitViewInstanceInQueue(viewHost->Options());
      }
    }

    return whenLoaded.AsFuture().Then(m_executor, [this](Mso::Maybe<void> &&value) noexcept {
      std::vector<Mso::Future<void>> loadCompletionList;

      if (value.IsError()) {
        return Mso::MakeFailedFuture<void>(std::move(value.TakeError()));
      }

      ForEachViewHost([&loadCompletionList](auto &viewHost) noexcept {
        loadCompletionList.push_back(viewHost.UpdateViewInstanceInQueue());
      });

      return Mso::WhenAllCompleted(loadCompletionList);
    });
  });
}

Mso::Future<void> ReactHost::UnloadInQueue(UnloadReason reason, size_t unloadActionId) noexcept {
  Mso::Internal::VerifyIsInQueueElseCrash(Queue());

  // If the pending unload action Id does not match, then we have newer unload action,
  // and thus we should cancel this one.
  if (unloadActionId != PendingUnloadActionId()) {
    return Mso::MakeCanceledFuture();
  }

  // Clear the pending unload action Id
  m_pendingUnloadActionId = 0;

  // This allows us to avoid initializing any new ReactViews against the old instance that is being unloaded
  m_isInstanceUnloading.Store(true);

  std::vector<Mso::Future<void>> unloadCompletionList;
  ForEachViewHost([&unloadCompletionList](auto &viewHost) noexcept {
    unloadCompletionList.push_back(viewHost.UninitViewInstanceInQueue(0));
  });

  // We unload ReactInstance after all view instances are unloaded.
  // It is safe to capture 'this' because the Unload action keeps a strong reference to ReactHost.
  return Mso::WhenAllCompleted(unloadCompletionList)
      .Then(m_executor, [this, reason](Mso::Maybe<void> && /*value*/) noexcept {
        Mso::Future<void> onUnloaded;
        if (auto reactInstance = m_reactInstance.Exchange(nullptr)) {
          onUnloaded = reactInstance->Destroy();
        }

        m_isInstanceUnloading.Store(false);
        m_lastError.Store({});

        if (!onUnloaded) {
          onUnloaded = Mso::MakeSucceededFuture();
        }

        if (reason == UnloadReason::CloseHost) {
          RemoveInspectorPage();
        }
        return onUnloaded;
      });
}

void ReactHost::ForEachViewHost(const Mso::FunctorRef<void(ReactViewHost &)> &action) noexcept {
  for (const auto &entry : m_viewHosts.Load()) {
    if (auto viewHost = entry.second.GetStrongPtr()) {
      action(*viewHost);
    }
  }
}

void ReactHost::AttachViewHost(ReactViewHost &viewHost) noexcept {
  auto &viewHosts = m_viewHosts.Load();

  uintptr_t viewHostKey = reinterpret_cast<uintptr_t>(&viewHost);
  auto it = viewHosts.find(viewHostKey);
  VerifyElseCrashSzTag(it == viewHosts.end(), "The view host is already attached", 0x0281e3d8 /* tag_c64py */);
  viewHosts[viewHostKey] = &viewHost;
}

void ReactHost::DetachViewHost(ReactViewHost &viewHost) noexcept {
  auto &viewHosts = m_viewHosts.Load();

  uintptr_t viewHostKey = reinterpret_cast<uintptr_t>(&viewHost);
  auto it = viewHosts.find(viewHostKey);
  VerifyElseCrashSzTag(it != viewHosts.end(), "The view host is not attached", 0x0281e3d9 /* tag_c64pz */);
  viewHosts.erase(it);
}

bool ReactHost::IsInspectable() noexcept {
  ReactOptions &options = m_options.Load();
  return options.JsiEngine() == JSIEngine::Hermes && options.UseDirectDebugger();
}

void ReactHost::AddInspectorPage() noexcept {
  std::optional<int32_t> &inspectorPageId = m_inspectorPageId.Load();
  if (inspectorPageId.has_value())
    return;

  jsinspector_modern::InspectorTargetCapabilities capabilities;
  capabilities.nativePageReloads = true;
  capabilities.prefersFuseboxFrontend = true;
  
  auto metadata = m_inspectorHostTargetDelegate->getMetadata();
  std::string pageName;
  if (metadata.appDisplayName.has_value() && !metadata.appDisplayName.value().empty()) {
    pageName = metadata.appDisplayName.value();
  } else {
    pageName = "React Native Windows (Experimental)";
  }
  if (metadata.deviceName.has_value() && !metadata.deviceName.value().empty()) {
    pageName += " (" + metadata.deviceName.value() + ")";
  }
  
  inspectorPageId = jsinspector_modern::getInspectorInstance().addPage(
      pageName,
      "Hermes",
      [weakInspectorHostTarget =
           std::weak_ptr(m_inspectorHostTarget)](std::unique_ptr<jsinspector_modern::IRemoteConnection> remote)
          -> std::unique_ptr<jsinspector_modern::ILocalConnection> {
        if (std::shared_ptr<jsinspector_modern::HostTarget> inspectorHostTarget = weakInspectorHostTarget.lock()) {
          return inspectorHostTarget->connect(std::move(remote));
        }

        // This can happen if we're about to shut down. Reject the connection.
        return nullptr;
      },
      capabilities);
}

void ReactHost::RemoveInspectorPage() noexcept {
  std::optional<int32_t> &inspectorPageId = m_inspectorPageId.Load();
  if (!inspectorPageId.has_value())
    return;

  jsinspector_modern::getInspectorInstance().removePage(*inspectorPageId);
  inspectorPageId.reset();
}

void ReactHost::OnDebuggerResume() noexcept {
  ::Microsoft::ReactNative::ReactInspectorThread::Instance().Post(
      [weakInspectorHostTarget = std::weak_ptr(m_inspectorHostTarget)]() {
        if (std::shared_ptr<jsinspector_modern::HostTarget> inspectorHostTarget = weakInspectorHostTarget.lock()) {
          inspectorHostTarget->sendCommand(jsinspector_modern::HostCommand::DebuggerResume);
        }
      });
}

//=============================================================================================
// ReactViewHost implementation
//=============================================================================================

ReactViewHost::ReactViewHost(Mso::React::ReactHost &reactHost, ReactViewOptions &&options) noexcept
    : Super{reactHost.NativeQueue()},
      m_actionQueue{reactHost.ActionQueue(), Queue()},
      m_reactHost{&reactHost},
      m_options{std::move(options), Queue(), m_mutex} {}

ReactViewHost::~ReactViewHost() noexcept {}

IReactHost &ReactViewHost::ReactHost() const noexcept {
  return *m_reactHost;
}

ReactViewOptions ReactViewHost::Options() const noexcept {
  return m_options.LoadWithLock();
}

void ReactViewHost::SetOptions(ReactViewOptions &&options) noexcept {
  m_options.Exchange(std::move(options));
}

Mso::Future<void> ReactViewHost::ReloadViewInstance() noexcept {
  return PostInQueue([this]() noexcept {
    return m_actionQueue.Load()->PostActions({MakeUninitViewInstanceAction(), MakeInitViewInstanceAction()});
  });
}

Mso::Future<void> ReactViewHost::ReloadViewInstanceWithOptions(ReactViewOptions &&options) noexcept {
  return PostInQueue([this, options = std::move(options)]() mutable noexcept {
    return m_actionQueue.Load()->PostActions(
        {MakeUninitViewInstanceAction(), MakeInitViewInstanceAction(std::move(options))});
  });
}

Mso::Future<void> ReactViewHost::UnloadViewInstance() noexcept {
  return PostInQueue([this, spThis = Mso::CntPtr{this}]() noexcept {
    return m_actionQueue.Load()->PostAction(MakeUninitViewInstanceAction());
  });
}

Mso::Future<void> ReactViewHost::AttachViewInstance(IReactViewInstance &viewInstance) noexcept {
  return PostInQueue([this, viewInstance = Mso::CntPtr{&viewInstance}]() noexcept {
    auto previousViewInstance = m_viewInstance.Exchange(Mso::Copy(viewInstance));
    VerifyElseCrashSzTag(
        !previousViewInstance, "ViewInstance must not be previously attached.", 0x028508d6 /* tag_c7q9w */);
    m_reactHost->AttachViewHost(*this);

    return InitViewInstanceInQueue();
    // Schedule the viewInstance load in the action queue since there can be other load actions in the queue that need
    // to be consolidated.
    // return m_actionQueue.Load()->PostAction(MakeInitViewInstanceAction());
  });
}

Mso::Future<void> ReactViewHost::DetachViewInstance() noexcept {
  return PostInQueue([this, spThis = Mso::CntPtr{this}]() noexcept {
    auto viewInstance = m_viewInstance.Exchange(nullptr);
    VerifyElseCrashSzTag(viewInstance, "ViewInstance is not attached.", 0x0281e3db /* tag_c64p1 */);
    m_reactHost->DetachViewHost(*this);

    // We unload the viewInstance here as soon as possible without using the action queue,
    // otherwise we would not have the viewInstance to call the Unload() later.
    m_isViewInstanceInited.Store(false);
    return viewInstance->UninitRootView();
  });
}

AsyncAction ReactViewHost::MakeInitViewInstanceAction() noexcept {
  return [spThis = Mso::CntPtr{this}]() mutable noexcept { return spThis->InitViewInstanceInQueue(); };
}

AsyncAction ReactViewHost::MakeInitViewInstanceAction(ReactViewOptions &&options) noexcept {
  return [spThis = Mso::CntPtr{this}, options = std::move(options)]() mutable noexcept {
    return spThis->InitViewInstanceInQueue(std::move(options));
  };
}

AsyncAction ReactViewHost::MakeUninitViewInstanceAction() noexcept {
  size_t uninitActionId = ++m_nextUninitActionId.Load();
  m_pendingUninitActionId.Store(std::move(uninitActionId));
  return [spThis = Mso::CntPtr{this}, uninitActionId]() noexcept {
    return spThis->UninitViewInstanceInQueue(uninitActionId);
  };
}

Mso::Future<void> ReactViewHost::InitViewInstanceInQueue() noexcept {
  // It is already loaded: nothing to do.
  if (m_isViewInstanceInited.Load()) {
    return Mso::MakeSucceededFuture();
  }

  // Cancel if the ReactHost is already closed.
  if (m_reactHost->IsClosed()) {
    return Mso::MakeCanceledFuture();
  }

  // If there is a pending unload action, then we cancel loading ReactViewInstance.
  if (m_reactHost->PendingUnloadActionId() || m_pendingUninitActionId.Load()) {
    return Mso::MakeCanceledFuture();
  }

  // We cannot load if instance is in the process of being unloaded.
  if (m_reactHost->IsInstanceUnloading()) {
    return Mso::MakeCanceledFuture();
  }

  // Make sure that we have a ReactInstance
  if (!m_reactHost->Instance()) {
    return Mso::MakeCanceledFuture();
  }

  if (auto viewInstance = m_viewInstance.Load().Get()) {
    m_isViewInstanceInited.Store(true);
    return viewInstance->InitRootView(m_reactHost->Instance(), Mso::Copy(m_options.Load()));
  }

  return Mso::MakeCanceledFuture();
}

Mso::Future<void> ReactViewHost::UpdateViewInstanceInQueue() noexcept {
  // Check if we already initialized.
  if (!m_isViewInstanceInited.Load()) {
    return Mso::MakeCanceledFuture();
  }

  // Cancel if the ReactHost is already closed.
  if (m_reactHost->IsClosed()) {
    return Mso::MakeCanceledFuture();
  }

  // If there is a pending unload action, then we cancel loading ReactViewInstance.
  if (m_reactHost->PendingUnloadActionId() || m_pendingUninitActionId.Load()) {
    return Mso::MakeCanceledFuture();
  }

  //// We cannot load if instance is not loaded.
  // if (!m_reactHost->IsInstanceLoaded()) {
  //  return Mso::MakeCanceledFuture();
  //}

  // Make sure that we have a ReactInstance
  if (!m_reactHost->Instance()) {
    return Mso::MakeCanceledFuture();
  }

  if (auto viewInstance = m_viewInstance.Load().Get()) {
    return viewInstance->UpdateRootView();
  }

  return Mso::MakeCanceledFuture();
}

Mso::Future<void> ReactViewHost::InitViewInstanceInQueue(ReactViewOptions &&options) noexcept {
  // Make sure that we set new options even if we do not load due to the pending unload.
  m_options.Exchange(std::move(options));

  VerifyElseCrashSzTag(
      !m_isViewInstanceInited.Load(),
      "The viewInstance must be unloaded before the load with new options.",
      0x0285411a /* tag_c7ue0 */);

  return InitViewInstanceInQueue();
}

Mso::Future<void> ReactViewHost::UninitViewInstanceInQueue(size_t unloadActionId) noexcept {
  // If the pending unload action Id does not match, then we have newer unload action,
  // and thus we should cancel this one.
  if (unloadActionId != m_pendingUninitActionId.Load()) {
    return Mso::MakeCanceledFuture();
  }

  // Clear the pending unload action Id
  m_pendingUninitActionId.Store(0);

  if (m_reactHost->PendingUnloadActionId()) {
    return Mso::MakeCanceledFuture();
  }

  if (auto viewInstance = m_viewInstance.Load().Get()) {
    m_isViewInstanceInited.Store(false);
    return viewInstance->UninitRootView();
  }

  return Mso::MakeCanceledFuture();
}

//=============================================================================================
// ReactHostRegistry implementation
//=============================================================================================

/*static*/ std::mutex ReactHostRegistry::s_mutex;
/*static*/ Mso::CntPtr<ReactHostRegistry> ReactHostRegistry::s_registry;

ReactHostRegistry::~ReactHostRegistry() noexcept {}

/*static*/ void ReactHostRegistry::OnLibletInit() noexcept {
  // Create new global ReactHostRegistry
  std::lock_guard<std::mutex> lock{s_mutex};
  VerifyElseCrashSzTag(!s_registry, "No ReactHostRegistry must exist yet.", 0x0281e3dc /* tag_c64p2 */);
  s_registry = Mso::Make<ReactHostRegistry>();
}

/*static*/ void ReactHostRegistry::OnLibletUninit() noexcept {
  // Wait for all pending ReactHost futures indicating closing state and remove the global ReactHostRegistry.
  std::vector<Mso::Future<void>> futures;

  {
    std::lock_guard<std::mutex> lock{s_mutex};
    auto registry = std::move(s_registry);

    for (const auto &entry : registry->m_entries) {
      if (auto reactHost = entry.second.WeakReactHost.GetStrongPtr()) {
        reactHost->Close();
      }

      futures.push_back(entry.second.WhenReactHostClosed);
    }
  }

  Mso::FutureWait(Mso::WhenAllCompleted(futures));
}

/*static*/ Mso::Promise<void> ReactHostRegistry::Register(ReactHost & /*reactHost*/) noexcept {
  // TODO:
  // std::lock_guard<std::mutex> lock{s_mutex};

  // if (!s_registry) {
  //  // Return an empty Promise to indicate failure to register.
  //  return Mso::Promise<void>{nullptr};
  //}

  // auto &entries = s_registry->m_entries;
  // uintptr_t reactHostKey = reinterpret_cast<uintptr_t>(&reactHost);
  // auto it = entries.find(reactHostKey);
  // VerifyElseCrashSzTag(it == entries.end(), "The ReactHost is already registered", 0x0281e3dd /* tag_c64p3 */);

  Mso::Promise<void> notifyWhenClosed;
  // TODO:
  // Mso::Future<void> whenReactHostClosed = notifyWhenClosed.AsFuture().Then<Mso::Executors::Inline>(
  //    [ registry{s_registry}, reactHostKey ](Mso::Maybe<void> && result) noexcept {
  //      registry->Unregister(reactHostKey);
  //      return std::move(result);
  //    });
  // entries.emplace(reactHostKey, Entry{&reactHost, std::move(whenReactHostClosed)});
  return notifyWhenClosed;
}

void ReactHostRegistry::Unregister(uintptr_t reactHostKey) noexcept {
  std::lock_guard<std::mutex> lock{s_mutex};

  auto it = m_entries.find(reactHostKey);
  VerifyElseCrashSzTag(it != m_entries.end(), "The ReactHost is not registered", 0x0281e3de /* tag_c64p4 */);
  m_entries.erase(it);
}

//=============================================================================================
// Standalone functions
//=============================================================================================

LIBLET_PUBLICAPI OnErrorCallback GetDefaultOnErrorHandler() noexcept {
  return [](const Mso::ErrorCode &errorCode) noexcept {
    // If error is due to std::exception, write out its message in Debug Assert
    if (const std::exception_ptr *exc = errorCode.HandleAs(Mso::ExceptionErrorProvider())) {
      try {
        std::rethrow_exception(*exc);
      } catch (const std::exception &ex) {
        UNREFERENCED_PARAMETER(ex);
        AssertSz(false, ex.what());
      }
    }

    Assert(false);
  };
}

LIBLET_PUBLICAPI Mso::CntPtr<IReactHost> MakeReactHost() noexcept {
  return Mso::Make<ReactHost, IReactHost>(nullptr);
}

} // namespace Mso::React
