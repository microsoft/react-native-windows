// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.
#include "pch.h"
#include "CompositionRootView.h"
#include "CompositionRootView.g.cpp"
#include "FocusNavigationRequest.g.cpp"

#include <AutoDraw.h>
#include <DynamicWriter.h>
#include <Fabric/FabricUIManagerModule.h>
#include <IReactInstance.h>
#include <Modules/ReactRootViewTagGenerator.h>
#include <QuirkSettings.h>
#include <ReactHost/MsoUtils.h>
#include <ReactPropertyBag.h>
#include <Utils/Helpers.h>
#include <dispatchQueue/dispatchQueue.h>
#include <eventWaitHandle/eventWaitHandle.h>
#include <react/renderer/attributedstring/AttributedStringBox.h>
#include <react/renderer/core/LayoutConstraints.h>
#include <react/renderer/core/LayoutContext.h>
#include <react/renderer/textlayoutmanager/TextLayoutManager.h>
#include <winrt/Microsoft.ReactNative.Composition.h>
#include <winrt/Microsoft.ReactNative.h>
#include <winrt/Windows.UI.Core.h>
#include "CompositionContextHelper.h"
#include "CompositionHelpers.h"
#include "CompositionRootAutomationProvider.h"
#include "CompositionUIService.h"
#include "ReactNativeHost.h"
#include "RootComponentView.h"
#include "TextDrawing.h"

#ifdef USE_WINUI3
#include <winrt/Microsoft.UI.Content.h>
#include <winrt/Microsoft.UI.Input.h>
#endif

namespace winrt::Microsoft::ReactNative::implementation {

constexpr float loadingActivitySize = 12.0f;
constexpr float loadingActivityHorizontalOffset = 16.0f;
constexpr float loadingBarHeight = 36.0f;
constexpr float loadingBarFontSize = 20.0f;
constexpr float loadingTextHorizontalOffset = 48.0f;

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

  assert(m_uiDispatcher.HasThreadAccess());
  if (auto rootControl = m_weakRootControl.get()) {
    rootControl->InitRootView(std::move(context), std::move(viewOptions));
  }
}

void CompositionReactViewInstance::UpdateRootView() noexcept {
  assert(m_uiDispatcher.HasThreadAccess());
  if (auto rootControl = m_weakRootControl.get()) {
    rootControl->UpdateRootView();
  }
}

