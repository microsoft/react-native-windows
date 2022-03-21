// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.
#include "pch.h"
#include "CompRootView.h"
#include "CompRootView.g.cpp"

#include <QuirkSettings.h>
#include <ReactHost/MsoUtils.h>
#include <UI.Xaml.Input.h>
#include <UI.Xaml.Media.Media3D.h>
#include <Utils/Helpers.h>
#include <dispatchQueue/dispatchQueue.h>
#include <winrt/Windows.UI.Core.h>
#include "ReactNativeHost.h"
#include "ReactViewInstance.h"

#include <winrt/Microsoft.UI.Xaml.Controls.h>

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
      winrt::weak_ref<winrt::Microsoft::ReactNative::implementation::CompRootView> &&weakRootControl,
      Mso::DispatchQueue const &uiQueue) noexcept;

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
  Mso::DispatchQueue m_uiQueue;
};

CompReactViewInstance::CompReactViewInstance(
    winrt::weak_ref<winrt::Microsoft::ReactNative::implementation::CompRootView> &&weakRootControl,
    Mso::DispatchQueue const &uiQueue) noexcept
    : m_weakRootControl{std::move(weakRootControl)}, m_uiQueue{uiQueue} {}

Mso::Future<void> CompReactViewInstance::InitRootView(
    Mso::CntPtr<Mso::React::IReactInstance> &&reactInstance,
    Mso::React::ReactViewOptions &&viewOptions) noexcept {
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
  return Mso::PostFuture(
      m_uiQueue, [weakThis = Mso::WeakPtr{this}, action{std::forward<TAction>(action)}]() mutable noexcept {
        if (auto strongThis = weakThis.GetStrongPtr()) {
          if (auto rootControl = strongThis->m_weakRootControl.get()) {
            action(rootControl);
            return Mso::Maybe<void>{};
          }
        }

        return Mso::CancellationErrorProvider().MakeMaybe<void>();
      });
}

CompRootView::CompRootView() noexcept : m_uiQueue(Mso::DispatchQueue::GetCurrentUIThreadQueue()) {
  VerifyElseCrashSz(m_uiQueue, "Cannot get UI dispatch queue for the current thread");

  /*
    Loaded([this](auto &&, auto &&) {
      ::Microsoft::ReactNative::SetCompositor(::Microsoft::ReactNative::GetCompositor(*this));
    });
    */
}

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
  /*

switch (msg) {
  case WM_MOUSEWHEEL: {
    POINT pt = {GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
    ::ScreenToClient(m_hwnd, &pt);
    int32_t delta = GET_WHEEL_DELTA_WPARAM(wParam);
    m_compRootView.OnScrollWheel({static_cast<float>(pt.x), static_cast<float>(pt.y)}, delta);
    return 0;
  }
  case WM_POINTERDOWN: {
      if (m_compEventHandler) {
  m_compEventHandler->PointerPressed(static_cast<facebook::react::SurfaceId>(m_rootTag), {GET_X_LPARAM(lParam),
GET_Y_LPARAM(lParam)}, 1);
}
    return 0;
  }
  case WM_POINTERUP: {
    if (m_compEventHandler) {
      m_compEventHandler->PointerUp(static_cast<facebook::react::SurfaceId>(m_rootTag), {GET_X_LPARAM(lParam),
GET_Y_LPARAM(lParam)}, 1);
    }
    return 0;
  }
  //case WM_WINDOWPOSCHANGED: {

}
  */

  return 0;
}

/*
void CompRootView::OnPointerPressed(const PointerPressedArgs &args) noexcept {
  if (m_rootTag == -1)
    return;

  if (m_compEventHandler) {
    m_compEventHandler->PointerPressed(static_cast<facebook::react::SurfaceId>(m_rootTag), args, 1);
  }
}

void CompRootView::OnMouseUp(Windows::Foundation::Point point) noexcept {
  if (m_rootTag == -1)
    return;
  if (m_compEventHandler) {
    m_compEventHandler->PointerUp(static_cast<facebook::react::SurfaceId>(m_rootTag), {point.X, point.Y}, 1);
  }
}
*/

/*
void CompRootView::OnPointerDown(int32_t pointerId) noexcept
{
  if (m_rootTag == -1)
return;

m_compEventHandler->PointerDown(static_cast<facebook::react::SurfaceId>(m_rootTag), pointerId);
}
*/

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
  VerifyElseCrash(m_uiQueue.HasThreadAccess());

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
  VerifyElseCrash(m_uiQueue.HasThreadAccess());
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
  /*
  if (m_xamlRootView) {
    auto children = m_xamlRootView.Children();
    uint32_t index{0};
    if (m_greenBoxGrid && children.IndexOf(m_greenBoxGrid, index)) {
      children.RemoveAt(index);
    }
  }
  */
}

