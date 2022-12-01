// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"
#include <DynamicReader.h>
#include <DynamicWriter.h>
#include <Fabric/ComponentView.h>
#include <Fabric/Composition/CompositionUIService.h>
#include <Fabric/Composition/CompositionViewComponentView.h>
#include <Fabric/Composition/TextInput/WindowsTextInputComponentDescriptor.h>
#include <Fabric/FabricUIManagerModule.h>
#include <Fabric/ReactNativeConfigProperties.h>
#include <ICompositionRootView.h>
#include <IReactContext.h>
#include <IReactRootView.h>
#include <JSI/jsi.h>
#include <SchedulerSettings.h>
#include <UI.Xaml.Controls.h>
#include <react/components/rnwcore/ComponentDescriptors.h>
#include <react/renderer/componentregistry/ComponentDescriptorProviderRegistry.h>
#include <react/renderer/components/image/ImageComponentDescriptor.h>
#include <react/renderer/components/text/ParagraphComponentDescriptor.h>
#include <react/renderer/components/text/RawTextComponentDescriptor.h>
#include <react/renderer/components/text/TextComponentDescriptor.h>
#include <react/renderer/components/view/ViewComponentDescriptor.h>
#include <react/renderer/core/EventBeat.h>
#include <react/renderer/scheduler/Scheduler.h>
#include <react/renderer/scheduler/SchedulerToolbox.h>
#include <react/utils/ContextContainer.h>
#include <runtimeexecutor/ReactCommon/RuntimeExecutor.h>
#include <winrt/Windows.Graphics.Display.h>
#include <winrt/Windows.UI.Composition.Desktop.h>
#include "Unicode.h"

#pragma warning(push)
#pragma warning(disable : 4305)
#include <react/renderer/components/scrollview/ScrollViewComponentDescriptor.h>
#pragma warning(pop)

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

FabricUIManager::FabricUIManager() {}

FabricUIManager::~FabricUIManager() {}

class AsyncEventBeat final : public facebook::react::EventBeat {
 public:
  AsyncEventBeat(
      facebook::react::EventBeat::SharedOwnerBox const &ownerBox,
      const winrt::Microsoft::ReactNative::ReactContext &context,
      facebook::react::RuntimeExecutor runtimeExecutor)
      : EventBeat(ownerBox), m_context(context), m_runtimeExecutor(runtimeExecutor) {}

  void induce() const override {
    if (!isRequested_ || m_isBeatCallbackScheduled) {
      isRequested_ = false;
      return;
    }
    isRequested_ = false;
    m_isBeatCallbackScheduled = true;

    m_runtimeExecutor([this, ownerBox = ownerBox_](jsi::Runtime &runtime) {
      auto owner = ownerBox->owner.lock();
      if (!owner) {
        return;
      }

      m_isBeatCallbackScheduled = false;
      if (beatCallback_) {
        beatCallback_(runtime);
      }
    });
  }

  void request() const override {
    bool alreadyRequested = isRequested_;
    EventBeat::request();
    if (!alreadyRequested) {
      m_context.UIDispatcher().Post([this, ownerBox = ownerBox_]() {
        auto owner = ownerBox->owner.lock();
        if (!owner) {
          return;
        }
        induce();
      });
    }
  }

 private:
  mutable std::atomic<bool> m_isBeatCallbackScheduled{false};
  winrt::Microsoft::ReactNative::ReactContext m_context;
  facebook::react::RuntimeExecutor m_runtimeExecutor;
};

std::shared_ptr<facebook::react::ComponentDescriptorProviderRegistry const> sharedProviderRegistry() {
  static auto providerRegistry = []() -> std::shared_ptr<facebook::react::ComponentDescriptorProviderRegistry> {
    auto providerRegistry = std::make_shared<facebook::react::ComponentDescriptorProviderRegistry>();
    providerRegistry->add(facebook::react::concreteComponentDescriptorProvider<
                          facebook::react::ActivityIndicatorViewComponentDescriptor>());
    providerRegistry->add(
        facebook::react::concreteComponentDescriptorProvider<facebook::react::ImageComponentDescriptor>());
    providerRegistry->add(
        facebook::react::concreteComponentDescriptorProvider<facebook::react::ParagraphComponentDescriptor>());
    providerRegistry->add(
        facebook::react::concreteComponentDescriptorProvider<facebook::react::RawTextComponentDescriptor>());
    providerRegistry->add(
        facebook::react::concreteComponentDescriptorProvider<facebook::react::ScrollViewComponentDescriptor>());
    providerRegistry->add(
        facebook::react::concreteComponentDescriptorProvider<facebook::react::SwitchComponentDescriptor>());
    providerRegistry->add(
        facebook::react::concreteComponentDescriptorProvider<facebook::react::TextComponentDescriptor>());
    providerRegistry->add(
        facebook::react::concreteComponentDescriptorProvider<facebook::react::ViewComponentDescriptor>());
    providerRegistry->add(
        facebook::react::concreteComponentDescriptorProvider<facebook::react::WindowsTextInputComponentDescriptor>());
    return providerRegistry;
  }();

  return providerRegistry;
}