void CompositionReactViewInstance::UninitRootView() noexcept {
  // m_uiDispatcher will not be initialized if InitRootView has not been run in which case we do not need to run
  // UninitRootView
  if (m_uiDispatcher) {
    assert(m_uiDispatcher.HasThreadAccess());
    if (auto rootControl = m_weakRootControl.get()) {
      rootControl->UninitRootView();
    }
  }
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
CompositionRootView::CompositionRootView(const winrt::Microsoft::UI::Composition::Compositor &compositor) noexcept
    : m_compositor(compositor) {}
#endif

CompositionRootView::~CompositionRootView() noexcept {
  if (m_uiDispatcher) {
    assert(m_uiDispatcher.HasThreadAccess());
    UninitRootView();
  }
}

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

winrt::Microsoft::UI::Composition::Visual CompositionRootView::RootVisual() noexcept {
  return winrt::Microsoft::ReactNative::Composition::Experimental::MicrosoftCompositionContextHelper::InnerVisual(
      m_rootVisual);
}

winrt::Microsoft::ReactNative::Composition::Experimental::IVisual CompositionRootView::InternalRootVisual() noexcept {
  return m_rootVisual;
}

void CompositionRootView::InternalRootVisual(
    winrt::Microsoft::ReactNative::Composition::Experimental::IVisual const &value) noexcept {
  if (m_rootVisual != value) {
    assert(!m_rootVisual);
    m_rootVisual = value;
    UpdateRootVisualSize();
  }
}

void CompositionRootView::AddRenderedVisual(
    const winrt::Microsoft::ReactNative::Composition::Experimental::IVisual &visual) noexcept {
  assert(!m_hasRenderedVisual);
  InternalRootVisual().InsertAt(visual, 0);
  m_hasRenderedVisual = true;
}

void CompositionRootView::RemoveRenderedVisual(
    const winrt::Microsoft::ReactNative::Composition::Experimental::IVisual &visual) noexcept {
  assert(m_hasRenderedVisual);
  InternalRootVisual().Remove(visual);
  m_hasRenderedVisual = false;
}

bool CompositionRootView::TrySetFocus() noexcept {
#ifdef USE_WINUI3
  if (m_island) {
    auto focusController = winrt::Microsoft::UI::Input::InputFocusController::GetForIsland(m_island);
    return focusController.TrySetFocus();
  }
#endif
  return false;
}

winrt::Windows::Foundation::Size CompositionRootView::Size() noexcept {
  return m_size;
}

void CompositionRootView::Size(winrt::Windows::Foundation::Size value) noexcept {
  m_size = value;
  UpdateRootVisualSize();
}

void CompositionRootView::UpdateRootVisualSize() noexcept {
  if (m_rootVisual)
    m_rootVisual.Size({m_size.Width * m_scaleFactor, m_size.Height * m_scaleFactor});

  UpdateLoadingVisualSize();
}

void CompositionRootView::UpdateLoadingVisualSize() noexcept {
  if (m_loadingVisual) {
    auto drawingSurface = CreateLoadingVisualBrush();
    m_loadingVisual.Brush(drawingSurface);
    m_loadingVisual.Offset({0.0f, -(loadingBarHeight * m_scaleFactor / 2.0f), 0.0f}, {0.0f, 0.5f, 0.0f});
    m_loadingVisual.Size({m_size.Width * m_scaleFactor, loadingBarHeight * m_scaleFactor});
    m_loadingActivityVisual.Size(loadingActivitySize * m_scaleFactor);
    const float loadingActivityVerticalOffset = ((loadingBarHeight - (loadingActivitySize * 2)) * m_scaleFactor) / 2;

    m_loadingActivityVisual.Offset(
        {loadingActivityHorizontalOffset * m_scaleFactor, loadingActivityVerticalOffset, 0.0f});
  }
}

float CompositionRootView::ScaleFactor() noexcept {
  return m_scaleFactor;
}

void CompositionRootView::ScaleFactor(float value) noexcept {
  if (m_scaleFactor != value) {
    m_scaleFactor = value;
    // Lifted ContentIslands apply a scale that we need to reverse
    if (auto rootView = RootVisual()) {
      auto invScale = 1.0f / value;
      rootView.Scale({invScale, invScale, invScale});
    }
    UpdateRootVisualSize();
  }
}

winrt::Microsoft::ReactNative::Composition::Theme CompositionRootView::Theme() noexcept {
  if (!m_theme) {
    Theme(winrt::Microsoft::ReactNative::Composition::Theme::GetDefaultTheme(m_context.Handle()));
    m_themeChangedSubscription = m_context.Notifications().Subscribe(
        winrt::Microsoft::ReactNative::ReactNotificationId<void>(
            winrt::Microsoft::ReactNative::Composition::Theme::ThemeChangedEventName()),
        m_context.UIDispatcher(),
        [wkThis = get_weak()](
            IInspectable const & /*sender*/,
            winrt::Microsoft::ReactNative::ReactNotificationArgs<void> const & /*args*/) {
          auto pThis = wkThis.get();
          pThis->Theme(winrt::Microsoft::ReactNative::Composition::Theme::GetDefaultTheme(pThis->m_context.Handle()));
        });
  }
  return m_theme;
}

void CompositionRootView::Theme(const winrt::Microsoft::ReactNative::Composition::Theme &value) noexcept {
  if (m_themeChangedSubscription) {
    m_themeChangedSubscription.Unsubscribe();
    m_themeChangedSubscription = nullptr;
  }

  if (value == m_theme)
    return;

  m_theme = value;

  m_themeChangedRevoker = m_theme.ThemeChanged(
      winrt::auto_revoke,
      [this](
          const winrt::Windows::Foundation::IInspectable & /*sender*/,
          const winrt::Windows::Foundation::IInspectable & /*args*/) {
        if (auto rootView = GetComponentView()) {
          Mso::Functor<bool(const winrt::Microsoft::ReactNative::ComponentView &)> fn =
              [](const winrt::Microsoft::ReactNative::ComponentView &view) noexcept {
                winrt::get_self<winrt::Microsoft::ReactNative::implementation::ComponentView>(view)->onThemeChanged();
                return false;
              };

          winrt::Microsoft::ReactNative::ComponentView view{nullptr};
          winrt::check_hresult(rootView->QueryInterface(
              winrt::guid_of<winrt::Microsoft::ReactNative::ComponentView>(), winrt::put_abi(view)));
          walkTree(view, true, fn);
        }
      });

  if (auto rootView = GetComponentView()) {
    rootView->theme(winrt::get_self<winrt::Microsoft::ReactNative::Composition::implementation::Theme>(value));
  }
}

winrt::IInspectable CompositionRootView::GetUiaProvider() noexcept {
  if (m_uiaProvider == nullptr) {
    m_uiaProvider =
        winrt::make<winrt::Microsoft::ReactNative::implementation::CompositionRootAutomationProvider>(*this);
    if (m_hwnd) {
      auto pRootProvider =
          static_cast<winrt::Microsoft::ReactNative::implementation::CompositionRootAutomationProvider *>(
              m_uiaProvider.as<IRawElementProviderSimple>().get());
      if (pRootProvider != nullptr) {
        pRootProvider->SetHwnd(m_hwnd);
      }
    }
  }
  return m_uiaProvider;
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

void CompositionRootView::SetWindow(uint64_t hwnd) noexcept {
  m_hwnd = reinterpret_cast<HWND>(hwnd);
}

int64_t CompositionRootView::SendMessage(uint32_t msg, uint64_t wParam, int64_t lParam) noexcept {
  if (m_rootTag == -1)
    return 0;

  // SetWindow must be called when not using ContentIsland hosting
  assert(m_hwnd);

  if (m_CompositionEventHandler) {
    auto result = m_CompositionEventHandler->SendMessage(m_hwnd, msg, wParam, lParam);
    if (result)
      return result;
  }

  return 0;
}

bool CompositionRootView::CapturePointer(
    const winrt::Microsoft::ReactNative::Composition::Input::Pointer &pointer,
    facebook::react::Tag tag) noexcept {
  if (m_hwnd) {
    SetCapture(m_hwnd);
  }
  return m_CompositionEventHandler->CapturePointer(pointer, tag);
}

void CompositionRootView::ReleasePointerCapture(
    const winrt::Microsoft::ReactNative::Composition::Input::Pointer &pointer,
    facebook::react::Tag tag) noexcept {
  if (m_CompositionEventHandler->ReleasePointerCapture(pointer, tag)) {
    if (m_hwnd) {
      if (m_hwnd == GetCapture()) {
        ReleaseCapture();
      }
    }
  }
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

  m_reactViewOptions = std::move(viewOptions);
  m_CompositionEventHandler = std::make_shared<::Microsoft::ReactNative::CompositionEventHandler>(m_context, *this);

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

struct AutoMRE {
  ~AutoMRE() {
    mre.Set();
  }
  Mso::ManualResetEvent mre;
};

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

    // This is needed to ensure that the unmount JS logic is completed before the the instance is shutdown during
    // instance destruction. Aligns with similar code in ReactInstanceWin::DetachRootView for paper Future: Instead this
    // method should return a Promise, which should be resolved when the JS logic is complete.
    // The task will auto set the event on destruction to ensure that the event is set if the JS Queue has already been
    // shutdown
    Mso::ManualResetEvent mre;
    m_context.JSDispatcher().Post([autoMRE = std::make_unique<AutoMRE>(AutoMRE{mre})]() {});
    mre.Wait();

    // Paper version gives the JS thread time to finish executing - Is this needed?
    // m_jsMessageThread.Load()->runOnQueueSync([]() {});
  }

  m_rootTag = -1;
  m_context = nullptr;
  m_reactViewOptions = nullptr;

  m_isInitialized = false;
}

void CompositionRootView::ClearLoadingUI() noexcept {
  if (!m_loadingVisual)
    return;

  InternalRootVisual().Remove(m_loadingVisual);

  m_loadingVisual = nullptr;
  m_loadingActivityVisual = nullptr;
}

void CompositionRootView::EnsureLoadingUI() noexcept {}

void CompositionRootView::ShowInstanceLoaded() noexcept {
  if (m_rootVisual) {
    ClearLoadingUI();

    auto uiManager = ::Microsoft::ReactNative::FabricUIManager::FromProperties(
        winrt::Microsoft::ReactNative::ReactPropertyBag(m_context.Properties()));

    auto rootTag = ::Microsoft::ReactNative::getNextRootViewTag();
    SetTag(rootTag);
    auto initProps = DynamicWriter::ToDynamic(Mso::Copy(m_reactViewOptions.InitialProps()));
    if (initProps.isNull()) {
      initProps = folly::dynamic::object();
    }
    initProps["concurrentRoot"] = true;
    uiManager->startSurface(*this, rootTag, JSComponentName(), initProps);

    m_isJSViewAttached = true;
  }
}

void CompositionRootView::ShowInstanceError() noexcept {
  ClearLoadingUI();
}

Composition::Experimental::IDrawingSurfaceBrush CompositionRootView::CreateLoadingVisualBrush() noexcept {
  auto compContext =
      winrt::Microsoft::ReactNative::Composition::implementation::CompositionUIService::GetCompositionContext(
          m_context.Properties().Handle());

  winrt::Windows::Foundation::Size surfaceSize = {
      m_size.Width * m_scaleFactor, std::min(m_size.Height, loadingBarHeight) * m_scaleFactor};
  auto drawingSurface = compContext.CreateDrawingSurfaceBrush(
      surfaceSize,
      winrt::Windows::Graphics::DirectX::DirectXPixelFormat::B8G8R8A8UIntNormalized,
      winrt::Windows::Graphics::DirectX::DirectXAlphaMode::Premultiplied);

  POINT offset;
  {
    ::Microsoft::ReactNative::Composition::AutoDrawDrawingSurface autoDraw(drawingSurface, m_scaleFactor, &offset);
    if (auto d2dDeviceContext = autoDraw.GetRenderTarget()) {
      d2dDeviceContext->Clear(D2D1::ColorF(D2D1::ColorF::Green));

      facebook::react::LayoutConstraints constraints;
      constraints.maximumSize.width = std::max(0.0f, m_size.Width - loadingTextHorizontalOffset);
      constraints.maximumSize.height = std::max(0.0f, m_size.Height - loadingBarHeight);

      auto attributedString = facebook::react::AttributedString{};
      auto fragment = facebook::react::AttributedString::Fragment{};
      fragment.string = "Loading";
      fragment.textAttributes.fontSize = loadingBarFontSize;
      attributedString.appendFragment(fragment);
      auto attributedStringBox = facebook::react::AttributedStringBox{attributedString};

      auto textAttributes = facebook::react::TextAttributes{};
      textAttributes.foregroundColor = facebook::react::whiteColor();

      winrt::com_ptr<::IDWriteTextLayout> textLayout;
      facebook::react::TextLayoutManager::GetTextLayout(
          attributedStringBox, {} /*paragraphAttributes*/, constraints, textLayout);

      DWRITE_TEXT_METRICS tm;
      textLayout->GetMetrics(&tm);
      const float textVerticalOffsetWithinLoadingBar = ((loadingBarHeight - tm.height) * m_scaleFactor) / 2;

      auto theme = winrt::get_self<winrt::Microsoft::ReactNative::Composition::implementation::Theme>(Theme());

      Composition::RenderText(
          *d2dDeviceContext,
          *textLayout,
          attributedStringBox.getValue(),
          textAttributes,
          {static_cast<float>(offset.x + (loadingTextHorizontalOffset * m_scaleFactor)),
           static_cast<float>(offset.y + textVerticalOffsetWithinLoadingBar)},
          m_scaleFactor,
          *theme);
    }
  }

  return drawingSurface;
}

void CompositionRootView::ShowInstanceLoading() noexcept {
  if (!Mso::React::ReactOptions::UseDeveloperSupport(m_context.Properties().Handle()))
    return;

  if (m_loadingVisual)
    return;

  auto compContext =
      winrt::Microsoft::ReactNative::Composition::implementation::CompositionUIService::GetCompositionContext(
          m_context.Properties().Handle());

  auto drawingSurface = CreateLoadingVisualBrush();

  m_loadingVisual = compContext.CreateSpriteVisual();
  m_loadingVisual.Brush(drawingSurface);

  auto foregroundBrush = compContext.CreateColorBrush({255, 255, 255, 255});

  m_loadingActivityVisual = compContext.CreateActivityVisual();
  m_loadingActivityVisual.Brush(foregroundBrush);
  m_loadingVisual.InsertAt(m_loadingActivityVisual, 0);

  UpdateLoadingVisualSize();

  InternalRootVisual().InsertAt(m_loadingVisual, m_hasRenderedVisual ? 1 : 0);
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

    InternalRootVisual(
        winrt::Microsoft::ReactNative::Composition::Experimental::MicrosoftCompositionContextHelper::CreateVisual(
            rootVisual));
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

winrt::Microsoft::ReactNative::Composition::implementation::RootComponentView *
CompositionRootView::GetComponentView() noexcept {
  if (!m_context || m_context.Handle().LoadingState() != winrt::Microsoft::ReactNative::LoadingState::Loaded ||
      m_rootTag == -1)
    return nullptr;

  if (auto fabricuiManager = ::Microsoft::ReactNative::FabricUIManager::FromProperties(
          winrt::Microsoft::ReactNative::ReactPropertyBag(m_context.Properties()))) {
    auto rootComponentViewDescriptor = fabricuiManager->GetViewRegistry().componentViewDescriptorWithTag(
        static_cast<facebook::react::SurfaceId>(m_rootTag));
    return rootComponentViewDescriptor.view
        .as<winrt::Microsoft::ReactNative::Composition::implementation::RootComponentView>()
        .get();
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
