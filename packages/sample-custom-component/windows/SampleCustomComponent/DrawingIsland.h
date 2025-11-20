// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.
#pragma once

#include "pch.h"
#include "resource.h"

#include "DrawingIsland.g.h"

#include "NativeModules.h"

#include "codegen/react/components/SampleCustomComponent/DrawingIsland.g.h"

#ifdef RNW_NEW_ARCH

#include <strsafe.h>
#include <winrt/Microsoft.ReactNative.Composition.Experimental.h>
#include <winrt/Microsoft.ReactNative.Composition.Input.h>
#include <winrt/Microsoft.ReactNative.Composition.h>
#include <winrt/Microsoft.ReactNative.h>
#include <winrt/Microsoft.UI.Composition.SystemBackdrops.h>
#include <winrt/Microsoft.UI.Composition.h>
#include <winrt/Microsoft.UI.Content.h>
#include <winrt/Microsoft.UI.interop.h>
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

struct DrawingIsland
    : public DrawingIslandT<DrawingIsland /*, Microsoft::UI::Input::IInputPreTranslateKeyboardSourceHandler*/> {
 public:
  DrawingIsland(const winrt::Microsoft::UI::Composition::Compositor &compositor);

  ~DrawingIsland();

  // IClosable methods
  void Close();

  // Properties
  boolean UseSystemBackdrop();

  void UseSystemBackdrop(boolean value);

  boolean InputActivatePopup();

  void InputActivatePopup(boolean value);

  boolean LightDismissPopup();

  void LightDismissPopup(boolean value);

  boolean IgnoreLeftButtonPressed();

  void IgnoreLeftButtonPressed(boolean value);

  winrt::ContentIsland Island() const;

  // Methods

  void LeftClickAndRelease(const winrt::float2 currentPoint);

  void RightClickAndRelease(const winrt::float2 currentPoint);

  void SetHostBridge(const winrt::IContentSiteBridge &bridge);

  void InitializeForCrossProc();

  void SetSystemBackdropController(const winrt::ISystemBackdropControllerWithTargets &backdropController);

  void SetBackroundOpacity(float backgroundOpacity);

  void SetColorIndex(std::uint32_t colorIndex);

  /*
  // IInputKeyboardSourcePreTranslateHandler methods
  IFACEMETHOD(OnDirectMessage)
  (IInputPreTranslateKeyboardSourceInterop *source, const MSG *msg, UINT keyboardModifiers, _Inout_ bool *handled);

  IFACEMETHOD(OnTreeMessage)
  (IInputPreTranslateKeyboardSourceInterop *source, const MSG *msg, UINT keyboardModifiers, _Inout_ bool *handled);
  */

 private:
  void Accessibility_Initialize();

  /*
void Accessibility_OnAutomationProviderRequested(
    const winrt::ContentIsland &island,
    const winrt::ContentIslandAutomationProviderRequestedEventArgs &args);
*/

  void CreateUIAProviderForVisual();

  void EnqueueFromBackgroundThread();

  void EvaluateUseSystemBackdrop();

  void EvaluateInputActivatePopup();

  void EvaluateLightDismissPopup();

  winrt::Visual HitTestVisual(winrt::float2 const point);

  void Input_Initialize();

  bool IsHostedByPopupWindowSiteBridge();

  bool Input_OnKeyDown(winrt::Windows::System::VirtualKey virtualKey);

  bool Input_AcceleratorKeyActivated(winrt::Windows::System::VirtualKey virtualKey);

  void Input_OnLeftButtonPressed(const winrt::Microsoft::UI::Input::PointerEventArgs &args);

  void Input_OnRightButtonPressed(const winrt::Microsoft::UI::Input::PointerEventArgs &args);

  void Input_OnPointerMoved(const winrt::Microsoft::UI::Input::PointerEventArgs &args);

  void Input_OnPointerReleased();

  void Island_OnStateChanged();

  void Island_OnConnected();

  void Island_OnDisconnected();

  void Island_OnClosed();

  void LightDismiss_Initialize();

  void OnLeftClick(const winrt::float2 point, bool controlPressed);

  void OnRightClick(const winrt::float2 point);

  void Output_Initialize();

  void Output_AddVisual(const winrt::float2 point, bool halfTransparent);

  void Output_UpdateCurrentColorVisual();

  void SystemBackdrop_Initialize();

  void SetLayoutDirectionForVisuals();

  void Window_Initialize();

  void Window_OnSettingChanged(const winrt::ContentEnvironmentSettingChangedEventArgs &args);
  void Window_OnThemeChanged();

  void Window_OnStateChanged(winrt::ContentIslandEnvironment const &sender);

 private:
  static inline winrt::Color s_colors[] = {
      {0xFF, 0x5B, 0x9B, 0xD5},
      {0xFF, 0xED, 0x7D, 0x31},
      {0xFF, 0x70, 0xAD, 0x47},
      {0xFF, 0xFF, 0xC0, 0x00},
      {0xFF, 0xFA, 0xEB, 0xD7},
      {0xFF, 0xFF, 0xFF, 0xFF},
      {0xFF, 0xFF, 0xFA, 0xFA},
      {0xFF, 0xFF, 0xC0, 0xCB},
      {0xFF, 0xB0, 0xE0, 0xE6},
      {0xFF, 0x98, 0xFB, 0x98},
      {0xFF, 0x87, 0xCE, 0xFA},
  };

  static inline std::wstring s_colorNames[] = {
      L"Blue",
      L"Orange",
      L"Green",
      L"Yellow",
      L"AntiqueWhite",
      L"White",
      L"Snow",
      L"Pink",
      L"PowderBlue",
      L"PaleGreen",
      L"LightSkyBlue",
  };

  // NodeSimpleFragmentFactory m_fragmentFactory;
  // winrt::com_ptr<IslandFragmentRoot> m_fragmentRoot{nullptr};
  winrt::Compositor m_compositor{nullptr};
  winrt::ContentIsland m_island{nullptr};
  winrt::InputKeyboardSource m_keyboardSource{nullptr};
  winrt::InputPreTranslateKeyboardSource m_pretranslateSource{nullptr};
  winrt::InputPointerSource m_pointerSource{nullptr};
  winrt::InputLightDismissAction m_lightDismissAction{nullptr};
  winrt::InputFocusController m_focusController{nullptr};

  // Background
  winrt::CompositionColorBrush m_backgroundBrushDefault{nullptr};
  winrt::CompositionColorBrush m_backgroundBrushA{nullptr};
  winrt::CompositionColorBrush m_backgroundBrushB{nullptr};
  winrt::CompositionColorBrush m_backgroundBrushC{nullptr};
  winrt::Microsoft::UI::Composition::SpriteVisual m_backgroundVisual{nullptr};
  winrt::RectangleClip m_backgroundClip{nullptr};
  winrt::RectangleClip m_backdropClip{nullptr};
#ifdef USE_EXPERIMENTAL_WINUI3
  winrt::Microsoft::UI::Content::ContentExternalBackdropLink m_backdropLink{nullptr};
#endif
  winrt::ICompositionSupportsSystemBackdrop m_backdropTarget{nullptr};

  // Drawing squares
  winrt::Microsoft::UI::Composition::VisualCollection m_visuals{nullptr};
  winrt::Microsoft::UI::Composition::Visual m_selectedVisual{nullptr};
  winrt::Microsoft::UI::Composition::SpriteVisual m_currentColorVisual{nullptr};
  winrt::float2 m_offset{};
  float m_backgroundOpacity = 0.5f;

  unsigned int m_currentColorIndex = 0;
  winrt::CompositionColorBrush m_colorBrushes[_countof(
      s_colors)]{nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr};
  winrt::CompositionColorBrush m_halfTransparentColorBrushes[_countof(
      s_colors)]{nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr};

  // Popups
  // https://task.ms/32440118: Add ContentIsland.SiteBridge to avoid this workaround
  winrt::IContentSiteBridge m_siteBridge{nullptr};
  winrt::DesktopPopupSiteBridge m_popupSiteBridge{nullptr};

  // SystemBackdrops for Popups
  winrt::ISystemBackdropControllerWithTargets m_backdropController{nullptr};
  winrt::SystemBackdropConfiguration m_backdropConfiguration{nullptr};

  boolean m_ignoreLeftButtonPressed = false;
  boolean m_useSystemBackdrop = false;
  boolean m_inputActivatePopup = true;
  boolean m_lightDismissPopup = false;
  float m_prevRasterizationScale = 0;
  winrt::ContentLayoutDirection m_prevLayout = winrt::ContentLayoutDirection::LeftToRight;

  boolean m_crossProcUIA = false;
};

} // namespace winrt::SampleCustomComponent::implementation

namespace winrt::SampleCustomComponent::factory_implementation {
struct DrawingIsland : DrawingIslandT<DrawingIsland, implementation::DrawingIsland> {};
} // namespace winrt::SampleCustomComponent::factory_implementation

void RegisterDrawingIslandComponentView(winrt::Microsoft::ReactNative::IReactPackageBuilder const &packageBuilder);

#endif // #ifdef RNW_NEW_ARCH
