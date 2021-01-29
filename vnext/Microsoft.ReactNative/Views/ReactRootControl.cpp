// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"

#include "ReactRootControl.h"

#include <CxxMessageQueue.h>
#include <InstanceFactory.h>
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

#include <ReactHost/React.h>
#include <ReactHost/React_Win.h>
#include <dispatchQueue/dispatchQueue.h>
#include <object/unknownObject.h>

#include <ReactHost/MsoUtils.h>
#include <Utils/Helpers.h>

#include <UI.Xaml.Controls.Primitives.h>
#include <UI.Xaml.Controls.h>
#include <UI.Xaml.Input.h>
#include <UI.Xaml.Markup.h>

#include "DevMenu.h"
#include "Modules/DevSettingsModule.h"

namespace react::uwp {

using XamlView = Microsoft::ReactNative::XamlView;

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

Mso::React::IReactContext *ReactRootControl::GetReactContext() const noexcept {
  return m_context.Get();
}

XamlView ReactRootControl::GetXamlView() const noexcept {
  return m_weakXamlRootView.get();
}

void ReactRootControl::SetJSComponentName(std::string && /*mainComponentName*/) noexcept {
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
    if (auto element = xamlRootView.as<xaml::FrameworkElement>()) {
      return static_cast<int64_t>(element.ActualHeight());
    }
  }

  return 0;
}

