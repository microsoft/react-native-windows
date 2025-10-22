// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include "WindowsTextInputComponentView.h"

#include <AutoDraw.h>
#include <Fabric/Composition/CompositionDynamicAutomationProvider.h>
#include <Fabric/Composition/UiaHelpers.h>
#include <Fabric/platform/react/renderer/graphics/PlatformColorUtils.h>
#include <Utils/ThemeUtils.h>
#include <Utils/ValueUtils.h>
#include <react/renderer/components/textinput/TextInputState.h>
#include <react/renderer/textlayoutmanager/WindowsTextLayoutManager.h>
#include <react/renderer/attributedstring/AttributedString.h>
#include <react/renderer/attributedstring/AttributedStringBox.h>
#include <react/renderer/components/textinput/TextInputState.h>
#include <react/renderer/textlayoutmanager/WindowsTextLayoutManager.h>
#include <tom.h>
#include <unicode.h>
#include <winrt/Microsoft.UI.Input.h>
#include <winrt/Windows.System.h>
#include <winrt/Windows.UI.h>
#include "../Composition.Input.h"
#include "../CompositionHelpers.h"
#include "../RootComponentView.h"
#include "JSValueReader.h"
#include "WindowsTextInputShadowNode.h"
#include "guid/msoGuid.h"

#include <unicode.h>

// convert a BSTR to a std::string.
std::string &BstrToStdString(const BSTR bstr, std::string &dst, int cp = CP_UTF8) {
  if (!bstr) {
    // define NULL functionality. I just clear the target.
    dst.clear();
    return dst;
  }

  // request content length in single-chars through a terminating
  //  nullchar in the BSTR. note: BSTR's support imbedded nullchars,
  //  so this will only convert through the first nullchar.
  int res = WideCharToMultiByte(cp, 0, bstr, -1, NULL, 0, NULL, NULL);
  if (res > 0) {
    dst.resize(res);
    WideCharToMultiByte(cp, 0, bstr, -1, &dst[0], res, NULL, NULL);
  } else { // no content. clear target
    dst.clear();
  }
  return dst;
}

// conversion with temp.
std::string BstrToStdString(BSTR bstr, int cp = CP_UTF8) {
  std::string str;
  BstrToStdString(bstr, str, cp);
  return str;
}

MSO_CLASS_GUID(ITextHost, "13E670F4-1A5A-11cf-ABEB-00AA00B65EA1") // IID_ITextHost
MSO_CLASS_GUID(ITextServices, "8D33F740-CF58-11CE-A89D-00AA006CADC5") // IID_ITextServices
MSO_CLASS_GUID(ITextServices2, "8D33F741-CF58-11CE-A89D-00AA006CADC5") // IID_ITextServices2

namespace winrt::Microsoft::ReactNative::Composition::implementation {

// RichEdit doesn't handle us calling Draw during the middle of a TxTranslateMessage call.
WindowsTextInputComponentView::DrawBlock::DrawBlock(WindowsTextInputComponentView &view) : m_view(view) {
  m_view.m_cDrawBlock++;
}

WindowsTextInputComponentView::DrawBlock::~DrawBlock() {
  m_view.m_cDrawBlock--;
  if (!m_view.m_cDrawBlock && m_view.m_needsRedraw) {
    m_view.DrawText();
  }
}

// 	Msftedit.dll vs "Riched20.dll"?

static HINSTANCE g_hInstRichEdit = nullptr;
static PCreateTextServices g_pfnCreateTextServices;

HRESULT HrEnsureRichEd20Loaded() noexcept {
  if (g_hInstRichEdit == nullptr) {
    g_hInstRichEdit = LoadLibrary(L"Msftedit.dll");
    if (!g_hInstRichEdit)
      return E_FAIL;

    // Create the windowless control (text services object)
    g_pfnCreateTextServices = (PCreateTextServices)GetProcAddress(g_hInstRichEdit, "CreateTextServices");
    if (!g_pfnCreateTextServices)
      return E_FAIL;

    /*
    // Calling the REExtendedRegisterClass() function is required for
    // registering the REComboboxW and REListBoxW window classes.
    PFNREGISTER pfnRegister = (PFNREGISTER)GetProcAddress(g_hInstRichEdit, "REExtendedRegisterClass");
    if (pfnRegister) {
      pfnRegister();
      return S_OK;
    } else
      return E_FAIL;
      */
  }
  return NOERROR;
}

struct CompTextHost : public winrt::implements<CompTextHost, ITextHost> {
  CompTextHost(WindowsTextInputComponentView *outer) : m_outer(outer) {}

  //@cmember Get the DC for the host
  HDC TxGetDC() override {
    assert(false);
    return {};
  }

  //@cmember Release the DC gotten from the host
  INT TxReleaseDC(HDC hdc) override {
    assert(false);
    return {};
  }

  //@cmember Show the scroll bar
  BOOL TxShowScrollBar(INT fnBar, BOOL fShow) override {
    // assert(false);
    return {};
  }

  //@cmember Enable the scroll bar
  BOOL TxEnableScrollBar(INT fuSBFlags, INT fuArrowflags) override {
    // assert(false);
    return {};
  }

  //@cmember Set the scroll range
  BOOL TxSetScrollRange(INT fnBar, LONG nMinPos, INT nMaxPos, BOOL fRedraw) override {
    // assert(false);
    return {};
  }

  //@cmember Set the scroll position
  BOOL TxSetScrollPos(INT fnBar, INT nPos, BOOL fRedraw) override {
    // assert(false);
    return {};
  }

  //@cmember InvalidateRect
  void TxInvalidateRect(LPCRECT prc, BOOL fMode) override {
    if (m_outer->m_drawing)
      return;
    m_outer->DrawText();
  }

  //@cmember Send a WM_PAINT to the window
  void TxViewChange(BOOL fUpdate) override {
    // When keyboard scrolling without scrollbar, TxInvalidateRect is not called.
    // Instead TxViewChange is called with fUpdate = true
    // if (fUpdate && !OnInnerViewerExtentChanged())
    {
      // If inner viewer size changed, a redraw will have be queued.
      // If not, we need to redraw at least once here.
      m_outer->DrawText();
    }
  }

  //@cmember Create the caret
  BOOL TxCreateCaret(HBITMAP hbmp, INT xWidth, INT yHeight) override {
    m_outer->m_caretVisual.Size({static_cast<float>(xWidth), static_cast<float>(yHeight)});
    return true;
  }

  //@cmember Show the caret
  BOOL TxShowCaret(BOOL fShow) override {
    // Only show the caret if we have focus
    if (fShow && !m_outer->m_hasFocus) {
      return false;
    }
    m_outer->ShowCaret(m_outer->windowsTextInputProps().caretHidden ? false : fShow);
    return true;
  }

  //@cmember Set the caret position
  BOOL TxSetCaretPos(INT x, INT y) override {
    if (x < 0 && y < 0) {
      // RichEdit sends (-32000,-32000) when the caret is not currently visible.
      return false;
    }

    auto pt = m_outer->getClientOffset();
    m_outer->m_caretVisual.Position({x - pt.x, y - pt.y});
    return true;
  }

  //@cmember Create a timer with the specified timeout
  BOOL TxSetTimer(UINT idTimer, UINT uTimeout) override {
    // TODO timers
    // assert(false);
    // return {};
    return false;
  }

  //@cmember Destroy a timer
  void TxKillTimer(UINT idTimer) override {
    // TODO timers
  }

  //@cmember Scroll the content of the specified window's client area
  void TxScrollWindowEx(
      INT dx,
      INT dy,
      LPCRECT lprcScroll,
      LPCRECT lprcClip,
      HRGN hrgnUpdate,
      LPRECT lprcUpdate,
      UINT fuScroll) override {
    assert(false);
  }

  //@cmember Get mouse capture
  void TxSetCapture(BOOL fCapture) override {
    auto mousePointer = winrt::make<winrt::Microsoft::ReactNative::Composition::Input::implementation::Pointer>(
        winrt::Microsoft::ReactNative::Composition::Input::PointerDeviceType::Mouse, 1 /* 1 is Mouse PointerId*/);

    if (fCapture) {
      m_outer->CapturePointer(mousePointer);
    } else {
      m_outer->ReleasePointerCapture(mousePointer);
    }
  }

  //@cmember Set the focus to the text window
  void TxSetFocus() override {
    winrt::Microsoft::ReactNative::ComponentView view{nullptr};
    winrt::check_hresult(
        m_outer->QueryInterface(winrt::guid_of<winrt::Microsoft::ReactNative::ComponentView>(), winrt::put_abi(view)));
    m_outer->rootComponentView()->TrySetFocusedComponent(
        view, winrt::Microsoft::ReactNative::FocusNavigationDirection::None);
    // assert(false);
    // TODO focus
  }

  //@cmember Establish a new cursor shape
  void TxSetCursor(HCURSOR hcur, BOOL fText) override {
    m_outer->m_hcursor = hcur;
  }

  //@cmember Converts screen coordinates of a specified point to the client coordinates
  BOOL TxScreenToClient(LPPOINT lppt) override {
    winrt::Windows::Foundation::Point pt{static_cast<float>(lppt->x), static_cast<float>(lppt->y)};
    auto localpt = m_outer->ScreenToLocal(pt);
    lppt->x = static_cast<LONG>(localpt.X);
    lppt->y = static_cast<LONG>(localpt.Y);
    return true;
  }

  //@cmember Converts the client coordinates of a specified point to screen coordinates
  BOOL TxClientToScreen(LPPOINT lppt) override {
    winrt::Windows::Foundation::Point pt{static_cast<float>(lppt->x), static_cast<float>(lppt->y)};
    auto screenpt = m_outer->LocalToScreen(pt);
    lppt->x = static_cast<LONG>(screenpt.X);
    lppt->y = static_cast<LONG>(screenpt.Y);
    return true;
  }

  //@cmember Request host to activate text services
  HRESULT TxActivate(LONG *plOldState) override {
    assert(false);
    return {};
  }

  //@cmember Request host to deactivate text services
  HRESULT TxDeactivate(LONG lNewState) override {
    assert(false);
    return {};
  }

