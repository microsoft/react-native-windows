// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"
#include <DynamicReader.h>
#include <DynamicWriter.h>
#include <Fabric/ComponentView.h>
#include <Fabric/FabricUIManagerModule.h>
#include <Fabric/ReactNativeConfigProperties.h>
#include <Fabric/ViewComponentView.h>
#include <IReactContext.h>
#include <IReactRootView.h>
#include <JSI/jsi.h>
#include <Modules\NativeUIManager.h>
#include <IXamlRootView.h>
#include <Views/ViewManager.h>
#include <XamlUtils.h>
#include <react/renderer/core/EventBeat.h>
#include <runtimeexecutor/ReactCommon/RuntimeExecutor.h>
#include "ShadowNodeBase.h"
#include <UI.Xaml.Controls.h>
#include "Unicode.h"
#include "XamlUIService.h"
#pragma warning(push)
#pragma warning(disable : 4244)
#include <react/renderer/scheduler/Scheduler.h>
#include <react/renderer/scheduler/SchedulerToolbox.h>
#include <react/utils/ContextContainer.h>
#pragma warning(pop)
#include <SchedulerSettings.h>

#pragma warning(push)
#pragma warning(disable : 4244 4305)
#include <react/renderer/componentregistry/ComponentDescriptorProviderRegistry.h>
#include <react/renderer/components/image/ImageComponentDescriptor.h>
#include <react/renderer/components/scrollview/ScrollViewComponentDescriptor.h>
#include <react/renderer/components/text/ParagraphComponentDescriptor.h>
#include <react/renderer/components/text/RawTextComponentDescriptor.h>
#include <react/renderer/components/text/TextComponentDescriptor.h>
#include <react/renderer/components/textinput/iostextinput/TextInputComponentDescriptor.h>
#include <react/renderer/components/view/ViewComponentDescriptor.h>
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

FabricUIManager::~FabricUIManager() {
  // To make sure that we destroy UI components in UI thread.
  // if (!m_context.UIDispatcher().HasThreadAccess()) {
  //    m_context.UIDispatcher().Post([module = std::move(m_module)]() {});
  //  }
}

// Equiv of AsyncEventBeat (ReactAndroid\src\main\java\com\facebook\react\fabric\jni\AsyncEventBeat.h)
class AsyncEventBeat final : public facebook::react::EventBeat { //, public facebook::react::EventBeatManagerObserver {
 public:
  AsyncEventBeat(
      facebook::react::EventBeat::SharedOwnerBox const &ownerBox,
      // EventBeatManager *eventBeatManager,
      facebook::react::RuntimeExecutor runtimeExecutor,
      std::weak_ptr<FabricUIManager> uiManager)
      : EventBeat(ownerBox),
        // eventBeatManager_(eventBeatManager),
        runtimeExecutor_(runtimeExecutor),
        uiManager_(uiManager) {
    // eventBeatManager->addObserver(*this);
  }

  ~AsyncEventBeat() {
    // eventBeatManager_->removeObserver(*this);
  }

  void tick() const /* override */ {
    runtimeExecutor_([this, ownerBox = ownerBox_](facebook::jsi::Runtime &runtime) {
      auto owner = ownerBox->owner.lock();
      if (!owner) {
        return;
      }

      this->beat(runtime);
    });
  }

  void induce() const override {
    tick();
  }

  void request() const override {
    bool alreadyRequested = isRequested_;
    EventBeat::request();
    if (!alreadyRequested) {
      if (auto uiManager = uiManager_.lock()) {
        // Notifies that an event will be dispatched (e.g. LayoutEvent)
        // uiManager->onRequestEventBeat();
      }
    }
  }

 private:
  // EventBeatManager *eventBeatManager_;
  facebook::react::RuntimeExecutor runtimeExecutor_;
  std::weak_ptr<FabricUIManager> uiManager_;
};