void CompRootView::EnsureLoadingUI() noexcept {
  /*
  if (m_xamlRootView) {
    // Create Grid & TextBlock to hold text
    if (m_waitingTextBlock == nullptr) {
      m_waitingTextBlock = winrt::TextBlock();

      m_greenBoxGrid = winrt::Grid{};
      auto c = xaml::Controls::ColumnDefinition{};
      m_greenBoxGrid.ColumnDefinitions().Append(c);
      c = xaml::Controls::ColumnDefinition{};
      c.Width(xaml::GridLengthHelper::Auto());
      m_greenBoxGrid.ColumnDefinitions().Append(c);
      c = xaml::Controls::ColumnDefinition{};
      c.Width(xaml::GridLengthHelper::Auto());
      m_greenBoxGrid.ColumnDefinitions().Append(c);
      c = xaml::Controls::ColumnDefinition{};
      m_greenBoxGrid.ColumnDefinitions().Append(c);

      m_waitingTextBlock.SetValue(xaml::Controls::Grid::ColumnProperty(), winrt::box_value(1));
      m_greenBoxGrid.Background(xaml::Media::SolidColorBrush(winrt::ColorHelper::FromArgb(0x80, 0x03, 0x29, 0x29)));
      m_greenBoxGrid.Children().Append(m_waitingTextBlock);
      m_greenBoxGrid.VerticalAlignment(xaml::VerticalAlignment::Center);
      Microsoft::UI::Xaml::Controls::ProgressRing ring{};
      ring.SetValue(xaml::Controls::Grid::ColumnProperty(), winrt::box_value(2));
      ring.IsActive(true);
      m_greenBoxGrid.Children().Append(ring);

      // Format TextBlock
      m_waitingTextBlock.TextAlignment(winrt::TextAlignment::Center);
      m_waitingTextBlock.TextWrapping(xaml::TextWrapping::Wrap);
      m_waitingTextBlock.FontFamily(winrt::FontFamily(L"Segoe UI"));
      m_waitingTextBlock.Foreground(xaml::Media::SolidColorBrush(winrt::Colors::White()));
      winrt::Thickness margin = {10.0f, 10.0f, 10.0f, 10.0f};
      m_waitingTextBlock.Margin(margin);
    }

    auto children = m_xamlRootView.Children();
    uint32_t index;
    if (m_greenBoxGrid && !children.IndexOf(m_greenBoxGrid, index)) {
      children.Append(m_greenBoxGrid);
    }
  }
  */
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
  /*
  if (m_xamlRootView) {
    ClearLoadingUI();
  }
  */
}

void CompRootView::ShowInstanceWaiting() noexcept {
  /*
  if (m_xamlRootView) {
    EnsureLoadingUI();

    // Place message into TextBlock
    std::wstring wstrMessage(L"Connecting to remote debugger");
    m_waitingTextBlock.Text(wstrMessage);
  }
  */
}

void CompRootView::ShowInstanceLoading() noexcept {
  if (!m_context->SettingsSnapshot().UseDeveloperSupport())
    return;

  /*
    if (m_xamlRootView) {
      EnsureLoadingUI();

      // Place message into TextBlock
      std::wstring wstrMessage(L"Loading bundle.");
      m_waitingTextBlock.Text(wstrMessage);
    }
    */
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
    auto viewInstance = Mso::Make<CompReactViewInstance>(this->get_weak(), m_uiQueue);
    m_reactViewHost->AttachViewInstance(*viewInstance);
  }
}

Windows::Foundation::Size CompRootView::Measure(Windows::Foundation::Size const &availableSize) const {
  Windows::Foundation::Size size{0.0f, 0.0f};
  /*

  for (xaml::UIElement child : Children()) {
    child.Measure(availableSize);
    auto desired = child.DesiredSize();
    if (desired.Height > size.Height)
      size.Height = desired.Height;
    if (desired.Width > size.Width)
      size.Width = desired.Width;
  }

#ifdef USE_FABRIC
*/
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
      constraints.layoutDirection = FlowDirection() == xaml::FlowDirection::LeftToRight
          ? facebook::react::LayoutDirection::LeftToRight
          : facebook::react::LayoutDirection::RightToLeft;

      auto yogaSize =
          fabricuiManager->measureSurface(static_cast<facebook::react::SurfaceId>(m_rootTag), constraints, context);
      return {std::min(yogaSize.width, availableSize.Width), std::min(yogaSize.height, availableSize.Height)};
    }
  }

  return size;
}

Windows::Foundation::Size CompRootView::Arrange(Windows::Foundation::Size finalSize) const {
  /*
  for (xaml::UIElement child : Children()) {
    child.Arrange(winrt::Rect(0, 0, finalSize.Width, finalSize.Height));
  }

#ifdef USE_FABRIC
*/
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
      constraints.layoutDirection = FlowDirection() == xaml::FlowDirection::LeftToRight
          ? facebook::react::LayoutDirection::LeftToRight
          : facebook::react::LayoutDirection::RightToLeft;

      fabricuiManager->constraintSurfaceLayout(
          static_cast<facebook::react::SurfaceId>(m_rootTag), constraints, context);
    }
  }
  //#endif
  return finalSize;
}

/*
// Maps react-native's view of the root view to the actual UI
// react-native is unaware that there are non-RN elements within the CompRootView
uint32_t CompRootView::RNIndexToXamlIndex(uint32_t index) noexcept {
  // If m_focusSafeHarbor exists, it should be at index 0
  // m_xamlRootView is the next element, followed by any RN content.
#if DEBUG
  uint32_t findIndex{0};
  Assert(!m_focusSafeHarbor || Children().IndexOf(m_focusSafeHarbor, findIndex) && findIndex == 0);
  Assert(Children().IndexOf(m_xamlRootView, findIndex) && findIndex == (m_focusSafeHarbor ? 1 : 0));
#endif

  return index + (m_focusSafeHarbor ? 2 : 1);
}
void CompRootView::AddView(uint32_t index, xaml::UIElement child) {
  Children().InsertAt(RNIndexToXamlIndex(index), child);
}

void CompRootView::RemoveAllChildren() {
  const uint32_t numLeft = m_focusSafeHarbor ? 2 : 1;
  while (Children().Size() > numLeft)
    Children().RemoveAt(numLeft);
}

void CompRootView::RemoveChildAt(uint32_t index) {
  Children().RemoveAt(RNIndexToXamlIndex(index));
}
*/

} // namespace winrt::Microsoft::ReactNative::implementation
