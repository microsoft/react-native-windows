// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.
#include "pch.h"
#include "ReactNativeIsland.h"
#include "FocusNavigationRequest.g.cpp"
#include "ReactNativeIsland.g.cpp"
#include <RootViewSizeChangedEventArgs.g.h>

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
      winrt::weak_ref<winrt::Microsoft::ReactNative::implementation::ReactNativeIsland> &&weakRootControl) noexcept;

  void InitRootView(
      winrt::Microsoft::ReactNative::IReactContext context,
      winrt::Microsoft::ReactNative::ReactViewOptions viewOptions) noexcept;

  void UpdateRootView() noexcept;
  void UninitRootView() noexcept;

 private:
  template <class TAction>
  Mso::Future<void> PostInUIQueue(TAction &&action) noexcept;

 private:
  winrt::weak_ref<winrt::Microsoft::ReactNative::implementation::ReactNativeIsland> m_weakRootControl;
  IReactDispatcher m_uiDispatcher{nullptr};
};

CompositionReactViewInstance::CompositionReactViewInstance(
    winrt::weak_ref<winrt::Microsoft::ReactNative::implementation::ReactNativeIsland> &&weakRootControl) noexcept
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

ReactNativeIsland::ReactNativeIsland() noexcept {}

#ifdef USE_WINUI3
ReactNativeIsland::ReactNativeIsland(const winrt::Microsoft::UI::Composition::Compositor &compositor) noexcept
    : m_compositor(compositor) {}
#endif

ReactNativeIsland::~ReactNativeIsland() noexcept {
#ifdef USE_WINUI3
  if (m_island && m_island.IsConnected()) {
    m_island.AutomationProviderRequested(m_islandAutomationProviderRequestedToken);
  }
#endif

  if (m_uiDispatcher) {
    assert(m_uiDispatcher.HasThreadAccess());
    UninitRootView();
  }
}

ReactNative::IReactViewHost ReactNativeIsland::ReactViewHost() noexcept {
  return m_reactViewHost;
}