std::shared_ptr<facebook::react::ComponentDescriptorProviderRegistry const> sharedProviderRegistry() {
  static auto providerRegistry = []() -> std::shared_ptr<facebook::react::ComponentDescriptorProviderRegistry> {
    auto providerRegistry = std::make_shared<facebook::react::ComponentDescriptorProviderRegistry>();
    providerRegistry->add(
        facebook::react::concreteComponentDescriptorProvider<facebook::react::ImageComponentDescriptor>());
    providerRegistry->add(
        facebook::react::concreteComponentDescriptorProvider<facebook::react::ParagraphComponentDescriptor>());
    providerRegistry->add(
        facebook::react::concreteComponentDescriptorProvider<facebook::react::RawTextComponentDescriptor>());
    providerRegistry->add(
        facebook::react::concreteComponentDescriptorProvider<facebook::react::ScrollViewComponentDescriptor>());
    providerRegistry->add(
        facebook::react::concreteComponentDescriptorProvider<facebook::react::TextComponentDescriptor>());
    providerRegistry->add(
        facebook::react::concreteComponentDescriptorProvider<facebook::react::TextInputComponentDescriptor>());
    providerRegistry->add(
        facebook::react::concreteComponentDescriptorProvider<facebook::react::ViewComponentDescriptor>());
    return providerRegistry;
  }();

  return providerRegistry;
}

void FabricUIManager::installFabricUIManager( // in android this is Binding::installFabricUIManager(
                                              // jni::alias_ref<JRuntimeExecutor::javaobject> runtimeExecutorHolder,
                                              // jni::alias_ref<jobject> javaUIManager,
                                              // EventBeatManager *eventBeatManager,
                                              // jni::alias_ref<JavaMessageQueueThread::javaobject>
                                              // jsMessageQueueThread, ComponentFactory *componentsRegistry,
                                              // jni::alias_ref<jobject> reactNativeConfig
    ) noexcept {
  // SystraceSection s("FabricUIManagerBinding::installFabricUIManager");

  std::shared_ptr<const facebook::react::ReactNativeConfig> config =
      std::make_shared<const ReactNativeConfigProperties>(m_context);

  /*
    enableFabricLogs_ =
        config->getBool("react_fabric:enabled_android_fabric_logs");

    if (enableFabricLogs_) {
      LOG(WARNING) << "Binding::installFabricUIManager() was called (address: "
                   << this << ").";
    }
    */

  std::lock_guard<std::mutex> schedulerLock(m_schedulerMutex);

  facebook::react::ContextContainer::Shared contextContainer = std::make_shared<facebook::react::ContextContainer>();

  // auto sharedJSMessageQueueThread =
  // std::make_shared<JMessageQueueThread>(jsMessageQueueThread);
  auto runtimeExecutor = SchedulerSettings::GetRuntimeExecutor(m_context.Properties());

  // TODO: T31905686 Create synchronous Event Beat
  facebook::react::EventBeat::Factory synchronousBeatFactory =
      [/*eventBeatManager,*/ runtimeExecutor,
       localUIManager = weak_from_this()](facebook::react::EventBeat::SharedOwnerBox const &ownerBox) {
        return std::make_unique<AsyncEventBeat>(ownerBox, /* eventBeatManager, */ runtimeExecutor, localUIManager);
      };

  facebook::react::EventBeat::Factory asynchronousBeatFactory =
      [/*eventBeatManager,*/ runtimeExecutor,
       localUIManager = weak_from_this()](facebook::react::EventBeat::SharedOwnerBox const &ownerBox) {
        return std::make_unique<AsyncEventBeat>(ownerBox, /* eventBeatManager, */ runtimeExecutor, localUIManager);
      };

  contextContainer->insert("ReactNativeConfig", config);
  // Only used by android view components.. probably do not need
  // We will store FabricUIManager in the context propertybag if needed.
  // contextContainer->insert("FabricUIManager", javaUIManager_);

  // Keep reference to config object and cache some feature flags here
  m_reactNativeConfig = config;
  m_collapseDeleteCreateMountingInstructions =
      m_reactNativeConfig->getBool("react_fabric:enabled_collapse_delete_create_mounting_instructions") &&
      !m_reactNativeConfig->getBool("react_fabric:enable_reparenting_detection_android") &&
      !m_reactNativeConfig->getBool("react_fabric:enabled_layout_animations_android");

  m_disablePreallocateViews = m_reactNativeConfig->getBool("react_fabric:disabled_view_preallocation_android");

  /*
    bool enableLayoutAnimations = reactNativeConfig_->getBool(
        "react_fabric:enabled_layout_animations_android");
  */

  auto toolbox = facebook::react::SchedulerToolbox{};
  toolbox.contextContainer = contextContainer;
  toolbox.componentRegistryFactory = [](facebook::react::EventDispatcher::Weak const &eventDispatcher,
                                        facebook::react::ContextContainer::Shared const &contextContainer)
      -> facebook::react::ComponentDescriptorRegistry::Shared {
    auto registry = sharedProviderRegistry()->createComponentDescriptorRegistry({eventDispatcher, contextContainer});
    /*
    auto mutableRegistry = std::const_pointer_cast<facebook::react::ComponentDescriptorRegistry>(registry);
    mutableRegistry->setFallbackComponentDescriptor(
        std::make_shared<facebook::react::UnimplementedNativeViewComponentDescriptor>(
            facebook::react::ComponentDescriptorParameters{
                eventDispatcher, contextContainer, nullptr}));
                */
    return registry;
  };
  toolbox.runtimeExecutor = runtimeExecutor;
  toolbox.synchronousEventBeatFactory = synchronousBeatFactory;
  toolbox.asynchronousEventBeatFactory = asynchronousBeatFactory;

  /*
    if (m_reactNativeConfig->getBool(
            "react_fabric:enable_background_executor_android")) {
      backgroundExecutor_ = std::make_unique<JBackgroundExecutor>();
      toolbox.backgroundExecutor = backgroundExecutor_->get();
    }
    */
  /*
    if (enableLayoutAnimations) {
      animationDriver_ =
          std::make_shared<LayoutAnimationDriver>(runtimeExecutor, this);
    }
    */
  m_scheduler = std::make_shared<facebook::react::Scheduler>(
      toolbox, (/*animationDriver_ ? animationDriver_.get() :*/ nullptr), this);
}

