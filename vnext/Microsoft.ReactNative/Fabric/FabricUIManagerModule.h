// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.
#pragma once

#include <NativeModules.h>
#include <React.h>
#include <react/renderer/scheduler/SchedulerDelegate.h>
#include <react/renderer/scheduler/SurfaceManager.h>
#include "ComponentViewRegistry.h"

namespace facebook::react {
class Scheduler;
class ReactNativeConfig;
} // namespace facebook::react

namespace Microsoft::ReactNative {

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

  void stopSurface(facebook::react::SurfaceId surfaceId) noexcept;

  void constraintSurfaceLayout(
      facebook::react::SurfaceId surfaceId,
      const facebook::react::LayoutConstraints &layoutConstraints,
      const facebook::react::LayoutContext &layoutContext) const noexcept;

  facebook::react::Size measureSurface(
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

  winrt::Microsoft::ReactNative::ReactContext m_context;
  std::shared_ptr<facebook::react::Scheduler> m_scheduler;
  std::shared_ptr<facebook::react::SurfaceManager> m_surfaceManager;
  std::mutex m_schedulerMutex; // Protect m_scheduler
  bool m_transactionInFlight{false};
  bool m_followUpTransactionRequired{false};

  ComponentViewRegistry m_registry;
  std::unordered_map<facebook::react::SurfaceId, XamlView> m_surfaceRegistry;

  // Inherited via SchedulerDelegate
  virtual void schedulerDidFinishTransaction(
      facebook::react::MountingCoordinator::Shared const &mountingCoordinator) override;
  virtual void schedulerDidRequestPreliminaryViewAllocation(
      facebook::react::SurfaceId surfaceId,
      const facebook::react::ShadowNode &shadowView) override;
  virtual void schedulerDidCloneShadowNode(
      facebook::react::SurfaceId surfaceId,
      const facebook::react::ShadowNode &oldShadowNode,
      const facebook::react::ShadowNode &newShadowNode) override;
  virtual void schedulerDidDispatchCommand(
      const facebook::react::ShadowView &shadowView,
      std::string const &commandName,
      folly::dynamic const args) override;
  virtual void schedulerDidSetIsJSResponder(
      facebook::react::ShadowView const &shadowView,
      bool isJSResponder,
      bool blockNativeResponder) override;
  virtual void schedulerDidSendAccessibilityEvent(
      const facebook::react::ShadowView &shadowView,
      std::string const &eventType) override;
};

} // namespace Microsoft::ReactNative