int64_t ReactRootControl::GetActualWidth() const noexcept {
  if (auto xamlRootView = m_weakXamlRootView.get()) {
    if (auto element = xamlRootView.as<xaml::FrameworkElement>()) {
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
    xaml::Input::FocusManager::TryFocusAsync(m_focusSafeHarbor, winrt::FocusState::Pointer);
  } else
    xaml::Input::FocusManager::TryFocusAsync(xamlView, winrt::FocusState::Pointer);
}

void ReactRootControl::InitRootView(
    Mso::CntPtr<Mso::React::IReactInstance> &&reactInstance,
    Mso::React::ReactViewOptions &&reactViewOptions) noexcept {
  VerifyElseCrash(m_uiQueue.HasThreadAccess());

  if (m_isInitialized) {
    UninitRootView();
  }

  m_reactOptions = std::make_unique<Mso::React::ReactOptions>(reactInstance->Options());
  m_weakReactInstance = Mso::WeakPtr{reactInstance};
  m_context = &reactInstance->GetReactContext();
  m_reactViewOptions = std::make_unique<Mso::React::ReactViewOptions>(std::move(reactViewOptions));

  if (!m_touchEventHandler) {
    m_touchEventHandler = std::make_shared<Microsoft::ReactNative::TouchEventHandler>(*m_context);
  }

  if (!m_SIPEventHandler) {
    m_SIPEventHandler = std::make_shared<Microsoft::ReactNative::SIPEventHandler>(*m_context);
  }

  if (!m_previewKeyboardEventHandlerOnRoot) {
    m_previewKeyboardEventHandlerOnRoot =
        std::make_shared<Microsoft::ReactNative::PreviewKeyboardEventHandlerOnRoot>(*m_context);
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

void ReactRootControl::UninitRootView() noexcept {
  if (!m_isInitialized) {
    return;
  }

  if (m_isJSViewAttached) {
    if (auto reactInstance = m_weakReactInstance.GetStrongPtr()) {
      reactInstance->DetachRootView(this);
    }
  }

  if (m_touchEventHandler != nullptr) {
    m_touchEventHandler->RemoveTouchHandlers();
  }

  if (!m_previewKeyboardEventHandlerOnRoot) {
    m_previewKeyboardEventHandlerOnRoot->unhook();
  }

  RemoveBackHandlers();

  // Clear members with a dependency on the reactInstance
  m_touchEventHandler.reset();
  m_SIPEventHandler.reset();

  m_reactOptions = nullptr;
  m_context.Clear();
  m_reactViewOptions = nullptr;
  m_weakReactInstance = nullptr;

  m_isInitialized = false;
}

void ReactRootControl::ClearLoadingUI() noexcept {
  if (XamlView xamlRootView = m_weakXamlRootView.get()) {
    auto xamlRootGrid{xamlRootView.as<winrt::Grid>()};

    auto children = xamlRootGrid.Children();
    uint32_t index{0};
    if (m_greenBoxGrid && children.IndexOf(m_greenBoxGrid, index)) {
      children.RemoveAt(index);
    }
  }
}

void ReactRootControl::EnsureLoadingUI() noexcept {
  if (XamlView xamlRootView = m_weakXamlRootView.get()) {
    auto xamlRootGrid{xamlRootView.as<winrt::Grid>()};

    // Create Grid & TextBlock to hold text
    if (m_waitingTextBlock == nullptr) {
      m_waitingTextBlock = winrt::TextBlock();
      m_greenBoxGrid = winrt::Grid{};
      m_greenBoxGrid.Background(xaml::Media::SolidColorBrush(winrt::ColorHelper::FromArgb(0xff, 0x03, 0x59, 0)));
      m_greenBoxGrid.Children().Append(m_waitingTextBlock);
      m_greenBoxGrid.VerticalAlignment(xaml::VerticalAlignment::Center);

      // Format TextBlock
      m_waitingTextBlock.TextAlignment(winrt::TextAlignment::Center);
      m_waitingTextBlock.TextWrapping(xaml::TextWrapping::Wrap);
      m_waitingTextBlock.FontFamily(winrt::FontFamily(L"Consolas"));
      m_waitingTextBlock.Foreground(xaml::Media::SolidColorBrush(winrt::Colors::White()));
      winrt::Thickness margin = {10.0f, 10.0f, 10.0f, 10.0f};
      m_waitingTextBlock.Margin(margin);
    }

    auto children = xamlRootGrid.Children();
    uint32_t index;
    if (m_greenBoxGrid && !children.IndexOf(m_greenBoxGrid, index)) {
      children.Append(m_greenBoxGrid);
    }
  }
}

void ReactRootControl::ShowInstanceLoaded() noexcept {
  if (XamlView xamlRootView = m_weakXamlRootView.get()) {
    auto xamlRootGrid{xamlRootView.as<winrt::Grid>()};

    ClearLoadingUI();

    if (auto reactInstance = m_weakReactInstance.GetStrongPtr()) {
      reactInstance->AttachMeasuredRootView(this, Mso::Copy(m_reactViewOptions->InitialProps));
    }
    m_isJSViewAttached = true;
  }
}

void ReactRootControl::ShowInstanceError() noexcept {
  if (XamlView xamlRootView = m_weakXamlRootView.get()) {
    auto xamlRootGrid{xamlRootView.as<winrt::Grid>()};

    ClearLoadingUI();
  }
}

void ReactRootControl::ShowInstanceWaiting() noexcept {
  if (XamlView xamlRootView = m_weakXamlRootView.get()) {
    auto xamlRootGrid{xamlRootView.as<winrt::Grid>()};

    EnsureLoadingUI();

    // Place message into TextBlock
    std::wstring wstrMessage(L"Connecting to remote debugger");
    m_waitingTextBlock.Text(wstrMessage);
  }
}

void ReactRootControl::ShowInstanceLoading() noexcept {
  if (!m_context->SettingsSnapshot().UseDeveloperSupport())
    return;

  if (XamlView xamlRootView = m_weakXamlRootView.get()) {
    auto xamlRootGrid{xamlRootView.as<winrt::Grid>()};

    EnsureLoadingUI();

    // Place message into TextBlock
    std::wstring wstrMessage(L"Loading bundle.");
    m_waitingTextBlock.Text(wstrMessage);
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

    m_focusSafeHarbor = xaml::Controls::ContentControl{};
    m_focusSafeHarbor.Width(0.0);
    m_focusSafeHarbor.IsTabStop(false);
    panel.Children().InsertAt(0, m_focusSafeHarbor);

    m_focusSafeHarborLosingFocusRevoker = m_focusSafeHarbor.LosingFocus(
        winrt::auto_revoke, [this](const auto & /*sender*/, const winrt::LosingFocusEventArgs & /*args*/) {
          m_focusSafeHarbor.IsTabStop(false);
        });
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
  auto rootElement(rootView.try_as<xaml::UIElement>());
  if (rootElement == nullptr) {
    assert(false);
    return;
  }

  // Handle keyboard "back" button press
  xaml::Input::KeyboardAccelerator goBack{};
  goBack.Key(winrt::Windows::System::VirtualKey::GoBack);
  goBack.Invoked([weakThis](
                     xaml::Input::KeyboardAccelerator const & /*sender*/,
                     xaml::Input::KeyboardAcceleratorInvokedEventArgs const &args) {
    if (auto self = weakThis.lock()) {
      args.Handled(self->OnBackRequested());
    }
  });
  rootElement.KeyboardAccelerators().Append(goBack);

  // Handle Alt+Left keyboard shortcut
  xaml::Input::KeyboardAccelerator altLeft{};
  altLeft.Key(winrt::Windows::System::VirtualKey::Left);
  altLeft.Invoked([weakThis](
                      xaml::Input::KeyboardAccelerator const & /*sender*/,
                      xaml::Input::KeyboardAcceleratorInvokedEventArgs const &args) {
    if (auto self = weakThis.lock()) {
      args.Handled(self->OnBackRequested());
    }
  });
  rootElement.KeyboardAccelerators().Append(altLeft);
  altLeft.Modifiers(winrt::Windows::System::VirtualKeyModifiers::Menu);

  // Hide keyboard accelerator tooltips
  if (react::uwp::IsRS4OrHigher()) {
    rootElement.KeyboardAcceleratorPlacementMode(xaml::Input::KeyboardAcceleratorPlacementMode::Hidden);
  }
}

void ReactRootControl::RemoveBackHandlers() noexcept {
  m_backRequestedRevoker.revoke();
  if (auto rootView = m_weakRootView.get()) {
    if (auto element = rootView.try_as<xaml::UIElement>()) {
      element.KeyboardAccelerators().Clear();
    }
  }
}

bool ReactRootControl::OnBackRequested() noexcept {
  if (m_context->State() != Mso::React::ReactInstanceState::Loaded)
    return false;

  m_context->CallJSFunction("RCTDeviceEventEmitter", "emit", folly::dynamic::array("hardwareBackPress"));
  return true;
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
  return PostInUIQueue([reactInstance{std::move(reactInstance)},
                        viewOptions{std::move(viewOptions)}](ReactRootControl &rootControl) mutable noexcept {
    rootControl.InitRootView(std::move(reactInstance), std::move(viewOptions));
  });
}

Mso::Future<void> ReactViewInstance::UpdateRootView() noexcept {
  return PostInUIQueue([](ReactRootControl &rootControl) mutable noexcept { rootControl.UpdateRootView(); });
}

Mso::Future<void> ReactViewInstance::UninitRootView() noexcept {
  return PostInUIQueue([](ReactRootControl &rootControl) mutable noexcept { rootControl.UninitRootView(); });
}

} // namespace react::uwp