void FabricUIManager::startSurface(
    facebook::react::IReactRootView* rootview,
    facebook::react::SurfaceId surfaceId,
    const std::string &moduleName,
    const folly::dynamic &initialProps) noexcept {
  // SystraceSection s("FabricUIManagerBinding::startSurface");

  auto xamlRootView = static_cast<::react::uwp::IXamlRootView *>(rootview);
  auto rootFE = xamlRootView->GetXamlView().as<xaml::FrameworkElement>();

  m_surfaceRegistry.insert({surfaceId, xamlRootView->GetXamlView()});
  //m_surfaceRoot = xamlRootView->GetXamlView();

  m_context.UIDispatcher().Post([self = shared_from_this(), surfaceId]() {
    self->m_registry.dequeueComponentViewWithComponentHandle(facebook::react::RootShadowNode::Handle(), surfaceId);
  });

  /*
    std::shared_ptr<Scheduler> scheduler = getScheduler();
    if (!scheduler) {
      LOG(ERROR) << "Binding::startSurface: scheduler disappeared";
      return;
    }
    */

  facebook::react::LayoutContext context;
  // TODO scaling factor
  context.pointScaleFactor = 1; // pointScaleFactor_;



  // TODO correct constaints, and add calls to measureSurfaceWithLayoutConstraints to update them 
  facebook::react::LayoutConstraints constraints;
  constraints.minimumSize.height = 0;
  constraints.minimumSize.width = 0;
  constraints.maximumSize.height = 1000;
  constraints.maximumSize.width = 1000;
  constraints.layoutDirection = rootFE.FlowDirection() == xaml::FlowDirection::LeftToRight
      ? facebook::react::LayoutDirection::LeftToRight
      : facebook::react::LayoutDirection::RightToLeft;

  // LayoutContext context;
  // context.pointScaleFactor = pointScaleFactor_;
  m_scheduler->startSurface(
      surfaceId,
      moduleName,
      initialProps,
      std::move(constraints), // layout constraints
      context, // layout context
      {} // animationDriver_
  );
}


