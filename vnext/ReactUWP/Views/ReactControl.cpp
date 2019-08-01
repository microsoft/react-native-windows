// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include <pch.h>

#include "ReactControl.h"

#include <CxxMessageQueue.h>
#include <ReactUWP/InstanceFactory.h>
#include <Utils/ValueUtils.h>
#include "unicode.h"

#include <INativeUIManager.h>
#include <Views/KeyboardEventHandler.h>
#include <Views/ShadowNodeBase.h>

#include <winrt/Windows.ApplicationModel.Core.h>
#include <winrt/Windows.Devices.Input.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.UI.Core.h>
#include <winrt/Windows.UI.Input.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Input.h>
#include <winrt/Windows.UI.Xaml.Markup.h>
#include <winrt/Windows.UI.Xaml.Media.h>
#include <winrt/Windows.UI.Xaml.h>

namespace react {
namespace uwp {

ReactControl::ReactControl(IXamlRootView *parent, XamlView rootView)
    : m_pParent(parent), m_rootView(rootView) {
  PrepareXamlRootView(rootView);
}

ReactControl::~ReactControl() {
  if (m_reactInstance != nullptr) {
    m_reactInstance->UnregisterLiveReloadCallback(m_liveReloadCallbackCookie);
    m_reactInstance->UnregisterErrorCallback(m_errorCallbackCookie);
  }

  // remove safe harbor and child grid from visual tree
  if (m_focusSafeHarbor) {
    if (auto root = m_focusSafeHarbor.Parent().try_as<winrt::Panel>()) {
      root.Children().Clear();
    }
  }
}

std::shared_ptr<IReactInstance> ReactControl::GetReactInstance() const
    noexcept {
  return m_reactInstance;
}

void ReactControl::HandleInstanceError() {
  auto weakThis = weak_from_this();
  m_reactInstance->DefaultNativeMessageQueueThread()->runOnQueue([weakThis]() {
    if (auto This = weakThis.lock()) {
      This->HandleInstanceErrorOnUIThread();
    }
  });
}

void ReactControl::HandleInstanceErrorOnUIThread() {
  if (m_reactInstance->IsInError()) {
    auto xamlRootGrid(m_xamlRootView.as<winrt::Grid>());

    // Remove existing children from root view (from the hosted app)
    xamlRootGrid.Children().Clear();

    // Create Grid & TextBlock to hold error text
    if (m_errorTextBlock == nullptr) {
      m_errorTextBlock = winrt::TextBlock();
      m_redBoxGrid = winrt::Grid();
      m_redBoxGrid.Background(winrt::SolidColorBrush(winrt::Colors::Crimson()));
      m_redBoxGrid.Children().Append(m_errorTextBlock);
    }

    // Add red box grid to root view
    xamlRootGrid.Children().Append(m_redBoxGrid);

    // Place error message into TextBlock
    std::wstring wstrErrorMessage(L"ERROR: Instance failed to start.\n\n");
    wstrErrorMessage += facebook::react::unicode::utf8ToUtf16(
                            m_reactInstance->LastErrorMessage())
                            .c_str();
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

void ReactControl::AttachRoot() noexcept {
  if (m_isAttached)
    return;

  if (!m_reactInstance)
    m_reactInstance = m_instanceCreator->getInstance();

  // Handle any errors that occurred during creation before we add our callback
  if (m_reactInstance->IsInError())
    HandleInstanceError();

  if (!m_touchEventHandler)
    m_touchEventHandler = std::make_shared<TouchEventHandler>(m_reactInstance);

  m_previewKeyboardEventHandlerOnRoot =
      std::make_shared<PreviewKeyboardEventHandlerOnRoot>(m_reactInstance);

  // Register callback from instance for live reload
  m_errorCallbackCookie = m_reactInstance->RegisterErrorCallback(
      [this]() { HandleInstanceError(); });

  // Register callback from instance for live reload
  m_liveReloadCallbackCookie =
      m_reactInstance->RegisterLiveReloadCallback([this]() {
        auto weakThis = weak_from_this();
        m_reactInstance->DefaultNativeMessageQueueThread()->runOnQueue(
            [weakThis]() {
              if (auto This = weakThis.lock()) {
                This->Reload(true);
              }
            });
      });

  // We assume Attach has been called from the UI thread
#ifdef DEBUG
  auto coreWindow = winrt::CoreWindow::GetForCurrentThread();
  assert(coreWindow != nullptr);
#endif

  m_touchEventHandler->AddTouchHandlers(m_xamlRootView);
  m_previewKeyboardEventHandlerOnRoot->hook(m_xamlRootView);

  auto initialProps = m_initialProps;
  m_reactInstance->AttachMeasuredRootView(m_pParent, std::move(initialProps));
  m_isAttached = true;

#ifdef DEBUG
  InitializeDeveloperMenu();
#endif
}

void ReactControl::DetachRoot() noexcept {
  if (!m_isAttached)
    return;

  if (m_touchEventHandler != nullptr) {
    m_touchEventHandler->RemoveTouchHandlers();
  }

  if (!m_previewKeyboardEventHandlerOnRoot)
    m_previewKeyboardEventHandlerOnRoot->unhook();

  if (m_reactInstance != nullptr) {
    m_reactInstance->DetachRootView(m_pParent);

    // If the redbox error UI is shown we need to remove it, otherwise let the
    // natural teardown process do this
    if (m_reactInstance->IsInError()) {
      auto grid(m_xamlRootView.as<winrt::Grid>());
      if (grid != nullptr)
        grid.Children().Clear();

      m_redBoxGrid = nullptr;
      m_errorTextBlock = nullptr;
    }
  }

  m_isAttached = false;
}

// Xaml doesn't provide Blur.
// If 'focus safe harbor' exists, make harbor to allow tabstop and focus on
// harbor with ::Pointer otherwise, just changing the FocusState to ::Pointer
// for the element.
void ReactControl::blur(XamlView const &xamlView) noexcept {
  EnsureFocusSafeHarbor();
  if (m_focusSafeHarbor) {
    m_focusSafeHarbor.IsTabStop(true);
    winrt::FocusManager::TryFocusAsync(
        m_focusSafeHarbor, winrt::FocusState::Pointer);
  } else
    winrt::FocusManager::TryFocusAsync(xamlView, winrt::FocusState::Pointer);
}

void ReactControl::DetachInstance() {
  if (m_reactInstance != nullptr) {
    std::shared_ptr<IReactInstance> instance = m_reactInstance;

    m_reactInstance->UnregisterLiveReloadCallback(m_liveReloadCallbackCookie);
    m_reactInstance->UnregisterErrorCallback(m_errorCallbackCookie);
    m_reactInstance.reset();

    // Keep instance alive by capturing and queuing an empty func.
    // This extends the lifetime of NativeModules which may have
    // pending calls in these queues.
    // TODO prevent or check if even more is queued while these drain.
    CreateWorkerMessageQueue()->runOnQueue([instance]() {});
    instance->DefaultNativeMessageQueueThread()->runOnQueue([instance]() {});

    // Clear members with a dependency on the reactInstance
    m_touchEventHandler.reset();
  }
}

void ReactControl::Reload(bool shouldRetireCurrentInstance) {
  // DetachRoot the current view and detach it
  DetachRoot();

  m_reactInstance->DefaultNativeMessageQueueThread()->runOnQueue(
      [this, shouldRetireCurrentInstance]() {
        if (shouldRetireCurrentInstance && m_reactInstance != nullptr)
          m_instanceCreator->markAsNeedsReload();

        // Detach ourselves from the instance
        DetachInstance();

        // Restart the view which will re-create the instance and attach the
        // root view
        AttachRoot();
      });
}

XamlView ReactControl::GetXamlView() const noexcept {
  return m_xamlRootView;
}

void ReactControl::SetJSComponentName(std::string &&jsComponentName) noexcept {
  m_jsComponentName = std::move(jsComponentName);

  if (m_reactInstance != nullptr)
    m_reactInstance->SetAsNeedsReload();
}

void ReactControl::SetInstanceCreator(
    const ReactInstanceCreator &instanceCreator) noexcept {
  // TODO - Handle swapping this out after the control is running
  m_instanceCreator = instanceCreator;
}

void ReactControl::SetInitialProps(folly::dynamic &&initialProps) noexcept {
  m_initialProps = initialProps;
}

std::string ReactControl::JSComponentName() const noexcept {
  return m_jsComponentName;
}

int64_t ReactControl::GetActualHeight() const {
  auto element = m_xamlRootView.as<winrt::FrameworkElement>();
  assert(element != nullptr);

  return static_cast<int64_t>(element.ActualHeight());
}

int64_t ReactControl::GetActualWidth() const {
  auto element = m_xamlRootView.as<winrt::FrameworkElement>();
  assert(element != nullptr);

  return static_cast<int64_t>(element.ActualWidth());
}

void ReactControl::PrepareXamlRootView(XamlView const &rootView) {
  if (auto panel = rootView.try_as<winrt::Panel>()) {
    // Xaml don't have blur concept.
    // A ContentControl is created in the middle to act as a 'focus safe harbor'
    // When a XamlView is blurred, make the ContentControl to allow tabstop, and
    // move the pointer focus to safe harbor When the safe harbor is
    // LosingFocus, disable tabstop on ContentControl. The creation of safe
    // harbor is delayed to EnsureFocusSafeHarbor
    auto children = panel.Children();
    children.Clear();

    auto newRootView = winrt::Grid();
    children.Append(newRootView);
    m_xamlRootView = newRootView;
  } else
    m_xamlRootView = rootView;
}

void ReactControl::EnsureFocusSafeHarbor() {
  if (!m_focusSafeHarbor && m_xamlRootView != m_rootView) {
    // focus safe harbor is delayed to be inserted to the visual tree
    auto panel = m_rootView.try_as<winrt::Panel>();
    assert(panel.Children().Size() == 1);

    m_focusSafeHarbor = winrt::ContentControl();
    m_focusSafeHarbor.Width(0.0);
    m_focusSafeHarbor.IsTabStop(false);
    panel.Children().InsertAt(0, m_focusSafeHarbor);

    m_focusSafeHarborLosingFocusRevoker = m_focusSafeHarbor.LosingFocus(
        winrt::auto_revoke,
        [this](const auto &sender, const winrt::LosingFocusEventArgs &args) {
          m_focusSafeHarbor.IsTabStop(false);
        });
  }
}

void ReactControl::InitializeDeveloperMenu() {
  auto coreWindow = winrt::CoreWindow::GetForCurrentThread();
  m_coreDispatcherAKARevoker = coreWindow.Dispatcher().AcceleratorKeyActivated(
      winrt::auto_revoke,
      [this](const auto &sender, const winrt::AcceleratorKeyEventArgs &args) {
        if (((args.VirtualKey() == winrt::Windows::System::VirtualKey::F10) ||
             (args.VirtualKey() == winrt::Windows::System::VirtualKey::Menu)) &&
            KeyboardHelper::IsModifiedKeyPressed(
                winrt::CoreWindow::GetForCurrentThread(),
                winrt::VirtualKey::Shift)) {
          if (!IsDeveloperMenuShowing()) {
            ShowDeveloperMenu();
          }
        }
      });
}

void ReactControl::ShowDeveloperMenu() {
  assert(m_developerMenuRoot == nullptr);

  winrt::hstring xamlString =
      L"<Grid Background='{ThemeResource SystemControlBackgroundChromeMediumBrush}'"
      L"  xmlns='http://schemas.microsoft.com/winfx/2006/xaml/presentation'"
      L"  xmlns:x='http://schemas.microsoft.com/winfx/2006/xaml'>"
      L"  <StackPanel HorizontalAlignment='Center'>"
      L"    <TextBlock Margin='0,0,0,10' FontSize='40'>Developer Menu</TextBlock>"
      L"    <Button HorizontalAlignment='Stretch' x:Name='Reload'>Reload Javascript (TBD) </Button>"
      L"    <Button HorizontalAlignment='Stretch' x:Name='RemoteDebug'></Button>"
      L"    <Button HorizontalAlignment='Stretch' x:Name='LiveReload'>Enable Live Reload (TBD) </Button>"
      L"    <Button HorizontalAlignment='Stretch' x:Name='Inspector'>Show Inspector (TBD) </Button>"
      L"    <Button HorizontalAlignment='Stretch' x:Name='Cancel'>Cancel</Button>"
      L"  </StackPanel>"
      L"</Grid>";
  m_developerMenuRoot = winrt::unbox_value<winrt::Grid>(
      winrt::Markup::XamlReader::Load(xamlString));
  auto remoteDebugJSButton =
      m_developerMenuRoot.FindName(L"RemoteDebug").as<winrt::Button>();
  auto cancelButton =
      m_developerMenuRoot.FindName(L"Cancel").as<winrt::Button>();
  bool useWebDebugger =
      m_reactInstance->GetReactInstanceSettings().UseWebDebugger;
  remoteDebugJSButton.Content(winrt::box_value(
      useWebDebugger ? L"Disable Remote JS Debugging"
                     : L"Enable Remote JS Debugging"));
  m_remoteDebugJSRevoker = remoteDebugJSButton.Click(
      winrt::auto_revoke,
      [this, useWebDebugger](
          const auto &sender, const winrt::RoutedEventArgs &args) {
        DismissDeveloperMenu();
        m_instanceCreator->persistUseWebDebugger(!useWebDebugger);
        Reload(true);
      });
  m_cancelRevoker = cancelButton.Click(
      winrt::auto_revoke,
      [this](const auto &sender, const winrt::RoutedEventArgs &args) {
        DismissDeveloperMenu();
      });

  auto xamlRootGrid(m_xamlRootView.as<winrt::Grid>());
  xamlRootGrid.Children().Append(m_developerMenuRoot);
}

void ReactControl::DismissDeveloperMenu() {
  assert(m_developerMenuRoot != nullptr);
  auto xamlRootGrid(m_xamlRootView.as<winrt::Grid>());
  uint32_t indexToRemove = 0;
  xamlRootGrid.Children().IndexOf(m_developerMenuRoot, indexToRemove);
  xamlRootGrid.Children().RemoveAt(indexToRemove);
  m_developerMenuRoot = nullptr;
}

bool ReactControl::IsDeveloperMenuShowing() const {
  return (m_developerMenuRoot != nullptr);
}

} // namespace uwp
} // namespace react
