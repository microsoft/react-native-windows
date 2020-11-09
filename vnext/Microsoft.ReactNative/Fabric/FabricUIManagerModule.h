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

  REACT_SYNC_METHOD(getConstantsForViewManager)
  winrt::Microsoft::ReactNative::JSValueObject getConstantsForViewManager(std::string viewManagerName) noexcept;

  // Not part of the spec, but core polyfils this on the JS side.
  REACT_SYNC_METHOD(getViewManagerConfig)
  winrt::Microsoft::ReactNative::JSValueObject getViewManagerConfig(std::string viewManagerName) noexcept;

  REACT_SYNC_METHOD(getDefaultEventTypes)
  winrt::Microsoft::ReactNative::JSValueArray getDefaultEventTypes() noexcept;

  REACT_SYNC_METHOD(lazilyLoadView)
  winrt::Microsoft::ReactNative::JSValueObject lazilyLoadView(std::string name) noexcept;

  REACT_METHOD(createView)
  void createView(
      double reactTag, // How come these cannot be int64_t?
      std::string viewName,
      double rootTag,
      winrt::Microsoft::ReactNative::JSValueObject &&props) noexcept;

  REACT_METHOD(updateView)
  void updateView(double reactTag, std::string viewName, winrt::Microsoft::ReactNative::JSValueObject &&props) noexcept;

  REACT_METHOD(focus)
  void focus(double reactTag) noexcept;

  REACT_METHOD(blur)
  void blur(double reactTag) noexcept;

  REACT_METHOD(findSubviewIn)
  void findSubviewIn(
      double reactTag,
      winrt::Microsoft::ReactNative::JSValueArray &&point,
      std::function<void(double nativeViewTag, double left, double top, double width, double height)> const
          &callback) noexcept;

  // The spec is incorrect in that it specifies a number for the command ID, but its actually a number or a string
  REACT_METHOD(dispatchViewManagerCommand)
  void dispatchViewManagerCommand(
      double reactTag,
      winrt::Microsoft::ReactNative::JSValue &&commandID,
      winrt::Microsoft::ReactNative::JSValueArray &&commandArgs) noexcept;

  REACT_METHOD(measure)
  void measure(
      double reactTag,
      std::function<void(double left, double top, double width, double height, double pageX, double pageY)> const
          &callback) noexcept;

  REACT_METHOD(measureInWindow)
  void measureInWindow(
      double reactTag,
      std::function<void(double x, double y, double width, double height)> const &callback) noexcept;

  REACT_METHOD(viewIsDescendantOf)
  void viewIsDescendantOf(
      double reactTag,
      double ancestorReactTag,
      std::function<void(winrt::Microsoft::ReactNative::JSValue const &)> const &callback) noexcept;

  REACT_METHOD(measureLayout)
  void measureLayout(
      double reactTag,
      double ancestorReactTag,
      std::function<void(winrt::Microsoft::ReactNative::JSValue const &)> const &errorCallback,
      std::function<void(double left, double top, double width, double height)> const &callback) noexcept;

  REACT_METHOD(measureLayoutRelativeToParent)
  void measureLayoutRelativeToParent(
      double reactTag,
      std::function<void(winrt::Microsoft::ReactNative::JSValue const &)> const &errorCallback,
      std::function<void(winrt::Microsoft::ReactNative::JSValue const &)> const &callback) noexcept;

  REACT_METHOD(setJSResponder)
  void setJSResponder(double reactTag, bool blockNativeResponder) noexcept;

  REACT_METHOD(clearJSResponder)
  void clearJSResponder() noexcept;

  REACT_METHOD(configureNextLayoutAnimation)
  void configureNextLayoutAnimation(
      winrt::Microsoft::ReactNative::JSValueObject &&config,
      std::function<void()> const &callback,
      std::function<void(winrt::Microsoft::ReactNative::JSValue const &)> const &errorCallback) noexcept;

  REACT_METHOD(removeSubviewsFromContainerWithID)
  void removeSubviewsFromContainerWithID(double containerID) noexcept;

  REACT_METHOD(replaceExistingNonRootView)
  void replaceExistingNonRootView(double reactTag, double newReactTag) noexcept;

  REACT_METHOD(removeRootView)
  void removeRootView(double reactTag) noexcept;

  REACT_METHOD(setChildren)
  void setChildren(double containerTag, winrt::Microsoft::ReactNative::JSValueArray &&reactTags) noexcept;

  REACT_METHOD(manageChildren)
  void manageChildren(
      double containerTag,
      winrt::Microsoft::ReactNative::JSValueArray &&moveFromIndices,
      winrt::Microsoft::ReactNative::JSValueArray &&moveToIndices,
      winrt::Microsoft::ReactNative::JSValueArray &&addChildReactTags,
      winrt::Microsoft::ReactNative::JSValueArray &&addAtIndices,
      winrt::Microsoft::ReactNative::JSValueArray &&removeAtIndices) noexcept;

  REACT_METHOD(setLayoutAnimationEnabledExperimental)
  void setLayoutAnimationEnabledExperimental(bool enabled) noexcept;

  REACT_METHOD(sendAccessibilityEvent)
  void sendAccessibilityEvent(double reactTag, double eventType) noexcept;

  REACT_METHOD(showPopupMenu)
  void showPopupMenu(
      double reactTag,
      winrt::Microsoft::ReactNative::JSValueArray &&items,
      std::function<void(winrt::Microsoft::ReactNative::JSValue const &)> const &error,
      std::function<void(winrt::Microsoft::ReactNative::JSValue const &)> const &success) noexcept;

  REACT_METHOD(dismissPopupMenu)
  void dismissPopupMenu() noexcept;

  void startSurface(
      facebook::react::IReactRootView *rootview,
      facebook::react::SurfaceId surfaceId,
      const std::string &moduleName,
      const folly::dynamic &initialProps) noexcept;

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
  // TODO how to fix m_surfaceRegistry to not hit compiler error... for now just store a single instance.. so only support one rootview
  XamlView m_surfaceRoot{nullptr};

  std::recursive_mutex m_commitMutex;

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
};

} // namespace Microsoft::ReactNative