void FabricUIManager::didMountComponentsWithRootTag(facebook::react::SurfaceId surfaceId) noexcept {
  auto rootComponentViewDescriptor = m_registry.componentViewDescriptorWithTag(surfaceId);


  auto s = m_surfaceRegistry.at(surfaceId);
  auto p = s.as<xaml::Controls::Panel>();
  auto c = p.Children();


  //m_surfaceRoot.as<xaml::Controls::Panel>().Children().Append(static_cast<ViewComponentView &>(*rootComponentViewDescriptor.view).Element());
  m_surfaceRegistry.at(surfaceId).as<xaml::Controls::Panel>().Children().Append(static_cast<ViewComponentView &>(*rootComponentViewDescriptor.view).Element());
}

struct RemoveDeleteMetadata {
  facebook::react::Tag tag;
  facebook::react::Tag parentTag;
  int index;
  bool shouldRemove;
  bool shouldDelete;
};

class RCTComponentViewDescriptor final {
 public:
  /*
   * Associated (and owned) native view instance.
   */
  xaml::UIElement view{nullptr};

  /*
   * Indicates a requirement to call on the view methods from
   * `RCTMountingTransactionObserving` protocol.
   */
  bool observesMountingTransactionWillMount{false};
  bool observesMountingTransactionDidMount{false};
};

