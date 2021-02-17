// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.
#pragma once

#include <CxxMessageQueue.h>
#include <INativeUIManager.h>
#include <NativeModules.h>
#include <React.h>
#include <Views/ShadowNodeRegistry.h>
#include <winrt/Windows.ApplicationModel.h>
#include <winrt/Windows.Foundation.h>
#include "ComponentViewRegistry.h"

#pragma warning(push)
#pragma warning(disable : 4244)
#include <react/renderer/scheduler/SchedulerDelegate.h>
#pragma warning(pop)

namespace facebook::react {
class Scheduler;
class ReactNativeConfig;
} // namespace facebook::react

namespace Microsoft::ReactNative {

class UIManagerModule;
class IViewManager;
class NativeUIManager;

REACT_MODULE(FabricUIManager)
struct FabricUIManager final : public std::enable_shared_from_this<FabricUIManager>,
                               facebook::react::SchedulerDelegate {
  FabricUIManager();
  ~FabricUIManager();

  static std::shared_ptr<FabricUIManager> FromProperties(const winrt::Microsoft::ReactNative::ReactPropertyBag &props);

  REACT_INIT(Initialize)
  void Initialize(winrt::Microsoft::ReactNative::ReactContext const &reactContext) noexcept;

  void startSurface(
      facebook::react::IReactRootView *rootview,
      facebook::react::SurfaceId surfaceId,
      const std::string &moduleName,
      const folly::dynamic &initialProps) noexcept;

  void constraintSurfaceLayout(
      facebook::react::SurfaceId surfaceId,
      const facebook::react::LayoutConstraints &layoutConstraints,
      const facebook::react::LayoutContext &layoutContext) const noexcept;

  const ComponentViewRegistry &GetViewRegistry() const noexcept;

 private:
  void installFabricUIManager() noexcept;
  void initiateTransaction(facebook::react::MountingCoordinator::Shared const &mountingCoordinator);
  void performTransaction(facebook::react::MountingCoordinator::Shared const &mountingCoordinator);
  void RCTPerformMountInstructions(
      facebook::react::ShadowViewMutationList const &mutations,
      // facebook::react::RCTComponentViewRegistry* registry,
      // facebook::react::RCTMountingTransactionObserverCoordinator& observerCoordinator,
      facebook::react::SurfaceId surfaceId);
  void didMountComponentsWithRootTag(facebook::react::SurfaceId surfaceId) noexcept;

  // std::shared_ptr<facebook::react::MessageQueueThread> m_batchingUIMessageQueue;
  // std::shared_ptr<UIManagerModule> m_module;
  winrt::Microsoft::ReactNative::ReactContext m_context;
  std::shared_ptr<facebook::react::Scheduler> m_scheduler;
  std::mutex m_schedulerMutex; // Protect m_scheduler
  bool m_collapseDeleteCreateMountingInstructions{false};
  bool m_disablePreallocateViews{false};
  bool m_transactionInFlight{false};
  bool m_followUpTransactionRequired{false};

  ComponentViewRegistry m_registry;

  std::unordered_map<facebook::react::SurfaceId, XamlView> m_surfaceRegistry;

  std::shared_ptr<const facebook::react::ReactNativeConfig> m_reactNativeConfig;

  // Inherited via SchedulerDelegate
  virtual void schedulerDidFinishTransaction(
      facebook::react::MountingCoordinator::Shared const &mountingCoordinator) override;
  virtual void schedulerDidRequestPreliminaryViewAllocation(
      facebook::react::SurfaceId surfaceId,
      const facebook::react::ShadowView &shadowView) override;
  virtual void schedulerDidDispatchCommand(
      const facebook::react::ShadowView &shadowView,
      std::string const &commandName,
      folly::dynamic const args) override;
  virtual void schedulerDidSetJSResponder(
      facebook::react::SurfaceId surfaceId,
      const facebook::react::ShadowView &shadowView,
      const facebook::react::ShadowView &initialShadowView,
      bool blockNativeResponder) override;
  virtual void schedulerDidClearJSResponder() override;
  virtual void schedulerDidSendAccessibilityEvent(
      const facebook::react::ShadowView &shadowView,
      std::string const &eventType) override;
};

} // namespace Microsoft::ReactNative
