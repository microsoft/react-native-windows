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

void ReactNativeIsland::ApplyConstraints(
    const winrt::Microsoft::ReactNative::LayoutConstraints &layoutConstraintsIn,
    facebook::react::LayoutConstraints &layoutConstraintsOut) const noexcept {
  layoutConstraintsOut.minimumSize = {layoutConstraintsIn.MinimumSize.Width, layoutConstraintsIn.MinimumSize.Height};
  layoutConstraintsOut.maximumSize = {layoutConstraintsIn.MaximumSize.Width, layoutConstraintsIn.MaximumSize.Height};
  if (layoutConstraintsIn.LayoutDirection == winrt::Microsoft::ReactNative::LayoutDirection::Undefined) {
    if (m_island) {
      layoutConstraintsOut.layoutDirection =
          (m_island.LayoutDirection() == winrt::Microsoft::UI::Content::ContentLayoutDirection::LeftToRight)
          ? facebook::react::LayoutDirection::LeftToRight
          : facebook::react::LayoutDirection::RightToLeft;
    } else if (m_hwnd) {
      auto styles = GetWindowLongPtrW(m_hwnd, GWL_EXSTYLE);
      layoutConstraintsOut.layoutDirection = ((styles & WS_EX_LAYOUTRTL) == WS_EX_LAYOUTRTL)
          ? facebook::react::LayoutDirection::RightToLeft
          : facebook::react::LayoutDirection::LeftToRight;
    }
  } else {
    layoutConstraintsOut.layoutDirection =
        static_cast<facebook::react::LayoutDirection>(layoutConstraintsIn.LayoutDirection);
  }
}

ReactNativeIsland::ReactNativeIsland(const winrt::Microsoft::UI::Composition::Compositor &compositor) noexcept
    : m_compositor(compositor),
      m_layoutConstraints({{0, 0}, {0, 0}, winrt::Microsoft::ReactNative::LayoutDirection::Undefined}) {
  InitTextScaleMultiplier();
}

ReactNativeIsland::ReactNativeIsland(
    const winrt::Microsoft::ReactNative::Composition::PortalComponentView &portal) noexcept
    : m_compositor(portal.ContentRoot().Compositor()),
      m_context(portal.ReactContext()),
      m_layoutConstraints({{0, 0}, {0, 0}, winrt::Microsoft::ReactNative::LayoutDirection::Undefined}),
      m_isFragment(true) {
  m_portal = winrt::make_weak(portal);

  auto trueRoot =
      winrt::get_self<winrt::Microsoft::ReactNative::Composition::implementation::PortalComponentView>(portal)
          ->rootComponentView();
  while (auto p = trueRoot->Portal()) {
    trueRoot = winrt::get_self<winrt::Microsoft::ReactNative::Composition::implementation::PortalComponentView>(p)
                   ->rootComponentView();
  };
  m_rootTag = trueRoot->Tag();

  InitTextScaleMultiplier();
  AddFragmentCompositionEventHandler(m_context.Handle(), portal.ContentRoot());
  auto selfPortal = winrt::get_self<winrt::Microsoft::ReactNative::Composition::implementation::RootComponentView>(
      portal.ContentRoot());
  selfPortal->ReactNativeIsland(*this);
  NotifySizeChanged();
  selfPortal->start(*this);
}

winrt::Microsoft::ReactNative::ReactNativeIsland ReactNativeIsland::CreatePortal(
    const winrt::Microsoft::ReactNative::Composition::PortalComponentView &portal) noexcept {
  return winrt::make<ReactNativeIsland>(portal);
}

ReactNativeIsland::ReactNativeIsland() noexcept
    : ReactNativeIsland(winrt::Microsoft::UI::Composition::Compositor{nullptr}) {}

ReactNativeIsland::~ReactNativeIsland() noexcept {
#ifdef USE_WINUI3
  if (m_island) {
    m_island.AutomationProviderRequested(m_islandAutomationProviderRequestedToken);
    m_island.StateChanged(m_islandStateChangedToken);
#ifdef USE_EXPERIMENTAL_WINUI3
    m_island.Connected(m_islandConnectedToken);
    m_island.Disconnected(m_islandDisconnectedToken);
#endif
  }
#endif

  if (m_uiDispatcher) {
    assert(m_uiDispatcher.HasThreadAccess());
    UninitRootView();
  }

  if (m_island) {
    m_island.Close();
  }
}