  //@cmember Retrieves the coordinates of a window's client area
  HRESULT TxGetClientRect(LPRECT prc) override {
    *prc = m_outer->getClientRect();
    return S_OK;
  }

  //@cmember Get the view rectangle relative to the inset
  HRESULT TxGetViewInset(LPRECT prc) override {
    // Inset is in HIMETRIC
    constexpr float HmPerInchF = 2540.0f;
    constexpr float PointsPerInch = 96.0f;
    constexpr float dipToHm = HmPerInchF / PointsPerInch;

    prc->left = static_cast<LONG>(m_outer->m_layoutMetrics.contentInsets.left * dipToHm);
    prc->top = static_cast<LONG>(m_outer->m_layoutMetrics.contentInsets.top * dipToHm);
    prc->bottom = static_cast<LONG>(m_outer->m_layoutMetrics.contentInsets.bottom * dipToHm);
    prc->right = static_cast<LONG>(m_outer->m_layoutMetrics.contentInsets.right * dipToHm);

    return NOERROR;
  }

  //@cmember Get the default character format for the text
  HRESULT TxGetCharFormat(const CHARFORMATW **ppCF) override {
    m_outer->UpdateCharFormat();

    *ppCF = &(m_outer->m_cf);
    return S_OK;
  }

  //@cmember Get the default paragraph format for the text
  HRESULT TxGetParaFormat(const PARAFORMAT **ppPF) override {
    m_outer->UpdateParaFormat();

    *ppPF = &(m_outer->m_pf);
    return S_OK;
  }

  //@cmember Get the background color for the window
  COLORREF TxGetSysColor(int nIndex) override {
    // if (/* !m_isDisabled || */ nIndex != COLOR_WINDOW && nIndex != COLOR_WINDOWTEXT && nIndex != COLOR_GRAYTEXT) {
    // This window is either not disabled or the color isn't interesting
    // in the disabled case.
    COLORREF cr = (COLORREF)tomAutoColor;

    switch (nIndex) {
      case COLOR_WINDOWTEXT:
        if (m_outer->windowsTextInputProps().textAttributes.foregroundColor)
          return (*m_outer->windowsTextInputProps().textAttributes.foregroundColor).AsColorRefNoAlpha();
        // cr = 0x000000FF;
        break;
      case COLOR_WINDOW:
        if (m_outer->viewProps()->backgroundColor)
          return (*m_outer->viewProps()->backgroundColor).AsColorRefNoAlpha();
        break;

      case COLOR_HIGHLIGHT:
        if (m_outer->windowsTextInputProps().selectionColor)
          return (*m_outer->windowsTextInputProps().selectionColor).AsColorRefNoAlpha();
        break;

      case COLOR_HIGHLIGHTTEXT:
        // For selected text color, we use the same color as the selection background
        // or the text color if selection color is not specified
        if (m_outer->windowsTextInputProps().selectionColor) {
          // Calculate appropriate text color based on selection background
          auto selectionColor = (*m_outer->windowsTextInputProps().selectionColor).AsColorRefNoAlpha();
          // Use white text for dark selection, black text for light selection
          int r = GetRValue(selectionColor);
          int g = GetGValue(selectionColor);
          int b = GetBValue(selectionColor);
          int brightness = ::Microsoft::ReactNative::CalculateColorBrightness(r, g, b);
          return brightness > ::Microsoft::ReactNative::kCaretSelectionBrightnessThreshold ? RGB(0, 0, 0)
                                                                                           : RGB(255, 255, 255);
        }
        break;

        // case COLOR_GRAYTEXT:
        // cr = RGB(128, 128, 128);
        // cr = 0x777777FF;
        //          break;
    }
    return GetSysColor(nIndex);

    // return GetSysColor(nIndex);
    // assert(false);
    // return 0xFF00FF00;

    /*
    // Disabled case. When the richedit control is disabled, both the placeholder text and input text should appear as
    // disabled text.
    if (COLOR_WINDOWTEXT == nIndex || COLOR_GRAYTEXT == nIndex) {
      // Color of text for disabled window
      return m_crDisabledText == (COLORREF)tomAutoColor ? MsoCrSysColorGet(COLOR_GRAYTEXT) : m_crDisabledText;
    }

    // Background color for disabled window
    return m_crDisabledBackground == (COLORREF)tomAutoColor ? MsoCrSysColorGet(COLOR_3DFACE) : m_crDisabledBackground;
    */
  }

  //@cmember Get the background (either opaque or transparent)
  HRESULT TxGetBackStyle(TXTBACKSTYLE *pstyle) override {
    // We draw the background color as part of the composition visual, not the text
    *pstyle = TXTBACK_TRANSPARENT;
    return S_OK;
  }

  //@cmember Get the maximum length for the text
  HRESULT TxGetMaxLength(DWORD *plength) override {
    auto length = m_outer->windowsTextInputProps().maxLength;
    if (length > static_cast<decltype(m_outer->windowsTextInputProps().maxLength)>(std::numeric_limits<DWORD>::max())) {
      length = std::numeric_limits<DWORD>::max();
    }
    *plength = static_cast<DWORD>(length);
    return S_OK;
  }

  //@cmember Get the bits representing requested scroll bars for the window
  HRESULT TxGetScrollBars(DWORD *pdwScrollBar) override {
    if (m_outer->m_multiline) {
      *pdwScrollBar = WS_VSCROLL | WS_HSCROLL | ES_AUTOVSCROLL | ES_AUTOHSCROLL;
    } else {
      *pdwScrollBar = WS_HSCROLL | ES_AUTOHSCROLL;
    }
    return S_OK;
  }

  //@cmember Get the character to display for password input
  HRESULT TxGetPasswordChar(_Out_ wchar_t *pch) override {
    *pch = L'\u2022';
    return S_OK;
  }

  //@cmember Get the accelerator character
  HRESULT TxGetAcceleratorPos(LONG *pcp) override {
    assert(false);
    return {};
  }

  //@cmember Get the native size
  HRESULT TxGetExtent(LPSIZEL lpExtent) override {
    return E_NOTIMPL;
    // This shouldn't be implemented
  }

  //@cmember Notify host that default character format has changed
  HRESULT OnTxCharFormatChange(const CHARFORMATW *pCF) override {
    assert(false);
    return {};
  }

  //@cmember Notify host that default paragraph format has changed
  HRESULT OnTxParaFormatChange(const PARAFORMAT *pPF) override {
    assert(false);
    return {};
  }

  //@cmember Bulk access to bit properties
  HRESULT TxGetPropertyBits(DWORD dwMask, DWORD *pdwBits) override {
    DWORD dwProperties = TXTBIT_RICHTEXT | TXTBIT_WORDWRAP | TXTBIT_D2DDWRITE | TXTBIT_D2DSIMPLETYPOGRAPHY;
    *pdwBits = dwProperties & dwMask;
    return NOERROR;
  }

  //@cmember Notify host of events
  HRESULT TxNotify(DWORD iNotify, void *pv) override {
    // TODO

    switch (iNotify) {
      case EN_UPDATE:
        if (!m_outer->m_drawing) {
          m_outer->DrawText();
        }
        break;
      case EN_CHANGE:
        m_outer->OnTextUpdated();
        break;
      case EN_SELCHANGE: {
        auto selChange = (SELCHANGE *)pv;
        m_outer->OnSelectionChanged(selChange->chrg.cpMin, selChange->chrg.cpMax);
        break;
      }
    }

    return S_OK;
  }

  // East Asia Methods for getting the Input Context
  HIMC TxImmGetContext() override {
    assert(false);
    return {};
  }
  void TxImmReleaseContext(HIMC himc) override {
    assert(false);
  }

  //@cmember Returns HIMETRIC size of the control bar.
  HRESULT TxGetSelectionBarWidth(LONG *lSelBarWidth) override {
    *lSelBarWidth = 0;
    return S_OK;
  }

