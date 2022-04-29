// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.
#include "pch.h"
#include "CompRootView.h"
#include "CompRootView.g.cpp"

#include <IReactInstance.h>
#include <QuirkSettings.h>
#include <ReactHost/MsoUtils.h>
#include <Utils/Helpers.h>
#include <dispatchQueue/dispatchQueue.h>
#include <winrt/Windows.UI.Core.h>
#include "ReactNativeHost.h"

#ifdef USE_FABRIC
#include <Fabric/FabricUIManagerModule.h>
#include <react/renderer/core/LayoutConstraints.h>
#include <react/renderer/core/LayoutContext.h>
#endif

namespace winrt::Microsoft::ReactNative::implementation {

//! This class ensures that we access ReactRootView from UI thread.
struct CompReactViewInstance
    : public Mso::UnknownObject<Mso::RefCountStrategy::WeakRef, Mso::React::IReactViewInstance> {
  CompReactViewInstance(
      winrt::weak_ref<winrt::Microsoft::ReactNative::implementation::CompRootView> &&weakRootControl) noexcept;

  Mso::Future<void> InitRootView(
      Mso::CntPtr<Mso::React::IReactInstance> &&reactInstance,
      Mso::React::ReactViewOptions &&viewOptions) noexcept override;
  Mso::Future<void> UpdateRootView() noexcept override;
  Mso::Future<void> UninitRootView() noexcept override;

 private:
  template <class TAction>
  Mso::Future<void> PostInUIQueue(TAction &&action) noexcept;

 private:
  winrt::weak_ref<winrt::Microsoft::ReactNative::implementation::CompRootView> m_weakRootControl;
  IReactDispatcher m_uiDispatcher{nullptr};
};

CompReactViewInstance::CompReactViewInstance(
    winrt::weak_ref<winrt::Microsoft::ReactNative::implementation::CompRootView> &&weakRootControl) noexcept
    : m_weakRootControl{std::move(weakRootControl)} {}

Mso::Future<void> CompReactViewInstance::InitRootView(
    Mso::CntPtr<Mso::React::IReactInstance> &&reactInstance,
    Mso::React::ReactViewOptions &&viewOptions) noexcept {
  m_uiDispatcher = reactInstance->GetReactContext()
                       .Properties()
                       .Get(winrt::Microsoft::ReactNative::ReactDispatcherHelper::UIDispatcherProperty())
                       .try_as<IReactDispatcher>();

  return PostInUIQueue(
      [reactInstance{std::move(reactInstance)}, viewOptions{std::move(viewOptions)}](
          winrt::com_ptr<winrt::Microsoft::ReactNative::implementation::CompRootView> &rootControl) mutable noexcept {
        rootControl->InitRootView(std::move(reactInstance), std::move(viewOptions));
      });
}

Mso::Future<void> CompReactViewInstance::UpdateRootView() noexcept {
  return PostInUIQueue(
      [](winrt::com_ptr<winrt::Microsoft::ReactNative::implementation::CompRootView> &rootControl) mutable noexcept {
        rootControl->UpdateRootView();
      });
}

Mso::Future<void> CompReactViewInstance::UninitRootView() noexcept {
  return PostInUIQueue(
      [](winrt::com_ptr<winrt::Microsoft::ReactNative::implementation::CompRootView> &rootControl) mutable noexcept {
        rootControl->UninitRootView();
      });
}

//===========================================================================
// ReactViewInstance inline implementation
//===========================================================================

template <class TAction>
inline Mso::Future<void> CompReactViewInstance::PostInUIQueue(TAction &&action) noexcept {
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

CompRootView::CompRootView() noexcept {}

ReactNative::ReactNativeHost CompRootView::ReactNativeHost() noexcept {
  return m_reactNativeHost;
}

void CompRootView::ReactNativeHost(ReactNative::ReactNativeHost const &value) noexcept {
  if (m_reactNativeHost != value) {
    ReactViewHost(nullptr);
    m_reactNativeHost = value;
    ReloadView();
  }
}

winrt::Windows::UI::Composition::Compositor CompRootView::Compositor() noexcept {
  return m_compContext ? m_compContext->Compositor() : nullptr;
}

void CompRootView::Compositor(winrt::Windows::UI::Composition::Compositor const &value) noexcept {
  if (Compositor() != value) {
    assert(!Compositor());
    m_compContext = std::make_shared<::Microsoft::ReactNative::CompContext>(value);
  }
}

std::shared_ptr<::Microsoft::ReactNative::CompContext> CompRootView::CompContext() noexcept {
  return m_compContext;
}

winrt::Windows::UI::Composition::Visual CompRootView::RootVisual() noexcept {
  return m_rootVisual;
}

void CompRootView::RootVisual(winrt::Windows::UI::Composition::Visual const &value) noexcept {
  if (m_rootVisual != value) {
    assert(!m_rootVisual);
    m_rootVisual = value;
  }
}

winrt::Windows::Foundation::Size CompRootView::Size() noexcept {
  return m_size;
}

void CompRootView::Size(winrt::Windows::Foundation::Size value) noexcept {
  m_size = value;
}

double CompRootView::ScaleFactor() noexcept {
  return m_scaleFactor;
}

void CompRootView::ScaleFactor(double value) noexcept {
  m_scaleFactor = value;
}

winrt::hstring CompRootView::ComponentName() noexcept {
  return m_componentName;
}

void CompRootView::ComponentName(winrt::hstring const &value) noexcept {
  if (m_componentName != value) {
    m_componentName = value;
    ReloadView();
  }
}

ReactNative::JSValueArgWriter CompRootView::InitialProps() noexcept {
  return m_initialPropsWriter;
}

void CompRootView::InitialProps(ReactNative::JSValueArgWriter const &value) noexcept {
  if (m_initialPropsWriter != value) {
    m_initialPropsWriter = value;
    ReloadView();
  }
}

void CompRootView::ReloadView() noexcept {
  if (m_reactNativeHost && !m_componentName.empty()) {
    Mso::React::ReactViewOptions viewOptions{};
    viewOptions.ComponentName = to_string(m_componentName);
    viewOptions.InitialProps = m_initialPropsWriter;
    viewOptions.UseFabric = true;
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

winrt::Windows::UI::Composition::Visual CompRootView::GetVisual() const noexcept {
  return m_rootVisual;
}

std::string CompRootView::JSComponentName() const noexcept {
  return to_string(m_componentName);
}

int64_t CompRootView::GetActualHeight() const noexcept {
  return static_cast<int64_t>(m_size.Height);
}

int64_t CompRootView::GetActualWidth() const noexcept {
  return static_cast<int64_t>(m_size.Width);
  // return static_cast<int64_t>(m_xamlRootView.ActualWidth());
}

int64_t CompRootView::GetTag() const noexcept {
  return m_rootTag;
}

void CompRootView::SetTag(int64_t tag) noexcept {
  m_rootTag = tag;
}

int64_t CompRootView::SendMessage(uint32_t msg, uint64_t wParam, int64_t lParam) noexcept {
  if (m_rootTag == -1)
    return 0;

  if (m_compEventHandler) {
    auto result =
        m_compEventHandler->SendMessage(static_cast<facebook::react::SurfaceId>(m_rootTag), msg, wParam, lParam);
    if (result)
      return result;
  }

  return 0;
}

void CompRootView::OnScrollWheel(Windows::Foundation::Point point, int32_t delta) noexcept {
  if (m_rootTag == -1)
    return;

  if (m_compEventHandler) {
    m_compEventHandler->ScrollWheel(static_cast<facebook::react::SurfaceId>(m_rootTag), {point.X, point.Y}, delta);
  }
}

void CompRootView::InitRootView(
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

  winrt::Microsoft::ReactNative::CompRootView compRootView;
  get_strong().as(compRootView);

  m_reactViewOptions = std::make_unique<Mso::React::ReactViewOptions>(std::move(reactViewOptions));
  m_compEventHandler = std::make_shared<::Microsoft::ReactNative::CompEventHandler>(
      *m_context, m_reactViewOptions->UseFabric && !reactInstance->Options().UseWebDebugger(), compRootView);

  UpdateRootViewInternal();

  m_isInitialized = true;
}

void CompRootView::UpdateRootView() noexcept {
  VerifyElseCrash(m_uiDispatcher.HasThreadAccess());
  VerifyElseCrash(m_isInitialized);
  UpdateRootViewInternal();
}

void CompRootView::UpdateRootViewInternal() noexcept {
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

void CompRootView::UninitRootView() noexcept {
  if (!m_isInitialized) {
    return;
  }

  if (m_isJSViewAttached) {
    if (auto reactInstance = m_weakReactInstance.GetStrongPtr()) {
      reactInstance->DetachRootView(this, m_reactViewHost->Options().UseFabric);
    }
  }

  m_rootTag = -1;
  m_reactOptions = nullptr;
  m_context.Clear();
  m_reactViewOptions = nullptr;
  m_weakReactInstance = nullptr;

  m_isInitialized = false;
}

void CompRootView::ClearLoadingUI() noexcept {
}

void CompRootView::EnsureLoadingUI() noexcept {
}

void CompRootView::ShowInstanceLoaded() noexcept {
  if (m_rootVisual) {
    ClearLoadingUI();

    if (auto reactInstance = m_weakReactInstance.GetStrongPtr()) {
      reactInstance->AttachMeasuredRootView(
          this, Mso::Copy(m_reactViewOptions->InitialProps), m_reactViewOptions->UseFabric);
    }
    m_isJSViewAttached = true;
  }
}

void CompRootView::ShowInstanceError() noexcept {
}

void CompRootView::ShowInstanceWaiting() noexcept {
}

void CompRootView::ShowInstanceLoading() noexcept {
  if (!m_context->SettingsSnapshot().UseDeveloperSupport())
    return;

}

Mso::React::IReactViewHost *CompRootView::ReactViewHost() noexcept {
  return m_reactViewHost.Get();
}

void CompRootView::ReactViewHost(Mso::React::IReactViewHost *viewHost) noexcept {
  if (m_reactViewHost.Get() == viewHost) {
    return;
  }

  if (m_reactViewHost) {
    UninitRootView();
    m_reactViewHost->DetachViewInstance();
  }

  m_reactViewHost = viewHost;

  if (m_reactViewHost) {
    auto viewInstance = Mso::Make<CompReactViewInstance>(this->get_weak());
    m_reactViewHost->AttachViewInstance(*viewInstance);
  }
}

Windows::Foundation::Size CompRootView::Measure(Windows::Foundation::Size const &availableSize) const {
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

Windows::Foundation::Size CompRootView::Arrange(Windows::Foundation::Size finalSize) const {
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