void FabricUIManager::installFabricUIManager() noexcept {
  std::shared_ptr<const facebook::react::ReactNativeConfig> config =
      std::make_shared<const ReactNativeConfigProperties>(m_context);

  std::lock_guard<std::mutex> schedulerLock(m_schedulerMutex);

  facebook::react::ContextContainer::Shared contextContainer = std::make_shared<facebook::react::ContextContainer>();

  // This allows access to our ReactContext from the contextContainer thats passed around the fabric codebase
  contextContainer->insert("MSRN.ReactContext", m_context);

  auto runtimeExecutor = SchedulerSettings::GetRuntimeExecutor(m_context.Properties());

  facebook::react::EventBeat::Factory synchronousBeatFactory =
      [runtimeExecutor, context = m_context](facebook::react::EventBeat::SharedOwnerBox const &ownerBox) {
        return std::make_unique<AsyncEventBeat>(ownerBox, context, runtimeExecutor);
      };

  facebook::react::EventBeat::Factory asynchronousBeatFactory =
      [runtimeExecutor, context = m_context](facebook::react::EventBeat::SharedOwnerBox const &ownerBox) {
        return std::make_unique<AsyncEventBeat>(ownerBox, context, runtimeExecutor);
      };

  contextContainer->insert("ReactNativeConfig", config);

  auto toolbox = facebook::react::SchedulerToolbox{};
  toolbox.contextContainer = contextContainer;
  toolbox.componentRegistryFactory = [](facebook::react::EventDispatcher::Weak const &eventDispatcher,
                                        facebook::react::ContextContainer::Shared const &contextContainer)
      -> facebook::react::ComponentDescriptorRegistry::Shared {
    auto registry = sharedProviderRegistry()->createComponentDescriptorRegistry({eventDispatcher, contextContainer});
    auto mutableRegistry = std::const_pointer_cast<facebook::react::ComponentDescriptorRegistry>(registry);
    mutableRegistry->setFallbackComponentDescriptor(
        std::make_shared<facebook::react::UnimplementedNativeViewComponentDescriptor>(
            facebook::react::ComponentDescriptorParameters{eventDispatcher, contextContainer, nullptr}));
    return registry;
  };
  toolbox.runtimeExecutor = runtimeExecutor;
  toolbox.synchronousEventBeatFactory = synchronousBeatFactory;
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
    facebook::react::IReactRootView *rootview,
    facebook::react::SurfaceId surfaceId,
    const std::string &moduleName,
    const folly::dynamic &initialProps) noexcept {
  auto CompositionRootView = static_cast<ICompositionRootView *>(rootview);
  m_surfaceRegistry.insert({surfaceId, {CompositionRootView->GetVisual()}});

  m_context.UIDispatcher().Post([self = shared_from_this(), surfaceId]() {
    auto &rootComponentViewDescriptor = self->m_registry.dequeueComponentViewWithComponentHandle(
        facebook::react::RootShadowNode::Handle(), surfaceId, self->m_compContext);

    self->m_surfaceRegistry.at(surfaceId).rootVisual.InsertAt(
        static_cast<const CompositionBaseComponentView &>(*rootComponentViewDescriptor.view).Visual(), 0);
  });

  facebook::react::LayoutContext context;
  facebook::react::LayoutConstraints constraints;
  context.pointScaleFactor = static_cast<float>(CompositionRootView->ScaleFactor());
  context.fontSizeMultiplier = static_cast<float>(CompositionRootView->ScaleFactor());
  constraints.minimumSize.height = static_cast<float>(CompositionRootView->GetActualHeight());
  constraints.minimumSize.width = static_cast<float>(CompositionRootView->GetActualWidth());
  constraints.maximumSize.height = static_cast<float>(CompositionRootView->GetActualHeight());
  constraints.maximumSize.width = static_cast<float>(CompositionRootView->GetActualWidth());
  constraints.layoutDirection = facebook::react::LayoutDirection::LeftToRight;

  m_surfaceManager->startSurface(
      surfaceId,
      moduleName,
      initialProps,
      constraints, // layout constraints
      context // layout context
  );
}

