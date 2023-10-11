// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.
#include "pch.h"
#include "CompositionRootView.h"
#include "CompositionRootView.g.cpp"
#include "FocusNavigationRequest.g.cpp"

#include <DynamicWriter.h>
#include <Fabric/FabricUIManagerModule.h>
#include <IReactInstance.h>
#include <Modules/ReactRootViewTagGenerator.h>
#include <QuirkSettings.h>
#include <ReactHost/MsoUtils.h>
#include <Utils/Helpers.h>
#include <dispatchQueue/dispatchQueue.h>
#include <react/renderer/core/LayoutConstraints.h>
#include <react/renderer/core/LayoutContext.h>
#include <winrt/Windows.UI.Core.h>
#include "CompositionContextHelper.h"
#include "CompositionHelpers.h"
#include "ReactNativeHost.h"
#include "RootComponentView.h"

#ifdef USE_WINUI3
#include <winrt/Microsoft.UI.Content.h>
#include "CompositionRootAutomationProvider.h"
#endif

namespace winrt::Microsoft::ReactNative::implementation {

//! This class ensures that we access ReactRootView from UI thread.
struct CompositionReactViewInstance
    : public winrt::implements<CompositionReactViewInstance, winrt::Microsoft::ReactNative::IReactViewInstance> {
  CompositionReactViewInstance(
      winrt::weak_ref<winrt::Microsoft::ReactNative::implementation::CompositionRootView> &&weakRootControl) noexcept;

  void InitRootView(
      winrt::Microsoft::ReactNative::IReactContext context,
      winrt::Microsoft::ReactNative::ReactViewOptions viewOptions) noexcept;

  void UpdateRootView() noexcept;
  void UninitRootView() noexcept;

 private:
  template <class TAction>
  Mso::Future<void> PostInUIQueue(TAction &&action) noexcept;

 private:
  winrt::weak_ref<winrt::Microsoft::ReactNative::implementation::CompositionRootView> m_weakRootControl;
  IReactDispatcher m_uiDispatcher{nullptr};
};

CompositionReactViewInstance::CompositionReactViewInstance(
    winrt::weak_ref<winrt::Microsoft::ReactNative::implementation::CompositionRootView> &&weakRootControl) noexcept
    : m_weakRootControl{std::move(weakRootControl)} {}

void CompositionReactViewInstance::InitRootView(
    winrt::Microsoft::ReactNative::IReactContext context,
    winrt::Microsoft::ReactNative::ReactViewOptions viewOptions) noexcept {
  m_uiDispatcher = context.Properties()
                       .Get(winrt::Microsoft::ReactNative::ReactDispatcherHelper::UIDispatcherProperty())
                       .try_as<IReactDispatcher>();

  PostInUIQueue([context{std::move(context)}, viewOptions{std::move(viewOptions)}](
                    winrt::com_ptr<winrt::Microsoft::ReactNative::implementation::CompositionRootView>
                        &rootControl) mutable noexcept {
    rootControl->InitRootView(std::move(context), std::move(viewOptions));
  });
}

void CompositionReactViewInstance::UpdateRootView() noexcept {
  PostInUIQueue([](winrt::com_ptr<winrt::Microsoft::ReactNative::implementation::CompositionRootView>
                       &rootControl) mutable noexcept { rootControl->UpdateRootView(); });
}

void CompositionReactViewInstance::UninitRootView() noexcept {
  PostInUIQueue([](winrt::com_ptr<winrt::Microsoft::ReactNative::implementation::CompositionRootView>
                       &rootControl) mutable noexcept { rootControl->UninitRootView(); });
}

//===========================================================================
// ReactViewInstance inline implementation
//===========================================================================

template <class TAction>
inline Mso::Future<void> CompositionReactViewInstance::PostInUIQueue(TAction &&action) noexcept {
  // ReactViewInstance has shorter lifetime than ReactRootControl. Thus, we capture this WeakPtr.
  auto promise = Mso::Promise<void>();

  m_uiDispatcher.Post([promise, weakThis{get_weak()}, action{std::forward<TAction>(action)}]() mutable {
    if (auto strongThis = weakThis.get()) {
      if (auto rootControl = strongThis->m_weakRootControl.get()) {
        action(rootControl);
        promise.SetValue();
        return;
      }
    }
    promise.TryCancel();
  });

  return promise.AsFuture();
}

CompositionRootView::CompositionRootView() noexcept {}

#ifdef USE_WINUI3
CompositionRootView::CompositionRootView(winrt::Microsoft::UI::Composition::Compositor compositor) noexcept
    : m_compositor(compositor) {}
#endif

ReactNative::IReactViewHost CompositionRootView::ReactViewHost() noexcept {
  return m_reactViewHost;
}

void CompositionRootView::ReactViewHost(winrt::Microsoft::ReactNative::IReactViewHost const &value) noexcept {
  if (m_reactViewHost == value) {
    return;
  }

  if (m_reactViewHost) {
    m_reactViewHost.DetachViewInstance();
  }

  m_reactViewHost = value;

  if (m_reactViewHost) {
    auto viewInstance = winrt::make<CompositionReactViewInstance>(this->get_weak());
    m_reactViewHost.AttachViewInstance(viewInstance);
  }
}

winrt::Microsoft::ReactNative::Composition::IVisual CompositionRootView::RootVisual() noexcept {
  return m_rootVisual;
}

void CompositionRootView::RootVisual(winrt::Microsoft::ReactNative::Composition::IVisual const &value) noexcept {
  if (m_rootVisual != value) {
    assert(!m_rootVisual);
    m_rootVisual = value;
  }
}

winrt::Windows::Foundation::Size CompositionRootView::Size() noexcept {
  return m_size;
}

void CompositionRootView::Size(winrt::Windows::Foundation::Size value) noexcept {
  m_size = value;
}

float CompositionRootView::ScaleFactor() noexcept {
  return m_scaleFactor;
}

void CompositionRootView::ScaleFactor(float value) noexcept {
  m_scaleFactor = value;
}

winrt::IInspectable CompositionRootView::GetUiaProvider() noexcept {
  auto componentView = GetComponentView();
  if (componentView == nullptr)
    return nullptr;

  return componentView->EnsureUiaProvider();
}

winrt::Microsoft::ReactNative::Composition::IVisual CompositionRootView::GetVisual() const noexcept {
  return m_rootVisual;
}

std::string CompositionRootView::JSComponentName() const noexcept {
  return to_string(m_reactViewOptions.ComponentName());
}

int64_t CompositionRootView::GetActualHeight() const noexcept {
  return static_cast<int64_t>(m_size.Height);
}

int64_t CompositionRootView::GetActualWidth() const noexcept {
  return static_cast<int64_t>(m_size.Width);
}

int64_t CompositionRootView::GetTag() const noexcept {
  return m_rootTag;
}

void CompositionRootView::SetTag(int64_t tag) noexcept {
  m_rootTag = tag;
}

int64_t CompositionRootView::SendMessage(uint32_t msg, uint64_t wParam, int64_t lParam) noexcept {
  if (m_rootTag == -1)
    return 0;

  if (m_CompositionEventHandler) {
    auto result = m_CompositionEventHandler->SendMessage(msg, wParam, lParam);
    if (result)
      return result;
  }

  return 0;
}

void CompositionRootView::InitRootView(
    winrt::Microsoft::ReactNative::IReactContext &&context,
    winrt::Microsoft::ReactNative::ReactViewOptions &&viewOptions) noexcept {
  m_uiDispatcher = context.Properties()
                       .Get(winrt::Microsoft::ReactNative::ReactDispatcherHelper::UIDispatcherProperty())
                       .try_as<IReactDispatcher>();
  VerifyElseCrash(m_uiDispatcher.HasThreadAccess());

  if (m_isInitialized) {
    UninitRootView();
  }

  m_context = winrt::Microsoft::ReactNative::ReactContext(std::move(context));

  winrt::Microsoft::ReactNative::CompositionRootView compositionRootView;
  get_strong().as(compositionRootView);

  m_reactViewOptions = std::move(viewOptions);
  m_CompositionEventHandler =
      std::make_shared<::Microsoft::ReactNative::CompositionEventHandler>(m_context, compositionRootView);

  UpdateRootViewInternal();

  m_isInitialized = true;
}

void CompositionRootView::UpdateRootView() noexcept {
  VerifyElseCrash(m_uiDispatcher.HasThreadAccess());
  VerifyElseCrash(m_isInitialized);
  UpdateRootViewInternal();
}

void CompositionRootView::UpdateRootViewInternal() noexcept {
  switch (m_context.Handle().LoadingState()) {
    case winrt::Microsoft::ReactNative::LoadingState::Loading:
      ShowInstanceLoading();
      break;
    case winrt::Microsoft::ReactNative::LoadingState::Loaded:
      ShowInstanceLoaded();
      break;
    case winrt::Microsoft::ReactNative::LoadingState::HasError:
      ShowInstanceError();
      break;
    default:
      VerifyElseCrashSz(false, "Unexpected value");
  }
}

void CompositionRootView::UninitRootView() noexcept {
  if (!m_isInitialized) {
    return;
  }

  if (m_isJSViewAttached) {
    if (m_context.Handle().LoadingState() == winrt::Microsoft::ReactNative::LoadingState::HasError)
      return;

    auto uiManager = ::Microsoft::ReactNative::FabricUIManager::FromProperties(
        winrt::Microsoft::ReactNative::ReactPropertyBag(m_context.Properties()));
    uiManager->stopSurface(static_cast<facebook::react::SurfaceId>(GetTag()));

    m_context.CallJSFunction(L"ReactFabric", L"unmountComponentAtNode", GetTag());

    // Paper version gives the JS thread time to finish executing - Is this needed?
    // m_jsMessageThread.Load()->runOnQueueSync([]() {});
  }

  m_rootTag = -1;
  m_context = nullptr;
  m_reactViewOptions = nullptr;

  m_isInitialized = false;
}

void CompositionRootView::ClearLoadingUI() noexcept {}

void CompositionRootView::EnsureLoadingUI() noexcept {}

void CompositionRootView::ShowInstanceLoaded() noexcept {
  if (m_rootVisual) {
    ClearLoadingUI();

    auto uiManager = ::Microsoft::ReactNative::FabricUIManager::FromProperties(
        winrt::Microsoft::ReactNative::ReactPropertyBag(m_context.Properties()));

    auto rootTag = ::Microsoft::ReactNative::getNextRootViewTag();
    SetTag(rootTag);
    uiManager->startSurface(
        this, rootTag, JSComponentName(), DynamicWriter::ToDynamic(Mso::Copy(m_reactViewOptions.InitialProps())));

    m_isJSViewAttached = true;
  }
}

void CompositionRootView::ShowInstanceError() noexcept {}

void CompositionRootView::ShowInstanceLoading() noexcept {
  if (!Mso::React::ReactOptions::UseDeveloperSupport(m_context.Properties().Handle()))
    return;

  // TODO: Show loading UI here
}

winrt::Windows::Foundation::Size CompositionRootView::Measure(
    winrt::Windows::Foundation::Size const &availableSize) const {
  winrt::Windows::Foundation::Size size{0.0f, 0.0f};

  if (m_isInitialized && m_rootTag != -1) {
    if (auto fabricuiManager = ::Microsoft::ReactNative::FabricUIManager::FromProperties(
            winrt::Microsoft::ReactNative::ReactPropertyBag(m_context.Properties()))) {
      facebook::react::LayoutContext context;
      // TODO scaling factor
      context.pointScaleFactor = static_cast<facebook::react::Float>(m_scaleFactor);
      context.fontSizeMultiplier = static_cast<facebook::react::Float>(m_scaleFactor);

      facebook::react::LayoutConstraints constraints;
      // TODO should support MinHeight/MinWidth
      constraints.minimumSize.height = static_cast<facebook::react::Float>(0.0f);
      constraints.minimumSize.width = static_cast<facebook::react::Float>(0.0f);

      // TODO should support MaxHeight/MaxWidth props?
      constraints.minimumSize.height = constraints.maximumSize.height =
          static_cast<facebook::react::Float>(availableSize.Height);
      constraints.minimumSize.width = constraints.maximumSize.width =
          static_cast<facebook::react::Float>(availableSize.Width);
      // TODO get RTL
      constraints.layoutDirection = facebook::react::LayoutDirection::LeftToRight;

      auto yogaSize =
          fabricuiManager->measureSurface(static_cast<facebook::react::SurfaceId>(m_rootTag), constraints, context);
      return {std::min(yogaSize.width, availableSize.Width), std::min(yogaSize.height, availableSize.Height)};
    }
  }

  return size;
}

winrt::Windows::Foundation::Size CompositionRootView::Arrange(winrt::Windows::Foundation::Size finalSize) const {
  if (m_isInitialized && m_rootTag != -1) {
    if (auto fabricuiManager = ::Microsoft::ReactNative::FabricUIManager::FromProperties(
            winrt::Microsoft::ReactNative::ReactPropertyBag(m_context.Properties()))) {
      facebook::react::LayoutContext context;
      context.pointScaleFactor = static_cast<facebook::react::Float>(m_scaleFactor);
      context.fontSizeMultiplier = static_cast<facebook::react::Float>(m_scaleFactor);

      facebook::react::LayoutConstraints constraints;
      // TODO should support MinHeight/MinWidth
      constraints.minimumSize.height = static_cast<facebook::react::Float>(0.0f);
      constraints.minimumSize.width = static_cast<facebook::react::Float>(0.0f);

      // TODO should support MaxHeight/MaxWidth props?
      constraints.minimumSize.height = constraints.maximumSize.height =
          static_cast<facebook::react::Float>(finalSize.Height);
      constraints.minimumSize.width = constraints.maximumSize.width =
          static_cast<facebook::react::Float>(finalSize.Width);
      // TODO get RTL
      constraints.layoutDirection = facebook::react::LayoutDirection::LeftToRight;

      fabricuiManager->constraintSurfaceLayout(
          static_cast<facebook::react::SurfaceId>(m_rootTag), constraints, context);
    }
  }
  return finalSize;
}

#ifdef USE_WINUI3
winrt::Microsoft::UI::Content::ContentIsland CompositionRootView::Island() noexcept {
  if (!m_compositor) {
    return nullptr;
  }

  if (!m_island) {
    auto rootVisual = m_compositor.CreateSpriteVisual();
    rootVisual.RelativeSizeAdjustment({1, 1});

    RootVisual(winrt::Microsoft::ReactNative::Composition::MicrosoftCompositionContextHelper::CreateVisual(rootVisual));
    m_island = winrt::Microsoft::UI::Content::ContentIsland::Create(rootVisual);

    m_island.AutomationProviderRequested(
        [this](
            winrt::Microsoft::UI::Content::ContentIsland const &,
            winrt::Microsoft::UI::Content::ContentIslandAutomationProviderRequestedEventArgs const &args) {
          auto provider = GetUiaProvider();
          auto pRootProvider =
              static_cast<winrt::Microsoft::ReactNative::implementation::CompositionRootAutomationProvider *>(
                  provider.as<IRawElementProviderSimple>().get());
          if (pRootProvider != nullptr) {
            pRootProvider->SetIsland(m_island);
          }
          args.AutomationProvider(std::move(provider));
          args.Handled(true);
        });
  }
  return m_island;
}
#endif

::Microsoft::ReactNative::RootComponentView *CompositionRootView::GetComponentView() noexcept {
  if (!m_context || m_context.Handle().LoadingState() != winrt::Microsoft::ReactNative::LoadingState::Loaded)
    return nullptr;

  if (auto fabricuiManager = ::Microsoft::ReactNative::FabricUIManager::FromProperties(
          winrt::Microsoft::ReactNative::ReactPropertyBag(m_context.Properties()))) {
    auto rootComponentViewDescriptor = fabricuiManager->GetViewRegistry().componentViewDescriptorWithTag(
        static_cast<facebook::react::SurfaceId>(m_rootTag));
    return static_cast<::Microsoft::ReactNative::RootComponentView *>(rootComponentViewDescriptor.view.get());
  }
  return nullptr;
}

winrt::Microsoft::ReactNative::FocusNavigationResult CompositionRootView::NavigateFocus(
    const winrt::Microsoft::ReactNative::FocusNavigationRequest &request) noexcept {
  if (auto view = GetComponentView()) {
    return winrt::make<winrt::Microsoft::ReactNative::implementation::FocusNavigationResult>(
        view->NavigateFocus(request));
  }
  return winrt::make<winrt::Microsoft::ReactNative::implementation::FocusNavigationResult>(false);
}

} // namespace winrt::Microsoft::ReactNative::implementation
