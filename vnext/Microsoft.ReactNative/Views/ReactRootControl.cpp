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
#include <Utils/Helpers.h>

#include <winrt/Windows.UI.Xaml.Controls.Primitives.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Input.h>
#include <winrt/Windows.UI.Xaml.Markup.h>
#include <winrt/Windows.UI.Xaml.Media.Media3D.h>

namespace react::uwp {

//===========================================================================
// ReactRootControl implementation
//===========================================================================

ReactRootControl::ReactRootControl(XamlView const &rootView) noexcept
    : m_weakRootView{rootView}, m_uiQueue(Mso::DispatchQueue::GetCurrentUIThreadQueue()) {
  VerifyElseCrashSz(m_uiQueue, "Cannot get UI dispatch queue for the current thread");
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
  m_useFastRefresh = devSettings.UseFastRefresh;
  m_useWebDebugger = devSettings.UseWebDebugger;
  m_isDevModeEnabled = devSettings.IsDevModeEnabled;
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
  AttachBackHandlers(xamlRootView);

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
        ShowInstanceError();
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

  RemoveBackHandlers();

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

void ReactRootControl::ShowInstanceError() noexcept {
  if (XamlView xamlRootView = m_weakXamlRootView.get()) {
    auto xamlRootGrid{xamlRootView.as<winrt::Grid>()};

    // Remove existing children from root view (from the hosted app)
    xamlRootGrid.Children().Clear();
  }
}

void ReactRootControl::ShowInstanceWaiting(Mso::React::IReactInstance & /*reactInstance*/) noexcept {
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

void ReactRootControl::ShowInstanceLoading(Mso::React::IReactInstance & /*reactInstance*/) noexcept {
  if (!m_isDevModeEnabled)
    return;

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
        winrt::auto_revoke, [this](const auto & /*sender*/, const winrt::LosingFocusEventArgs & /*args*/) {
          m_focusSafeHarbor.IsTabStop(false);
        });
  }
}

// Set keyboard event listener for developer menu
void ReactRootControl::InitializeDeveloperMenu() noexcept {
  VerifyElseCrashSz(m_uiQueue.HasThreadAccess(), "Must be called on UI thread");

  auto coreWindow = winrt::CoreWindow::GetForCurrentThread();
  m_coreDispatcherAKARevoker = coreWindow.Dispatcher().AcceleratorKeyActivated(
      winrt::auto_revoke, [this](const auto & /*sender*/, const winrt::AcceleratorKeyEventArgs &args) {
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
        L"  <Grid.Resources>"
        L"    <StaticResource x:Key='ButtonRevealBackground' ResourceKey='AppBarButtonRevealBackground' />"
        L"    <StaticResource x:Key='ButtonRevealBackgroundPointerOver' ResourceKey='AppBarButtonRevealBackgroundPointerOver' />"
        L"    <StaticResource x:Key='ButtonRevealBackgroundPressed' ResourceKey='AppBarButtonRevealBackgroundPressed' />"
        L"    <StaticResource x:Key='ButtonRevealBackgroundDisabled' ResourceKey='AppBarButtonRevealBackgroundDisabled' />"
        L"    <StaticResource x:Key='ButtonForeground' ResourceKey='AppBarButtonForeground' />"
        L"    <StaticResource x:Key='ButtonForegroundPointerOver' ResourceKey='AppBarButtonForegroundPointerOver' />"
        L"    <StaticResource x:Key='ButtonForegroundPressed' ResourceKey='AppBarButtonForegroundPressed' />"
        L"    <StaticResource x:Key='ButtonForegroundDisabled' ResourceKey='AppBarButtonForegroundDisabled' />"
        L"    <StaticResource x:Key='ButtonRevealBorderBrush' ResourceKey='AppBarButtonRevealBorderBrush' />"
        L"    <StaticResource x:Key='ButtonRevealBorderBrushPointerOver' ResourceKey='AppBarButtonRevealBorderBrushPointerOver' />"
        L"    <StaticResource x:Key='ButtonRevealBorderBrushPressed' ResourceKey='AppBarButtonRevealBorderBrushPressed' />"
        L"    <StaticResource x:Key='ButtonRevealBorderBrushDisabled' ResourceKey='AppBarButtonRevealBorderBrushDisabled' />"
        L"  </Grid.Resources>"
        L"  <Grid.Transitions>"
        L"    <TransitionCollection>"
        L"      <EntranceThemeTransition />"
        L"    </TransitionCollection>"
        L"  </Grid.Transitions>"
        L"  <ScrollViewer"
        L"    VerticalAlignment='Center'"
        L"    MaxWidth='900'"
        L"    >"
        L"  <StackPanel HorizontalAlignment='Center'"
        L"    Background='{ThemeResource ContentDialogBackground}'"
        L"    BorderBrush='{ThemeResource ContentDialogBorderBrush}'"
        L"    BorderThickness='{ThemeResource ContentDialogBorderWidth}'"
        L"    Padding='8'"
        L"    Spacing='4' >"
        L"    <Button HorizontalAlignment='Stretch' HorizontalContentAlignment='Stretch' x:Name='Reload' Style='{StaticResource ButtonRevealStyle}'>"
        L"      <Grid HorizontalAlignment='Stretch'><Grid.ColumnDefinitions><ColumnDefinition Width='Auto'/><ColumnDefinition Width='*'/></Grid.ColumnDefinitions><Grid.RowDefinitions><RowDefinition/><RowDefinition/></Grid.RowDefinitions>"
        L"        <FontIcon Grid.Column='0' Grid.Row='0' Grid.RowSpan='2' VerticalAlignment='Top' FontFamily='{StaticResource SymbolThemeFontFamily}' Foreground='{StaticResource SystemControlForegroundAccentBrush}' Margin='8,8,16,8' Glyph='&#xE72C;'/>"
        L"        <TextBlock Grid.Column='1' Grid.Row='0'>Reload Javascript</TextBlock>"
        L"        <TextBlock Grid.Column='1' Grid.Row='1' FontSize='12' Opacity='0.5' TextWrapping='Wrap'>Restarts the JS instance. Any javascript state will be lost.</TextBlock>"
        L"      </Grid>"
        L"    </Button>"
        L"    <Button HorizontalAlignment='Stretch' HorizontalContentAlignment='Stretch' x:Name='RemoteDebug' Style='{StaticResource ButtonRevealStyle}'>"
        L"      <Grid HorizontalAlignment='Stretch'><Grid.ColumnDefinitions><ColumnDefinition Width='Auto'/><ColumnDefinition Width='*'/></Grid.ColumnDefinitions><Grid.RowDefinitions><RowDefinition/><RowDefinition/></Grid.RowDefinitions>"
        L"        <FontIcon Grid.Column='0' Grid.Row='0' Grid.RowSpan='2' VerticalAlignment='Top' FontFamily='{StaticResource SymbolThemeFontFamily}' Foreground='{StaticResource SystemControlForegroundAccentBrush}' Margin='8,8,16,8' Glyph='&#xE8AF;'/>"
        L"        <TextBlock Grid.Column='1' Grid.Row='0' x:Name='RemoteDebugText'/>"
        L"        <TextBlock Grid.Column='1' Grid.Row='1' FontSize='12' Opacity='0.5' TextWrapping='Wrap'>When enabled runs the JS remotely in VSCode or Chrome based on what you attach to the packager.  This means that the JS may run with a different JS engine than it runs in on in the real application, in addition synchronous native module calls, and JSI native modules will not work.</TextBlock>"
        L"      </Grid>"
        L"    </Button>"
        L"    <Button HorizontalAlignment='Stretch' HorizontalContentAlignment='Stretch' x:Name='DirectDebug' Style='{StaticResource ButtonRevealStyle}'>"
        L"      <Grid HorizontalAlignment='Stretch'><Grid.ColumnDefinitions><ColumnDefinition Width='Auto'/><ColumnDefinition Width='*'/></Grid.ColumnDefinitions><Grid.RowDefinitions><RowDefinition/><RowDefinition/></Grid.RowDefinitions>"
        L"        <FontIcon Grid.Column='0' Grid.Row='0' Grid.RowSpan='2' VerticalAlignment='Top' FontFamily='{StaticResource SymbolThemeFontFamily}' Foreground='{StaticResource SystemControlForegroundAccentBrush}' Margin='8,8,16,8' Glyph='&#xEBE8;'/>"
        L"        <TextBlock Grid.Column='1' Grid.Row='0' x:Name='DirectDebugText'/>"
        L"        <TextBlock Grid.Column='1' Grid.Row='1' FontSize='12' Opacity='0.5' TextWrapping='Wrap'>If using Chakra, this will allow Visual Studio to be attached directly to the application using \"Script Debugging\" to debug the JS running directly in this app.\nIf using V8/Hermes, this will enable standard JS debugging tools such as VSCode to attach to the application.</TextBlock>"
        L"      </Grid>"
        L"    </Button>"
        L"    <Button HorizontalAlignment='Stretch' HorizontalContentAlignment='Stretch' x:Name='BreakOnNextLine' Style='{StaticResource ButtonRevealStyle}'>"
        L"      <Grid HorizontalAlignment='Stretch'><Grid.ColumnDefinitions><ColumnDefinition Width='Auto'/><ColumnDefinition Width='*'/></Grid.ColumnDefinitions><Grid.RowDefinitions><RowDefinition/><RowDefinition/></Grid.RowDefinitions>"
        L"        <FontIcon Grid.Column='0' Grid.Row='0' Grid.RowSpan='2' VerticalAlignment='Top' FontFamily='{StaticResource SymbolThemeFontFamily}' Foreground='{StaticResource SystemControlForegroundAccentBrush}' Margin='8,8,16,8' Glyph='&#xE769;'/>"
        L"        <TextBlock Grid.Column='1' Grid.Row='0' x:Name='BreakOnNextLineText'/>"
        L"        <TextBlock Grid.Column='1' Grid.Row='1' FontSize='12' Opacity='0.5' TextWrapping='Wrap'>If using V8/Hermes, the JS engine will break on the first statement, until you attach a debugger to it, and hit continue. (Requires Direct Debugging to be enabled)</TextBlock>"
        L"      </Grid>"
        L"    </Button>"
        L"    <Button HorizontalAlignment='Stretch' HorizontalContentAlignment='Stretch' x:Name='FastRefresh' Style='{StaticResource ButtonRevealStyle}'>"
        L"      <Grid HorizontalAlignment='Stretch'><Grid.ColumnDefinitions><ColumnDefinition Width='Auto'/><ColumnDefinition Width='*'/></Grid.ColumnDefinitions><Grid.RowDefinitions><RowDefinition/><RowDefinition/></Grid.RowDefinitions>"
        L"        <FontIcon Grid.Column='0' Grid.Row='0' Grid.RowSpan='2' VerticalAlignment='Top' FontFamily='{StaticResource SymbolThemeFontFamily}' Foreground='{StaticResource SystemControlForegroundAccentBrush}' Margin='8,8,16,8' Glyph='&#xEC58;'/>"
        L"        <TextBlock Grid.Column='1' Grid.Row='0' x:Name='FastRefreshText'/>"
        L"        <TextBlock Grid.Column='1' Grid.Row='1' FontSize='12' Opacity='0.5' TextWrapping='Wrap'>When loading a bundle from a bundler server that is watching files, this will cause the instance to be reloaded with new bundles when a file changes.</TextBlock>"
        L"      </Grid>"
        L"    </Button>"
        L"    <Button HorizontalAlignment='Stretch' HorizontalContentAlignment='Stretch' x:Name='Inspector' Style='{StaticResource ButtonRevealStyle}'>"
        L"      <Grid HorizontalAlignment='Stretch'><Grid.ColumnDefinitions><ColumnDefinition Width='Auto'/><ColumnDefinition Width='*'/></Grid.ColumnDefinitions><Grid.RowDefinitions><RowDefinition/><RowDefinition/></Grid.RowDefinitions>"
        L"        <FontIcon Grid.Column='0' Grid.Row='0' Grid.RowSpan='2' VerticalAlignment='Top' FontFamily='{StaticResource SymbolThemeFontFamily}' Foreground='{StaticResource SystemControlForegroundAccentBrush}' Margin='8,8,16,8' Glyph='&#xE773;'/>"
        L"        <TextBlock Grid.Column='1' Grid.Row='0' x:Name='InspectorText'>Toggle Inspector</TextBlock>"
        L"        <TextBlock Grid.Column='1' Grid.Row='1' FontSize='12' Opacity='0.5' TextWrapping='Wrap'>Will bring up an overlay that lets you tap on any UI element and see information about it</TextBlock>"
        L"      </Grid>"
        L"    </Button>"
        L"    <Button HorizontalAlignment='Stretch' x:Name='Cancel' Style='{StaticResource ButtonRevealStyle}'>Cancel</Button>"
        L"  </StackPanel>"
        L"  </ScrollViewer>"
        L"</Grid>";
    m_developerMenuRoot = winrt::unbox_value<winrt::Grid>(winrt::Markup::XamlReader::Load(xamlString));
    auto remoteDebugJSText = m_developerMenuRoot.FindName(L"RemoteDebugText").as<winrt::TextBlock>();
    auto remoteDebugJSButton = m_developerMenuRoot.FindName(L"RemoteDebug").as<winrt::Button>();
    auto reloadJSButton = m_developerMenuRoot.FindName(L"Reload").as<winrt::Button>();
    auto directDebugText = m_developerMenuRoot.FindName(L"DirectDebugText").as<winrt::TextBlock>();
    auto directDebugButton = m_developerMenuRoot.FindName(L"DirectDebug").as<winrt::Button>();
    auto breakOnNextLineText = m_developerMenuRoot.FindName(L"BreakOnNextLineText").as<winrt::TextBlock>();
    auto breakOnNextLineButton = m_developerMenuRoot.FindName(L"BreakOnNextLine").as<winrt::Button>();
    auto fastRefreshText = m_developerMenuRoot.FindName(L"FastRefreshText").as<winrt::TextBlock>();
    auto fastRefreshButton = m_developerMenuRoot.FindName(L"FastRefresh").as<winrt::Button>();
    auto toggleInspector = m_developerMenuRoot.FindName(L"Inspector").as<winrt::Button>();
    auto cancelButton = m_developerMenuRoot.FindName(L"Cancel").as<winrt::Button>();

    m_reloadJSRevoker = reloadJSButton.Click(
        winrt::auto_revoke, [this](auto const & /*sender*/, winrt::RoutedEventArgs const & /*args*/) noexcept {
          DismissDeveloperMenu();
          ReloadHost();
        });

    remoteDebugJSText.Text(m_useWebDebugger ? L"Disable Remote JS Debugging" : L"Enable Remote JS Debugging");
    m_remoteDebugJSRevoker = remoteDebugJSButton.Click(
        winrt::auto_revoke, [this](auto const & /*sender*/, winrt::RoutedEventArgs const & /*args*/) noexcept {
          DismissDeveloperMenu();
          m_useWebDebugger = !m_useWebDebugger;
          m_directDebugging = false; // Remote debugging is incompatible with direct debugging
          ReloadHost();
        });

    directDebugText.Text(m_directDebugging ? L"Disable Direct Debugging" : L"Enable Direct Debugging");
    m_directDebuggingRevoker = directDebugButton.Click(
        winrt::auto_revoke, [this](auto const & /*sender*/, winrt::RoutedEventArgs const & /*args*/) noexcept {
          DismissDeveloperMenu();
          m_directDebugging = !m_directDebugging;
          m_useWebDebugger = false; // Remote debugging is incompatible with direct debugging
          ReloadHost();
        });

    breakOnNextLineText.Text(m_breakOnNextLine ? L"Disable Break on First Line" : L"Enable Break on First Line");
    m_breakOnNextLineRevoker = breakOnNextLineButton.Click(
        winrt::auto_revoke, [this](auto const & /*sender*/, winrt::RoutedEventArgs const & /*args*/) noexcept {
          DismissDeveloperMenu();
          m_breakOnNextLine = !m_breakOnNextLine;
          ReloadHost();
        });

    fastRefreshText.Text(m_useFastRefresh ? L"Disable Fast Refresh" : L"Enable Fast Refresh");
    m_fastRefreshRevoker = fastRefreshButton.Click(
        winrt::auto_revoke, [this](auto & /*sender*/, winrt::RoutedEventArgs const & /*args*/) noexcept {
          DismissDeveloperMenu();
          m_useFastRefresh = !m_useFastRefresh;
          ReloadHost();
        });

    m_toggleInspectorRevoker = toggleInspector.Click(
        winrt::auto_revoke, [this](auto const & /*sender*/, winrt::RoutedEventArgs const & /*args*/) noexcept {
          DismissDeveloperMenu();
          ToggleInspector();
        });

    m_cancelRevoker = cancelButton.Click(
        winrt::auto_revoke,
        [this](auto const & /*sender*/, winrt::RoutedEventArgs const & /*args*/) { DismissDeveloperMenu(); });

    auto xamlRootGrid{xamlRootView.as<winrt::Grid>()};
    xamlRootGrid.Children().Append(m_developerMenuRoot);
  }

  // Notify instance that dev menu is shown -- This is used to trigger a connection to dev tools
  if (auto instance = m_weakReactInstance.GetStrongPtr()) {
    query_cast<Mso::React::ILegacyReactInstance &>(*instance).CallJsFunction(
        "RCTNativeAppEventEmitter", "emit", folly::dynamic::array("RCTDevMenuShown"));
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
    options.DeveloperSettings.IsDevModeEnabled = m_isDevModeEnabled;
    options.DeveloperSettings.UseFastRefresh = m_useFastRefresh;
    options.DeveloperSettings.UseWebDebugger = m_useWebDebugger;
    options.DeveloperSettings.UseDirectDebugger = m_directDebugging;
    options.DeveloperSettings.DebuggerBreakOnNextLine = m_breakOnNextLine;
    reactViewHost->ReactHost().ReloadInstanceWithOptions(std::move(options));
  }
}

void ReactRootControl::ReloadViewHost() noexcept {
  if (auto reactViewHost = m_reactViewHost.Get()) {
    reactViewHost->ReloadViewInstance();
  }
}

void ReactRootControl::AttachBackHandlers(XamlView const &rootView) noexcept {
  /*
   * If we are running in a Xaml Island or some other environment where the SystemNavigationManager is unavailable,
   * we should just skip hooking up the BackButton handler. SystemNavigationManager->GetForCurrentView seems to
   * crash with XamlIslands so we can't just bail if that call fails.
   */
  if (react::uwp::IsXamlIsland())
    return;

  auto weakThis = weak_from_this();
  m_backRequestedRevoker = winrt::Windows::UI::Core::SystemNavigationManager::GetForCurrentView().BackRequested(
      winrt::auto_revoke,
      [weakThis](winrt::IInspectable const & /*sender*/, winrt::BackRequestedEventArgs const &args) {
        if (auto self = weakThis.lock()) {
          args.Handled(self->OnBackRequested());
        }
      });

  // In addition to handling the BackRequested event, UWP suggests that we listen for other user inputs that should
  // trigger back navigation that don't fire that event:
  // https://docs.microsoft.com/en-us/windows/uwp/design/basics/navigation-history-and-backwards-navigation
  auto rootElement(rootView.try_as<winrt::UIElement>());
  if (rootElement == nullptr) {
    assert(false);
    return;
  }

  // Handle keyboard "back" button press
  winrt::KeyboardAccelerator goBack{};
  goBack.Key(winrt::VirtualKey::GoBack);
  goBack.Invoked(
      [weakThis](
          winrt::KeyboardAccelerator const & /*sender*/, winrt::KeyboardAcceleratorInvokedEventArgs const &args) {
        if (auto self = weakThis.lock()) {
          args.Handled(self->OnBackRequested());
        }
      });
  rootElement.KeyboardAccelerators().Append(goBack);

  // Handle Alt+Left keyboard shortcut
  winrt::KeyboardAccelerator altLeft{};
  altLeft.Key(winrt::VirtualKey::Left);
  altLeft.Invoked(
      [weakThis](
          winrt::KeyboardAccelerator const & /*sender*/, winrt::KeyboardAcceleratorInvokedEventArgs const &args) {
        if (auto self = weakThis.lock()) {
          args.Handled(self->OnBackRequested());
        }
      });
  rootElement.KeyboardAccelerators().Append(altLeft);
  altLeft.Modifiers(winrt::VirtualKeyModifiers::Menu);

  // Hide keyboard accelerator tooltips
  rootElement.KeyboardAcceleratorPlacementMode(winrt::KeyboardAcceleratorPlacementMode::Hidden);
}

void ReactRootControl::RemoveBackHandlers() noexcept {
  m_backRequestedRevoker.revoke();
  if (auto rootView = m_weakRootView.get()) {
    if (auto element = rootView.try_as<winrt::UIElement>()) {
      element.KeyboardAccelerators().Clear();
    }
  }
}

bool ReactRootControl::OnBackRequested() noexcept {
  if (auto reactInstance = m_weakReactInstance.GetStrongPtr()) {
    query_cast<Mso::React::ILegacyReactInstance &>(*reactInstance)
        .CallJsFunction("RCTDeviceEventEmitter", "emit", folly::dynamic::array("hardwareBackPress"));
    return true;
  }

  return false;
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