void FabricUIManager::stopSurface(facebook::react::SurfaceId surfaceId) noexcept {
  m_surfaceManager->stopSurface(surfaceId);
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

struct RemoveDeleteMetadata {
  facebook::react::Tag tag;
  facebook::react::Tag parentTag;
  int index;
  bool shouldRemove;
  bool shouldDelete;
};

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
        auto &parentViewDescriptor = m_registry.componentViewDescriptorWithTag(parentShadowView.tag);

        auto &newChildComponentView = newChildViewDescriptor.view;

        newChildComponentView->updateProps(newChildShadowView.props, oldChildShadowView.props);
        newChildComponentView->updateEventEmitter(newChildShadowView.eventEmitter);
        newChildComponentView->updateState(newChildShadowView.state, oldChildShadowView.state);
        newChildComponentView->updateLayoutMetrics(newChildShadowView.layoutMetrics, oldChildShadowView.layoutMetrics);
        newChildComponentView->finalizeUpdates(RNComponentViewUpdateMask::All);

        parentViewDescriptor.view->mountChildComponentView(*newChildComponentView, mutation.index);
        break;
      }

      case facebook::react::ShadowViewMutation::Remove: {
        auto &oldChildShadowView = mutation.oldChildShadowView;
        auto &parentShadowView = mutation.parentShadowView;
        auto &oldChildViewDescriptor = m_registry.componentViewDescriptorWithTag(oldChildShadowView.tag);
        auto &parentViewDescriptor = m_registry.componentViewDescriptorWithTag(parentShadowView.tag);
        parentViewDescriptor.view->unmountChildComponentView(*oldChildViewDescriptor.view, mutation.index);
        break;
      }

      case facebook::react::ShadowViewMutation::Update: {
        auto &oldChildShadowView = mutation.oldChildShadowView;
        auto &newChildShadowView = mutation.newChildShadowView;
        auto &newChildViewDescriptor = m_registry.componentViewDescriptorWithTag(newChildShadowView.tag);
        auto &newChildComponentView = newChildViewDescriptor.view;

        auto mask = RNComponentViewUpdateMask{};

        if (oldChildShadowView.props != newChildShadowView.props) {
          newChildComponentView->updateProps(newChildShadowView.props, oldChildShadowView.props);
          mask |= RNComponentViewUpdateMask::Props;
        }

        if (oldChildShadowView.eventEmitter != newChildShadowView.eventEmitter) {
          newChildComponentView->updateEventEmitter(newChildShadowView.eventEmitter);
          mask |= RNComponentViewUpdateMask::EventEmitter;
        }

        if (oldChildShadowView.state != newChildShadowView.state) {
          newChildComponentView->updateState(newChildShadowView.state, oldChildShadowView.state);
          mask |= RNComponentViewUpdateMask::State;
        }

        if (oldChildShadowView.layoutMetrics != newChildShadowView.layoutMetrics) {
          newChildComponentView->updateLayoutMetrics(
              newChildShadowView.layoutMetrics, oldChildShadowView.layoutMetrics);
          mask |= RNComponentViewUpdateMask::LayoutMetrics;
        }

        if (mask != RNComponentViewUpdateMask::None) {
          newChildComponentView->finalizeUpdates(mask);
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

void FabricUIManager::initiateTransaction(facebook::react::MountingCoordinator::Shared const &mountingCoordinator) {
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
    facebook::react::MountingCoordinator::Shared const &mountingCoordinator) {
  // Should cache this locally

  if (m_context.UIDispatcher().HasThreadAccess()) {
    initiateTransaction(mountingCoordinator);
  } else {
    m_context.UIDispatcher().Post(
        [mountingCoordinator, self = shared_from_this()]() { self->initiateTransaction(mountingCoordinator); });
  }
}

void FabricUIManager::schedulerDidRequestPreliminaryViewAllocation(
    facebook::react::SurfaceId surfaceId,
    const facebook::react::ShadowNode &shadowView) {
  // iOS does not do this optimization, but Android does.  It maybe that android's allocations are more expensive due to
  // the Java boundary.
  // TODO: We should do some perf tests to see if this is worth doing.

  if (m_context.UIDispatcher().HasThreadAccess()) {
    m_registry.dequeueComponentViewWithComponentHandle(shadowView.getComponentHandle(), surfaceId, m_compContext);
  } else {
    m_context.UIDispatcher().Post(
        [componentHandle = shadowView.getComponentHandle(), surfaceId, self = shared_from_this()]() {
          self->m_registry.dequeueComponentViewWithComponentHandle(componentHandle, surfaceId, self->m_compContext);
        });
  }
}

void FabricUIManager::schedulerDidDispatchCommand(
    facebook::react::ShadowView const &shadowView,
    std::string const &commandName,
    folly::dynamic const &arg) {
  if (m_context.UIDispatcher().HasThreadAccess()) {
    auto descriptor = m_registry.componentViewDescriptorWithTag(shadowView.tag);
    descriptor.view->handleCommand(commandName, arg);
  } else {
    m_context.UIDispatcher().Post(
        [wkThis = weak_from_this(), commandName, tag = shadowView.tag, args = folly::dynamic(arg)]() {
          if (auto pThis = wkThis.lock()) {
            auto view = pThis->m_registry.findComponentViewWithTag(tag);
            if (view) {
              view->handleCommand(commandName, args);
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