void ReactNativeIsland::ReactViewHost(winrt::Microsoft::ReactNative::IReactViewHost const &value) noexcept {
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

winrt::Microsoft::UI::Composition::Visual ReactNativeIsland::RootVisual() noexcept {
  return winrt::Microsoft::ReactNative::Composition::Experimental::MicrosoftCompositionContextHelper::InnerVisual(
      m_rootVisual);
}

winrt::Microsoft::ReactNative::Composition::Experimental::IVisual ReactNativeIsland::InternalRootVisual() noexcept {
  return m_rootVisual;
}

void ReactNativeIsland::InternalRootVisual(
    winrt::Microsoft::ReactNative::Composition::Experimental::IVisual const &value) noexcept {
  if (m_rootVisual != value) {
    assert(!m_rootVisual);
    m_rootVisual = value;
    UpdateRootVisualSize();
  }
}

void ReactNativeIsland::AddRenderedVisual(
    const winrt::Microsoft::ReactNative::Composition::Experimental::IVisual &visual) noexcept {
  assert(!m_hasRenderedVisual);
  InternalRootVisual().InsertAt(visual, 0);
  m_hasRenderedVisual = true;
}

void ReactNativeIsland::RemoveRenderedVisual(
    const winrt::Microsoft::ReactNative::Composition::Experimental::IVisual &visual) noexcept {
  assert(m_hasRenderedVisual);
  InternalRootVisual().Remove(visual);
  m_hasRenderedVisual = false;
}

bool ReactNativeIsland::TrySetFocus() noexcept {
#ifdef USE_WINUI3
  if (m_island && m_island.IsConnected()) {
    auto focusController = winrt::Microsoft::UI::Input::InputFocusController::GetForIsland(m_island);
    return focusController.TrySetFocus();
  }
#endif
  return false;
}

winrt::Windows::Foundation::Size ReactNativeIsland::Size() noexcept {
  return m_size;
}

void ReactNativeIsland::Size(winrt::Windows::Foundation::Size value) noexcept {
  m_size = value;
  UpdateRootVisualSize();
}

void ReactNativeIsland::UpdateRootVisualSize() noexcept {
  if (m_rootVisual)
    m_rootVisual.Size({m_size.Width * m_scaleFactor, m_size.Height * m_scaleFactor});

  UpdateLoadingVisualSize();
}

void ReactNativeIsland::UpdateLoadingVisualSize() noexcept {
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

float ReactNativeIsland::ScaleFactor() noexcept {
  return m_scaleFactor;
}

void ReactNativeIsland::ScaleFactor(float value) noexcept {
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

int64_t ReactNativeIsland::RootTag() const noexcept {
  return m_rootTag;
}

winrt::Microsoft::ReactNative::Composition::ICustomResourceLoader ReactNativeIsland::Resources() noexcept {
  return m_resources;
}

void ReactNativeIsland::Resources(
    const winrt::Microsoft::ReactNative::Composition::ICustomResourceLoader &resources) noexcept {
  m_resources = resources;

  if (m_context && m_theme) {
    Theme(winrt::make<winrt::Microsoft::ReactNative::Composition::implementation::Theme>(m_context, m_resources));
  }
}

winrt::Microsoft::ReactNative::Composition::Theme ReactNativeIsland::Theme() noexcept {
  if (!m_theme) {
    assert(m_context);
    if (m_resources) {
      Theme(winrt::make<winrt::Microsoft::ReactNative::Composition::implementation::Theme>(m_context, m_resources));
    } else {
      Theme(winrt::Microsoft::ReactNative::Composition::Theme::GetDefaultTheme(m_context.Handle()));
    }
  }
  return m_theme;
}

void ReactNativeIsland::Theme(const winrt::Microsoft::ReactNative::Composition::Theme &value) noexcept {
  if (value == m_theme)
    return;

  m_theme = value;

  m_themeChangedRevoker = m_theme.ThemeChanged(
      winrt::auto_revoke,
      [wkThis = get_weak()](
          const winrt::Windows::Foundation::IInspectable & /*sender*/,
          const winrt::Windows::Foundation::IInspectable & /*args*/) {
        if (auto strongThis = wkThis.get()) {
          if (auto rootView = strongThis->GetComponentView()) {
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
        }
      });

  if (auto rootView = GetComponentView()) {
    rootView->theme(winrt::get_self<winrt::Microsoft::ReactNative::Composition::implementation::Theme>(value));
  }
}

winrt::IInspectable ReactNativeIsland::GetUiaProvider() noexcept {
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

void ReactNativeIsland::SetWindow(uint64_t hwnd) noexcept {
  m_hwnd = reinterpret_cast<HWND>(hwnd);
}

int64_t ReactNativeIsland::SendMessage(uint32_t msg, uint64_t wParam, int64_t lParam) noexcept {
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

bool ReactNativeIsland::CapturePointer(
    const winrt::Microsoft::ReactNative::Composition::Input::Pointer &pointer,
    facebook::react::Tag tag) noexcept {
  if (m_hwnd) {
    SetCapture(m_hwnd);
  }
  return m_CompositionEventHandler->CapturePointer(pointer, tag);
}

void ReactNativeIsland::ReleasePointerCapture(
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

void ReactNativeIsland::InitRootView(
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

void ReactNativeIsland::UpdateRootView() noexcept {
  VerifyElseCrash(m_uiDispatcher.HasThreadAccess());
  VerifyElseCrash(m_isInitialized);
  UpdateRootViewInternal();
}

void ReactNativeIsland::UpdateRootViewInternal() noexcept {
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

void ReactNativeIsland::UninitRootView() noexcept {
  if (!m_isInitialized) {
    return;
  }

  if (m_isJSViewAttached) {
    if (m_context.Handle().LoadingState() == winrt::Microsoft::ReactNative::LoadingState::HasError)
      return;

    auto uiManager = ::Microsoft::ReactNative::FabricUIManager::FromProperties(
        winrt::Microsoft::ReactNative::ReactPropertyBag(m_context.Properties()));
    uiManager->stopSurface(static_cast<facebook::react::SurfaceId>(RootTag()));

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

void ReactNativeIsland::ClearLoadingUI() noexcept {
  if (!m_loadingVisual)
    return;

  InternalRootVisual().Remove(m_loadingVisual);

  m_loadingVisual = nullptr;
  m_loadingActivityVisual = nullptr;
}

void ReactNativeIsland::EnsureLoadingUI() noexcept {}

void ReactNativeIsland::ShowInstanceLoaded() noexcept {
  if (m_rootVisual) {
    ClearLoadingUI();

    auto uiManager = ::Microsoft::ReactNative::FabricUIManager::FromProperties(
        winrt::Microsoft::ReactNative::ReactPropertyBag(m_context.Properties()));

    m_rootTag = ::Microsoft::ReactNative::getNextRootViewTag();
    auto initProps = DynamicWriter::ToDynamic(Mso::Copy(m_reactViewOptions.InitialProps()));
    if (initProps.isNull()) {
      initProps = folly::dynamic::object();
    }
    initProps["concurrentRoot"] = true;
    uiManager->startSurface(
        *this,
        static_cast<facebook::react::SurfaceId>(m_rootTag),
        m_layoutConstraints,
        to_string(m_reactViewOptions.ComponentName()),
        initProps);

    m_isJSViewAttached = true;
  }
}

facebook::react::AttributedStringBox CreateLoadingAttributedString() noexcept {
  auto attributedString = facebook::react::AttributedString{};
  auto fragment = facebook::react::AttributedString::Fragment{};
  fragment.string = "Loading";
  fragment.textAttributes.fontSize = loadingBarFontSize;
  attributedString.appendFragment(fragment);
  return facebook::react::AttributedStringBox{attributedString};
}

facebook::react::Size MeasureLoading(const facebook::react::LayoutConstraints &layoutConstraints, float scaleFactor) {
  auto attributedStringBox = CreateLoadingAttributedString();
  winrt::com_ptr<::IDWriteTextLayout> textLayout;
  facebook::react::TextLayoutManager::GetTextLayout(
      attributedStringBox, {} /*paragraphAttributes*/, layoutConstraints, textLayout);

  DWRITE_TEXT_METRICS tm;
  winrt::check_hresult(textLayout->GetMetrics(&tm));

  return layoutConstraints.clamp(
      {loadingActivityHorizontalOffset * scaleFactor + tm.width, loadingBarHeight * scaleFactor});
}

winrt::event_token ReactNativeIsland::SizeChanged(
    winrt::Windows::Foundation::EventHandler<winrt::Microsoft::ReactNative::RootViewSizeChangedEventArgs> const
        &handler) noexcept {
  return m_sizeChangedEvent.add(handler);
}

void ReactNativeIsland::SizeChanged(winrt::event_token const &token) noexcept {
  m_sizeChangedEvent.remove(token);
}

struct RootViewSizeChangedEventArgs : RootViewSizeChangedEventArgsT<RootViewSizeChangedEventArgs> {
  RootViewSizeChangedEventArgs(const winrt::Windows::Foundation::Size &size) : m_size(size) {}
  winrt::Windows::Foundation::Size Size() const noexcept {
    return m_size;
  }

 private:
  const winrt::Windows::Foundation::Size m_size;
};

void ReactNativeIsland::NotifySizeChanged() noexcept {
  auto oldSize = m_size;
  facebook::react::Size size;
  auto rootComponentView = GetComponentView();
  if (rootComponentView) {
    size = rootComponentView->layoutMetrics().frame.size;
  } else if (m_loadingVisual) {
    size = MeasureLoading(m_layoutConstraints, m_scaleFactor);
  }

  m_size = {size.width, size.height};
  UpdateRootVisualSize();
  if (oldSize != m_size) {
    m_sizeChangedEvent(*this, winrt::make<RootViewSizeChangedEventArgs>(m_size));
  }
}

void ReactNativeIsland::ShowInstanceError() noexcept {
  ClearLoadingUI();
}

Composition::Experimental::IDrawingSurfaceBrush ReactNativeIsland::CreateLoadingVisualBrush() noexcept {
  auto compContext =
      winrt::Microsoft::ReactNative::Composition::implementation::CompositionUIService::GetCompositionContext(
          m_context.Properties().Handle());

  if (m_size.Height == 0 || m_size.Width == 0)
    return nullptr;

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

      auto attributedStringBox = CreateLoadingAttributedString();

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

void ReactNativeIsland::ShowInstanceLoading() noexcept {
  if (!Mso::React::ReactOptions::UseDeveloperSupport(m_context.Properties().Handle()))
    return;

  if (m_loadingVisual)
    return;

  auto compContext =
      winrt::Microsoft::ReactNative::Composition::implementation::CompositionUIService::GetCompositionContext(
          m_context.Properties().Handle());

  m_loadingVisual = compContext.CreateSpriteVisual();

  auto foregroundBrush = compContext.CreateColorBrush({255, 255, 255, 255});

  m_loadingActivityVisual = compContext.CreateActivityVisual();
  m_loadingActivityVisual.Brush(foregroundBrush);
  m_loadingVisual.InsertAt(m_loadingActivityVisual, 0);

  NotifySizeChanged();
  UpdateLoadingVisualSize();

  InternalRootVisual().InsertAt(m_loadingVisual, m_hasRenderedVisual ? 1 : 0);
}

void ApplyConstraints(
    const winrt::Microsoft::ReactNative::LayoutConstraints &layoutConstraintsIn,
    facebook::react::LayoutConstraints &layoutConstraintsOut) noexcept {
  layoutConstraintsOut.minimumSize = {layoutConstraintsIn.MinimumSize.Width, layoutConstraintsIn.MinimumSize.Height};
  layoutConstraintsOut.maximumSize = {layoutConstraintsIn.MaximumSize.Width, layoutConstraintsIn.MaximumSize.Height};
  layoutConstraintsOut.layoutDirection =
      static_cast<facebook::react::LayoutDirection>(layoutConstraintsIn.LayoutDirection);
}

winrt::Windows::Foundation::Size ReactNativeIsland::Measure(
    const winrt::Microsoft::ReactNative::LayoutConstraints &layoutConstraints,
    const winrt::Windows::Foundation::Point &viewportOffset) const noexcept {
  facebook::react::Size size{0, 0};

  facebook::react::LayoutConstraints constraints;
  ApplyConstraints(layoutConstraints, constraints);

  if (m_isInitialized && m_rootTag != -1) {
    if (auto fabricuiManager = ::Microsoft::ReactNative::FabricUIManager::FromProperties(
            winrt::Microsoft::ReactNative::ReactPropertyBag(m_context.Properties()))) {
      facebook::react::LayoutContext context;
      // TODO scaling factor
      context.pointScaleFactor = static_cast<facebook::react::Float>(m_scaleFactor);
      context.fontSizeMultiplier = static_cast<facebook::react::Float>(m_scaleFactor);
      context.viewportOffset = {viewportOffset.X, viewportOffset.Y};

      size = fabricuiManager->measureSurface(static_cast<facebook::react::SurfaceId>(m_rootTag), constraints, context);
    }
  } else if (m_loadingVisual) {
    size = MeasureLoading(constraints, m_scaleFactor);
  }

  auto clampedSize = constraints.clamp(size);
  return {clampedSize.width, clampedSize.height};
}

void ReactNativeIsland::Arrange(
    const winrt::Microsoft::ReactNative::LayoutConstraints &layoutConstraints,
    const winrt::Windows::Foundation::Point &viewportOffset) noexcept {
  ApplyConstraints(layoutConstraints, m_layoutConstraints);

  if (m_isInitialized && m_rootTag != -1) {
    if (auto fabricuiManager = ::Microsoft::ReactNative::FabricUIManager::FromProperties(
            winrt::Microsoft::ReactNative::ReactPropertyBag(m_context.Properties()))) {
      facebook::react::LayoutContext context;
      context.pointScaleFactor = static_cast<facebook::react::Float>(m_scaleFactor);
      context.fontSizeMultiplier = static_cast<facebook::react::Float>(m_scaleFactor);
      context.viewportOffset = {viewportOffset.X, viewportOffset.Y};

      fabricuiManager->constraintSurfaceLayout(
          static_cast<facebook::react::SurfaceId>(m_rootTag), m_layoutConstraints, context);
    }
  } else if (m_loadingVisual) {
    // TODO: Resize to align loading
    auto s = m_layoutConstraints.clamp(MeasureLoading(m_layoutConstraints, m_scaleFactor));
    NotifySizeChanged();
  }
}

#ifdef USE_WINUI3
winrt::Microsoft::UI::Content::ContentIsland ReactNativeIsland::Island() {
  if (!m_compositor) {
    return nullptr;
  }

  if (!m_island) {
    winrt::Microsoft::UI::Composition::SpriteVisual rootVisual{nullptr};
    try {
      rootVisual = m_compositor.CreateSpriteVisual();
    } catch (const winrt::hresult_error &e) {
      // If the compositor has been shutdown, then we shouldn't attempt to initialize the island
      if (e.code() == RO_E_CLOSED)
        return nullptr;
      throw e;
    }

    InternalRootVisual(
        winrt::Microsoft::ReactNative::Composition::Experimental::MicrosoftCompositionContextHelper::CreateVisual(
            rootVisual));
    m_island = winrt::Microsoft::UI::Content::ContentIsland::Create(rootVisual);

    // ContentIsland does not support weak_ref, so we cannot use auto_revoke for these events
    m_islandAutomationProviderRequestedToken = m_island.AutomationProviderRequested(
        [weakThis = get_weak()](
            winrt::Microsoft::UI::Content::ContentIsland const &,
            winrt::Microsoft::UI::Content::ContentIslandAutomationProviderRequestedEventArgs const &args) {
          if (auto pThis = weakThis.get()) {
            auto provider = pThis->GetUiaProvider();
            auto pRootProvider =
                static_cast<winrt::Microsoft::ReactNative::implementation::CompositionRootAutomationProvider *>(
                    provider.as<IRawElementProviderSimple>().get());
            if (pRootProvider != nullptr) {
              pRootProvider->SetIsland(pThis->m_island);
            }
            args.AutomationProvider(std::move(provider));
            args.Handled(true);
          }
        });

    m_islandFrameworkClosedToken = m_island.FrameworkClosed([weakThis = get_weak()]() {
      if (auto pThis = weakThis.get()) {
        pThis->m_island = nullptr;
      }
    });
  }
  return m_island;
}
#endif

winrt::Microsoft::ReactNative::Composition::implementation::RootComponentView *
ReactNativeIsland::GetComponentView() noexcept {
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

winrt::Microsoft::ReactNative::FocusNavigationResult ReactNativeIsland::NavigateFocus(
    const winrt::Microsoft::ReactNative::FocusNavigationRequest &request) noexcept {
  if (auto view = GetComponentView()) {
    return winrt::make<winrt::Microsoft::ReactNative::implementation::FocusNavigationResult>(
        view->NavigateFocus(request));
  }
  return winrt::make<winrt::Microsoft::ReactNative::implementation::FocusNavigationResult>(false);
}

} // namespace winrt::Microsoft::ReactNative::implementation