  WindowsTextInputComponentView *m_outer;
};

int WINAPI
AutoCorrectOffCallback(LANGID langid, const WCHAR *pszBefore, WCHAR *pszAfter, LONG cchAfter, LONG *pcchReplaced) {
  wcsncpy_s(pszAfter, cchAfter, pszBefore, _TRUNCATE);
  *pcchReplaced = static_cast<LONG>(wcslen(pszAfter));
  return ATP_CHANGE;
}

facebook::react::AttributedString WindowsTextInputComponentView::getAttributedString() const {
  // Use BaseTextShadowNode to get attributed string from children

  auto childTextAttributes = facebook::react::TextAttributes::defaultTextAttributes();
  childTextAttributes.fontSizeMultiplier = m_fontSizeMultiplier;

  childTextAttributes.apply(windowsTextInputProps().textAttributes);

  auto attributedString = facebook::react::AttributedString{};
  // auto attachments = facebook::react::BaseTextShadowNode::Attachments{};

  // BaseTextShadowNode only gets children. We must detect and prepend text
  // value attributes manually.
  auto text = GetTextFromRichEdit();
  if (!text.empty()) {
    auto textAttributes = facebook::react::TextAttributes::defaultTextAttributes();
    textAttributes.fontSizeMultiplier = m_fontSizeMultiplier;
    textAttributes.apply(windowsTextInputProps().textAttributes);
    auto fragment = facebook::react::AttributedString::Fragment{};
    fragment.string = text;
    // fragment.string = m_props->text;
    fragment.textAttributes = textAttributes;
    // If the TextInput opacity is 0 < n < 1, the opacity of the TextInput and
    // text value's background will stack. This is a hack/workaround to prevent
    // that effect.
    fragment.textAttributes.backgroundColor = facebook::react::clearColor();
    // fragment.parentShadowView = facebook::react::ShadowView(*this);
    attributedString.prependFragment(std::move(fragment));
  }

  return attributedString;
}

WindowsTextInputComponentView::WindowsTextInputComponentView(
    const winrt::Microsoft::ReactNative::Composition::Experimental::ICompositionContext &compContext,
    facebook::react::Tag tag,
    winrt::Microsoft::ReactNative::ReactContext const &reactContext)
    : Super(
          WindowsTextInputComponentView::defaultProps(),
          compContext,
          tag,
          reactContext,
          ComponentViewFeatures::Default & ~ComponentViewFeatures::Background) {}

void WindowsTextInputComponentView::HandleCommand(
    const winrt::Microsoft::ReactNative::HandleCommandArgs &args) noexcept {
  Super::HandleCommand(args);
  if (args.Handled())
    return;

  auto commandName = args.CommandName();
  if (commandName == L"setTextAndSelection") {
    int eventCount, begin, end;
    std::optional<winrt::hstring> text;

    winrt::Microsoft::ReactNative::ReadArgs(args.CommandArgs(), eventCount, text, begin, end);
    // Allow text updates that are very close to the current native event count
    // This handles the case where JavaScript immediately calls setValue during onSubmitEditing
    // In that case, eventCount might be one less than m_nativeEventCount due to timing
    if (eventCount >= m_nativeEventCount - 1) {
      m_comingFromJS = true;
      {
        if (text.has_value()) {
          DrawBlock db(*this);
          UpdateText(winrt::to_string(text.value()));
        }

        SELCHANGE sc;
        memset(&sc, 0, sizeof(sc));
        sc.chrg.cpMin = static_cast<LONG>(begin);
        sc.chrg.cpMax = static_cast<LONG>(end);
        sc.seltyp = (begin == end) ? SEL_EMPTY : SEL_TEXT;

        LRESULT res;
        winrt::check_hresult(
            m_textServices->TxSendMessage(EM_SETSEL, static_cast<WPARAM>(begin), static_cast<LPARAM>(end), &res));
      }

      m_comingFromJS = false;
    }
  }
}

WPARAM PointerPointToPointerWParam(const winrt::Microsoft::ReactNative::Composition::Input::PointerPoint &pp) noexcept {
  WPARAM wParam = pp.PointerId();
  wParam |= (POINTER_MESSAGE_FLAG_NEW << 16);
  auto ppp = pp.Properties();
  if (ppp.IsInRange()) {
    wParam |= (POINTER_MESSAGE_FLAG_INRANGE << 16);
  }
  if (pp.IsInContact()) {
    wParam |= (POINTER_MESSAGE_FLAG_INCONTACT << 16);
  }
  if (ppp.IsLeftButtonPressed()) {
    wParam |= (POINTER_MESSAGE_FLAG_FIRSTBUTTON << 16);
  }
  if (ppp.IsRightButtonPressed()) {
    wParam |= (POINTER_MESSAGE_FLAG_SECONDBUTTON << 16);
  }
  if (ppp.IsMiddleButtonPressed()) {
    wParam |= (POINTER_MESSAGE_FLAG_THIRDBUTTON << 16);
  }
  if (ppp.IsXButton1Pressed()) {
    wParam |= (POINTER_MESSAGE_FLAG_FOURTHBUTTON << 16);
  }
  if (ppp.IsXButton2Pressed()) {
    wParam |= (POINTER_MESSAGE_FLAG_FIFTHBUTTON << 16);
  }
  if (ppp.IsPrimary()) {
    wParam |= (POINTER_MESSAGE_FLAG_PRIMARY << 16);
  }
  if (ppp.TouchConfidence()) {
    wParam |= (POINTER_MESSAGE_FLAG_CONFIDENCE << 16);
  }
  if (ppp.IsCanceled()) {
    wParam |= (POINTER_MESSAGE_FLAG_CANCELED << 16);
  }
  return wParam;
}

WPARAM PointerRoutedEventArgsToMouseWParam(
    const winrt::Microsoft::ReactNative::Composition::Input::PointerRoutedEventArgs &args) noexcept {
  WPARAM wParam = 0;
  auto pp = args.GetCurrentPoint(-1);

  auto keyModifiers = args.KeyModifiers();
  if ((keyModifiers & winrt::Windows::System::VirtualKeyModifiers::Control) ==
      winrt::Windows::System::VirtualKeyModifiers::Control) {
    wParam |= MK_CONTROL;
  }
  if ((keyModifiers & winrt::Windows::System::VirtualKeyModifiers::Shift) ==
      winrt::Windows::System::VirtualKeyModifiers::Shift) {
    wParam |= MK_SHIFT;
  }

  auto ppp = pp.Properties();
  if (ppp.IsLeftButtonPressed()) {
    wParam |= MK_LBUTTON;
  }
  if (ppp.IsMiddleButtonPressed()) {
    wParam |= MK_MBUTTON;
  }
  if (ppp.IsRightButtonPressed()) {
    wParam |= MK_RBUTTON;
  }
  if (ppp.IsXButton1Pressed()) {
    wParam |= MK_XBUTTON1;
  }
  if (ppp.IsXButton2Pressed()) {
    wParam |= MK_XBUTTON2;
  }
  return wParam;
}

bool WindowsTextInputComponentView::IsDoubleClick() {
  using namespace std::chrono;

  auto now = steady_clock::now();
  auto duration = duration_cast<milliseconds>(now - m_lastClickTime).count();

  const int DOUBLE_CLICK_TIME_MS = ::GetDoubleClickTime();

  m_lastClickTime = now;

  return (duration < DOUBLE_CLICK_TIME_MS);
}

void WindowsTextInputComponentView::OnPointerPressed(
    const winrt::Microsoft::ReactNative::Composition::Input::PointerRoutedEventArgs &args) noexcept {
  UINT msg = 0;
  LPARAM lParam = 0;
  WPARAM wParam = 0;

  auto pp = args.GetCurrentPoint(-1); // TODO use local coords?
  auto position = pp.Position();
  POINT ptContainer = {
      static_cast<LONG>(position.X * m_layoutMetrics.pointScaleFactor),
      static_cast<LONG>(position.Y * m_layoutMetrics.pointScaleFactor)};
  lParam = static_cast<LPARAM>(POINTTOPOINTS(ptContainer));

  if (pp.PointerDeviceType() == winrt::Microsoft::ReactNative::Composition::Input::PointerDeviceType::Mouse) {
    switch (pp.Properties().PointerUpdateKind()) {
      case winrt::Microsoft::ReactNative::Composition::Input::PointerUpdateKind::LeftButtonPressed:
        if (IsDoubleClick()) {
          msg = WM_LBUTTONDBLCLK;
        } else {
          msg = WM_LBUTTONDOWN;
        }
        break;
      case winrt::Microsoft::ReactNative::Composition::Input::PointerUpdateKind::MiddleButtonPressed:
        msg = WM_MBUTTONDOWN;
        break;
      case winrt::Microsoft::ReactNative::Composition::Input::PointerUpdateKind::RightButtonPressed:
        msg = WM_RBUTTONDOWN;
        break;
      case winrt::Microsoft::ReactNative::Composition::Input::PointerUpdateKind::XButton1Pressed:
        msg = WM_XBUTTONDOWN;
        wParam |= (XBUTTON1 << 16);
        break;
      case winrt::Microsoft::ReactNative::Composition::Input::PointerUpdateKind::XButton2Pressed:
        msg = WM_XBUTTONDOWN;
        wParam |= (XBUTTON2 << 16);
        break;
    }
    wParam = PointerRoutedEventArgsToMouseWParam(args);
  } else {
    msg = WM_POINTERDOWN;
    wParam = PointerPointToPointerWParam(pp);
  }

  if (m_textServices && msg) {
    LRESULT lresult;
    DrawBlock db(*this);
    auto hr = m_textServices->TxSendMessage(msg, static_cast<WPARAM>(wParam), static_cast<LPARAM>(lParam), &lresult);
    args.Handled(hr != S_FALSE);
  }

  // Emits the OnPressIn event
  if (m_eventEmitter && !m_comingFromJS) {
    auto emitter = std::static_pointer_cast<const facebook::react::WindowsTextInputEventEmitter>(m_eventEmitter);
    float offsetX = position.X - m_layoutMetrics.frame.origin.x;
    float offsetY = position.Y - m_layoutMetrics.frame.origin.y;
    float neutralX = m_layoutMetrics.frame.origin.x;
    float neutralY = m_layoutMetrics.frame.origin.y;

    facebook::react::GestureResponderEvent pressInArgs;
    pressInArgs.target = m_tag;
    pressInArgs.pagePoint = {position.X, position.Y};
    pressInArgs.offsetPoint = {offsetX, offsetY}; //{LocationX,LocationY}
    pressInArgs.timestamp = static_cast<double>(pp.Timestamp()) / 1000.0;
    pressInArgs.identifier = pp.PointerId();

    emitter->onPressIn(pressInArgs);
  }
}

void WindowsTextInputComponentView::OnPointerReleased(
    const winrt::Microsoft::ReactNative::Composition::Input::PointerRoutedEventArgs &args) noexcept {
  UINT msg = 0;
  LPARAM lParam = 0;
  WPARAM wParam = 0;

  auto pp = args.GetCurrentPoint(-1);
  auto position = pp.Position();
  POINT ptContainer = {
      static_cast<LONG>(position.X * m_layoutMetrics.pointScaleFactor),
      static_cast<LONG>(position.Y * m_layoutMetrics.pointScaleFactor)};
  lParam = static_cast<LPARAM>(POINTTOPOINTS(ptContainer));

  if (pp.PointerDeviceType() == winrt::Microsoft::ReactNative::Composition::Input::PointerDeviceType::Mouse) {
    switch (pp.Properties().PointerUpdateKind()) {
      case winrt::Microsoft::ReactNative::Composition::Input::PointerUpdateKind::LeftButtonReleased:
        msg = WM_LBUTTONUP;
        break;
      case winrt::Microsoft::ReactNative::Composition::Input::PointerUpdateKind::MiddleButtonReleased:
        msg = WM_MBUTTONUP;
        break;
      case winrt::Microsoft::ReactNative::Composition::Input::PointerUpdateKind::RightButtonReleased:
        msg = WM_RBUTTONUP;
        break;
      case winrt::Microsoft::ReactNative::Composition::Input::PointerUpdateKind::XButton1Released:
        msg = WM_XBUTTONUP;
        wParam |= (XBUTTON1 << 16);
        break;
      case winrt::Microsoft::ReactNative::Composition::Input::PointerUpdateKind::XButton2Released:
        msg = WM_XBUTTONUP;
        wParam |= (XBUTTON2 << 16);
        break;
    }
    wParam = PointerRoutedEventArgsToMouseWParam(args);
  } else {
    msg = WM_POINTERUP;
    wParam = PointerPointToPointerWParam(pp);
  }

  if (m_textServices && msg) {
    LRESULT lresult;
    DrawBlock db(*this);
    auto hr = m_textServices->TxSendMessage(msg, static_cast<WPARAM>(wParam), static_cast<LPARAM>(lParam), &lresult);
    args.Handled(hr != S_FALSE);
  }

  // Emits the OnPressOut event
  if (m_eventEmitter && !m_comingFromJS) {
    auto emitter = std::static_pointer_cast<const facebook::react::WindowsTextInputEventEmitter>(m_eventEmitter);
    float offsetX = position.X - m_layoutMetrics.frame.origin.x;
    float offsetY = position.Y - m_layoutMetrics.frame.origin.y;
    float neutralX = m_layoutMetrics.frame.origin.x;
    float neutralY = m_layoutMetrics.frame.origin.y;

    facebook::react::GestureResponderEvent pressOutArgs;
    pressOutArgs.target = m_tag;
    pressOutArgs.pagePoint = {position.X, position.Y};
    pressOutArgs.offsetPoint = {offsetX, offsetY}; //{LocationX,LocationY}
    pressOutArgs.timestamp = static_cast<double>(pp.Timestamp()) / 1000.0;
    pressOutArgs.identifier = pp.PointerId();

    emitter->onPressOut(pressOutArgs);
  }
}

void WindowsTextInputComponentView::OnPointerMoved(
    const winrt::Microsoft::ReactNative::Composition::Input::PointerRoutedEventArgs &args) noexcept {
  UINT msg = 0;
  LPARAM lParam = 0;
  WPARAM wParam = 0;

  auto pp = args.GetCurrentPoint(-1);
  auto position = pp.Position();
  POINT ptContainer = {
      static_cast<LONG>(position.X * m_layoutMetrics.pointScaleFactor),
      static_cast<LONG>(position.Y * m_layoutMetrics.pointScaleFactor)};
  lParam = static_cast<LPARAM>(POINTTOPOINTS(ptContainer));

  if (pp.PointerDeviceType() == winrt::Microsoft::ReactNative::Composition::Input::PointerDeviceType::Mouse) {
    msg = WM_MOUSEMOVE;
    wParam = PointerRoutedEventArgsToMouseWParam(args);
  } else {
    msg = WM_POINTERUPDATE;
    wParam = PointerPointToPointerWParam(pp);
  }

  if (m_textServices) {
    LRESULT lresult;
    DrawBlock db(*this);
    auto hr = m_textServices->TxSendMessage(msg, static_cast<WPARAM>(wParam), static_cast<LPARAM>(lParam), &lresult);
    args.Handled(hr != S_FALSE);
  }

  m_textServices->OnTxSetCursor(
      DVASPECT_CONTENT, -1, nullptr, nullptr, nullptr, nullptr, nullptr, ptContainer.x, ptContainer.y);
}

void WindowsTextInputComponentView::OnPointerWheelChanged(
    const winrt::Microsoft::ReactNative::Composition::Input::PointerRoutedEventArgs &args) noexcept {
  if (windowsTextInputProps().scrollEnabled && windowsTextInputProps().multiline) {
    auto pointerPointProperties = args.GetCurrentPoint(-1).Properties();

    auto delta = static_cast<float>(pointerPointProperties.MouseWheelDelta());

    if (m_textServices && !pointerPointProperties.IsHorizontalMouseWheel()) {
      // Vertical scrolling
      if (delta > 0) {
        m_textServices->TxSendMessage(WM_VSCROLL, SB_LINEUP, 0, nullptr);
      } else {
        m_textServices->TxSendMessage(WM_VSCROLL, SB_LINEDOWN, 0, nullptr);
      }
      args.Handled(true);
    }
    // Emit the onScroll event
    EmitOnScrollEvent();

    // We don't support horizontal scrolling yet cause it is not implemented in Android/IOS
  }
  Super::OnPointerWheelChanged(args);
}
void WindowsTextInputComponentView::OnKeyDown(
    const winrt::Microsoft::ReactNative::Composition::Input::KeyRoutedEventArgs &args) noexcept {
  // Do not forward tab keys into the TextInput, since we want that to do the tab loop instead.  This aligns with WinUI
  // behavior We do forward Ctrl+Tab to the textinput.
  if (args.Key() != winrt::Windows::System::VirtualKey::Tab ||
      (args.KeyboardSource().GetKeyState(winrt::Windows::System::VirtualKey::Control) &
       winrt::Microsoft::UI::Input::VirtualKeyStates::Down) == winrt::Microsoft::UI::Input::VirtualKeyStates::Down) {
    WPARAM wParam = static_cast<WPARAM>(args.Key());
    LPARAM lParam = 0;
    lParam = args.KeyStatus().RepeatCount; // bits 0-15
    lParam |= args.KeyStatus().ScanCode << 16; // bits 16-23
    if (args.KeyStatus().IsExtendedKey)
      lParam |= 0x01000000; // bit 24
    // if sysKey - bit 29 = 1, otherwise 0
    if (args.KeyStatus().WasKeyDown)
      lParam |= 0x40000000; // bit 30

    LRESULT lresult;
    DrawBlock db(*this);
    auto hr = m_textServices->TxSendMessage(
        args.KeyStatus().IsMenuKeyDown ? WM_SYSKEYDOWN : WM_KEYDOWN, wParam, lParam, &lresult);
    if (hr == S_OK) { // S_FALSE or S_MSG_KEY_IGNORED means RichEdit didn't handle the key
      args.Handled(true);
    }
  }
  Super::OnKeyDown(args);
}

void WindowsTextInputComponentView::OnKeyUp(
    const winrt::Microsoft::ReactNative::Composition::Input::KeyRoutedEventArgs &args) noexcept {
  // Do not forward tab keys into the TextInput, since we want that to do the tab loop instead.  This aligns with WinUI
  // behavior We do forward Ctrl+Tab to the textinput.
  if (args.Key() != winrt::Windows::System::VirtualKey::Tab ||
      (args.KeyboardSource().GetKeyState(winrt::Windows::System::VirtualKey::Control) &
       winrt::Microsoft::UI::Input::VirtualKeyStates::Down) == winrt::Microsoft::UI::Input::VirtualKeyStates::Down) {
    WPARAM wParam = static_cast<WPARAM>(args.Key());
    LPARAM lParam = 1;
    lParam = args.KeyStatus().RepeatCount; // bits 0-15
    lParam |= args.KeyStatus().ScanCode << 16; // bits 16-23
    if (args.KeyStatus().IsExtendedKey)
      lParam |= 0x01000000; // bit 24
    // if sysKey - bit 29 = 1, otherwise 0
    if (args.KeyStatus().WasKeyDown)
      lParam |= 0x40000000; // bit 30
    lParam |= 0x80000000; // bit 31 always 1 for WM_KEYUP

    LRESULT lresult;
    DrawBlock db(*this);
    auto hr = m_textServices->TxSendMessage(
        args.KeyStatus().IsMenuKeyDown ? WM_SYSKEYUP : WM_KEYUP, wParam, lParam, &lresult);
    if (hr == S_OK) { // S_FALSE or S_MSG_KEY_IGNORED means RichEdit didn't handle the key
      args.Handled(true);
    }
  }
  Super::OnKeyUp(args);
}

bool WindowsTextInputComponentView::ShouldSubmit(
    const winrt::Microsoft::ReactNative::Composition::Input::CharacterReceivedRoutedEventArgs &args) noexcept {
  bool shouldSubmit = true;

  if (shouldSubmit) {
    if (!m_multiline && m_submitKeyEvents.size() == 0) {
      // If no 'submitKeyEvents' are supplied, use the default behavior for single-line TextInput
      shouldSubmit = args.KeyCode() == '\r';
    } else if (m_submitKeyEvents.size() > 0) {
      auto submitKeyEvent = m_submitKeyEvents.at(0);
      // If 'submitKeyEvents' are supplied, use them to determine whether to emit onSubmitEditing' for either
      // single-line or multi-line TextInput
      if (args.KeyCode() == '\r') {
        bool shiftDown = (args.KeyboardSource().GetKeyState(winrt::Windows::System::VirtualKey::Shift) &
                          winrt::Microsoft::UI::Input::VirtualKeyStates::Down) ==
            winrt::Microsoft::UI::Input::VirtualKeyStates::Down;
        bool ctrlDown = (args.KeyboardSource().GetKeyState(winrt::Windows::System::VirtualKey::Control) &
                         winrt::Microsoft::UI::Input::VirtualKeyStates::Down) ==
            winrt::Microsoft::UI::Input::VirtualKeyStates::Down;
        bool altDown = (args.KeyboardSource().GetKeyState(winrt::Windows::System::VirtualKey::Control) &
                        winrt::Microsoft::UI::Input::VirtualKeyStates::Down) ==
            winrt::Microsoft::UI::Input::VirtualKeyStates::Down;
        bool metaDown = (args.KeyboardSource().GetKeyState(winrt::Windows::System::VirtualKey::LeftWindows) &
                         winrt::Microsoft::UI::Input::VirtualKeyStates::Down) ==
                winrt::Microsoft::UI::Input::VirtualKeyStates::Down ||
            (args.KeyboardSource().GetKeyState(winrt::Windows::System::VirtualKey::RightWindows) &
             winrt::Microsoft::UI::Input::VirtualKeyStates::Down) ==
                winrt::Microsoft::UI::Input::VirtualKeyStates::Down;
        return (submitKeyEvent.shiftKey && shiftDown) || (submitKeyEvent.ctrlKey && ctrlDown) ||
            (submitKeyEvent.altKey && altDown) || (submitKeyEvent.metaKey && metaDown) ||
            (!submitKeyEvent.shiftKey && !submitKeyEvent.altKey && !submitKeyEvent.metaKey && !submitKeyEvent.altKey &&
             !shiftDown && !ctrlDown && !altDown && !metaDown);
      } else {
        shouldSubmit = false;
      }
    } else {
      shouldSubmit = false;
    }
  }
  return shouldSubmit;
}

void WindowsTextInputComponentView::OnCharacterReceived(
    const winrt::Microsoft::ReactNative::Composition::Input::CharacterReceivedRoutedEventArgs &args) noexcept {
  // Do not forward tab keys into the TextInput, since we want that to do the tab loop instead.  This aligns with WinUI
  // behavior We do forward Ctrl+Tab to the textinput.
  if ((args.KeyCode() == '\t') &&
      ((args.KeyboardSource().GetKeyState(winrt::Windows::System::VirtualKey::Control) &
        winrt::Microsoft::UI::Input::VirtualKeyStates::Down) != winrt::Microsoft::UI::Input::VirtualKeyStates::Down)) {
    return;
  }

  // Logic for submit events
  if (ShouldSubmit(args)) {
    // call onSubmitEditing event
    if (m_eventEmitter && !m_comingFromJS) {
      auto emitter = std::static_pointer_cast<const facebook::react::WindowsTextInputEventEmitter>(m_eventEmitter);
      facebook::react::WindowsTextInputEventEmitter::OnSubmitEditing onSubmitEditingArgs;
      onSubmitEditingArgs.text = GetTextFromRichEdit();
      onSubmitEditingArgs.eventCount = ++m_nativeEventCount;
      emitter->onSubmitEditing(onSubmitEditingArgs);
    }

    if (m_clearTextOnSubmit) {
      // clear text from RichEdit
      m_textServices->TxSetText(L"");

      // Also update the state to reflect the cleared text
      // This ensures consistency between native and JS state
      auto data = facebook::react::TextInputState{};
      data.attributedStringBox = facebook::react::AttributedStringBox{facebook::react::AttributedString{}};
      data.mostRecentEventCount = m_nativeEventCount;

      m_state->updateState(std::move(data));
    }
    return;
  }

  // convert keyCode to std::string
  wchar_t key[2] = L" ";
  key[0] = static_cast<wchar_t>(args.KeyCode());
  std::string keyString = ::Microsoft::Common::Unicode::Utf16ToUtf8(key, 1);
  // Call onKeyPress event
  auto emitter = std::static_pointer_cast<const facebook::react::WindowsTextInputEventEmitter>(m_eventEmitter);
  facebook::react::WindowsTextInputEventEmitter::OnKeyPress onKeyPressArgs;
  if (keyString.compare("\r") == 0) {
    onKeyPressArgs.key = "Enter";
  } else if (keyString.compare("\b") == 0) {
    onKeyPressArgs.key = "Backspace";
  } else {
    onKeyPressArgs.key = keyString;
  }
  emitter->onKeyPress(onKeyPressArgs);

  WPARAM wParam = static_cast<WPARAM>(args.KeyCode());

  LPARAM lParam = 0;
  lParam = args.KeyStatus().RepeatCount; // bits 0-15
  lParam |= args.KeyStatus().ScanCode << 16; // bits 16-23
  if (args.KeyStatus().IsExtendedKey)
    lParam |= 0x01000000; // bit 24
  // bit 25-28 reserved.
  if (args.KeyStatus().IsMenuKeyDown)
    lParam |= 0x20000000; // bit 29
  // if sysKey - bit 29 = 1, otherwise 0
  if (args.KeyStatus().WasKeyDown)
    lParam |= 0x40000000; // bit 30
  if (args.KeyStatus().IsKeyReleased)
    lParam |= 0x80000000; // bit 31

  LRESULT lresult;
  DrawBlock db(*this);
  auto hr = m_textServices->TxSendMessage(WM_CHAR, wParam, lParam, &lresult);
  if (hr >= 0) {
    args.Handled(true);
  }
}

void WindowsTextInputComponentView::MountChildComponentView(
    const winrt::Microsoft::ReactNative::ComponentView &childComponentView,
    uint32_t index) noexcept {
  assert(false);
  base_type::MountChildComponentView(childComponentView, index);
}

void WindowsTextInputComponentView::UnmountChildComponentView(
    const winrt::Microsoft::ReactNative::ComponentView &childComponentView,
    uint32_t index) noexcept {
  assert(false);
  base_type::UnmountChildComponentView(childComponentView, index);
}

void WindowsTextInputComponentView::onLostFocus(
    const winrt::Microsoft::ReactNative::Composition::Input::RoutedEventArgs &args) noexcept {
  m_hasFocus = false;
  Super::onLostFocus(args);
  if (m_textServices) {
    LRESULT lresult;
    DrawBlock db(*this);
    m_textServices->TxSendMessage(WM_KILLFOCUS, 0, 0, &lresult);
    if (windowsTextInputProps().selectTextOnFocus) {
      LRESULT res;
      m_textServices->TxSendMessage(EM_SETSEL, static_cast<WPARAM>(-1), static_cast<WPARAM>(-1), &res);
    }
  }
  m_caretVisual.IsVisible(false);

  // Call onEndEditing when focus is lost
  if (m_eventEmitter && !m_comingFromJS) {
    auto emitter = std::static_pointer_cast<const facebook::react::WindowsTextInputEventEmitter>(m_eventEmitter);
    facebook::react::WindowsTextInputEventEmitter::OnEndEditing onEndEditingArgs;

    // Set event arguments
    onEndEditingArgs.eventCount = ++m_nativeEventCount;
    onEndEditingArgs.text = GetTextFromRichEdit();

    // Emit the event
    emitter->onEndEditing(onEndEditingArgs);
  }
}

void WindowsTextInputComponentView::onGotFocus(
    const winrt::Microsoft::ReactNative::Composition::Input::RoutedEventArgs &args) noexcept {
  m_hasFocus = true;
  Super::onGotFocus(args);
  if (m_textServices) {
    LRESULT lresult;
    DrawBlock db(*this);
    m_textServices->TxSendMessage(WM_SETFOCUS, 0, 0, &lresult);

    if (windowsTextInputProps().clearTextOnFocus) {
      m_textServices->TxSetText(L"");
    } else if (windowsTextInputProps().selectTextOnFocus) {
      LRESULT res;
      m_textServices->TxSendMessage(EM_SETSEL, static_cast<WPARAM>(0), static_cast<WPARAM>(-1), &res);
    }
  }
}

std::string WindowsTextInputComponentView::DefaultControlType() const noexcept {
  return "textinput";
}

std::string WindowsTextInputComponentView::DefaultAccessibleName() const noexcept {
  return windowsTextInputProps().placeholder;
}

std::string WindowsTextInputComponentView::DefaultHelpText() const noexcept {
  return windowsTextInputProps().placeholder;
}

void WindowsTextInputComponentView::updateCursorColor(
    const facebook::react::SharedColor &cursorColor,
    const facebook::react::SharedColor &foregroundColor) noexcept {
  const auto &props = windowsTextInputProps();
  auto caretColor = ::Microsoft::ReactNative::GetCaretColor(cursorColor, foregroundColor, props.backgroundColor);
  m_caretVisual.Brush(theme()->Brush(*caretColor));
}

void WindowsTextInputComponentView::updateProps(
    facebook::react::Props::Shared const &props,
    facebook::react::Props::Shared const &oldProps) noexcept {
  const auto &oldTextInputProps =
      *std::static_pointer_cast<const facebook::react::WindowsTextInputProps>(oldProps ? oldProps : viewProps());
  const auto &newTextInputProps = *std::static_pointer_cast<const facebook::react::WindowsTextInputProps>(props);

  Super::updateProps(props, oldProps);

  if (!facebook::react::floatEquality(
          oldTextInputProps.textAttributes.fontSize, newTextInputProps.textAttributes.fontSize) ||
      (oldTextInputProps.textAttributes.allowFontScaling != newTextInputProps.textAttributes.allowFontScaling) ||
      oldTextInputProps.textAttributes.fontWeight != newTextInputProps.textAttributes.fontWeight ||
      !facebook::react::floatEquality(
          oldTextInputProps.textAttributes.letterSpacing, newTextInputProps.textAttributes.letterSpacing) ||
      oldTextInputProps.textAttributes.fontFamily != newTextInputProps.textAttributes.fontFamily ||
      !facebook::react::floatEquality(
          oldTextInputProps.textAttributes.maxFontSizeMultiplier,
          newTextInputProps.textAttributes.maxFontSizeMultiplier)) {
    m_propBitsMask |= TXTBIT_CHARFORMATCHANGE;
    m_propBits |= TXTBIT_CHARFORMATCHANGE;
  }

  if (oldTextInputProps.secureTextEntry != newTextInputProps.secureTextEntry) {
    m_propBitsMask |= TXTBIT_USEPASSWORD;
    if (newTextInputProps.secureTextEntry) {
      m_propBits |= TXTBIT_USEPASSWORD;
    }
  }

  if (oldTextInputProps.multiline != newTextInputProps.multiline) {
    m_multiline = newTextInputProps.multiline;
    m_propBitsMask |= TXTBIT_MULTILINE | TXTBIT_WORDWRAP;
    if (newTextInputProps.multiline) {
      m_propBits |= TXTBIT_MULTILINE | TXTBIT_WORDWRAP;
    }
  }

  if (oldTextInputProps.editable != newTextInputProps.editable) {
    m_propBitsMask |= TXTBIT_READONLY;
    if (!newTextInputProps.editable) {
      m_propBits |= TXTBIT_READONLY;
    }
  }

  if (oldTextInputProps.placeholder != newTextInputProps.placeholder ||
      oldTextInputProps.placeholderTextColor != newTextInputProps.placeholderTextColor) {
    m_needsRedraw = true;
  }

  if (oldTextInputProps.backgroundColor != newTextInputProps.backgroundColor) {
    m_needsRedraw = true;
  }

  if (oldTextInputProps.cursorColor != newTextInputProps.cursorColor) {
    updateCursorColor(newTextInputProps.cursorColor, newTextInputProps.textAttributes.foregroundColor);
  }

  if (oldTextInputProps.clearTextOnSubmit != newTextInputProps.clearTextOnSubmit) {
    m_clearTextOnSubmit = newTextInputProps.clearTextOnSubmit;
  }

  if (oldTextInputProps.maxLength != newTextInputProps.maxLength) {
    LRESULT res;
    winrt::check_hresult(m_textServices->TxSendMessage(EM_LIMITTEXT, newTextInputProps.maxLength, 0, &res));
  }

  if ((!newTextInputProps.submitKeyEvents.empty())) {
    m_submitKeyEvents = newTextInputProps.submitKeyEvents;
  } else {
    m_submitKeyEvents.clear();
  }

  if (oldTextInputProps.autoCapitalize != newTextInputProps.autoCapitalize) {
    autoCapitalizeOnUpdateProps(oldTextInputProps.autoCapitalize, newTextInputProps.autoCapitalize);
  }

  if (oldTextInputProps.textAlign != newTextInputProps.textAlign) {
    // Let UpdateParaFormat() to refresh the text field with the new text alignment.
    m_propBitsMask |= TXTBIT_PARAFORMATCHANGE;
    m_propBits |= TXTBIT_PARAFORMATCHANGE;
  }

  // Please note: spellcheck performs both red lines and autocorrect as per windows behaviour
  bool shouldUpdateSpellCheck =
      (!oldProps || (oldTextInputProps.spellCheck != newTextInputProps.spellCheck) ||
       (oldTextInputProps.autoCorrect != newTextInputProps.autoCorrect));

  if (shouldUpdateSpellCheck) {
    bool effectiveSpellCheck = newTextInputProps.spellCheck || newTextInputProps.autoCorrect;
    updateSpellCheck(effectiveSpellCheck);
  }
  if (!oldProps || oldTextInputProps.autoCorrect != newTextInputProps.autoCorrect) {
    updateAutoCorrect(newTextInputProps.autoCorrect);
  }

  if (oldTextInputProps.selectionColor != newTextInputProps.selectionColor) {
    m_needsRedraw = true;
  }

  UpdatePropertyBits();
}

void WindowsTextInputComponentView::updateState(
    facebook::react::State::Shared const &state,
    facebook::react::State::Shared const &oldState) noexcept {
  m_state = std::static_pointer_cast<facebook::react::WindowsTextInputShadowNode::ConcreteState const>(state);

  if (!m_state) {
    assert(false && "State is `null` for <TextInput> component.");
    return;
  }

  if (!oldState) {
    m_mostRecentEventCount = m_state->getData().mostRecentEventCount;
  }

  if (auto root = rootComponentView()) {
    auto fontSizeMultiplier = root->FontSizeMultiplier();
    if (fontSizeMultiplier != m_fontSizeMultiplier) {
      fontSizeMultiplier = m_fontSizeMultiplier;
      m_propBitsMask |= TXTBIT_CHARFORMATCHANGE;
      m_propBits |= TXTBIT_CHARFORMATCHANGE;
    }
  }

  if (m_mostRecentEventCount == m_state->getData().mostRecentEventCount) {
    m_comingFromState = true;
    auto &fragments = m_state->getData().attributedStringBox.getValue().getFragments();
    UpdateText(fragments.size() ? fragments[0].string : "");

    m_comingFromState = false;
  }
}

void WindowsTextInputComponentView::UpdateText(const std::string &str) noexcept {
  SETTEXTEX stt;
  memset(&stt, 0, sizeof(stt));
  stt.flags = ST_DEFAULT;
  stt.codepage = CP_UTF8;
  LRESULT res;

  CHARRANGE cr;
  cr.cpMin = cr.cpMax = 0;
  winrt::check_hresult(m_textServices->TxSendMessage(EM_EXGETSEL, 0, reinterpret_cast<LPARAM>(&cr), &res));

  winrt::check_hresult(m_textServices->TxSendMessage(
      EM_SETTEXTEX, reinterpret_cast<WPARAM>(&stt), reinterpret_cast<LPARAM>(str.c_str()), &res));

  winrt::check_hresult(
      m_textServices->TxSendMessage(EM_SETSEL, static_cast<WPARAM>(cr.cpMin), static_cast<LPARAM>(cr.cpMax), &res));

  // enable colored emojis
  winrt::check_hresult(
      m_textServices->TxSendMessage(EM_SETTYPOGRAPHYOPTIONS, 0x1000 | 0x2000, 0x1000 | 0x2000, nullptr));
}

void WindowsTextInputComponentView::updateLayoutMetrics(
    facebook::react::LayoutMetrics const &layoutMetrics,
    facebook::react::LayoutMetrics const &oldLayoutMetrics) noexcept {
  // Set Position & Size Properties

  if ((layoutMetrics.pointScaleFactor != m_layoutMetrics.pointScaleFactor)) {
    LRESULT res;
    winrt::check_hresult(m_textServices->TxSendMessage(
        (WM_USER + 328), // EM_SETDPI
        static_cast<WPARAM>(layoutMetrics.pointScaleFactor * 96.0f),
        static_cast<LPARAM>(layoutMetrics.pointScaleFactor * 96.0f),
        &res));
  }

  Super::updateLayoutMetrics(layoutMetrics, oldLayoutMetrics);

  // TODO should ceil?
  unsigned int newWidth = static_cast<unsigned int>(layoutMetrics.frame.size.width * layoutMetrics.pointScaleFactor);
  unsigned int newHeight = static_cast<unsigned int>(layoutMetrics.frame.size.height * layoutMetrics.pointScaleFactor);

  if (newWidth != m_imgWidth || newHeight != m_imgHeight) {
    m_drawingSurface = nullptr; // Invalidate surface if we get a size change
  }

  m_imgWidth = newWidth;
  m_imgHeight = newHeight;
}

std::pair<float, float> WindowsTextInputComponentView::GetContentSize() const noexcept {
  if (!m_textServices)
    return {0.0f, 0.0f};

  // Get a device context for measurement
  HDC hdc = GetDC(nullptr);
  if (!hdc)
    return {0.0f, 0.0f};

  // Use the layout width as the constraint (always multiline)
  float availableWidth = m_layoutMetrics.frame.size.width;
  float scale = m_layoutMetrics.pointScaleFactor;
  float dpi = m_layoutMetrics.pointScaleFactor * GetDpiForSystem();
  constexpr float HIMETRIC_PER_INCH = 2540.0f;

  SIZE extentHimetric = {
      static_cast<LONG>(availableWidth * scale * HIMETRIC_PER_INCH / dpi),
      static_cast<LONG>(std::numeric_limits<LONG>::max() * HIMETRIC_PER_INCH / dpi)};

  SIZE naturalSize = {0, 0};

  HRESULT hr = m_textServices->TxGetNaturalSize(
      DVASPECT_CONTENT,
      hdc,
      nullptr,
      nullptr,
      static_cast<DWORD>(TXTNS_FITTOCONTENTWSP),
      reinterpret_cast<SIZEL *>(&extentHimetric),
      &naturalSize.cx,
      &naturalSize.cy);

  ReleaseDC(nullptr, hdc);

  if (FAILED(hr)) {
    return {0.0f, 0.0f};
  }

  float contentWidth = static_cast<float>(naturalSize.cx) / scale;
  float contentHeight = static_cast<float>(naturalSize.cy) / scale;

  return {contentWidth, contentHeight};
}

// When we are notified by RichEdit that the text changed, we need to notify JS
void WindowsTextInputComponentView::OnTextUpdated() noexcept {
  auto data = m_state->getData();
  // auto newAttributedString = getAttributedString();
  // if (data.attributedString == newAttributedString)
  //    return;
  data.attributedStringBox = facebook::react::AttributedStringBox(getAttributedString());
  data.mostRecentEventCount = m_nativeEventCount;

  m_state->updateState(std::move(data));

  if (m_eventEmitter && !m_comingFromJS) {
    // call onChange event
    auto emitter = std::static_pointer_cast<const facebook::react::WindowsTextInputEventEmitter>(m_eventEmitter);
    facebook::react::WindowsTextInputEventEmitter::OnChange onChangeArgs;
    onChangeArgs.text = GetTextFromRichEdit();
    onChangeArgs.eventCount = ++m_nativeEventCount;
    emitter->onChange(onChangeArgs);
    if (windowsTextInputProps().multiline) {
      auto [contentWidth, contentHeight] = GetContentSize();
      facebook::react::WindowsTextInputEventEmitter::OnContentSizeChange onContentSizeChangeArgs;
      onContentSizeChangeArgs.contentSize.width = contentWidth;
      onContentSizeChangeArgs.contentSize.height = contentHeight;
      emitter->onContentSizeChange(onContentSizeChangeArgs);
    }
  }

  if (UiaClientsAreListening()) {
    auto text = GetTextFromRichEdit();
    winrt::Microsoft::ReactNative::implementation::UpdateUiaProperty(
        EnsureUiaProvider(), UIA_ValueValuePropertyId, text, text);
  }
}

void WindowsTextInputComponentView::EmitOnScrollEvent() noexcept {
  if (!windowsTextInputProps().scrollEnabled || !m_eventEmitter || m_comingFromJS || !m_textServices) {
    return;
  }
  LONG hMin, hMax, hPos, hPage;
  LONG vMin, vMax, vPos, vPage;
  BOOL hEnabled, vEnabled;
  winrt::check_hresult(m_textServices->TxGetHScroll(&hMin, &hMax, &hPos, &hPage, &hEnabled));
  winrt::check_hresult(m_textServices->TxGetVScroll(&vMin, &vMax, &vPos, &vPage, &vEnabled));
  facebook::react::Point offset;
  offset.x = static_cast<float>(hPos);
  offset.y = static_cast<float>(vPos);
  auto emitter = std::static_pointer_cast<const facebook::react::WindowsTextInputEventEmitter>(m_eventEmitter);
  emitter->onScroll(offset);
}

void WindowsTextInputComponentView::OnSelectionChanged(LONG start, LONG end) noexcept {
  if (m_eventEmitter && !m_comingFromState /* && !m_comingFromJS ?? */) {
    auto emitter = std::static_pointer_cast<const facebook::react::WindowsTextInputEventEmitter>(m_eventEmitter);
    facebook::react::WindowsTextInputEventEmitter::OnSelectionChange onSelectionChangeArgs;
    onSelectionChangeArgs.selection.start = start;
    onSelectionChangeArgs.selection.end = end;
    emitter->onSelectionChange(onSelectionChangeArgs);
  }
}

std::string WindowsTextInputComponentView::GetTextFromRichEdit() const noexcept {
  if (!m_textServices)
    return {};

  BSTR bstr;
  winrt::check_hresult(m_textServices->TxGetText(&bstr));
  auto str = BstrToStdString(bstr);

  // JS gets confused by the \r\0 ending
  if (str.size() > 0 && *(str.end() - 1) == '\0') {
    str.pop_back();
  }
  if (str.size() > 0 && *(str.end() - 1) == '\r') {
    str.pop_back();
  }
  SysFreeString(bstr);
  return str;
}

void WindowsTextInputComponentView::FinalizeUpdates(
    winrt::Microsoft::ReactNative::ComponentViewUpdateMask updateMask) noexcept {
  Super::FinalizeUpdates(updateMask);
  InternalFinalize();
}

void WindowsTextInputComponentView::UpdatePropertyBits() noexcept {
  if (m_propBitsMask != 0) {
    DrawBlock db(*this);
    winrt::check_hresult(m_textServices->OnTxPropertyBitsChange(m_propBitsMask, m_propBits));
    m_propBitsMask = 0;
    m_propBits = 0;
  }
}

void WindowsTextInputComponentView::InternalFinalize() noexcept {
  if (m_mounted) {
    UpdatePropertyBits();

    ensureDrawingSurface();
    if (m_needsRedraw) {
      DrawText();
    }
  }
}

void WindowsTextInputComponentView::onMounted() noexcept {
  Super::onMounted();

  auto fontSizeMultiplier = rootComponentView()->FontSizeMultiplier();
  if (m_fontSizeMultiplier != fontSizeMultiplier) {
    m_fontSizeMultiplier = fontSizeMultiplier;
    m_propBitsMask |= TXTBIT_CHARFORMATCHANGE;
    m_propBits |= TXTBIT_CHARFORMATCHANGE;
  }
  InternalFinalize();

  // Handle autoFocus property - focus the component when mounted if autoFocus is true
  if (windowsTextInputProps().autoFocus) {
    if (auto root = rootComponentView()) {
      root->TrySetFocusedComponent(*get_strong(), winrt::Microsoft::ReactNative::FocusNavigationDirection::None);
    }
  }
}

std::optional<std::string> WindowsTextInputComponentView::getAccessiblityValue() noexcept {
  return GetTextFromRichEdit();
}

void WindowsTextInputComponentView::setAcccessiblityValue(std::string &&value) noexcept {
  UpdateText(value);
}

bool WindowsTextInputComponentView::getAcccessiblityIsReadOnly() noexcept {
  return !windowsTextInputProps().editable;
}

facebook::react::SharedViewProps WindowsTextInputComponentView::defaultProps() noexcept {
  static auto const defaultProps = std::make_shared<facebook::react::WindowsTextInputProps const>();
  return defaultProps;
}

const facebook::react::WindowsTextInputProps &WindowsTextInputComponentView::windowsTextInputProps() const noexcept {
  return *std::static_pointer_cast<const facebook::react::WindowsTextInputProps>(viewProps());
}

void WindowsTextInputComponentView::UpdateCharFormat() noexcept {
  CHARFORMAT2W cfNew = {0};
  // Set CHARFORMAT structure
  cfNew.cbSize = sizeof(CHARFORMAT2W);

  // ElementFontDetails fontDetails;
  // GetFontDetails(fontDetails);

  // Cache foreground color instead of setting in char format.
  // This will make RichEdit call us back for the colors.
  // if (fontDetails.HasColor) {
  //    m_crText = RemoveAlpha(fontDetails.FontColor);
  //  }

  // set font face
  // cfNew.dwMask |= CFM_FACE;
  // NetUIWzCchCopy(cfNew.szFaceName, _countof(cfNew.szFaceName), fontDetails.FontName.c_str());
  // cfNew.bPitchAndFamily = FF_DONTCARE;

  // set font size -- 15 to convert twips to pt
  const auto &props = windowsTextInputProps();
  float fontSize =
      (std::isnan(props.textAttributes.fontSize) ? facebook::react::TextAttributes::defaultTextAttributes().fontSize
                                                 : props.textAttributes.fontSize);

  // Apply maxFontSizeMultiplier if specified
  auto maxFontSizeMultiplier = windowsTextInputProps().textAttributes.maxFontSizeMultiplier;
  fontSize *=
      (maxFontSizeMultiplier >= 1.0f) ? std::min(maxFontSizeMultiplier, m_fontSizeMultiplier) : m_fontSizeMultiplier;

  // TODO get fontSize from props.textAttributes, or defaultTextAttributes, or fragment?
  cfNew.dwMask |= CFM_SIZE;
  cfNew.yHeight = static_cast<LONG>(fontSize * 15);

  // set bold
  cfNew.dwMask |= CFM_WEIGHT;
  cfNew.wWeight =
      props.textAttributes.fontWeight ? static_cast<WORD>(*props.textAttributes.fontWeight) : DWRITE_FONT_WEIGHT_NORMAL;

  // set font style
  // cfNew.dwMask |= (CFM_ITALIC | CFM_STRIKEOUT | CFM_UNDERLINE);
  // int dFontStyle = fontDetails.FontStyle;
  // if (dFontStyle & FS_Italic) {
  //    cfNew.dwEffects |= CFE_ITALIC;
  //  }
  //  if (dFontStyle & FS_StrikeOut) {
  // cfNew.dwEffects |= CFE_STRIKEOUT;
  //}
  // if (dFontStyle & FS_Underline) {
  //    cfNew.dwEffects |= CFE_UNDERLINE;
  //  }

  // set font family
  if (!props.textAttributes.fontFamily.empty()) {
    cfNew.dwMask |= CFM_FACE;
    std::wstring fontFamily =
        std::wstring(props.textAttributes.fontFamily.begin(), props.textAttributes.fontFamily.end());
    wcsncpy_s(cfNew.szFaceName, fontFamily.c_str(), LF_FACESIZE);
  }

  // set char offset
  cfNew.dwMask |= CFM_OFFSET;
  cfNew.yOffset = 0;

  // set letter spacing
  float letterSpacing = props.textAttributes.letterSpacing;
  if (!std::isnan(letterSpacing)) {
    updateLetterSpacing(letterSpacing);
  }

  // set charset
  cfNew.dwMask |= CFM_CHARSET;
  cfNew.bCharSet = DEFAULT_CHARSET;

  m_cf = cfNew;
}

void WindowsTextInputComponentView::UpdateParaFormat() noexcept {
  ZeroMemory(&m_pf, sizeof(m_pf));

  m_pf.cbSize = sizeof(PARAFORMAT2);
  m_pf.dwMask = PFM_ALL;

  auto &textAlign = windowsTextInputProps().textAlign;

  if (textAlign == facebook::react::TextAlignment::Center) {
    m_pf.wAlignment = PFA_CENTER;
  } else if (textAlign == facebook::react::TextAlignment::Right) {
    m_pf.wAlignment = PFA_RIGHT;
  } else {
    m_pf.wAlignment = PFA_LEFT;
  }

  m_pf.cTabCount = 1;
  m_pf.rgxTabs[0] = lDefaultTab;

  /*
        if (m_spcontroller->IsCurrentReadingOrderRTL())
        {
                m_pf.dwMask |= PFM_RTLPARA;
                m_pf.wEffects |= PFE_RTLPARA;
        }
  */
}

void WindowsTextInputComponentView::OnRenderingDeviceLost() noexcept {
  DrawText();
}

void WindowsTextInputComponentView::ensureDrawingSurface() noexcept {
  assert(m_reactContext.UIDispatcher().HasThreadAccess());

  if (!m_drawingSurface) {
    m_drawingSurface = m_compContext.CreateDrawingSurfaceBrush(
        {static_cast<float>(m_imgWidth), static_cast<float>(m_imgHeight)},
        winrt::Windows::Graphics::DirectX::DirectXPixelFormat::B8G8R8A8UIntNormalized,
        winrt::Windows::Graphics::DirectX::DirectXAlphaMode::Premultiplied);

    auto rc = getClientRect();
    winrt::check_hresult(m_textServices->OnTxInPlaceActivate(&rc));

    LRESULT lresult;
    winrt::check_hresult(
        m_textServices->TxSendMessage(EM_SETEVENTMASK, 0, ENM_CHANGE | ENM_SELCHANGE | ENM_ENDCOMPOSITION, &lresult));

    DrawText();

    m_drawingSurface.HorizontalAlignmentRatio(0.f);
    m_drawingSurface.VerticalAlignmentRatio(0.f);
    m_drawingSurface.Stretch(winrt::Microsoft::ReactNative::Composition::Experimental::CompositionStretch::None);
    Visual().as<Experimental::ISpriteVisual>().Brush(m_drawingSurface);
  }
}

void WindowsTextInputComponentView::ShowCaret(bool show) noexcept {
  ensureVisual();
  const auto &props = windowsTextInputProps();
  updateCursorColor(props.cursorColor, props.textAttributes.foregroundColor);
  m_caretVisual.IsVisible(show);
}

winrt::com_ptr<::IDWriteTextLayout> WindowsTextInputComponentView::CreatePlaceholderLayout() {
  // Create a fragment with text attributes
  winrt::com_ptr<::IDWriteTextLayout> textLayout = nullptr;
  facebook::react::AttributedString attributedString;
  facebook::react::AttributedString::Fragment fragment1;
  const auto &props = windowsTextInputProps();
  facebook::react::TextAttributes textAttributes = props.textAttributes;
  if (std::isnan(props.textAttributes.fontSize)) {
    textAttributes.fontSize = 12.0f;
  }
  textAttributes.fontSizeMultiplier = m_fontSizeMultiplier;
  fragment1.string = props.placeholder;
  fragment1.textAttributes = textAttributes;
  attributedString.appendFragment(std::move(fragment1));

  facebook::react::LayoutConstraints constraints;
  constraints.maximumSize.width = static_cast<FLOAT>(m_imgWidth);
  constraints.maximumSize.height = static_cast<FLOAT>(m_imgHeight);

  facebook::react::WindowsTextLayoutManager::GetTextLayout(
      facebook::react::AttributedStringBox(attributedString), {} /*TODO*/, constraints, textLayout);

  return textLayout;
}

void WindowsTextInputComponentView::DrawText() noexcept {
  m_needsRedraw = true;
  if (m_cDrawBlock || theme()->IsEmpty() || !m_textServices) {
    return;
  }

  bool isZeroSized =
      m_layoutMetrics.frame.size.width <= (m_layoutMetrics.contentInsets.left + m_layoutMetrics.contentInsets.right);
  if (!m_drawingSurface || isZeroSized)
    return;

  POINT offset;

  assert(m_reactContext.UIDispatcher().HasThreadAccess());

  m_drawing = true;
  {
    ::Microsoft::ReactNative::Composition::AutoDrawDrawingSurface autoDraw(
        m_drawingSurface, m_layoutMetrics.pointScaleFactor, &offset);
    if (auto d2dDeviceContext = autoDraw.GetRenderTarget()) {
      d2dDeviceContext->Clear(D2D1::ColorF(D2D1::ColorF::Black, 0.0f));
      assert(d2dDeviceContext->GetUnitMode() == D2D1_UNIT_MODE_DIPS);

      RECTL rc{
          static_cast<LONG>(offset.x),
          static_cast<LONG>(offset.y),
          static_cast<LONG>(offset.x) + static_cast<LONG>(m_imgWidth),
          static_cast<LONG>(offset.y) + static_cast<LONG>(m_imgHeight)};

      RECT rcClient{
          static_cast<LONG>(offset.x),
          static_cast<LONG>(offset.y),
          static_cast<LONG>(offset.x) + static_cast<LONG>(m_imgWidth),
          static_cast<LONG>(offset.y) + static_cast<LONG>(m_imgHeight)};

      {
        m_cDrawBlock++; // Dont use AutoDrawBlock as we are already in draw, and dont need to draw again.
        winrt::check_hresult(m_textServices->OnTxInPlaceActivate(&rcClient));
        m_cDrawBlock--;
      }

      const auto &props = windowsTextInputProps();
      if (facebook::react::isColorMeaningful(props.backgroundColor)) {
        auto backgroundColor = theme()->D2DColor(*props.backgroundColor);
        winrt::com_ptr<ID2D1SolidColorBrush> backgroundBrush;
        winrt::check_hresult(d2dDeviceContext->CreateSolidColorBrush(backgroundColor, backgroundBrush.put()));
        const D2D1_RECT_F fillRect = {
            static_cast<float>(rcClient.left) / m_layoutMetrics.pointScaleFactor,
            static_cast<float>(rcClient.top) / m_layoutMetrics.pointScaleFactor,
            static_cast<float>(rcClient.right) / m_layoutMetrics.pointScaleFactor,
            static_cast<float>(rcClient.bottom) / m_layoutMetrics.pointScaleFactor};
        d2dDeviceContext->FillRectangle(fillRect, backgroundBrush.get());
      }

      // TODO keep track of proper invalid rect
      auto hrDraw = m_textServices->TxDrawD2D(d2dDeviceContext, &rc, nullptr, TXTVIEW_ACTIVE);
      winrt::check_hresult(hrDraw);

      // draw placeholder text if needed
      if (!props.placeholder.empty() && GetTextFromRichEdit().empty()) {
        // set brush color
        winrt::com_ptr<ID2D1SolidColorBrush> brush;
        if (props.placeholderTextColor) {
          auto color = theme()->D2DColor(*props.placeholderTextColor);
          winrt::check_hresult(d2dDeviceContext->CreateSolidColorBrush(color, brush.put()));
        } else {
          // Use theme-aware placeholder color based on focus state and background
          // Color selection follows Windows 11 design system semantic colors:
          // - High contrast: System GrayText for accessibility
          // - Light backgrounds: Darker grays for better contrast
          // - Dark backgrounds: Lighter grays for readability
          winrt::Windows::UI::Color backgroundColor = {};
          if (facebook::react::isColorMeaningful(props.backgroundColor)) {
            auto bgColor = (*props.backgroundColor).AsWindowsColor();
            backgroundColor = bgColor;
          }

          auto placeholderColor = facebook::react::GetTextInputPlaceholderColor(m_hasFocus, backgroundColor);
          auto d2dColor = theme()->D2DColor(*placeholderColor);
          winrt::check_hresult(d2dDeviceContext->CreateSolidColorBrush(d2dColor, brush.put()));
        }

        // Create placeholder text layout
        winrt::com_ptr<::IDWriteTextLayout> textLayout = CreatePlaceholderLayout();

        // draw text
        d2dDeviceContext->DrawTextLayout(
            D2D1::Point2F(
                static_cast<FLOAT>((offset.x + m_layoutMetrics.contentInsets.left) / m_layoutMetrics.pointScaleFactor),
                static_cast<FLOAT>((offset.y + m_layoutMetrics.contentInsets.top) / m_layoutMetrics.pointScaleFactor)),
            textLayout.get(),
            brush.get(),
            D2D1_DRAW_TEXT_OPTIONS_ENABLE_COLOR_FONT);
      }
    }
  }
  m_drawing = false;
  m_needsRedraw = false;
}

winrt::Microsoft::ReactNative::Composition::Experimental::IVisual
WindowsTextInputComponentView::createVisual() noexcept {
  HrEnsureRichEd20Loaded();
  auto visual = m_compContext.CreateSpriteVisual();
  m_textHost = winrt::make<CompTextHost>(this);
  winrt::com_ptr<::IUnknown> spUnk;
  winrt::check_hresult(g_pfnCreateTextServices(nullptr, m_textHost.get(), spUnk.put()));
  spUnk.as(m_textServices);

  LRESULT res;
  winrt::check_hresult(m_textServices->TxSendMessage(EM_SETTEXTMODE, TM_PLAINTEXT, 0, &res));

  m_caretVisual = m_compContext.CreateCaretVisual();
  visual.InsertAt(m_caretVisual.InnerVisual(), 0);
  m_caretVisual.IsVisible(false);

  return visual;
}

std::pair<facebook::react::Cursor, HCURSOR> WindowsTextInputComponentView::cursor() const noexcept {
  return {viewProps()->cursor, m_hcursor};
}

void WindowsTextInputComponentView::onThemeChanged() noexcept {
  const auto &props = windowsTextInputProps();
  updateCursorColor(props.cursorColor, props.textAttributes.foregroundColor);
  DrawText();
  base_type::onThemeChanged();
}

winrt::Microsoft::ReactNative::ComponentView WindowsTextInputComponentView::Create(
    const winrt::Microsoft::ReactNative::Composition::Experimental::ICompositionContext &compContext,
    facebook::react::Tag tag,
    winrt::Microsoft::ReactNative::ReactContext const &reactContext) noexcept {
  return winrt::make<WindowsTextInputComponentView>(compContext, tag, reactContext);
}

// This function assumes that previous and new capitalization types are different.
void WindowsTextInputComponentView::autoCapitalizeOnUpdateProps(
    const std::string &previousCapitalizationType,
    const std::string &newCapitalizationType) noexcept {
  /*
    Possible values are:
     Characters - All characters.
     Words - First letter of each word.
     Sentences - First letter of each sentence.
     None - Do not autocapitalize anything.

     For now, only characters and none are supported.
  */

  if (previousCapitalizationType == "characters") {
    winrt::check_hresult(m_textServices->TxSendMessage(
        EM_SETEDITSTYLE, 0 /* disable */, SES_UPPERCASE /* flag affected */, nullptr /* LRESULT */));
  }

  if (newCapitalizationType == "characters") {
    winrt::check_hresult(m_textServices->TxSendMessage(
        EM_SETEDITSTYLE, SES_UPPERCASE /* enable */, SES_UPPERCASE /* flag affected */, nullptr /* LRESULT */));
  }
}

void WindowsTextInputComponentView::updateLetterSpacing(float letterSpacing) noexcept {
  CHARFORMAT2W cf = {};
  cf.cbSize = sizeof(CHARFORMAT2W);
  cf.dwMask = CFM_SPACING;
  cf.sSpacing = static_cast<SHORT>(letterSpacing * 20); // Convert to TWIPS

  LRESULT res;

  // Apply to all existing text like placeholder
  winrt::check_hresult(m_textServices->TxSendMessage(EM_SETCHARFORMAT, SCF_ALL, reinterpret_cast<LPARAM>(&cf), &res));

  // Apply to future text input
  winrt::check_hresult(
      m_textServices->TxSendMessage(EM_SETCHARFORMAT, SCF_SELECTION, reinterpret_cast<LPARAM>(&cf), &res));
}

void WindowsTextInputComponentView::updateAutoCorrect(bool enable) noexcept {
  LRESULT lresult;
  winrt::check_hresult(m_textServices->TxSendMessage(
      EM_SETAUTOCORRECTPROC, enable ? 0 : reinterpret_cast<WPARAM>(AutoCorrectOffCallback), 0, &lresult));
}

void WindowsTextInputComponentView::updateSpellCheck(bool enable) noexcept {
  LRESULT currentLangOptions;
  winrt::check_hresult(m_textServices->TxSendMessage(EM_GETLANGOPTIONS, 0, 0, &currentLangOptions));

  DWORD newLangOptions = static_cast<DWORD>(currentLangOptions);
  if (enable) {
    newLangOptions |= IMF_SPELLCHECKING;
  }

  LRESULT lresult;
  winrt::check_hresult(
      m_textServices->TxSendMessage(EM_SETLANGOPTIONS, IMF_SPELLCHECKING, enable ? newLangOptions : 0, &lresult));
}
} // namespace winrt::Microsoft::ReactNative::Composition::implementation
