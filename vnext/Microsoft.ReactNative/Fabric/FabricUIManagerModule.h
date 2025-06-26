// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.
#pragma once

#include <Fabric/Composition/CompositionHelpers.h>
#include <NativeModules.h>
#include <React.h>
#include <react/renderer/scheduler/SchedulerDelegate.h>
#include <winrt/Windows.UI.Composition.h>
#include "Composition/ComponentViewRegistry.h"

namespace facebook::react {
class Scheduler;
class ReactNativeConfig;
class SurfaceHandler;
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
      const winrt::Microsoft::ReactNative::ReactNativeIsland &rootView,
      facebook::react::SurfaceId surfaceId,
      const facebook::react::LayoutConstraints &layoutConstraints,
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

  void setProps(facebook::react::SurfaceId surfaceId, const folly::dynamic &props) const noexcept;

  const IComponentViewRegistry &GetViewRegistry() const noexcept;

  static winrt::Microsoft::ReactNative::ReactNotificationId<facebook::react::SurfaceId> NotifyMountedId() noexcept;

 private:
  void installFabricUIManager() noexcept;
  void initiateTransaction(std::shared_ptr<const facebook::react::MountingCoordinator> mountingCoordinator);
  void performTransaction(std::shared_ptr<const facebook::react::MountingCoordinator> const &mountingCoordinator);
  void RCTPerformMountInstructions(
      facebook::react::ShadowViewMutationList const &mutations,
      // facebook::react::RCTComponentViewRegistry* registry,
      // facebook::react::RCTMountingTransactionObserverCoordinator& observerCoordinator,
      facebook::react::SurfaceId surfaceId);
  void didMountComponentsWithRootTag(facebook::react::SurfaceId surfaceId) noexcept;

  void visit(
      facebook::react::SurfaceId surfaceId,
      const std::function<void(const facebook::react::SurfaceHandler &surfaceHandler)> &callback) const noexcept;

  winrt::Microsoft::ReactNative::ReactContext m_context;
  winrt::Microsoft::ReactNative::Composition::Experimental::ICompositionContext m_compContext;
  std::shared_ptr<facebook::react::Scheduler> m_scheduler;
  std::mutex m_schedulerMutex; // Protect m_scheduler
  bool m_transactionInFlight{false};
  bool m_followUpTransactionRequired{false};

  ComponentViewRegistry m_registry;
  struct SurfaceInfo {
    winrt::weak_ref<winrt::Microsoft::ReactNative::ReactNativeIsland> wkRootView{nullptr};
  };

  std::unordered_map<facebook::react::SurfaceId, SurfaceInfo> m_surfaceRegistry;

  std::unordered_map<facebook::react::SurfaceId, facebook::react::SurfaceHandler> m_handlerRegistry{};
  mutable std::shared_mutex m_handlerMutex;

  // Inherited via SchedulerDelegate
  virtual void schedulerDidFinishTransaction(
      const std::shared_ptr<const facebook::react::MountingCoordinator> &mountingCoordinator) override;
  virtual void schedulerShouldRenderTransactions(
      const std::shared_ptr<const facebook::react::MountingCoordinator> &mountingCoordinator) override;
  virtual void schedulerDidRequestPreliminaryViewAllocation(const facebook::react::ShadowNode &shadowView) override;
  virtual void schedulerDidDispatchCommand(
      facebook::react::ShadowView const &shadowView,
      std::string const &commandName,
      folly::dynamic const &arg) override;
  virtual void schedulerDidSetIsJSResponder(
      facebook::react::ShadowView const &shadowView,
      bool isJSResponder,
      bool blockNativeResponder) override;
  virtual void schedulerShouldSynchronouslyUpdateViewOnUIThread(facebook::react::Tag tag,
      const folly::dynamic &props) override;
  virtual void schedulerDidSendAccessibilityEvent(
      const facebook::react::ShadowView &shadowView,
      std::string const &eventType) override;
};

} // namespace Microsoft::ReactNative
