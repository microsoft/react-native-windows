// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.
#include "pch.h"

#include "DrawingIsland.h"

#include "DrawingIsland.g.cpp"

#ifdef RNW_NEW_ARCH

#include <strsafe.h>
#include <winrt/Microsoft.UI.Composition.SystemBackdrops.h>
#include <winrt/Microsoft.UI.Composition.h>
#include <winrt/Microsoft.UI.Content.h>
#include <winrt/Microsoft.UI.Dispatching.h>
#include <winrt/Microsoft.UI.Input.h>
#include <winrt/Microsoft.UI.interop.h>
#include <winrt/Windows.System.h>
#include <winrt/Windows.UI.Composition.h>
#include <winrt/Windows.UI.h>

namespace winrt {
using namespace winrt::Windows::Foundation;
using namespace winrt::Windows::Foundation::Collections;
using namespace winrt::Windows::Foundation::Numerics;
using namespace winrt::Windows::System;
using namespace winrt::Windows::UI;

using namespace winrt::Microsoft::UI;
using namespace winrt::Microsoft::UI::Composition;
using namespace winrt::Microsoft::UI::Composition::SystemBackdrops;
using namespace winrt::Microsoft::UI::Content;
using namespace winrt::Microsoft::UI::Input;
} // namespace winrt

