// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"

#include "ReactRootControl.h"

#include <CxxMessageQueue.h>
#include <ReactUWP/InstanceFactory.h>
#include <Utils/ValueUtils.h>
#include "Unicode.h"

#include <INativeUIManager.h>
#include <Views/KeyboardEventHandler.h>
#include <Views/ShadowNodeBase.h>
#include <crash/verifyElseCrash.h>
#include "Threading/MessageQueueThreadFactory.h"

#include <winrt/Windows.ApplicationModel.Core.h>
#include <winrt/Windows.Devices.Input.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.UI.Core.h>
#include <winrt/Windows.UI.Input.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Input.h>
#include <winrt/Windows.UI.Xaml.Markup.h>
#include <winrt/Windows.UI.Xaml.Media.Media3D.h>
#include <winrt/Windows.UI.Xaml.Media.h>
#include <winrt/Windows.UI.Xaml.h>

#include <ReactHost/React.h>
#include <ReactHost/React_Win.h>
#include <dispatchQueue/dispatchQueue.h>
#include <object/unknownObject.h>

#include <ReactHost/MsoUtils.h>

namespace react::uwp {

//===========================================================================
// ReactRootControl implementation
//===========================================================================

ReactRootControl::ReactRootControl(XamlView const &rootView) noexcept
    : m_weakRootView{rootView}, m_uiQueue(Mso::DispatchQueue::MakeCurrentThreadUIQueue()) {
  PrepareXamlRootView(rootView);
}

ReactRootControl::~ReactRootControl() noexcept {
  // TODO: detach from react instance

  // remove safe harbor and child grid from visual tree
  if (m_focusSafeHarbor) {
    if (auto root = m_focusSafeHarbor.Parent().try_as<winrt::Panel>()) {
      root.Children().Clear();
    }
  }
}

std::shared_ptr<IReactInstance> ReactRootControl::GetReactInstance() const noexcept {
  if (auto reactInstance = m_weakReactInstance.GetStrongPtr()) {
    auto &legacyInstance = query_cast<Mso::React::ILegacyReactInstance &>(*reactInstance);
    return legacyInstance.UwpReactInstance();
  }

  return {};
}

XamlView ReactRootControl::GetXamlView() const noexcept {
  return m_weakXamlRootView.get();
}

void ReactRootControl::SetJSComponentName(std::string && /*mainComponentName*/) noexcept {
  VerifyElseCrashSz(false, "Deprecated. Use ReactViewHost.");
}

void ReactRootControl::SetInstanceCreator(const ReactInstanceCreator & /*instanceCreator*/) noexcept {
  VerifyElseCrashSz(false, "Deprecated. Use ReactViewHost.");
}

void ReactRootControl::SetInitialProps(folly::dynamic && /*initialProps*/) noexcept {
  VerifyElseCrashSz(false, "Deprecated. Use ReactViewHost.");
}

void ReactRootControl::AttachRoot() noexcept {
  VerifyElseCrashSz(false, "Deprecated. Use ReactViewHost.");
}

void ReactRootControl::DetachRoot() noexcept {
  VerifyElseCrashSz(false, "Deprecated. Use ReactViewHost.");
}

std::shared_ptr<IXamlReactControl> ReactRootControl::GetXamlReactControl() const noexcept {
  return std::static_pointer_cast<IXamlReactControl>(const_cast<ReactRootControl *>(this)->shared_from_this());
}

void ReactRootControl::ResetView() noexcept {
  // Ignore the call.
}

std::string ReactRootControl::JSComponentName() const noexcept {
  return m_reactViewOptions->ComponentName;
}

int64_t ReactRootControl::GetActualHeight() const noexcept {
  if (auto xamlRootView = m_weakXamlRootView.get()) {
    if (auto element = xamlRootView.as<winrt::FrameworkElement>()) {
      return static_cast<int64_t>(element.ActualHeight());
    }
  }

  return 0;
}

int64_t ReactRootControl::GetActualWidth() const noexcept {
  if (auto xamlRootView = m_weakXamlRootView.get()) {
    if (auto element = xamlRootView.as<winrt::FrameworkElement>()) {
      return static_cast<int64_t>(element.ActualWidth());
    }
  }

  return 0;
}

int64_t ReactRootControl::GetTag() const noexcept {
  return m_rootTag;
}

void ReactRootControl::SetTag(int64_t tag) noexcept {
  m_rootTag = tag;
}

// Xaml doesn't provide Blur.
// If 'focus safe harbor' exists, make harbor to allow tabstop and focus on
// harbor with ::Pointer otherwise, just changing the FocusState to ::Pointer
// for the element.
void ReactRootControl::blur(XamlView const &xamlView) noexcept {
  EnsureFocusSafeHarbor();
  if (m_focusSafeHarbor) {
    m_focusSafeHarbor.IsTabStop(true);
    winrt::FocusManager::TryFocusAsync(m_focusSafeHarbor, winrt::FocusState::Pointer);
  } else
    winrt::FocusManager::TryFocusAsync(xamlView, winrt::FocusState::Pointer);
}

void ReactRootControl::InitRootView(
    Mso::CntPtr<Mso::React::IReactInstance> &&reactInstance,
    Mso::React::ReactViewOptions &&reactViewOptions) noexcept {
  VerifyElseCrash(m_uiQueue.HasThreadAccess());

  if (m_isInitialized) {
    UninitRootView();
  }

  m_weakReactInstance = Mso::WeakPtr{reactInstance};
  auto &legacyReactInstance = query_cast<Mso::React::ILegacyReactInstance &>(*reactInstance);
  m_reactOptions = std::make_unique<Mso::React::ReactOptions>(reactInstance->Options());
  m_reactViewOptions = std::make_unique<Mso::React::ReactViewOptions>(std::move(reactViewOptions));

  const auto &devSettings = m_reactOptions->DeveloperSettings;
  m_useLiveReload = devSettings.UseLiveReload;
  m_useWebDebugger = devSettings.UseWebDebugger;
  if (devSettings.IsDevModeEnabled) {
    InitializeDeveloperMenu();
  }

  auto uwpReactInstance = legacyReactInstance.UwpReactInstance();
  if (!m_touchEventHandler) {
    m_touchEventHandler = std::make_shared<TouchEventHandler>(uwpReactInstance);
  }

  if (!m_SIPEventHandler) {
    m_SIPEventHandler = std::make_shared<SIPEventHandler>(uwpReactInstance);
  }

  if (!m_previewKeyboardEventHandlerOnRoot) {
    m_previewKeyboardEventHandlerOnRoot = std::make_shared<PreviewKeyboardEventHandlerOnRoot>(uwpReactInstance);
  }

  auto xamlRootView = m_weakXamlRootView.get();
  m_touchEventHandler->AddTouchHandlers(xamlRootView);
  m_previewKeyboardEventHandlerOnRoot->hook(xamlRootView);
  m_SIPEventHandler->AttachView(xamlRootView, /*fireKeyboradEvents:*/ true);

  UpdateRootViewInternal();

  m_isInitialized = true;
}

void ReactRootControl::UpdateRootView() noexcept {
  VerifyElseCrash(m_uiQueue.HasThreadAccess());
  VerifyElseCrash(m_isInitialized);
  UpdateRootViewInternal();
}

void ReactRootControl::UpdateRootViewInternal() noexcept {
  if (auto reactInstance = m_weakReactInstance.GetStrongPtr()) {
    switch (reactInstance->State()) {
      case Mso::React::ReactInstanceState::Loading:
        ShowInstanceLoading(*reactInstance);
        break;
      case Mso::React::ReactInstanceState::WaitingForDebugger:
        ShowInstanceWaiting(*reactInstance);
        break;
      case Mso::React::ReactInstanceState::Loaded:
        ShowInstanceLoaded(*reactInstance);
        break;
      case Mso::React::ReactInstanceState::HasError:
        ShowInstanceError(*reactInstance);
        break;
      default:
        VerifyElseCrashSz(false, "Unexpected value");
    }
  }
}

void ReactRootControl::UninitRootView() noexcept {
  if (!m_isInitialized) {
    return;
  }

  if (auto fbReactInstance = m_fbReactInstance.lock()) {
    fbReactInstance->DetachRootView(this);
  }

  if (m_touchEventHandler != nullptr) {
    m_touchEventHandler->RemoveTouchHandlers();
  }

  if (!m_previewKeyboardEventHandlerOnRoot) {
    m_previewKeyboardEventHandlerOnRoot->unhook();
  }

  // If the redbox error UI is shown we need to remove it, otherwise let the
  // natural teardown process do this
  if (m_redBoxGrid) {
    if (auto xamlRootView = m_weakXamlRootView.get()) {
      if (auto grid = xamlRootView.try_as<winrt::Grid>()) {
        grid.Children().Clear();
      }
    }

    m_redBoxGrid = nullptr;
    m_errorTextBlock = nullptr;
  }

  // Clear members with a dependency on the reactInstance
  m_touchEventHandler.reset();
  m_SIPEventHandler.reset();

  m_reactOptions = nullptr;
  m_reactViewOptions = nullptr;
  m_weakReactInstance = nullptr;

  m_isInitialized = false;
}

void ReactRootControl::ShowInstanceLoaded(Mso::React::IReactInstance &reactInstance) noexcept {
  if (XamlView xamlRootView = m_weakXamlRootView.get()) {
    auto xamlRootGrid{xamlRootView.as<winrt::Grid>()};

    // Remove existing children from root view (from the hosted app)
    xamlRootGrid.Children().Clear();

    auto &legacyInstance = query_cast<Mso::React::ILegacyReactInstance &>(reactInstance);
    legacyInstance.AttachMeasuredRootView(this, Mso::Copy(m_reactViewOptions->InitialProps));
    m_isJSViewAttached = true;
  }
}

void ReactRootControl::ShowInstanceError(Mso::React::IReactInstance &reactInstance) noexcept {
  if (XamlView xamlRootView = m_weakXamlRootView.get()) {
    auto xamlRootGrid{xamlRootView.as<winrt::Grid>()};

    // Remove existing children from root view (from the hosted app)
    xamlRootGrid.Children().Clear();

    // Create Grid & TextBlock to hold error text
    if (!m_errorTextBlock) {
      m_errorTextBlock = winrt::TextBlock{};
      m_redBoxGrid = winrt::Grid{};
      m_redBoxGrid.Background(winrt::SolidColorBrush{winrt::ColorHelper::FromArgb(0xee, 0xcc, 0, 0)});
      m_redBoxGrid.Children().Append(m_errorTextBlock);
    }

    // Add red box grid to root view
    xamlRootGrid.Children().Append(m_redBoxGrid);

    // Place error message into TextBlock
    std::wstring wstrErrorMessage{L"ERROR: Instance failed to start.\n\n"};
    wstrErrorMessage += Microsoft::Common::Unicode::Utf8ToUtf16(reactInstance.LastErrorMessage()).c_str();
    m_errorTextBlock.Text(wstrErrorMessage);

    // Format TextBlock
    m_errorTextBlock.TextAlignment(winrt::TextAlignment::Center);
    m_errorTextBlock.TextWrapping(winrt::TextWrapping::Wrap);
    m_errorTextBlock.FontFamily(winrt::FontFamily(L"Consolas"));
    m_errorTextBlock.Foreground(winrt::SolidColorBrush(winrt::Colors::White()));
    winrt::Thickness margin = {10.0f, 10.0f, 10.0f, 10.0f};
    m_errorTextBlock.Margin(margin);
  }
}

void ReactRootControl::ShowInstanceWaiting(Mso::React::IReactInstance &reactInstance) noexcept {
  if (XamlView xamlRootView = m_weakXamlRootView.get()) {
    auto xamlRootGrid{xamlRootView.as<winrt::Grid>()};

    // Remove existing children from root view (from the hosted app)
    xamlRootGrid.Children().Clear();

    // Create Grid & TextBlock to hold text
    if (m_waitingTextBlock == nullptr) {
      m_waitingTextBlock = winrt::TextBlock();
      m_greenBoxGrid = winrt::Grid{};
      m_greenBoxGrid.Background(winrt::SolidColorBrush(winrt::ColorHelper::FromArgb(0xff, 0x03, 0x59, 0)));
      m_greenBoxGrid.Children().Append(m_waitingTextBlock);
      m_greenBoxGrid.VerticalAlignment(winrt::Windows::UI::Xaml::VerticalAlignment::Center);
    }

    // Add box grid to root view
    xamlRootGrid.Children().Append(m_greenBoxGrid);

    // Place message into TextBlock
    std::wstring wstrMessage(L"Connecting to remote debugger");
    m_waitingTextBlock.Text(wstrMessage);

    // Format TextBlock
    m_waitingTextBlock.TextAlignment(winrt::TextAlignment::Center);
    m_waitingTextBlock.TextWrapping(winrt::TextWrapping::Wrap);
    m_waitingTextBlock.FontFamily(winrt::FontFamily(L"Consolas"));
    m_waitingTextBlock.Foreground(winrt::SolidColorBrush(winrt::Colors::White()));
    winrt::Thickness margin = {10.0f, 10.0f, 10.0f, 10.0f};
    m_waitingTextBlock.Margin(margin);
  }
}

void ReactRootControl::ShowInstanceLoading(Mso::React::IReactInstance &reactInstance) noexcept {
  if (XamlView xamlRootView = m_weakXamlRootView.get()) {
    auto xamlRootGrid{xamlRootView.as<winrt::Grid>()};

    // Remove existing children from root view (from the hosted app)
    xamlRootGrid.Children().Clear();

    // Create Grid & TextBlock to hold text
    if (m_waitingTextBlock == nullptr) {
      m_waitingTextBlock = winrt::TextBlock();
      m_greenBoxGrid = winrt::Grid{};
      m_greenBoxGrid.Background(winrt::SolidColorBrush(winrt::ColorHelper::FromArgb(0xff, 0x03, 0x59, 0)));
      m_greenBoxGrid.Children().Append(m_waitingTextBlock);
      m_greenBoxGrid.VerticalAlignment(winrt::Windows::UI::Xaml::VerticalAlignment::Center);
    }

    // Add box grid to root view
    xamlRootGrid.Children().Append(m_greenBoxGrid);

    // Place message into TextBlock
    std::wstring wstrMessage(L"Loading bundle.");
    m_waitingTextBlock.Text(wstrMessage);

    // Format TextBlock
    m_waitingTextBlock.TextAlignment(winrt::TextAlignment::Center);
    m_waitingTextBlock.TextWrapping(winrt::TextWrapping::Wrap);
    m_waitingTextBlock.FontFamily(winrt::FontFamily(L"Consolas"));
    m_waitingTextBlock.Foreground(winrt::SolidColorBrush(winrt::Colors::White()));
    winrt::Thickness margin = {10.0f, 10.0f, 10.0f, 10.0f};
    m_waitingTextBlock.Margin(margin);
  }
}

void ReactRootControl::PrepareXamlRootView(XamlView const &rootView) noexcept {
  if (auto panel = rootView.try_as<winrt::Panel>()) {
    // Xaml don't have blur concept.
    // A ContentControl is created in the middle to act as a 'focus safe harbor'
    // When a XamlView is blurred, make the ContentControl to allow tabstop, and
    // move the pointer focus to safe harbor When the safe harbor is
    // LosingFocus, disable tabstop on ContentControl. The creation of safe
    // harbor is delayed to EnsureFocusSafeHarbor
    auto children = panel.Children();
    children.Clear();

    auto newRootView = winrt::Grid{};
    // Xaml's default projection in 3D is orthographic (all lines are parallel)
    // However React Native's default projection is a one-point perspective.
    // Set a default perspective projection on the main control to mimic this.
    auto perspectiveTransform3D = winrt::Windows::UI::Xaml::Media::Media3D::PerspectiveTransform3D();
    perspectiveTransform3D.Depth(850);
    winrt::Windows::UI::Xaml::Media::Media3D::Transform3D t3d(perspectiveTransform3D);
    newRootView.Transform3D(t3d);
    children.Append(newRootView);
    m_weakXamlRootView = newRootView.try_as<XamlView>();
  } else {
    m_weakXamlRootView = rootView;
  }
}

void ReactRootControl::EnsureFocusSafeHarbor() noexcept {
  auto rootView = m_weakRootView.get();
  auto xamlRootView = m_weakXamlRootView.get();
  if (!m_focusSafeHarbor && rootView && xamlRootView != rootView) {
    // focus safe harbor is delayed to be inserted to the visual tree
    auto panel = rootView.try_as<winrt::Panel>();
    VerifyElseCrash(panel.Children().Size() == 1);

    m_focusSafeHarbor = winrt::ContentControl{};
    m_focusSafeHarbor.Width(0.0);
    m_focusSafeHarbor.IsTabStop(false);
    panel.Children().InsertAt(0, m_focusSafeHarbor);

    m_focusSafeHarborLosingFocusRevoker = m_focusSafeHarbor.LosingFocus(
        winrt::auto_revoke,
        [this](const auto &sender, const winrt::LosingFocusEventArgs &args) { m_focusSafeHarbor.IsTabStop(false); });
  }
}

// Set keyboard event listener for developer menu
void ReactRootControl::InitializeDeveloperMenu() noexcept {
  VerifyElseCrashSz(m_uiQueue.HasThreadAccess(), "Must be called on UI thread");

  auto coreWindow = winrt::CoreWindow::GetForCurrentThread();
  m_coreDispatcherAKARevoker = coreWindow.Dispatcher().AcceleratorKeyActivated(
      winrt::auto_revoke, [this](const auto &sender, const winrt::AcceleratorKeyEventArgs &args) {
        if ((args.VirtualKey() == winrt::Windows::System::VirtualKey::D) &&
            KeyboardHelper::IsModifiedKeyPressed(winrt::CoreWindow::GetForCurrentThread(), winrt::VirtualKey::Shift) &&
            KeyboardHelper::IsModifiedKeyPressed(
                winrt::CoreWindow::GetForCurrentThread(), winrt::VirtualKey::Control)) {
          if (!IsDeveloperMenuShowing()) {
            ShowDeveloperMenu();
          }
        }
      });
}

void ReactRootControl::ShowDeveloperMenu() noexcept {
  VerifyElseCrashSz(m_uiQueue.HasThreadAccess(), "Must be called on UI thread");
  VerifyElseCrash(m_developerMenuRoot == nullptr);

  if (auto xamlRootView = m_weakXamlRootView.get()) {
    const winrt::hstring xamlString =
        L"<Grid Background='{ThemeResource ContentDialogDimmingThemeBrush}'"
        L"  xmlns='http://schemas.microsoft.com/winfx/2006/xaml/presentation'"
        L"  xmlns:x='http://schemas.microsoft.com/winfx/2006/xaml'>"
        L"  <Grid.Transitions>"
        L"    <TransitionCollection>"
        L"      <EntranceThemeTransition />"
        L"    </TransitionCollection>"
        L"  </Grid.Transitions>"
        L"  <StackPanel HorizontalAlignment='Center'"
        L"    VerticalAlignment='Center'"
        L"    Background='{ThemeResource ContentDialogBackground}'"
        L"    BorderBrush='{ThemeResource ContentDialogBorderBrush}'"
        L"    BorderThickness='{ThemeResource ContentDialogBorderWidth}'"
        L"    Padding='{ThemeResource ContentDialogPadding}'"
        L"    Spacing='4' >"
        L"    <TextBlock Margin='0,0,0,10' FontSize='40'>Developer Menu</TextBlock>"
        L"    <Button HorizontalAlignment='Stretch' x:Name='Reload'>Reload Javascript</Button>"
        L"    <Button HorizontalAlignment='Stretch' x:Name='RemoteDebug'></Button>"
        L"    <Button HorizontalAlignment='Stretch' x:Name='LiveReload'></Button>"
        L"    <Button HorizontalAlignment='Stretch' x:Name='Inspector'>Toggle Inspector</Button>"
        L"    <Button HorizontalAlignment='Stretch' x:Name='Cancel'>Cancel</Button>"
        L"  </StackPanel>"
        L"</Grid>";
    m_developerMenuRoot = winrt::unbox_value<winrt::Grid>(winrt::Markup::XamlReader::Load(xamlString));
    auto reloadJSButton = m_developerMenuRoot.FindName(L"Reload").as<winrt::Button>();
    auto remoteDebugJSButton = m_developerMenuRoot.FindName(L"RemoteDebug").as<winrt::Button>();
    auto liveReloadButton = m_developerMenuRoot.FindName(L"LiveReload").as<winrt::Button>();
    auto toggleInspector = m_developerMenuRoot.FindName(L"Inspector").as<winrt::Button>();
    auto cancelButton = m_developerMenuRoot.FindName(L"Cancel").as<winrt::Button>();

    m_reloadJSRevoker = reloadJSButton.Click(
        winrt::auto_revoke, [this](auto const &sender, winrt::RoutedEventArgs const &args) noexcept {
          DismissDeveloperMenu();
          ReloadHost();
        });

    remoteDebugJSButton.Content(
        winrt::box_value(m_useWebDebugger ? L"Disable Remote JS Debugging" : L"Enable Remote JS Debugging"));
    m_remoteDebugJSRevoker = remoteDebugJSButton.Click(
        winrt::auto_revoke, [this](auto const &sender, winrt::RoutedEventArgs const &args) noexcept {
          DismissDeveloperMenu();
          m_useWebDebugger = !m_useWebDebugger;
          ReloadHost();
        });

    liveReloadButton.Content(winrt::box_value(m_useLiveReload ? L"Disable Live Reload" : L"Enable Live Reload"));
    m_liveReloadRevoker =
        liveReloadButton.Click(winrt::auto_revoke, [this](auto &sender, winrt::RoutedEventArgs const &args) noexcept {
          DismissDeveloperMenu();
          m_useLiveReload = !m_useLiveReload;
          ReloadHost();
        });

    m_toggleInspectorRevoker = toggleInspector.Click(
        winrt::auto_revoke, [this](auto const &sender, winrt::RoutedEventArgs const &args) noexcept {
          DismissDeveloperMenu();
          ToggleInspector();
        });

    m_cancelRevoker = cancelButton.Click(
        winrt::auto_revoke, [this](auto const &sender, winrt::RoutedEventArgs const &args) { DismissDeveloperMenu(); });

    auto xamlRootGrid{xamlRootView.as<winrt::Grid>()};
    xamlRootGrid.Children().Append(m_developerMenuRoot);
  }
}

void ReactRootControl::DismissDeveloperMenu() noexcept {
  VerifyElseCrashSz(m_uiQueue.HasThreadAccess(), "Must be called on UI thread");
  VerifyElseCrash(m_developerMenuRoot != nullptr);

  if (auto xamlRootView = m_weakXamlRootView.get()) {
    auto xamlRootGrid{xamlRootView.as<winrt::Grid>()};
    uint32_t indexToRemove = 0;
    if (xamlRootGrid.Children().IndexOf(m_developerMenuRoot, indexToRemove)) {
      xamlRootGrid.Children().RemoveAt(indexToRemove);
    }

    m_developerMenuRoot = nullptr;
  }
}

bool ReactRootControl::IsDeveloperMenuShowing() const noexcept {
  return (m_developerMenuRoot != nullptr);
}

void ReactRootControl::ToggleInspector() noexcept {
  if (auto reactInstance = m_weakReactInstance.GetStrongPtr()) {
    query_cast<Mso::React::ILegacyReactInstance &>(*reactInstance)
        .CallJsFunction("RCTDeviceEventEmitter", "emit", folly::dynamic::array("toggleElementInspector", nullptr));
  }
}

void ReactRootControl::ReloadHost() noexcept {
  if (auto reactViewHost = m_reactViewHost.Get()) {
    auto options = reactViewHost->ReactHost().Options();
    options.DeveloperSettings.UseLiveReload = m_useLiveReload;
    options.DeveloperSettings.UseWebDebugger = m_useWebDebugger;
    reactViewHost->ReactHost().ReloadInstanceWithOptions(std::move(options));
  }
}

void ReactRootControl::ReloadViewHost() noexcept {
  if (auto reactViewHost = m_reactViewHost.Get()) {
    reactViewHost->ReloadViewInstance();
  }
}

Mso::React::IReactViewHost *ReactRootControl::ReactViewHost() noexcept {
  return m_reactViewHost.Get();
}

void ReactRootControl::ReactViewHost(Mso::React::IReactViewHost *viewHost) noexcept {
  if (m_reactViewHost.Get() == viewHost) {
    return;
  }

  if (m_reactViewHost) {
    UninitRootView();
    m_reactViewHost->DetachViewInstance();
  }

  m_reactViewHost = viewHost;

  if (m_reactViewHost) {
    auto viewInstance = Mso::Make<ReactViewInstance>(weak_from_this(), m_uiQueue);
    m_reactViewHost->AttachViewInstance(*viewInstance);
  }
}

//===========================================================================
// ReactViewInstance implementation
//===========================================================================

ReactViewInstance::ReactViewInstance(
    std::weak_ptr<ReactRootControl> &&weakRootControl,
    Mso::DispatchQueue const &uiQueue) noexcept
    : m_weakRootControl{std::move(weakRootControl)}, m_uiQueue{uiQueue} {}

Mso::Future<void> ReactViewInstance::InitRootView(
    Mso::CntPtr<Mso::React::IReactInstance> &&reactInstance,
    Mso::React::ReactViewOptions &&viewOptions) noexcept {
  return PostInUIQueue([ reactInstance{std::move(reactInstance)}, viewOptions{std::move(viewOptions)} ](
      ReactRootControl & rootControl) mutable noexcept {
    rootControl.InitRootView(std::move(reactInstance), std::move(viewOptions));
  });
}

Mso::Future<void> ReactViewInstance::UpdateRootView() noexcept {
  return PostInUIQueue([](ReactRootControl & rootControl) mutable noexcept { rootControl.UpdateRootView(); });
}

Mso::Future<void> ReactViewInstance::UninitRootView() noexcept {
  return PostInUIQueue([](ReactRootControl & rootControl) mutable noexcept { rootControl.UninitRootView(); });
}

} // namespace react::uwp
