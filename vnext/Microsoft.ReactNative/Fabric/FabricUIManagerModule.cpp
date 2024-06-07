// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"
#include <AsynchronousEventBeat.h>
#include <DynamicReader.h>
#include <DynamicWriter.h>
#include <Fabric/ComponentView.h>
#include <Fabric/Composition/CompositionUIService.h>
#include <Fabric/Composition/CompositionViewComponentView.h>
#include <Fabric/Composition/RootComponentView.h>
#include <Fabric/FabricUIManagerModule.h>
#include <Fabric/ReactNativeConfigProperties.h>
#include <Fabric/WindowsComponentDescriptorRegistry.h>
#include <IReactContext.h>
#include <IReactRootView.h>
#include <JSI/jsi.h>
#include <ReactCommon/RuntimeExecutor.h>
#include <SchedulerSettings.h>
#include <SynchronousEventBeat.h>
#include <UI.Xaml.Controls.h>
#include <react/components/rnwcore/ComponentDescriptors.h>
#include <react/renderer/componentregistry/ComponentDescriptorProviderRegistry.h>
#include <react/renderer/core/DynamicPropsUtilities.h>
#include <react/renderer/core/EventBeat.h>
#include <react/renderer/runtimescheduler/RuntimeScheduler.h>
#include <react/renderer/scheduler/Scheduler.h>
#include <react/renderer/scheduler/SchedulerToolbox.h>
#include <react/utils/ContextContainer.h>
#include <react/utils/CoreFeatures.h>
#include <winrt/Windows.Graphics.Display.h>
#include <winrt/Windows.UI.Composition.Desktop.h>
#include "DynamicReader.h"
#include "Unicode.h"