ReactNative::IReactViewHost ReactNativeIsland::ReactViewHost() noexcept {
  return m_reactViewHost;
}

void ReactNativeIsland::ReactViewHost(winrt::Microsoft::ReactNative::IReactViewHost const &value) {
  if (m_isFragment)
    winrt::throw_hresult(E_ACCESSDENIED);

  if (m_reactViewHost == value) {
    return;
  }

  m_props = nullptr;

  if (m_reactViewHost) {
    UninitRootView();
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

  if (m_mounted) {
    if (auto componentView = GetComponentView()) {
      componentView->onMounted();
    }
  }
}

void ReactNativeIsland::RemoveRenderedVisual(
    const winrt::Microsoft::ReactNative::Composition::Experimental::IVisual &visual) noexcept {
  if (m_isFragment)
    return;
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
    Arrange(m_layoutConstraints, m_viewportOffset);
  }
}

float ReactNativeIsland::FontSizeMultiplier() const noexcept {
  return m_textScaleMultiplier;
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
    if (m_hwnd && !m_island) {
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

winrt::Windows::Foundation::Point ReactNativeIsland::ConvertScreenToLocal(
    winrt::Windows::Foundation::Point pt) noexcept {
  if (m_island) {
    auto pp = m_island.CoordinateConverter().ConvertScreenToLocal(
        winrt::Windows::Graphics::PointInt32{static_cast<int32_t>(pt.X), static_cast<int32_t>(pt.Y)});
    return {static_cast<float>(pp.X), static_cast<float>(pp.Y)};
  }
  POINT p{static_cast<LONG>(pt.X), static_cast<LONG>(pt.Y)};
  ScreenToClient(m_hwnd, &p);
  return {static_cast<float>(p.x) / m_scaleFactor, static_cast<float>(p.y) / m_scaleFactor};
}

winrt::Windows::Foundation::Point ReactNativeIsland::ConvertLocalToScreen(
    winrt::Windows::Foundation::Point pt) noexcept {
  if (m_island) {
    auto pp = m_island.CoordinateConverter().ConvertLocalToScreen(pt);
    return {static_cast<float>(pp.X), static_cast<float>(pp.Y)};
  }
  POINT p{static_cast<LONG>(pt.X * m_scaleFactor), static_cast<LONG>(pt.Y * m_scaleFactor)};
  ClientToScreen(m_hwnd, &p);
  return {static_cast<float>(p.x), static_cast<float>(p.y)};
}

void ReactNativeIsland::SetWindow(uint64_t hwnd) noexcept {
  m_hwnd = reinterpret_cast<HWND>(hwnd);
}

HWND ReactNativeIsland::GetHwndForParenting() noexcept {
  return m_hwnd;
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
  if (m_hwnd && !m_island) {
    SetCapture(m_hwnd);
  }
  return m_CompositionEventHandler->CapturePointer(pointer, tag);
}

void ReactNativeIsland::ReleasePointerCapture(
    const winrt::Microsoft::ReactNative::Composition::Input::Pointer &pointer,
    facebook::react::Tag tag) noexcept {
  if (m_CompositionEventHandler->ReleasePointerCapture(pointer, tag)) {
    if (m_hwnd && !m_island) {
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
  m_CompositionEventHandler->Initialize();

  UpdateRootViewInternal();

  m_isInitialized = true;
}

void ReactNativeIsland::AddFragmentCompositionEventHandler(
    winrt::Microsoft::ReactNative::IReactContext context,
    winrt::Microsoft::ReactNative::ComponentView componentView) noexcept {
  m_uiDispatcher = context.Properties()
                       .Get(winrt::Microsoft::ReactNative::ReactDispatcherHelper::UIDispatcherProperty())
                       .try_as<IReactDispatcher>();
  VerifyElseCrash(m_uiDispatcher.HasThreadAccess());
  auto uiManager = ::Microsoft::ReactNative::FabricUIManager::FromProperties(
      winrt::Microsoft::ReactNative::ReactPropertyBag(context.Properties()));

  if (!m_CompositionEventHandler) {
    // Create CompositionEventHandler if not already created
    m_context = winrt::Microsoft::ReactNative::ReactContext(context);
    m_CompositionEventHandler = std::make_shared<::Microsoft::ReactNative::CompositionEventHandler>(m_context, *this);
    m_CompositionEventHandler->Initialize();
    m_isInitialized = true;
  }
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
    // instance destruction. Aligns with similar code in ReactInstanceWin::DetachRootView for paper Future: Instead
    // this method should return a Promise, which should be resolved when the JS logic is complete. The task will auto
    // set the event on destruction to ensure that the event is set if the JS Queue has already been shutdown
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
  VerifyElseCrash(!m_isFragment);
  if (m_rootVisual) {
    ClearLoadingUI();

    auto uiManager = ::Microsoft::ReactNative::FabricUIManager::FromProperties(
        winrt::Microsoft::ReactNative::ReactPropertyBag(m_context.Properties()));

    m_rootTag = ::Microsoft::ReactNative::getNextRootViewTag();

    auto initProps =
        m_props.isNull() ? m_props : DynamicWriter::ToDynamic(Mso::Copy(m_reactViewOptions.InitialProps()));
    if (initProps.isNull()) {
      initProps = folly::dynamic::object();
    }
    initProps["concurrentRoot"] = true;

    facebook::react::LayoutConstraints fbLayoutConstraints;
    ApplyConstraints(m_layoutConstraints, fbLayoutConstraints);

    uiManager->startSurface(
        *this,
        static_cast<facebook::react::SurfaceId>(m_rootTag),
        fbLayoutConstraints,
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
  attributedString.appendFragment(std::move(fragment));
  return facebook::react::AttributedStringBox{attributedString};
}

facebook::react::Size ReactNativeIsland::MeasureLoading(
    const winrt::Microsoft::ReactNative::LayoutConstraints &layoutConstraints) const noexcept {
  facebook::react::LayoutConstraints fbLayoutConstraints;
  ApplyConstraints(layoutConstraints, fbLayoutConstraints);

  auto attributedStringBox = CreateLoadingAttributedString();
  winrt::com_ptr<::IDWriteTextLayout> textLayout;
  facebook::react::TextLayoutManager::GetTextLayout(
      attributedStringBox, {} /*paragraphAttributes*/, fbLayoutConstraints, textLayout);

  DWRITE_TEXT_METRICS tm;
  winrt::check_hresult(textLayout->GetMetrics(&tm));

  return fbLayoutConstraints.clamp(
      {loadingActivityHorizontalOffset * m_scaleFactor + tm.width, loadingBarHeight * m_scaleFactor});
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
    size = MeasureLoading(m_layoutConstraints);
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

void ReactNativeIsland::InitTextScaleMultiplier() noexcept {
  m_uiSettings = winrt::Windows::UI::ViewManagement::UISettings();
  m_textScaleMultiplier = static_cast<float>(m_uiSettings.TextScaleFactor());
  m_textScaleChangedRevoker = m_uiSettings.TextScaleFactorChanged(
      winrt::auto_revoke,
      [this](const winrt::Windows::UI::ViewManagement::UISettings &uiSettings, const winrt::IInspectable &) {
        if (m_context) {
          m_context.UIDispatcher().Post(
              [wkThis = get_weak(), textScaleMultiplier = static_cast<float>(uiSettings.TextScaleFactor())]() {
                if (auto strongThis = wkThis.get()) {
                  strongThis->m_textScaleMultiplier = textScaleMultiplier;
                  strongThis->Arrange(strongThis->m_layoutConstraints, strongThis->m_viewportOffset);
                }
              });
        }
      });
}

winrt::Windows::Foundation::Size ReactNativeIsland::Measure(
    const winrt::Microsoft::ReactNative::LayoutConstraints &layoutConstraints,
    const winrt::Windows::Foundation::Point &viewportOffset) const {
  if (m_isFragment)
    winrt::throw_hresult(E_ILLEGAL_METHOD_CALL);

  facebook::react::Size size{0, 0};

  if (layoutConstraints.LayoutDirection != winrt::Microsoft::ReactNative::LayoutDirection::LeftToRight &&
      layoutConstraints.LayoutDirection != winrt::Microsoft::ReactNative::LayoutDirection::RightToLeft &&
      layoutConstraints.LayoutDirection != winrt::Microsoft::ReactNative::LayoutDirection::Undefined)
    winrt::throw_hresult(E_INVALIDARG);

  facebook::react::LayoutConstraints constraints;
  ApplyConstraints(layoutConstraints, constraints);

  if (m_isInitialized && m_rootTag != -1 && m_hasRenderedVisual) {
    if (auto fabricuiManager = ::Microsoft::ReactNative::FabricUIManager::FromProperties(
            winrt::Microsoft::ReactNative::ReactPropertyBag(m_context.Properties()))) {
      facebook::react::LayoutContext context;
      context.fontSizeMultiplier = m_textScaleMultiplier;
      context.pointScaleFactor = static_cast<facebook::react::Float>(m_scaleFactor);
      context.viewportOffset = {viewportOffset.X, viewportOffset.Y};

      size = fabricuiManager->measureSurface(static_cast<facebook::react::SurfaceId>(m_rootTag), constraints, context);
    }
  } else if (m_loadingVisual) {
    size = MeasureLoading(layoutConstraints);
  }

  auto clampedSize = constraints.clamp(size);
  return {clampedSize.width, clampedSize.height};
}

void ReactNativeIsland::Arrange(
    const winrt::Microsoft::ReactNative::LayoutConstraints &layoutConstraints,
    const winrt::Windows::Foundation::Point &viewportOffset) {
  if (layoutConstraints.LayoutDirection != winrt::Microsoft::ReactNative::LayoutDirection::LeftToRight &&
      layoutConstraints.LayoutDirection != winrt::Microsoft::ReactNative::LayoutDirection::RightToLeft &&
      layoutConstraints.LayoutDirection != winrt::Microsoft::ReactNative::LayoutDirection::Undefined)
    winrt::throw_hresult(E_INVALIDARG);

  m_layoutConstraints = layoutConstraints;
  m_viewportOffset = viewportOffset;
  facebook::react::LayoutConstraints fbLayoutConstraints;
  ApplyConstraints(layoutConstraints, fbLayoutConstraints);

  if (m_isInitialized && m_rootTag != -1 && !m_isFragment && m_hasRenderedVisual) {
    if (auto fabricuiManager = ::Microsoft::ReactNative::FabricUIManager::FromProperties(
            winrt::Microsoft::ReactNative::ReactPropertyBag(m_context.Properties()))) {
      facebook::react::LayoutContext context;
      context.fontSizeMultiplier = m_textScaleMultiplier;
      context.pointScaleFactor = static_cast<facebook::react::Float>(m_scaleFactor);
      context.viewportOffset = {viewportOffset.X, viewportOffset.Y};

      fabricuiManager->constraintSurfaceLayout(
          static_cast<facebook::react::SurfaceId>(m_rootTag), fbLayoutConstraints, context);
    }
  } else if (m_loadingVisual) {
    // TODO: Resize to align loading
    auto s = fbLayoutConstraints.clamp(MeasureLoading(layoutConstraints));
    NotifySizeChanged();
  }
}

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

    auto focusController = winrt::Microsoft::UI::Input::InputFocusController::GetForIsland(m_island);
    focusController.NavigateFocusRequested(
        [weakThis = get_weak()](
            const auto &sender, const winrt::Microsoft::UI::Input::FocusNavigationRequestEventArgs &args) {
          if (auto pThis = weakThis.get()) {
            if (auto rootView = pThis->GetComponentView()) {
              args.Result(
                  rootView->NavigateFocus(winrt::Microsoft::ReactNative::FocusNavigationRequest(
                      winrt::Microsoft::ReactNative::FocusNavigationReason::First))
                      ? winrt::Microsoft::UI::Input::FocusNavigationResult::Moved
                      : winrt::Microsoft::UI::Input::FocusNavigationResult::NotMoved);
            } else {
              args.Result(winrt::Microsoft::UI::Input::FocusNavigationResult::NoFocusableElements);
            }
          }
        });

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

    m_islandStateChangedToken =
        m_island.StateChanged([weakThis = get_weak()](
                                  winrt::Microsoft::UI::Content::ContentIsland const &island,
                                  winrt::Microsoft::UI::Content::ContentIslandStateChangedEventArgs const &args) {
          if (auto pThis = weakThis.get()) {
            if (args.DidRasterizationScaleChange()) {
              pThis->ScaleFactor(island.RasterizationScale());
            }
            if (args.DidLayoutDirectionChange()) {
              pThis->Arrange(pThis->m_layoutConstraints, pThis->m_viewportOffset);
            }
          }
        });
#ifdef USE_EXPERIMENTAL_WINUI3
    if (!m_isFragment) {
      m_islandConnectedToken = m_island.Connected(
          [weakThis = get_weak()](
              winrt::IInspectable const &, winrt::Microsoft::UI::Content::ContentIsland const &island) {
            if (auto pThis = weakThis.get()) {
              pThis->OnMounted();
            }
          });

      m_islandDisconnectedToken = m_island.Disconnected(
          [weakThis = get_weak()](
              winrt::IInspectable const &, winrt::Microsoft::UI::Content::ContentIsland const &island) {
            if (auto pThis = weakThis.get()) {
              pThis->OnUnmounted();
            }
          });
    }
#endif
  }
  return m_island;
}

void ReactNativeIsland::OnMounted() noexcept {
  if (m_mounted)
    return;
  m_mounted = true;
  if (auto componentView = GetComponentView()) {
    if (!componentView->isMounted()) {
      componentView->onMounted();
    }
  }
}

void ReactNativeIsland::OnUnmounted() noexcept {
  if (!m_mounted)
    return;
  m_mounted = false;

  if (m_island && m_island.IsConnected()) {
    auto focusController = winrt::Microsoft::UI::Input::InputFocusController::GetForIsland(m_island);
    auto request = winrt::Microsoft::UI::Input::FocusNavigationRequest::Create(
        winrt::Microsoft::UI::Input::FocusNavigationReason::Programmatic);
    if (focusController.HasFocus()) {
      focusController.DepartFocus(request);
    }
  }

  if (auto componentView = GetComponentView()) {
    componentView->onUnmounted();
  }
}

void ReactNativeIsland::SetProperties(winrt::Microsoft::ReactNative::JSValueArgWriter props) noexcept {
  auto initProps = DynamicWriter::ToDynamic(props);
  if (initProps.isNull()) {
    initProps = folly::dynamic::object();
  }

  if (m_isJSViewAttached) {
    if (auto fabricuiManager = ::Microsoft::ReactNative::FabricUIManager::FromProperties(
            winrt::Microsoft::ReactNative::ReactPropertyBag(m_context.Properties()))) {
      initProps["concurrentRoot"] = true;
      fabricuiManager->setProps(static_cast<facebook::react::SurfaceId>(m_rootTag), initProps);
      return;
    }
  }

  m_props = initProps;
}

winrt::com_ptr<winrt::Microsoft::ReactNative::Composition::implementation::RootComponentView>
ReactNativeIsland::GetComponentView() noexcept {
  if (auto portal = m_portal.get()) {
    return winrt::get_self<winrt::Microsoft::ReactNative::Composition::implementation::PortalComponentView>(portal)
        ->ContentRoot()
        .as<winrt::Microsoft::ReactNative::Composition::implementation::RootComponentView>();
  }

  if (!m_context || m_context.Handle().LoadingState() != winrt::Microsoft::ReactNative::LoadingState::Loaded ||
      m_rootTag == -1)
    return nullptr;

  if (auto fabricuiManager = ::Microsoft::ReactNative::FabricUIManager::FromProperties(
          winrt::Microsoft::ReactNative::ReactPropertyBag(m_context.Properties()))) {
    if (auto view = fabricuiManager->GetViewRegistry().findComponentViewWithTag(
            static_cast<facebook::react::SurfaceId>(m_rootTag))) {
      return view.as<winrt::Microsoft::ReactNative::Composition::implementation::RootComponentView>();
    }
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
