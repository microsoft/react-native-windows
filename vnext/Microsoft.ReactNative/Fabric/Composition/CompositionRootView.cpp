// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.
#include "pch.h"
#include "CompositionRootView.h"
#include "CompositionRootView.g.cpp"

#include <IReactInstance.h>
#include <QuirkSettings.h>
#include <ReactHost/MsoUtils.h>
#include <Utils/Helpers.h>
#include <dispatchQueue/dispatchQueue.h>
#include <winrt/Windows.UI.Core.h>
#include "CompositionContextHelper.h"
#include "CompositionHelpers.h"
#include "ReactNativeHost.h"

#ifdef USE_FABRIC
#include <Fabric/FabricUIManagerModule.h>
#include <react/renderer/core/LayoutConstraints.h>
#include <react/renderer/core/LayoutContext.h>
#endif

namespace winrt::Microsoft::ReactNative::implementation {

//! This class ensures that we access ReactRootView from UI thread.
struct CompositionReactViewInstance
    : public Mso::UnknownObject<Mso::RefCountStrategy::WeakRef, Mso::React::IReactViewInstance> {
  CompositionReactViewInstance(
      winrt::weak_ref<winrt::Microsoft::ReactNative::implementation::CompositionRootView> &&weakRootControl) noexcept;

  Mso::Future<void> InitRootView(
      Mso::CntPtr<Mso::React::IReactInstance> &&reactInstance,
      Mso::React::ReactViewOptions &&viewOptions) noexcept override;
  Mso::Future<void> UpdateRootView() noexcept override;
  Mso::Future<void> UninitRootView() noexcept override;

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

Mso::Future<void> CompositionReactViewInstance::InitRootView(
    Mso::CntPtr<Mso::React::IReactInstance> &&reactInstance,
    Mso::React::ReactViewOptions &&viewOptions) noexcept {
  m_uiDispatcher = reactInstance->GetReactContext()
                       .Properties()
                       .Get(winrt::Microsoft::ReactNative::ReactDispatcherHelper::UIDispatcherProperty())
                       .try_as<IReactDispatcher>();

  return PostInUIQueue([reactInstance{std::move(reactInstance)}, viewOptions{std::move(viewOptions)}](
                           winrt::com_ptr<winrt::Microsoft::ReactNative::implementation::CompositionRootView>
                               &rootControl) mutable noexcept {
    rootControl->InitRootView(std::move(reactInstance), std::move(viewOptions));
  });
}

Mso::Future<void> CompositionReactViewInstance::UpdateRootView() noexcept {
  return PostInUIQueue([](winrt::com_ptr<winrt::Microsoft::ReactNative::implementation::CompositionRootView>
                              &rootControl) mutable noexcept { rootControl->UpdateRootView(); });
}

Mso::Future<void> CompositionReactViewInstance::UninitRootView() noexcept {
  return PostInUIQueue([](winrt::com_ptr<winrt::Microsoft::ReactNative::implementation::CompositionRootView>
                              &rootControl) mutable noexcept { rootControl->UninitRootView(); });
}

//===========================================================================
// ReactViewInstance inline implementation
//===========================================================================

template <class TAction>
inline Mso::Future<void> CompositionReactViewInstance::PostInUIQueue(TAction &&action) noexcept {
  // ReactViewInstance has shorter lifetime than ReactRootControl. Thus, we capture this WeakPtr.
  auto promise = Mso::Promise<void>();

  m_uiDispatcher.Post([promise, weakThis = Mso::WeakPtr{this}, action{std::forward<TAction>(action)}]() mutable {
    if (auto strongThis = weakThis.GetStrongPtr()) {
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

ReactNative::ReactNativeHost CompositionRootView::ReactNativeHost() noexcept {
  return m_reactNativeHost;
}

void CompositionRootView::ReactNativeHost(ReactNative::ReactNativeHost const &value) noexcept {
  if (m_reactNativeHost != value) {
    ReactViewHost(nullptr);
    m_reactNativeHost = value;
    ReloadView();
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

double CompositionRootView::ScaleFactor() noexcept {
  return m_scaleFactor;
}

void CompositionRootView::ScaleFactor(double value) noexcept {
  m_scaleFactor = value;
}

winrt::hstring CompositionRootView::ComponentName() noexcept {
  return m_componentName;
}

void CompositionRootView::ComponentName(winrt::hstring const &value) noexcept {
  if (m_componentName != value) {
    m_componentName = value;
    ReloadView();
  }
}

ReactNative::JSValueArgWriter CompositionRootView::InitialProps() noexcept {
  return m_initialPropsWriter;
}

void CompositionRootView::InitialProps(ReactNative::JSValueArgWriter const &value) noexcept {
  if (m_initialPropsWriter != value) {
    m_initialPropsWriter = value;
    ReloadView();
  }
}

void CompositionRootView::ReloadView() noexcept {
  if (m_reactNativeHost && !m_componentName.empty()) {
    Mso::React::ReactViewOptions viewOptions{};
    viewOptions.ComponentName = to_string(m_componentName);
    viewOptions.InitialProps = m_initialPropsWriter;
    if (auto reactViewHost = ReactViewHost()) {
      reactViewHost->ReloadViewInstanceWithOptions(std::move(viewOptions));
    } else {
      auto reactNativeHost = winrt::get_self<implementation::ReactNativeHost>(m_reactNativeHost);
      auto newReactViewHost = reactNativeHost->ReactHost()->MakeViewHost(std::move(viewOptions));
      ReactViewHost(newReactViewHost.Get());
    }
  } else {
    ReactViewHost(nullptr);
  }
}

winrt::Microsoft::ReactNative::Composition::IVisual CompositionRootView::GetVisual() const noexcept {
  return m_rootVisual;
}

std::string CompositionRootView::JSComponentName() const noexcept {
  return to_string(m_componentName);
}

int64_t CompositionRootView::GetActualHeight() const noexcept {
  return static_cast<int64_t>(m_size.Height);
}

int64_t CompositionRootView::GetActualWidth() const noexcept {
  return static_cast<int64_t>(m_size.Width);
  // return static_cast<int64_t>(m_xamlRootView.ActualWidth());
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
    auto result =
        m_CompositionEventHandler->SendMessage(static_cast<facebook::react::SurfaceId>(m_rootTag), msg, wParam, lParam);
    if (result)
      return result;
  }

  return 0;
}

void CompositionRootView::OnScrollWheel(Windows::Foundation::Point point, int32_t delta) noexcept {
  if (m_rootTag == -1)
    return;

  if (m_CompositionEventHandler) {
    m_CompositionEventHandler->ScrollWheel(
        static_cast<facebook::react::SurfaceId>(m_rootTag), {point.X, point.Y}, delta);
  }
}

void CompositionRootView::InitRootView(
    Mso::CntPtr<Mso::React::IReactInstance> &&reactInstance,
    Mso::React::ReactViewOptions &&reactViewOptions) noexcept {
  m_uiDispatcher = reactInstance->GetReactContext()
                       .Properties()
                       .Get(winrt::Microsoft::ReactNative::ReactDispatcherHelper::UIDispatcherProperty())
                       .try_as<IReactDispatcher>();
  VerifyElseCrash(m_uiDispatcher.HasThreadAccess());

  if (m_isInitialized) {
    UninitRootView();
  }

  m_reactOptions = std::make_unique<Mso::React::ReactOptions>(reactInstance->Options());
  m_weakReactInstance = Mso::WeakPtr{reactInstance};
  m_context = &reactInstance->GetReactContext();

  winrt::Microsoft::ReactNative::CompositionRootView CompositionRootView;
  get_strong().as(CompositionRootView);

  m_reactViewOptions = std::make_unique<Mso::React::ReactViewOptions>(std::move(reactViewOptions));
  m_CompositionEventHandler =
      std::make_shared<::Microsoft::ReactNative::CompositionEventHandler>(*m_context, CompositionRootView);

  UpdateRootViewInternal();

  m_isInitialized = true;
}

void CompositionRootView::UpdateRootView() noexcept {
  VerifyElseCrash(m_uiDispatcher.HasThreadAccess());
  VerifyElseCrash(m_isInitialized);
  UpdateRootViewInternal();
}

void CompositionRootView::UpdateRootViewInternal() noexcept {
  if (auto reactInstance = m_weakReactInstance.GetStrongPtr()) {
    switch (reactInstance->State()) {
      case Mso::React::ReactInstanceState::Loading:
        ShowInstanceLoading();
        break;
      case Mso::React::ReactInstanceState::WaitingForDebugger:
        ShowInstanceWaiting();
        break;
      case Mso::React::ReactInstanceState::Loaded:
        ShowInstanceLoaded();
        break;
      case Mso::React::ReactInstanceState::HasError:
        ShowInstanceError();
        break;
      default:
        VerifyElseCrashSz(false, "Unexpected value");
    }
  }
}

void CompositionRootView::UninitRootView() noexcept {
  if (!m_isInitialized) {
    return;
  }

  if (m_isJSViewAttached) {
    if (auto reactInstance = m_weakReactInstance.GetStrongPtr()) {
      reactInstance->DetachRootView(this, true);
    }
  }

  m_rootTag = -1;
  m_reactOptions = nullptr;
  m_context.Clear();
  m_reactViewOptions = nullptr;
  m_weakReactInstance = nullptr;

  m_isInitialized = false;
}

void CompositionRootView::ClearLoadingUI() noexcept {}

void CompositionRootView::EnsureLoadingUI() noexcept {}

void CompositionRootView::ShowInstanceLoaded() noexcept {
  if (m_rootVisual) {
    ClearLoadingUI();

    if (auto reactInstance = m_weakReactInstance.GetStrongPtr()) {
      reactInstance->AttachMeasuredRootView(this, Mso::Copy(m_reactViewOptions->InitialProps), true);
    }
    m_isJSViewAttached = true;
  }
}

void CompositionRootView::ShowInstanceError() noexcept {}

void CompositionRootView::ShowInstanceWaiting() noexcept {}

void CompositionRootView::ShowInstanceLoading() noexcept {
  if (!m_context->SettingsSnapshot().UseDeveloperSupport())
    return;
}

Mso::React::IReactViewHost *CompositionRootView::ReactViewHost() noexcept {
  return m_reactViewHost.Get();
}

void CompositionRootView::ReactViewHost(Mso::React::IReactViewHost *viewHost) noexcept {
  if (m_reactViewHost.Get() == viewHost) {
    return;
  }

  if (m_reactViewHost) {
    UninitRootView();
    m_reactViewHost->DetachViewInstance();
  }

  m_reactViewHost = viewHost;

  if (m_reactViewHost) {
    auto viewInstance = Mso::Make<CompositionReactViewInstance>(this->get_weak());
    m_reactViewHost->AttachViewInstance(*viewInstance);
  }
}

Windows::Foundation::Size CompositionRootView::Measure(Windows::Foundation::Size const &availableSize) const {
  Windows::Foundation::Size size{0.0f, 0.0f};

  if (m_isInitialized && m_rootTag != -1) {
    if (auto fabricuiManager = ::Microsoft::ReactNative::FabricUIManager::FromProperties(
            winrt::Microsoft::ReactNative::ReactPropertyBag(m_context->Properties()))) {
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

Windows::Foundation::Size CompositionRootView::Arrange(Windows::Foundation::Size finalSize) const {
  if (m_isInitialized && m_rootTag != -1) {
    if (auto fabricuiManager = ::Microsoft::ReactNative::FabricUIManager::FromProperties(
            winrt::Microsoft::ReactNative::ReactPropertyBag(m_context->Properties()))) {
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

} // namespace winrt::Microsoft::ReactNative::implementation