namespace winrt::SampleCustomComponent::implementation {

// std::map<winrt::Visual, winrt::com_ptr<NodeSimpleFragment>> g_visualToFragmentMap;

DrawingIsland::DrawingIsland(const winrt::Microsoft::UI::Composition::Compositor &compositor) {
  m_compositor = compositor;

  // Create the Compositor and the Content:
  // - The Bridge's connection to the Window will keep everything alive, and perform an orderly
  //   tear-down:
  //
  //   Window -> Bridge -> Content -> Visual -> InputSite -> InputObject
  //
  // - When the ContentIsland is destroyed, ContentIsland.AppData will call IClosable.Close on
  //   this instance to break cycles and clean up expensive resources.

  m_backgroundVisual = m_compositor.CreateSpriteVisual();
  m_backgroundVisual.RelativeSizeAdjustment(winrt::float2(1.0f, 1.0f));

  m_island = winrt::ContentIsland::Create(m_backgroundVisual);
  m_island.AppData(get_strong().as<winrt::IInspectable>());

  Output_Initialize();
  Input_Initialize();
  Window_Initialize();
  m_prevRasterizationScale = m_island.RasterizationScale();

  // Get notifications for resize, bridge changes, etc.
  (void)m_island.StateChanged([&](auto &&...) { return Island_OnStateChanged(); });

  // Just adding EnqueueFromBackgroundThread method for testing.
  EnqueueFromBackgroundThread();

  // Get notifications for island disconnection.
#ifdef USE_EXPERIMENTAL_WINUI3
  (void)m_island.Connected([&](auto &&...) { return Island_OnConnected(); });

  (void)m_island.Disconnected([&](auto &&...) { return Island_OnDisconnected(); });
#endif
  (void)m_island.Closed([&]() { return Island_OnClosed(); });
}

DrawingIsland::~DrawingIsland() {
  m_siteBridge = nullptr;
  m_popupSiteBridge = nullptr;
  // m_fragmentRoot = nullptr;
  m_compositor = nullptr;
}

// IClosable methods
void DrawingIsland::Close() {
  WCHAR msg2[300];
  StringCbPrintf(msg2, sizeof(msg2), L"DrawingIsland::Close \n");
  OutputDebugStringW(msg2);
  m_visuals = nullptr;
  m_selectedVisual = nullptr;
  m_backgroundBrushDefault = nullptr;
  m_backgroundBrushA = nullptr;
  m_backgroundBrushB = nullptr;
  m_backgroundBrushC = nullptr;
  m_backgroundVisual = nullptr;

  // Destroy SystemBackdropController objects.
  if (m_backdropController != nullptr) {
    if (m_backdropConfiguration != nullptr) {
      // m_backdropConfiguration is only initialized for the DrawingIsland that owns the system backdrop controller.
      m_backdropController.Close();
      m_backdropController = nullptr;
      m_backdropConfiguration = nullptr;
    } else {
      // We're closing a DrawingIsland in a popup, not the owner DrawingIsland of the system backdrop controller.
      // Remove the current target from the system backdrop controller.
      m_backdropController.RemoveSystemBackdropTarget(m_backdropTarget);
    }
  }

  // Destroy Content:
  // - This handles if the ContentIsland has already started destruction and is notifying the app.
  m_island.Close();
  m_island = nullptr;

  // TODO: Add the following test case in automated tests:
  // 1. We are recursively calling ContentIsland.Close while inside the ContentIsland.Closed
  // event.
  // 2. We are testing the potential final Release() of ContentIsland while inside the Closed
  // event.
}

// Properties
boolean DrawingIsland::UseSystemBackdrop() {
  return m_useSystemBackdrop;
}

void DrawingIsland::UseSystemBackdrop(boolean value) {
  if (m_useSystemBackdrop != value) {
    m_useSystemBackdrop = value;

    EvaluateUseSystemBackdrop();
  }
}

boolean DrawingIsland::InputActivatePopup() {
  return m_inputActivatePopup;
}

void DrawingIsland::InputActivatePopup(boolean value) {
  if (m_inputActivatePopup != value) {
    m_inputActivatePopup = value;

    EvaluateInputActivatePopup();
  }
}

boolean DrawingIsland::LightDismissPopup() {
  return m_lightDismissPopup;
}

void DrawingIsland::LightDismissPopup(boolean value) {
  if (m_lightDismissPopup != value) {
    m_lightDismissPopup = value;

    EvaluateLightDismissPopup();
  }
}

boolean DrawingIsland::IgnoreLeftButtonPressed() {
  return m_ignoreLeftButtonPressed;
}

void DrawingIsland::IgnoreLeftButtonPressed(boolean value) {
  if (m_ignoreLeftButtonPressed != value) {
    m_ignoreLeftButtonPressed = value;
  }
}

winrt::ContentIsland DrawingIsland::Island() const {
  return m_island;
}

// Methods

void DrawingIsland::LeftClickAndRelease(const winrt::float2 currentPoint) {
  OnLeftClick(currentPoint, false);
  Input_OnPointerReleased();
}

void DrawingIsland::RightClickAndRelease(const winrt::float2 currentPoint) {
  OnRightClick(currentPoint);
  Input_OnPointerReleased();
}

void DrawingIsland::SetHostBridge(const winrt::IContentSiteBridge &bridge) {
  m_siteBridge = bridge;
  m_popupSiteBridge = m_siteBridge.try_as<winrt::DesktopPopupSiteBridge>();
  Accessibility_Initialize();
  SystemBackdrop_Initialize();
  EvaluateLightDismissPopup();
  EvaluateInputActivatePopup();
}

void DrawingIsland::InitializeForCrossProc() {
  m_crossProcUIA = true;
  Accessibility_Initialize();
  SystemBackdrop_Initialize();
}

void DrawingIsland::SetSystemBackdropController(const winrt::ISystemBackdropControllerWithTargets &backdropController) {
  m_backdropController = backdropController;
}

void DrawingIsland::SetBackroundOpacity(float backgroundOpacity) {
  m_backgroundOpacity = backgroundOpacity;
}

void DrawingIsland::SetColorIndex(std::uint32_t colorIndex) {
  m_currentColorIndex = std::clamp<std::uint32_t>(colorIndex, 0, _countof(s_colors) - 1);
  if (m_currentColorIndex >= 4) {
    m_backgroundBrushDefault = m_compositor.CreateColorBrush(s_colors[m_currentColorIndex]);

    m_backgroundVisual.Brush(m_backgroundBrushDefault);
  }
  Output_UpdateCurrentColorVisual();
}

/*
// IInputKeyboardSourcePreTranslateHandler methods
IFACEMETHOD(DrawingIsland::OnDirectMessage)
(IInputPreTranslateKeyboardSourceInterop *source, const MSG *msg, UINT keyboardModifiers, _Inout_ bool *handled) {
  *handled = false;

  // Alt+A Debug print to see the order of the PreTranslate callbacks
  if ((keyboardModifiers & FALT) && (msg->message == WM_SYSKEYDOWN) &&
      (static_cast<winrt::Windows::System::VirtualKey>(msg->wParam) == winrt::VirtualKey::A)) {
    WCHAR msg2[300];
    StringCbPrintf(msg2, sizeof(msg2), L"OnDirectMessage called on ContentIslandId=%d\n", m_island.Id());
    OutputDebugStringW(msg2);
  }

  if (keyboardModifiers & FALT) {
    *handled = Input_AcceleratorKeyActivated(static_cast<winrt::Windows::System::VirtualKey>(msg->wParam));
  }

  return S_OK;
}

IFACEMETHOD(DrawingIsland::OnTreeMessage)
(IInputPreTranslateKeyboardSourceInterop *source, const MSG *msg, UINT keyboardModifiers, _Inout_ bool *handled) {
  // Alt+A Debug print to see the order of the PreTranslate callbacks
  if ((keyboardModifiers & FALT) && (msg->message == WM_SYSKEYDOWN) &&
      (static_cast<winrt::Windows::System::VirtualKey>(msg->wParam) == winrt::VirtualKey::A)) {
    WCHAR msg2[300];
    StringCbPrintf(msg2, sizeof(msg2), L"OnTreeMessage called on ContentIslandId=%d\n", m_island.Id());
    OutputDebugStringW(msg2);
  }

  *handled = false;
  return S_OK;
}
*/

void DrawingIsland::Accessibility_Initialize() {
  /*
  m_fragmentRoot = winrt::make_self<IslandFragmentRoot>(m_island);

  if (m_crossProcUIA) {
    m_fragmentRoot->SetName(L"CrossProc Island");
  } else if (m_siteBridge != nullptr) {
    auto desktopChildBridge = m_siteBridge.try_as<winrt::IDesktopChildSiteBridge>();
    if (desktopChildBridge != nullptr) {
      m_fragmentRoot->SetName(L"Child Island");
    } else {
      m_fragmentRoot->SetName(L"Popup Island");
    }
  }

  (void)m_island.AutomationProviderRequested({this, &DrawingIsland::Accessibility_OnAutomationProviderRequested});
  */
}

/*
void DrawingIsland::Accessibility_OnAutomationProviderRequested(
  const winrt::ContentIsland &island,
  const winrt::ContentIslandAutomationProviderRequestedEventArgs &args) {
IInspectable providerAsIInspectable;
m_fragmentRoot->QueryInterface(winrt::guid_of<IInspectable>(), winrt::put_abi(providerAsIInspectable));
args.AutomationProvider(std::move(providerAsIInspectable));

args.Handled(true);
}
*/

void DrawingIsland::CreateUIAProviderForVisual() {
  /*
  winrt::com_ptr<NodeSimpleFragment> fragment =
      m_fragmentFactory.Create(s_colorNames[m_currentColorIndex].c_str(), m_fragmentRoot);

  g_visualToFragmentMap[m_selectedVisual] = fragment;

  fragment->SetVisual(m_selectedVisual);
  // Set up children roots
  m_fragmentRoot->AddChild(fragment);

  // Finally set up parent chain
  fragment->SetParent(m_fragmentRoot);
  */
}

void DrawingIsland::EnqueueFromBackgroundThread() {
  std::thread updateThread1{[&]() {
    winrt::Microsoft::UI::Dispatching::DispatcherQueue dispatcherQueue = m_island.DispatcherQueue();

    bool result = dispatcherQueue.TryEnqueue(winrt::Microsoft::UI::Dispatching::DispatcherQueuePriority::High, [&] {
      WCHAR msg2[300];
      StringCbPrintf(
          msg2, sizeof(msg2), L"Enqueued for dispatching on background thread ContentIslandId=%d\n", m_island.Id());
      OutputDebugStringW(msg2);
      winrt::Windows::Foundation::Numerics::float2 size = {500, 500};
      m_island.RequestSize(size);
    });

    if (!result) {
      WCHAR msg2[300];
      StringCbPrintf(
          msg2,
          sizeof(msg2),
          L"This means the dispatcherQueue has shutdown already and the message can't be enqueued.\n");
      OutputDebugStringW(msg2);
    }
  }};
  updateThread1.join();
}

void DrawingIsland::EvaluateUseSystemBackdrop() {
  BYTE backgroundBrushOpacity = 0xFF;
  // If we use system backdrops, it will be behind our background visual. Make sure we can
  // see through the background visual to reveal the system backdrop.
  // reveal the system backdrop underneath.
  if (m_useSystemBackdrop) {
    backgroundBrushOpacity = 0x30;
  }

  // Create the background parent Visual that the individual square will be added into.
  m_backgroundBrushDefault = m_compositor.CreateColorBrush(winrt::Color{backgroundBrushOpacity, 0x20, 0x20, 0x20});
  m_backgroundBrushA = m_compositor.CreateColorBrush(winrt::Color{backgroundBrushOpacity, 0x99, 0x20, 0x20});
  m_backgroundBrushB = m_compositor.CreateColorBrush(winrt::Color{backgroundBrushOpacity, 0x20, 0x99, 0x20});
  m_backgroundBrushC = m_compositor.CreateColorBrush(winrt::Color{backgroundBrushOpacity, 0x20, 0x20, 0x99});

  m_backgroundVisual.Brush(m_backgroundBrushDefault);
}

void DrawingIsland::EvaluateInputActivatePopup() {
  // This policy only applies to popups, check if we are a popup before proceeding.
  if (!IsHostedByPopupWindowSiteBridge())
    return;

  /*
  if (m_inputActivatePopup) {
    m_pointerSource.ActivationBehavior(winrt::Microsoft::UI::Input::InputPointerActivationBehavior::Activate);
  } else {
    m_pointerSource.ActivationBehavior(winrt::InputPointerActivationBehavior::NoActivate);
  }
    */
}

void DrawingIsland::EvaluateLightDismissPopup() {
  /*
  // This policy only applies to popups, check if we are a popup before proceeding.
  if (!IsHostedByPopupWindowSiteBridge())
    return;

  if (m_lightDismissPopup) {
    m_lightDismissAction = winrt::InputLightDismissAction::GetForIsland(m_island);
    m_lightDismissAction.Dismissed(
        [this](winrt::InputLightDismissAction const &, winrt::InputLightDismissEventArgs const & args) {
          m_popupSiteBridge.Hide();
          m_popupSiteBridge.Close();

          m_siteBridge = nullptr;
          m_popupSiteBridge = nullptr;
        });
  } else {
    m_lightDismissAction = nullptr;
  }
    */
}

winrt::Visual DrawingIsland::HitTestVisual(winrt::float2 const point) {
  winrt::Visual selectedVisual{nullptr};
  for (winrt::Visual visual : m_visuals) {
    winrt::float3 const offset = visual.Offset();
    winrt::float2 const size = visual.Size();

    if (point.x >= offset.x && point.x < offset.x + size.x && point.y >= offset.y && point.y < offset.y + size.y) {
      selectedVisual = visual;
    }
  }

  return selectedVisual;
}

void DrawingIsland::Input_Initialize() {
  m_pointerSource = winrt::Microsoft::UI::Input::InputPointerSource::GetForIsland(m_island);

  m_pointerSource.PointerPressed(
      [this](winrt::InputPointerSource const &, winrt::Microsoft::UI::Input::PointerEventArgs const &args) {
        auto currentPoint = args.CurrentPoint();
        auto properties = currentPoint.Properties();

        if (properties.IsLeftButtonPressed()) {
          Input_OnLeftButtonPressed(args);
        } else if (properties.IsRightButtonPressed()) {
          Input_OnRightButtonPressed(args);
        }
      });

  m_pointerSource.PointerMoved(
      [this](winrt::InputPointerSource const &, winrt::Microsoft::UI::Input::PointerEventArgs const &args) {
        Input_OnPointerMoved(args);
      });

  m_pointerSource.PointerReleased([&](auto &&...) { Input_OnPointerReleased(); });

  // Set up the keyboard source. We store this in a member variable so we can easily call
  // TrySetFocus() in response to left clicks in the content later on.
  m_keyboardSource = winrt::Microsoft::UI::Input::InputKeyboardSource::GetForIsland(m_island);

  m_keyboardSource.KeyDown([this](
                               winrt::Microsoft::UI::Input::InputKeyboardSource const &,
                               winrt::Microsoft::UI::Input::KeyEventArgs const &args) {
    bool handled = Input_OnKeyDown(args.VirtualKey());

    // Mark the event as handled
    if (handled) {
      args.Handled(true);
    }
  });

  /*
  m_pretranslateSource = winrt::InputPreTranslateKeyboardSource::GetForIsland(m_island);

  m_pretranslateSource.as<Microsoft::UI::Input::IInputPreTranslateKeyboardSourceInterop>()->SetPreTranslateHandler(
      this);
  */

  auto activationListener = winrt::Microsoft::UI::Input::InputActivationListener::GetForIsland(m_island);
  (void)activationListener.InputActivationChanged(
      [this, activationListener](
          winrt::InputActivationListener const &,
          winrt::Microsoft::UI::Input::InputActivationListenerActivationChangedEventArgs const &) {
        switch (activationListener.State()) {
          case winrt::InputActivationState::Activated:
            m_backgroundVisual.Opacity(1.0f);
            break;

          default:
            m_backgroundVisual.Opacity(m_backgroundOpacity);
            break;
        }
      });

  m_focusController = winrt::Microsoft::UI::Input::InputFocusController::GetForIsland(m_island);
  m_focusController.NavigateFocusRequested(
      [this](
          winrt::Microsoft::UI::Input::InputFocusController const &,
          winrt::Microsoft::UI::Input::FocusNavigationRequestEventArgs const &args) {
        bool setFocus = m_focusController.TrySetFocus();
        // Mark the event as handled
        if (setFocus) {
          args.Result(winrt::FocusNavigationResult::Moved);
        }
      });
}

bool DrawingIsland::IsHostedByPopupWindowSiteBridge() {
  return (m_popupSiteBridge != nullptr);
}

bool DrawingIsland::Input_OnKeyDown(winrt::Windows::System::VirtualKey virtualKey) {
  bool handled = false;

  switch (virtualKey) {
    case winrt::VirtualKey::A: {
      m_backgroundVisual.Brush(m_backgroundBrushA);
      handled = true;
      break;
    }

    case winrt::VirtualKey::B: {
      m_backgroundVisual.Brush(m_backgroundBrushB);
      handled = true;
      break;
    }

    case winrt::VirtualKey::C: {
      m_backgroundVisual.Brush(m_backgroundBrushC);
      handled = true;
      break;
    }

    case winrt::VirtualKey::Space: {
      m_backgroundVisual.Brush(m_backgroundBrushDefault);
      break;
    }

    case winrt::Windows::System::VirtualKey::Number1: {
      m_currentColorIndex = 0;
      handled = true;
      break;
    }

    case winrt::Windows::System::VirtualKey::Number2: {
      m_currentColorIndex = 1;
      handled = true;
      break;
    }

    case winrt::Windows::System::VirtualKey::Number3: {
      m_currentColorIndex = 2;
      handled = true;
      break;
    }

    case winrt::Windows::System::VirtualKey::Number4: {
      m_currentColorIndex = 3;
      handled = true;
      break;
    }

    case winrt::Windows::System::VirtualKey::Delete:
    case winrt::Windows::System::VirtualKey::Escape: {
      m_visuals.RemoveAll();
      handled = true;
      break;
    }

    case winrt::Windows::System::VirtualKey::Tab: {
      auto request = winrt::Microsoft::UI::Input::FocusNavigationRequest::Create(
          winrt::Microsoft::UI::Input::FocusNavigationReason::First);
      m_focusController.DepartFocus(request);
    }
  }

  Output_UpdateCurrentColorVisual();

  return handled;
}

bool DrawingIsland::Input_AcceleratorKeyActivated(winrt::Windows::System::VirtualKey virtualKey) {
  bool handled = false;

  switch (virtualKey) {
    case winrt::VirtualKey::X: {
      m_backgroundVisual.Brush(m_backgroundBrushA);
      handled = true;
      break;
    }

    case winrt::VirtualKey::Y: {
      m_backgroundVisual.Brush(m_backgroundBrushB);
      handled = true;
      break;
    }

    case winrt::VirtualKey::Z: {
      m_backgroundVisual.Brush(m_backgroundBrushC);
      handled = true;
      break;
    }
  }

  return handled;
}

void DrawingIsland::Input_OnLeftButtonPressed(const winrt::Microsoft::UI::Input::PointerEventArgs &args) {
  if (!m_ignoreLeftButtonPressed) {
    // Left button manipulates the custom-drawn content
    winrt::float2 const point = args.CurrentPoint().Position();

    auto keyModifiers = args.KeyModifiers();
    bool controlPressed =
        ((keyModifiers & winrt::Windows::System::VirtualKeyModifiers::Control) ==
         winrt::Windows::System::VirtualKeyModifiers::Control);

    OnLeftClick(point, controlPressed);
  }
}

void DrawingIsland::Input_OnRightButtonPressed(const winrt::Microsoft::UI::Input::PointerEventArgs &args) {
  OnRightClick(args.CurrentPoint().Position());
}

void DrawingIsland::Input_OnPointerMoved(const winrt::Microsoft::UI::Input::PointerEventArgs &args) {
  if (m_selectedVisual) {
    winrt::float2 const point = args.CurrentPoint().Position();
    m_selectedVisual.Offset({point.x + m_offset.x, point.y + m_offset.y, 0.0f});
  }
}

void DrawingIsland::Input_OnPointerReleased() {
  m_selectedVisual = nullptr;
}

void DrawingIsland::Island_OnStateChanged() {
  if (m_prevRasterizationScale != m_island.RasterizationScale()) {
    WCHAR msg2[300];
    StringCbPrintf(
        msg2,
        sizeof(msg2),
        L"ContentIsland Id= %d, \n RasterizationScale = %f, ActualSize = {%f, %f}  \n",
        m_island.Id(),
        m_island.RasterizationScale(),
        m_island.ActualSize().x,
        m_island.ActualSize().y);
    OutputDebugStringW(msg2);
    m_prevRasterizationScale = m_island.RasterizationScale();
  }

  if (m_prevLayout != m_island.LayoutDirection()) {
    SetLayoutDirectionForVisuals();
  }

  Output_UpdateCurrentColorVisual();
}

void DrawingIsland::Island_OnConnected() {
  SetLayoutDirectionForVisuals();
}

void DrawingIsland::Island_OnDisconnected() {
  WCHAR msg2[300];
  StringCbPrintf(msg2, sizeof(msg2), L"Island_OnDisconnected Disconnected \n");
  OutputDebugStringW(msg2);
}

void DrawingIsland::Island_OnClosed() {
  WCHAR msg2[300];
  StringCbPrintf(msg2, sizeof(msg2), L"Island_OnClosed %d\n", m_island.IsClosed());
  OutputDebugStringW(msg2);
}

void DrawingIsland::OnLeftClick(const winrt::float2 point, bool controlPressed) {
  m_selectedVisual = HitTestVisual(point);

  if (m_selectedVisual) {
    winrt::float3 const offset = m_selectedVisual.Offset();

    m_offset.x = offset.x - point.x;
    m_offset.y = offset.y - point.y;

    m_visuals.Remove(m_selectedVisual);
    m_visuals.InsertAtTop(m_selectedVisual);

    // TODO: The m_fragmentRoots child should be removed and added to the end as well.
  } else {
    Output_AddVisual(point, controlPressed);
  }

  // Only transfer focus when we are hosted inside a DesktopChildSiteBridge. When we are
  // hosted inside a PopupWindowSiteBridge, we expect to control focus and activaton by
  // setting InputPointerSource->ActivationBehavior.
  // auto desktopChildBridge = m_siteBridge.try_as<winrt::IDesktopChildSiteBridge>();
  // if (desktopChildBridge != nullptr) {
  m_focusController.TrySetFocus();
  //}
}

void DrawingIsland::OnRightClick(const winrt::float2 point) {
  winrt::Visual selectedVisual = HitTestVisual(point);

  if (selectedVisual != nullptr) {
    // Right button brings up a context menu if clicked on a visual
    if (m_siteBridge != nullptr) {
      // Convert the current position to screen coordinates and display.
      winrt::Rect initialPosition(point.x, point.y, 200, 300);

      auto convertedPosition = m_island.CoordinateConverter().ConvertLocalToScreen(initialPosition);

      // Put a new instance of the content and connect it with the popup bridge.
      auto popupContent = winrt::make_self<DrawingIsland>(m_compositor);

      // Transfer policies to the popup content.
      popupContent->InputActivatePopup(m_inputActivatePopup);
      popupContent->LightDismissPopup(m_lightDismissPopup);
      popupContent->UseSystemBackdrop(m_useSystemBackdrop);

#ifdef USE_EXPERIMENTAL_WINUI3
      // Funnel through the existing backdrop controller, we do not want to create a new one for every popup.
      popupContent->SetSystemBackdropController(m_backdropController);
#endif

      auto popupBridge = winrt::Microsoft::UI::Content::DesktopPopupSiteBridge::Create(popupContent->Island());
      popupBridge.MoveAndResize(convertedPosition);

      // Connect the content and input site to the DesktopWindowDrawingIslandBridge
      popupBridge.Connect(popupContent->Island());

      // https://task.ms/32440118: We will not have to do this once we have
      // ContentIsland.SiteBridge available, since then the ContentIsland will be able to call
      // SiteBridge.TryCreatePopupSiteBridge by itself.
      auto bridgeInterface = popupBridge.as<winrt::IContentSiteBridge>();
      popupContent->SetHostBridge(bridgeInterface);

      popupBridge.Show();

      if (m_inputActivatePopup) {
        // We are using the InputFocusController as a temporary workaround until we have
        // support on the "hosting" side for transfering activation/focus to a popup.

        auto focusController = winrt::InputFocusController::GetForIsland(m_island);
        focusController.TrySetFocus();
      }

      /*
      auto parentFragment = g_visualToFragmentMap[selectedVisual];
      auto childFragment = popupContent->m_fragmentRoot;

      parentFragment->AddChild(childFragment);

      // Finally set up parent chain
      childFragment->SetParent(parentFragment);
      */
    }
  }
}

void DrawingIsland::Output_Initialize() {
  for (int i = 0; i < _countof(m_colorBrushes); i++) {
    m_colorBrushes[i] = m_compositor.CreateColorBrush(s_colors[i]);

    winrt::Color halfTransparent = s_colors[i];
    halfTransparent.A = 0x80;
    m_halfTransparentColorBrushes[i] = m_compositor.CreateColorBrush(halfTransparent);
  }

  m_currentColorVisual = m_compositor.CreateSpriteVisual();
  m_currentColorVisual.Offset({0.0f, 0.0f, 0.0f});
  m_backgroundVisual.Children().InsertAtTop(m_currentColorVisual);

  winrt::ContainerVisual drawingVisualsRoot = m_compositor.CreateContainerVisual();
  m_visuals = drawingVisualsRoot.Children();
  m_backgroundVisual.Children().InsertAtTop(drawingVisualsRoot);

  EvaluateUseSystemBackdrop();

  Output_UpdateCurrentColorVisual();
}

void DrawingIsland::Output_AddVisual(const winrt::float2 point, bool halfTransparent) {
  winrt::SpriteVisual visual = m_compositor.CreateSpriteVisual();
  visual.Brush(
      halfTransparent ? m_halfTransparentColorBrushes[m_currentColorIndex] : m_colorBrushes[m_currentColorIndex]);

  float const BlockSize = 30.0f;
  visual.Size({BlockSize, BlockSize});
  visual.Offset({point.x - BlockSize / 2.0f, point.y - BlockSize / 2.0f, 0.0f});

  m_visuals.InsertAtTop(visual);

  m_selectedVisual = visual;
  m_offset.x = -BlockSize / 2.0f;
  m_offset.y = -BlockSize / 2.0f;

  // **FIX**: SystemVisuals currently crash configuring UIA
  if (m_siteBridge != nullptr || m_crossProcUIA) {
    CreateUIAProviderForVisual();
  }
}

void DrawingIsland::Output_UpdateCurrentColorVisual() {
  m_currentColorVisual.Brush(m_colorBrushes[m_currentColorIndex]);
  m_currentColorVisual.Offset({0.0f, m_island.ActualSize().y - 25.0f, 0.0f});
  m_currentColorVisual.Size({m_island.ActualSize().x, 25.0f});
}

void DrawingIsland::SystemBackdrop_Initialize() {
  // Don't initilize system backdrop if we haven't been configured for it.
  if (!m_useSystemBackdrop)
    return;

  if (m_backdropController == nullptr) {
    m_backdropConfiguration = winrt::SystemBackdropConfiguration();
    m_backdropController = winrt::DesktopAcrylicController();
    m_backdropController.SetSystemBackdropConfiguration(m_backdropConfiguration);

    auto activationListener = winrt::InputActivationListener::GetForIsland(m_island);
    (void)activationListener.InputActivationChanged(
        [this, activationListener](
            winrt::InputActivationListener const &, winrt::InputActivationListenerActivationChangedEventArgs const &) {
          switch (activationListener.State()) {
            case winrt::InputActivationState::Activated:
              m_backdropConfiguration.IsInputActive(true);
              break;

            default:
              m_backdropConfiguration.IsInputActive(false);
              break;
          }
        });
  }

#ifdef USE_EXPERIMENTAL_WINUI3
  if (IsHostedByPopupWindowSiteBridge()) {
    // For popups, we want to draw shadows around the edges, so clip the backdrop visual to
    // allow room on the edges for the shadows.
    m_backdropLink = winrt::ContentExternalBackdropLink::Create(m_compositor);

    // This will be the size of the "cut out" we will make in the lifted composition surface
    // so that the Backdrop System Sprite Visual will show through. This is specified in
    // logical coordinates.
    m_backdropLink.PlacementVisual().Size(m_island.ActualSize());

    // Clip the backdrop.
    m_backdropClip = m_compositor.CreateRectangleClip(
        10.0f,
        10.0f,
        m_island.ActualSize().x - 10.0f,
        m_island.ActualSize().y - 10.0f,
        {10.0f, 10.0f},
        {10.0f, 10.0f},
        {10.0f, 10.0f},
        {10.0f, 10.0f});
    m_backdropLink.PlacementVisual().Clip(m_backdropClip);

    // Clip the overall background.
    m_backgroundClip = m_compositor.CreateRectangleClip(
        0.0f,
        0.0f,
        m_island.ActualSize().x,
        m_island.ActualSize().y,
        {10.0f, 10.0f},
        {10.0f, 10.0f},
        {10.0f, 10.0f},
        {10.0f, 10.0f});
    m_backgroundVisual.Clip(m_backgroundClip);

    // Add the backdropLink into the LiftedVisual tree of the popup.
    m_backgroundVisual.Children().InsertAtBottom(m_backdropLink.PlacementVisual());

    auto animation = m_compositor.CreateVector3KeyFrameAnimation();
    animation.InsertKeyFrame(0.0f, {0.0f, -m_island.ActualSize().y, 0.0f});
    animation.InsertKeyFrame(1.0f, {0.0f, 0.0f, 0.0f});
    animation.Duration(std::chrono::milliseconds(2000));
    animation.IterationBehavior(AnimationIterationBehavior::Count);
    animation.IterationCount(1);
    m_backgroundVisual.StartAnimation(L"Offset", animation);

    // For Popups, we want to customize the clip and offset of the system backdrop, so we
    // pass the ContentExternalBackdropLink as the target to the BackdropController.

    m_backdropTarget = m_backdropLink;
  } else
#endif
  {
    // If we are the main content, we don't want to add custom clips or offsets to our
    // backdrop, so we can pass the ContentIsland as the target to the BackdropController.
    // This will by default fill the entire ContentIsland backdrop surface.

    m_backdropTarget = m_island;
  }

  m_backdropController.AddSystemBackdropTarget(m_backdropTarget);
}

void DrawingIsland::SetLayoutDirectionForVisuals() {
  if (m_island.LayoutDirection() == ContentLayoutDirection::RightToLeft) {
    // The following will mirror the visuals. If any text is inside the visuals the text
    // is also mirrored. The text will need another RelativeOffsetAdjustment and Scale to
    // return to the normal space.
    m_backgroundVisual.RelativeOffsetAdjustment(winrt::float3(1, 0, 0));
    m_backgroundVisual.Scale(winrt::float3(-1, 1, 1));
  } else {
    m_backgroundVisual.RelativeOffsetAdjustment(winrt::float3(0, 0, 0));
    m_backgroundVisual.Scale(winrt::float3(1, 1, 1));
  }
  m_prevLayout = m_island.LayoutDirection();
}

void DrawingIsland::Window_Initialize() {
  auto window = m_island.Environment();

  (void)window.SettingChanged(
      [this](winrt::ContentIslandEnvironment const &, winrt::ContentEnvironmentSettingChangedEventArgs const &args) {
        return Window_OnSettingChanged(args);
      });

#ifdef USE_EXPERIMENTAL_WINUI3
  (void)window.ThemeChanged(
      [this](winrt::ContentIslandEnvironment const &, winrt::IInspectable const &) { return Window_OnThemeChanged(); });
#endif

  (void)window.StateChanged([this](winrt::ContentIslandEnvironment const &sender, winrt::IInspectable const &) {
    return Window_OnStateChanged(sender);
  });
}

void DrawingIsland::Window_OnSettingChanged(const winrt::ContentEnvironmentSettingChangedEventArgs &args) {
  auto settingChanged = args.SettingName();

  if (settingChanged == L"intl") {
    m_backgroundVisual.Brush(m_backgroundBrushA);
  }
}

void DrawingIsland::Window_OnThemeChanged() {
  // Do nothing intentionally - For testing purposes only
}

void DrawingIsland::Window_OnStateChanged(winrt::ContentIslandEnvironment const &sender) {
  sender;
#ifdef USE_EXPERIMENTAL_WINUI3
  WCHAR msg[300];
  winrt::Microsoft::UI::DisplayId displayId = sender.DisplayId();
  float scale = sender.DisplayScale();
  winrt::Microsoft::UI::Content::ContentDisplayOrientations nativeOrientation = sender.NativeOrientation();
  winrt::Microsoft::UI::Content::ContentDisplayOrientations currentOrientation = sender.CurrentOrientation();
  HWND hwnd = winrt::GetWindowFromWindowId(sender.AppWindowId());
  RECT rect;
  GetWindowRect(hwnd, &rect);
  StringCbPrintf(
      msg,
      sizeof(msg),
      L"AppWindow Hwnd = %x, Rect.top = %d, Rect.right = %d, Rect.bottom = %d, Rect.left = %d, DisplayId: %p, DisplayScale: %f, NativeOrientation: %d, CurrentOrientation: %d\n",
      hwnd,
      rect.top,
      rect.right,
      rect.bottom,
      rect.left,
      displayId.Value,
      scale,
      nativeOrientation,
      currentOrientation);
  OutputDebugStringW(msg);
#endif
}

struct DrawingIslandComponentView : winrt::implements<DrawingIslandComponentView, winrt::IInspectable>,
                                    Codegen::BaseDrawingIsland<DrawingIslandComponentView> {
  void InitializeContentIsland(
      const winrt::Microsoft::ReactNative::Composition::ContentIslandComponentView &islandView) noexcept {
    m_drawingIsland = winrt::make_self<DrawingIsland>(islandView.Compositor());
    islandView.Connect(m_drawingIsland->Island());
    islandView.Destroying({this, &DrawingIslandComponentView::Destroying});
  }

  void Destroying(const winrt::IInspectable &, const winrt::IInspectable &) {
    m_drawingIsland->Close();
  }

 private:
  winrt::com_ptr<DrawingIsland> m_drawingIsland;
};

} // namespace winrt::SampleCustomComponent::implementation

void RegisterDrawingIslandComponentView(winrt::Microsoft::ReactNative::IReactPackageBuilder const &packageBuilder) {
  winrt::SampleCustomComponent::Codegen::RegisterDrawingIslandNativeComponent<
      winrt::SampleCustomComponent::implementation::DrawingIslandComponentView>(
      packageBuilder,
      [](const winrt::Microsoft::ReactNative::Composition::IReactCompositionViewComponentBuilder &builder) {
        builder.SetContentIslandComponentViewInitializer(
            [](const winrt::Microsoft::ReactNative::Composition::ContentIslandComponentView &islandView) noexcept {
              auto userData =
                  winrt::make_self<winrt::SampleCustomComponent::implementation::DrawingIslandComponentView>();
              userData->InitializeContentIsland(islandView);
              islandView.UserData(*userData);
            });
      });
}

#endif // #ifdef RNW_NEW_ARCH