void FabricUIManager::RCTPerformMountInstructions(
    facebook::react::ShadowViewMutationList const &mutations,
    // facebook::react::RCTComponentViewRegistry* registry,
    // facebook::react::RCTMountingTransactionObserverCoordinator& observerCoordinator,
    facebook::react::SurfaceId surfaceId) {
  // std::lock_guard<std::recursive_mutex> lock(m_commitMutex);

  for (auto const &mutation : mutations) {
    switch (mutation.type) {
      case facebook::react::ShadowViewMutation::Create: {
        auto &newChildShadowView = mutation.newChildShadowView;
        auto &newChildViewDescriptor = m_registry.dequeueComponentViewWithComponentHandle(
            newChildShadowView.componentHandle, newChildShadowView.tag);
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
        newChildComponentView->finalizeUpdates(RNComponentViewUpdateMaskAll);

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
          mask |= RNComponentViewUpdateMaskProps;
        }

        if (oldChildShadowView.eventEmitter != newChildShadowView.eventEmitter) {
          newChildComponentView->updateEventEmitter(newChildShadowView.eventEmitter);
          mask |= RNComponentViewUpdateMaskEventEmitter;
        }

        if (oldChildShadowView.state != newChildShadowView.state) {
          newChildComponentView->updateState(newChildShadowView.state, oldChildShadowView.state);
          mask |= RNComponentViewUpdateMaskState;
        }

        if (oldChildShadowView.layoutMetrics != newChildShadowView.layoutMetrics) {
          newChildComponentView->updateLayoutMetrics(
              newChildShadowView.layoutMetrics, oldChildShadowView.layoutMetrics);
          mask |= RNComponentViewUpdateMaskLayoutMetrics;
        }

        if (mask != RNComponentViewUpdateMaskNone) {
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
      [&](facebook::react::MountingTransactionMetadata metadata) {
        //[self.delegate mountingManager:self willMountComponentsWithRootTag:surfaceId];
        //_observerCoordinator.notifyObserversMountingTransactionWillMount(metadata);
      },
      [&](facebook::react::ShadowViewMutationList const &mutations) {
        RCTPerformMountInstructions(mutations, /* _componentViewRegistry, _observerCoordinator,*/ surfaceId);
      },
      [&](facebook::react::MountingTransactionMetadata metadata) {
        //_observerCoordinator.notifyObserversMountingTransactionDidMount(metadata);
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

void Microsoft::ReactNative::FabricUIManager::schedulerDidFinishTransaction(
    facebook::react::MountingCoordinator::Shared const &mountingCoordinator) {
  // Should cache this locally

  if (m_context.UIDispatcher().HasThreadAccess()) {
    initiateTransaction(mountingCoordinator);
  } else {
    m_context.UIDispatcher().Post(
        [mountingCoordinator, self = shared_from_this()]() { self->initiateTransaction(mountingCoordinator); });
  }
}

void Microsoft::ReactNative::FabricUIManager::schedulerDidRequestPreliminaryViewAllocation(
    facebook::react::SurfaceId surfaceId,
    const facebook::react::ShadowView &shadowView) {
  /*
    if (m_context.UIDispatcher().HasThreadAccess()) {
    m_registry.dequeueComponentViewWithComponentHandle(shadowView.componentHandle, surfaceId);
  } else {
      m_context.UIDispatcher().Post(
        [componentHandle = shadowView.componentHandle, surfaceId, self = shared_from_this()]() {
          self->m_registry.dequeueComponentViewWithComponentHandle(componentHandle, surfaceId);
          });
  }
  */


  // Not needed on iOS.. so maybe not needed in windows?
  /*
  bool isLayoutableShadowNode = shadowView.layoutMetrics != facebook::react::EmptyLayoutMetrics;

  shadowView.props

  local_ref<ReadableMap::javaobject> props =
      castReadableMap(ReadableNativeMap::newObjectCxxArgs(shadowView.props->rawProps));
  auto component = getPlatformComponentName(shadowView);

  preallocateView(
      localJavaUIManager,
      surfaceId,
      shadowView.tag,
      component.get(),
      props.get(),
      (javaStateWrapper != nullptr ? javaStateWrapper.get() : nullptr),
      isLayoutableShadowNode);

  assert(false);

  */
}

void Microsoft::ReactNative::FabricUIManager::schedulerDidDispatchCommand(
    const facebook::react::ShadowView &shadowView,
    std::string const &commandName,
    folly::dynamic const args) {
  assert(false);
}

void Microsoft::ReactNative::FabricUIManager::schedulerDidSetJSResponder(
    facebook::react::SurfaceId surfaceId,
    const facebook::react::ShadowView &shadowView,
    const facebook::react::ShadowView &initialShadowView,
    bool blockNativeResponder) {
  assert(false);
}

void Microsoft::ReactNative::FabricUIManager::schedulerDidClearJSResponder() {
  assert(false);
}

void FabricUIManager::Initialize(winrt::Microsoft::ReactNative::ReactContext const &reactContext) noexcept {
  m_context = reactContext;

  m_context.Properties().Set(FabicUIManagerProperty(), shared_from_this());

  // auto settings = m_context.Properties().Get(FabricUIManagerSettingsProperty());
  // m_batchingUIMessageQueue = std::move((*settings)->batchingUIMessageQueue);
  // m_module->Initialize(reactContext);

  /*

      final EventBeatManager eventBeatManager = new EventBeatManager(mReactApplicationContext);
      UIManagerModule nativeModule =
          Assertions.assertNotNull(mReactApplicationContext.getNativeModule(UIManagerModule.class));
      EventDispatcher eventDispatcher = nativeModule.getEventDispatcher();
      final FabricUIManager uiManager =
          new FabricUIManager(
              mReactApplicationContext,
              nativeModule.getViewManagerRegistry_DO_NOT_USE(),
              eventDispatcher,
              eventBeatManager);



    final Binding binding = new Binding();
      // TODO T31905686: remove this call
    loadClasses();
      MessageQueueThread jsMessageQueueThread =
          mReactApplicationContext
              .getCatalystInstance()
              .getReactQueueConfiguration()
              .getJSQueueThread();

      // binding.register( // This is register in java, which calls into Binding::installFabricUIManager
      */
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

React::JSValueObject FabricUIManager::getConstantsForViewManager(std::string viewManagerName) noexcept {
  assert(false);
  return {};
}

React::JSValueObject FabricUIManager::getViewManagerConfig(std::string viewManagerName) noexcept {
  assert(false);
  return {};
}

React::JSValueArray FabricUIManager::getDefaultEventTypes() noexcept {
  assert(false);
  // TODO
  return {};
}

React::JSValueObject FabricUIManager::lazilyLoadView(std::string name) noexcept {
  assert(false);
  // TODO
  return {};
}

void FabricUIManager::createView(
    double reactTag,
    std::string viewName,
    double rootTag,
    React::JSValueObject &&props) noexcept {
  assert(false);
  // TODO
}

void FabricUIManager::updateView(double reactTag, std::string viewName, React::JSValueObject &&props) noexcept {
  assert(false);
}

void FabricUIManager::focus(double reactTag) noexcept {
  assert(false);
  // TODO
}

void FabricUIManager::blur(double reactTag) noexcept {
  assert(false);
  // TODO
}

void FabricUIManager::findSubviewIn(
    double reactTag,
    React::JSValueArray &&point,
    std::function<void(double nativeViewTag, double left, double top, double width, double height)> const
        &callback) noexcept {
  assert(false);
  // TODO
}

void FabricUIManager::dispatchViewManagerCommand(
    double reactTag,
    winrt::Microsoft::ReactNative::JSValue &&commandID,
    React::JSValueArray &&commandArgs) noexcept {
  assert(false);
  // TODO
}

void FabricUIManager::measure(
    double reactTag,
    std::function<void(double left, double top, double width, double height, double pageX, double pageY)> const
        &callback) noexcept {
  assert(false);
  // TODO
}

void FabricUIManager::measureInWindow(
    double reactTag,
    std::function<void(double x, double y, double width, double height)> const &callback) noexcept {
  assert(false);
  // TODO
}

void FabricUIManager::viewIsDescendantOf(
    double reactTag,
    double ancestorReactTag,
    std::function<void(React::JSValue const &)> const &callback) noexcept {
  assert(false);
  // TODO
}

void FabricUIManager::measureLayout(
    double reactTag,
    double ancestorReactTag,
    std::function<void(React::JSValue const &)> const &errorCallback,
    std::function<void(double left, double top, double width, double height)> const &callback) noexcept {
  assert(false);
}

void FabricUIManager::measureLayoutRelativeToParent(
    double reactTag,
    std::function<void(React::JSValue const &)> const &errorCallback,
    std::function<void(React::JSValue const &)> const &callback) noexcept {
  assert(false);
  // TODO
}

void FabricUIManager::setJSResponder(double reactTag, bool blockNativeResponder) noexcept {
  assert(false);
  // TODO
}

void FabricUIManager::clearJSResponder() noexcept {
  assert(false);
  // TODO
}

void FabricUIManager::configureNextLayoutAnimation(
    React::JSValueObject &&config,
    std::function<void()> const &callback,
    std::function<void(React::JSValue const &)> const &errorCallback) noexcept {
  assert(false);
  // TODO
}

void FabricUIManager::removeSubviewsFromContainerWithID(double containerID) noexcept {
  assert(false);
  // TODO
}

void FabricUIManager::replaceExistingNonRootView(double reactTag, double newReactTag) noexcept {
  assert(false);
  // TODO
}

void FabricUIManager::removeRootView(double reactTag) noexcept {
  assert(false);
  // TODO
}

void FabricUIManager::setChildren(double containerTag, React::JSValueArray &&reactTags) noexcept {
  assert(false);
  // TODO
}

void FabricUIManager::manageChildren(
    double containerTag,
    React::JSValueArray &&moveFromIndices,
    React::JSValueArray &&moveToIndices,
    React::JSValueArray &&addChildReactTags,
    React::JSValueArray &&addAtIndices,
    React::JSValueArray &&removeAtIndices) noexcept {
  assert(false);
  // TODO
}

void FabricUIManager::setLayoutAnimationEnabledExperimental(bool enabled) noexcept {
  assert(false);
  // TODO
}

void FabricUIManager::sendAccessibilityEvent(double reactTag, double eventType) noexcept {
  assert(false);
  // TODO
}

void FabricUIManager::showPopupMenu(
    double reactTag,
    React::JSValueArray &&items,
    std::function<void(React::JSValue const &)> const &error,
    std::function<void(React::JSValue const &)> const &success) noexcept {
  assert(false);
  // TODO
}

void FabricUIManager::dismissPopupMenu() noexcept {
  assert(false);
  // TODO
}

} // namespace Microsoft::ReactNative