namespace Microsoft::ReactNative {

winrt::Microsoft::ReactNative::ReactPropertyId<
    winrt::Microsoft::ReactNative::ReactNonAbiValue<std::shared_ptr<FabricUIManager>>>
FabicUIManagerProperty() noexcept {
  winrt::Microsoft::ReactNative::ReactPropertyId<
      winrt::Microsoft::ReactNative::ReactNonAbiValue<std::shared_ptr<FabricUIManager>>>
      propId{L"ReactNative.Fabric", L"UIManager"};
  return propId;
}

/*static*/ std::shared_ptr<FabricUIManager> FabricUIManager::FromProperties(
    const winrt::Microsoft::ReactNative::ReactPropertyBag &props) {
  return props.Get(FabicUIManagerProperty()).Value();
}

FabricUIManager::FabricUIManager() {
  facebook::react::CoreFeatures::enablePropIteratorSetter = true;
}

FabricUIManager::~FabricUIManager() {
  // Make sure that we destroy UI components on UI thread.
  if (!m_context.UIDispatcher().HasThreadAccess()) {
    m_context.UIDispatcher().Post([registry = std::move(m_registry)]() {});
  }
}

void FabricUIManager::installFabricUIManager() noexcept {
  std::shared_ptr<const facebook::react::ReactNativeConfig> config =
      std::make_shared<const ReactNativeConfigProperties>(m_context.Properties());

  std::lock_guard<std::mutex> schedulerLock(m_schedulerMutex);

  facebook::react::ContextContainer::Shared contextContainer = std::make_shared<facebook::react::ContextContainer>();

  // This allows access to our ReactContext from the contextContainer thats passed around the fabric codebase
  contextContainer->insert("MSRN.ReactContext", m_context);

  facebook::react::RuntimeExecutor runtimeExecutor;
  auto toolbox = facebook::react::SchedulerToolbox{};

  if (auto runtimeScheduler = SchedulerSettings::RuntimeSchedulerFromProperties(m_context.Properties())) {
    contextContainer->insert("RuntimeScheduler", runtimeScheduler);
    runtimeExecutor = [runtimeScheduler](std::function<void(facebook::jsi::Runtime & runtime)> &&callback) {
      runtimeScheduler->scheduleWork(std::move(callback));
    };
  } else {
    runtimeExecutor = SchedulerSettings::GetRuntimeExecutor(m_context.Properties());
  }

  facebook::react::EventBeat::Factory asynchronousBeatFactory =
      [runtimeExecutor, context = m_context](facebook::react::EventBeat::SharedOwnerBox const &ownerBox) {
        return std::make_unique<AsynchronousEventBeat>(ownerBox, context, runtimeExecutor);
      };

  contextContainer->insert("ReactNativeConfig", config);

  toolbox.contextContainer = contextContainer;
  toolbox.componentRegistryFactory = [](facebook::react::EventDispatcher::Weak const &eventDispatcher,
                                        facebook::react::ContextContainer::Shared const &contextContainer)
      -> facebook::react::ComponentDescriptorRegistry::Shared {
    auto providerRegistry =
        WindowsComponentDescriptorRegistry::FromProperties(
            contextContainer->at<winrt::Microsoft::ReactNative::ReactContext>("MSRN.ReactContext").Properties())
            ->GetProviderRegistry();

    auto registry = providerRegistry->createComponentDescriptorRegistry({eventDispatcher, contextContainer});

    auto mutableRegistry = std::const_pointer_cast<facebook::react::ComponentDescriptorRegistry>(registry);
    mutableRegistry->setFallbackComponentDescriptor(
        std::make_shared<facebook::react::UnimplementedNativeViewComponentDescriptor>(
            facebook::react::ComponentDescriptorParameters{eventDispatcher, contextContainer, nullptr}));
    return registry;
  };
  toolbox.runtimeExecutor = runtimeExecutor;
  toolbox.asynchronousEventBeatFactory = asynchronousBeatFactory;
  toolbox.backgroundExecutor = [context = m_context,
                                dispatcher = Mso::DispatchQueue::MakeLooperQueue()](std::function<void()> &&callback) {
    if (context.UIDispatcher().HasThreadAccess()) {
      callback();
      return;
    }

    dispatcher.Post(std::move(callback));
  };

  m_scheduler = std::make_shared<facebook::react::Scheduler>(
      toolbox, (/*animationDriver_ ? animationDriver_.get() :*/ nullptr), this);
  m_surfaceManager = std::make_shared<facebook::react::SurfaceManager>(*m_scheduler);
}

const IComponentViewRegistry &FabricUIManager::GetViewRegistry() const noexcept {
  return m_registry;
}

void FabricUIManager::startSurface(
    const winrt::Microsoft::ReactNative::ReactNativeIsland &rootView,
    facebook::react::SurfaceId surfaceId,
    const facebook::react::LayoutConstraints &layoutConstraints,
    const std::string &moduleName,
    const folly::dynamic &initialProps) noexcept {
  m_surfaceRegistry.insert({surfaceId, {rootView}});

  m_context.UIDispatcher().Post([self = shared_from_this(), surfaceId, rootView]() {
    auto &rootComponentViewDescriptor = self->m_registry.dequeueComponentViewWithComponentHandle(
        facebook::react::RootShadowNode::Handle(), surfaceId, self->m_compContext);

    auto root = rootComponentViewDescriptor.view
                    .as<winrt::Microsoft::ReactNative::Composition::implementation::RootComponentView>();
    root->theme(winrt::get_self<winrt::Microsoft::ReactNative::Composition::implementation::Theme>(rootView.Theme()));
    root->start(rootView);
  });

  facebook::react::LayoutContext layoutContext;
  layoutContext.pointScaleFactor = rootView.ScaleFactor();
  layoutContext.fontSizeMultiplier = rootView.ScaleFactor();

  m_surfaceManager->startSurface(
      surfaceId,
      moduleName,
      initialProps,
      layoutConstraints,
      layoutContext // layout context
  );
}

void FabricUIManager::stopSurface(facebook::react::SurfaceId surfaceId) noexcept {
  m_surfaceManager->stopSurface(surfaceId);
}

winrt::Microsoft::ReactNative::ReactNativeIsland FabricUIManager::GetReactNativeIsland(
    facebook::react::SurfaceId surfaceId) const noexcept {
  return m_surfaceRegistry.at(surfaceId).wkRootView.get();
}

facebook::react::Size FabricUIManager::measureSurface(
    facebook::react::SurfaceId surfaceId,
    const facebook::react::LayoutConstraints &layoutConstraints,
    const facebook::react::LayoutContext &layoutContext) const noexcept {
  return m_surfaceManager->measureSurface(surfaceId, layoutConstraints, layoutContext);
}

void FabricUIManager::constraintSurfaceLayout(
    facebook::react::SurfaceId surfaceId,
    const facebook::react::LayoutConstraints &layoutConstraints,
    const facebook::react::LayoutContext &layoutContext) const noexcept {
  m_surfaceManager->constraintSurfaceLayout(surfaceId, layoutConstraints, layoutContext);
}

void FabricUIManager::didMountComponentsWithRootTag(facebook::react::SurfaceId surfaceId) noexcept {}

void FabricUIManager::RCTPerformMountInstructions(
    facebook::react::ShadowViewMutationList const &mutations,
    // facebook::react::RCTComponentViewRegistry* registry,
    // facebook::react::RCTMountingTransactionObserverCoordinator& observerCoordinator,
    facebook::react::SurfaceId surfaceId) {
  for (auto const &mutation : mutations) {
    switch (mutation.type) {
      case facebook::react::ShadowViewMutation::Create: {
        auto &newChildShadowView = mutation.newChildShadowView;
        auto &newChildViewDescriptor = m_registry.dequeueComponentViewWithComponentHandle(
            newChildShadowView.componentHandle, newChildShadowView.tag, m_compContext);
        // observerCoordinator.registerViewComponentDescriptor(newChildViewDescriptor, surfaceId);
        break;
      }

      case facebook::react::ShadowViewMutation::Delete: {
        auto &oldChildShadowView = mutation.oldChildShadowView;
        auto &oldChildViewDescriptor = m_registry.componentViewDescriptorWithTag(oldChildShadowView.tag);
        // observerCoordinator.unregisterViewComponentDescriptor(oldChildViewDescriptor, surfaceId);
        m_registry.enqueueComponentViewWithComponentHandle(
            oldChildShadowView.componentHandle, oldChildShadowView.tag, oldChildViewDescriptor);
        break;
      }

      case facebook::react::ShadowViewMutation::Insert: {
        auto &oldChildShadowView = mutation.oldChildShadowView;
        auto &newChildShadowView = mutation.newChildShadowView;
        auto &parentShadowView = mutation.parentShadowView;
        auto &newChildViewDescriptor = m_registry.componentViewDescriptorWithTag(newChildShadowView.tag);
        auto parentViewDescriptor = m_registry.componentViewDescriptorWithTag(parentShadowView.tag);
        auto newChildComponentView =
            winrt::get_self<winrt::Microsoft::ReactNative::implementation::ComponentView>(newChildViewDescriptor.view);

        newChildComponentView->updateProps(newChildShadowView.props, oldChildShadowView.props);
        newChildComponentView->updateEventEmitter(newChildShadowView.eventEmitter);
        newChildComponentView->updateState(newChildShadowView.state, oldChildShadowView.state);
        newChildComponentView->updateLayoutMetrics(newChildShadowView.layoutMetrics, oldChildShadowView.layoutMetrics);
        newChildViewDescriptor.view.FinalizeUpdates(winrt::Microsoft::ReactNative::ComponentViewUpdateMask::All);

        parentViewDescriptor.view.MountChildComponentView(*newChildComponentView, mutation.index);
        break;
      }

      case facebook::react::ShadowViewMutation::Remove: {
        auto &oldChildShadowView = mutation.oldChildShadowView;
        auto &parentShadowView = mutation.parentShadowView;
        auto &oldChildViewDescriptor = m_registry.componentViewDescriptorWithTag(oldChildShadowView.tag);
        auto &parentViewDescriptor = m_registry.componentViewDescriptorWithTag(parentShadowView.tag);
        parentViewDescriptor.view.UnmountChildComponentView(oldChildViewDescriptor.view, mutation.index);
        break;
      }

      case facebook::react::ShadowViewMutation::Update: {
        auto &oldChildShadowView = mutation.oldChildShadowView;
        auto &newChildShadowView = mutation.newChildShadowView;
        auto &newChildViewDescriptor = m_registry.componentViewDescriptorWithTag(newChildShadowView.tag);
        auto newChildComponentView =
            winrt::get_self<winrt::Microsoft::ReactNative::implementation::ComponentView>(newChildViewDescriptor.view);
        auto mask = winrt::Microsoft::ReactNative::ComponentViewUpdateMask::None;

        if (oldChildShadowView.props != newChildShadowView.props) {
          newChildComponentView->updateProps(newChildShadowView.props, oldChildShadowView.props);
          mask |= winrt::Microsoft::ReactNative::ComponentViewUpdateMask::Props;
        }

        if (oldChildShadowView.eventEmitter != newChildShadowView.eventEmitter) {
          newChildComponentView->updateEventEmitter(newChildShadowView.eventEmitter);
          mask |= winrt::Microsoft::ReactNative::ComponentViewUpdateMask::EventEmitter;
        }

        if (oldChildShadowView.state != newChildShadowView.state) {
          newChildComponentView->updateState(newChildShadowView.state, oldChildShadowView.state);
          mask |= winrt::Microsoft::ReactNative::ComponentViewUpdateMask::State;
        }

        if (oldChildShadowView.layoutMetrics != newChildShadowView.layoutMetrics) {
          newChildComponentView->updateLayoutMetrics(
              newChildShadowView.layoutMetrics, oldChildShadowView.layoutMetrics);
          mask |= winrt::Microsoft::ReactNative::ComponentViewUpdateMask::LayoutMetrics;
        }

        if (mask != winrt::Microsoft::ReactNative::ComponentViewUpdateMask::None) {
          newChildViewDescriptor.view.FinalizeUpdates(mask);
        }

        break;
      }
    }
  }
}

void FabricUIManager::performTransaction(facebook::react::MountingCoordinator::Shared const &mountingCoordinator) {
  auto surfaceId = mountingCoordinator->getSurfaceId();

  mountingCoordinator->getTelemetryController().pullTransaction(
      [&](facebook::react::MountingTransaction const &transaction,
          facebook::react::SurfaceTelemetry const &surfaceTelemetry) {
        //[self.delegate mountingManager:self willMountComponentsWithRootTag:surfaceId];
        // _observerCoordinator.notifyObserversMountingTransactionWillMount(transaction, surfaceTelemetry);
      },
      [&](facebook::react::MountingTransaction const &transaction,
          facebook::react::SurfaceTelemetry const &surfaceTelemetry) {
        RCTPerformMountInstructions(
            transaction.getMutations(), /* _componentViewRegistry, _observerCoordinator,*/ surfaceId);
      },
      [&](facebook::react::MountingTransaction const &transaction,
          facebook::react::SurfaceTelemetry const &surfaceTelemetry) {
        //_observerCoordinator.notifyObserversMountingTransactionDidMount(transaction, surfaceTelemetry);
        didMountComponentsWithRootTag(surfaceId);
        //[self.delegate mountingManager:self didMountComponentsWithRootTag:surfaceId];
      });
}

void FabricUIManager::initiateTransaction(facebook::react::MountingCoordinator::Shared mountingCoordinator) {
  if (m_transactionInFlight) {
    m_followUpTransactionRequired = true;
    return;
  }

  do {
    m_followUpTransactionRequired = false;
    m_transactionInFlight = true;
    performTransaction(mountingCoordinator);
    m_transactionInFlight = false;
  } while (m_followUpTransactionRequired);
}

void FabricUIManager::schedulerDidFinishTransaction(
    const facebook::react::MountingCoordinator::Shared &mountingCoordinator) {
  // Should cache this locally

  if (m_context.UIDispatcher().HasThreadAccess()) {
    initiateTransaction(mountingCoordinator);
  } else {
    m_context.UIDispatcher().Post(
        [mountingCoordinator, self = shared_from_this()]() { self->initiateTransaction(mountingCoordinator); });
  }
}

void FabricUIManager::schedulerShouldRenderTransactions(
    const facebook::react::MountingCoordinator::Shared &mountingCoordinator) {
  if (m_context.UIDispatcher().HasThreadAccess()) {
    initiateTransaction(mountingCoordinator);
  } else {
    m_context.UIDispatcher().Post(
        [mountingCoordinator, self = shared_from_this()]() { self->initiateTransaction(mountingCoordinator); });
  }
}

void FabricUIManager::schedulerDidRequestPreliminaryViewAllocation(const facebook::react::ShadowNode &shadowView) {
  // iOS does not do this optimization, but Android does.  It maybe that android's allocations are more expensive due to
  // the Java boundary.
  // TODO: We should do some perf tests to see if this is worth doing.

  /* -- No sense doing this until we recycle views... since this just throws away the view currently
  if (m_context.UIDispatcher().HasThreadAccess()) {
    m_registry.dequeueComponentViewWithComponentHandle(shadowView.getComponentHandle(), surfaceId, m_compContext);
  } else {
    m_context.UIDispatcher().Post(
        [componentHandle = shadowView.getComponentHandle(), surfaceId, self = shared_from_this()]() {
          self->m_registry.dequeueComponentViewWithComponentHandle(componentHandle, surfaceId, self->m_compContext);
        });
  }
  */
}

void FabricUIManager::schedulerDidDispatchCommand(
    facebook::react::ShadowView const &shadowView,
    std::string const &commandName,
    folly::dynamic const &arg) {
  if (m_context.UIDispatcher().HasThreadAccess()) {
    auto descriptor = m_registry.componentViewDescriptorWithTag(shadowView.tag);
    descriptor.view.HandleCommand(
        winrt::to_hstring(commandName), winrt::make<winrt::Microsoft::ReactNative::DynamicReader>(arg));
  } else {
    m_context.UIDispatcher().Post(
        [wkThis = weak_from_this(), commandName, tag = shadowView.tag, args = folly::dynamic(arg)]() {
          if (auto pThis = wkThis.lock()) {
            auto view = pThis->m_registry.findComponentViewWithTag(tag);
            if (view) {
              view.HandleCommand(
                  winrt::to_hstring(commandName), winrt::make<winrt::Microsoft::ReactNative::DynamicReader>(args));
            }
          }
        });
  }
}

void FabricUIManager::schedulerDidSetIsJSResponder(
    facebook::react::ShadowView const &shadowView,
    bool isJSResponder,
    bool blockNativeResponder) {}

void FabricUIManager::schedulerDidSendAccessibilityEvent(
    const facebook::react::ShadowView &shadowView,
    std::string const &eventType) {
  assert(false);
}

void FabricUIManager::Initialize(winrt::Microsoft::ReactNative::ReactContext const &reactContext) noexcept {
  m_context = reactContext;

  m_compContext =
      winrt::Microsoft::ReactNative::Composition::implementation::CompositionUIService::GetCompositionContext(
          reactContext.Properties().Handle());

  m_registry.Initialize(reactContext);

  m_context.Properties().Set(FabicUIManagerProperty(), shared_from_this());

  /*
  EventBeatManager eventBeatManager = new EventBeatManager(mReactApplicationContext);
  UIManagerModule nativeModule =
    Assertions.assertNotNull(mReactApplicationContext.getNativeModule(UIManagerModule.class));
  EventDispatcher eventDispatcher = nativeModule.getEventDispatcher();
  FabricUIManager uiManager =
    new FabricUIManager(
      mReactApplicationContext,
      nativeModule.getViewManagerRegistry_DO_NOT_USE(),
      eventDispatcher,
      eventBeatManager);

  Binding binding = new Binding();
  // TODO T31905686: remove this call
  loadClasses();
  MessageQueueThread jsMessageQueueThread =
    mReactApplicationContext
      .getCatalystInstance()
      .getReactQueueConfiguration()
      .getJSQueueThread();
  */

  // binding.register( // This is register in java, which calls into Binding::installFabricUIManager
  installFabricUIManager(
      /*
        mReactApplicationContext.getCatalystInstance().getRuntimeExecutor(),
        uiManager,
        eventBeatManager,
        jsMessageQueueThread,
        mComponentFactory,
        mConfig
        */);
  /*
  Systrace.endSection(Systrace.TRACE_TAG_REACT_JAVA_BRIDGE);
  return uiManager;

*/
}

} // namespace Microsoft::ReactNative
