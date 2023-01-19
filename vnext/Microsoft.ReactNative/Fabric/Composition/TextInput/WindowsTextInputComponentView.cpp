
// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include "WindowsTextInputComponentView.h"

#include <Utils/ValueUtils.h>
#include <tom.h>
#include <unicode.h>
#include <winrt/Windows.UI.h>
#include "../CompositionHelpers.h"
#include "WindowsTextInputShadowNode.h"
#include "WindowsTextInputState.h"
#include "guid/msoGuid.h"

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

namespace Microsoft::ReactNative {

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
    assert(false);
    return {};
  }

  //@cmember Enable the scroll bar
  BOOL TxEnableScrollBar(INT fuSBFlags, INT fuArrowflags) override {
    assert(false);
    return {};
  }

  //@cmember Set the scroll range
  BOOL TxSetScrollRange(INT fnBar, LONG nMinPos, INT nMaxPos, BOOL fRedraw) override {
    assert(false);
    return {};
  }

  //@cmember Set the scroll position
  BOOL TxSetScrollPos(INT fnBar, INT nPos, BOOL fRedraw) override {
    assert(false);
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
    m_outer->ShowCaret(fShow);
    return true;
  }

  //@cmember Set the caret position
  BOOL TxSetCaretPos(INT x, INT y) override {
    if (x < 0 && y < 0) {
      // RichEdit sends (-32000,-32000) when the caret is not currently visible.
      return false;
    }

    m_outer->m_caretVisual.Position(
        {x - (m_outer->m_layoutMetrics.frame.origin.x * m_outer->m_layoutMetrics.pointScaleFactor),
         y - (m_outer->m_layoutMetrics.frame.origin.y * m_outer->m_layoutMetrics.pointScaleFactor)});
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
    // assert(false);
    // TODO capture?
    /*
    if (fCapture) {
      ::SetCapture(m_hwndHost);
    } else {
      ::ReleaseCapture();
    }
    */
  }

  //@cmember Set the focus to the text window
  void TxSetFocus() override {
    SetFocusedComponent(m_outer);
    // assert(false);
    // TODO focus
  }

  //@cmember Establish a new cursor shape
  void TxSetCursor(HCURSOR hcur, BOOL fText) override {
    assert(false);
  }

  //@cmember Converts screen coordinates of a specified point to the client coordinates
  BOOL TxScreenToClient(LPPOINT lppt) override {
    assert(false);
    return {};
  }

  //@cmember Converts the client coordinates of a specified point to screen coordinates
  BOOL TxClientToScreen(LPPOINT lppt) override {
    assert(false);
    return {};
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
        if (m_outer->m_props->textAttributes.foregroundColor)
          return m_outer->m_props->textAttributes.foregroundColor.AsColorRefNoAlpha();
        // cr = 0x000000FF;
        break;

      case COLOR_WINDOW:
        if (m_outer->m_props->backgroundColor)
          return m_outer->m_props->backgroundColor.AsColorRefNoAlpha();
        break;
        // case COLOR_HIGHLIGHT:
        // cr = RGB(0, 0, 255);
        // cr = 0x0000ffFF;
        //          break;

        // case COLOR_HIGHLIGHTTEXT:
        // cr = RGB(255, 0, 0);
        // cr = 0xFFFFFFFF;
        //          break;

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
    *pstyle = TXTBACK_TRANSPARENT;
    //*pstyle = TXTBACK_OPAQUE;
    return S_OK;
  }

  //@cmember Get the maximum length for the text
  HRESULT TxGetMaxLength(DWORD *plength) override {
    auto length = m_outer->m_props->maxLength;
    if (length > static_cast<decltype(m_outer->m_props->maxLength)>(std::numeric_limits<DWORD>::max())) {
      length = std::numeric_limits<DWORD>::max();
    }
    *plength = static_cast<DWORD>(length);
    return S_OK;
  }

  //@cmember Get the bits representing requested scroll bars for the window
  HRESULT TxGetScrollBars(DWORD *pdwScrollBar) override {
    // TODO support scrolling
    *pdwScrollBar = 0;
    return S_OK;
  }

  //@cmember Get the character to display for password input
  HRESULT TxGetPasswordChar(_Out_ TCHAR *pch) override {
    assert(false);
    return {};
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

facebook::react::AttributedString WindowsTextInputComponentView::getAttributedString() const {
  // Use BaseTextShadowNode to get attributed string from children

  auto childTextAttributes = facebook::react::TextAttributes::defaultTextAttributes();

  childTextAttributes.apply(m_props->textAttributes);

  auto attributedString = facebook::react::AttributedString{};
  // auto attachments = facebook::react::BaseTextShadowNode::Attachments{};

  // BaseTextShadowNode only gets children. We must detect and prepend text
  // value attributes manually.
  auto text = GetTextFromRichEdit();
  // if (!m_props->text.empty()) {
  if (!text.empty()) {
    auto textAttributes = facebook::react::TextAttributes::defaultTextAttributes();
    textAttributes.apply(m_props->textAttributes);
    auto fragment = facebook::react::AttributedString::Fragment{};
    fragment.string = text;
    // fragment.string = m_props->text;
    fragment.textAttributes = textAttributes;
    // If the TextInput opacity is 0 < n < 1, the opacity of the TextInput and
    // text value's background will stack. This is a hack/workaround to prevent
    // that effect.
    fragment.textAttributes.backgroundColor = facebook::react::clearColor();
    // fragment.parentShadowView = facebook::react::ShadowView(*this);
    attributedString.prependFragment(fragment);
  }

  return attributedString;
}

WindowsTextInputComponentView::WindowsTextInputComponentView(
    const winrt::Microsoft::ReactNative::Composition::ICompositionContext &compContext,
    facebook::react::Tag tag,
    winrt::Microsoft::ReactNative::ReactContext const &reactContext)
    : Super(compContext, tag), m_context(reactContext) {
  static auto const defaultProps = std::make_shared<facebook::react::WindowsTextInputProps const>();
  m_props = defaultProps;

  /*
  m_textChangedRevoker =
      m_element.TextChanged(winrt::auto_revoke, [this](auto sender, xaml::Controls::TextChangedEventArgs args) {
        auto data = m_state->getData();
        data.attributedString = getAttributedString();
        data.mostRecentEventCount = m_nativeEventCount;
        m_state->updateState(std::move(data));

        if (m_eventEmitter && !m_comingFromJS) {
          auto emitter = std::static_pointer_cast<const facebook::react::WindowsTextInputEventEmitter>(m_eventEmitter);
          facebook::react::WindowsTextInputEventEmitter::OnChange onChangeArgs;
          onChangeArgs.text = winrt::to_string(m_element.Text());
          onChangeArgs.eventCount = ++m_nativeEventCount;
          emitter->onChange(onChangeArgs);
        }
      });

  m_SelectionChangedRevoker = m_element.SelectionChanged(winrt::auto_revoke, [this](auto sender, auto args) {
    if (m_eventEmitter) {
      auto emitter = std::static_pointer_cast<const facebook::react::WindowsTextInputEventEmitter>(m_eventEmitter);
      facebook::react::WindowsTextInputEventEmitter::OnSelectionChange onSelectionChangeArgs;
      onSelectionChangeArgs.selection.start = m_element.SelectionStart();
      onSelectionChangeArgs.selection.end = m_element.SelectionStart() + m_element.SelectionLength();
      emitter->onSelectionChange(onSelectionChangeArgs);
    }
  });
  */
}

void WindowsTextInputComponentView::handleCommand(std::string const &commandName, folly::dynamic const &arg) noexcept {
  if (commandName == "setTextAndSelection") {
    auto eventCount = arg[0].asInt();

    if (eventCount >= m_nativeEventCount) {
      auto text = arg[1].asString();
      auto begin = arg[2].asInt();
      auto end = arg[3].asInt();
      m_comingFromJS = true;
      UpdateText(text);

      SELCHANGE sc;
      memset(&sc, 0, sizeof(sc));
      sc.chrg.cpMin = static_cast<LONG>(begin);
      sc.chrg.cpMax = static_cast<LONG>(end);
      sc.seltyp = (begin == end) ? SEL_EMPTY : SEL_TEXT;

      LRESULT res;
      /*
      winrt::check_hresult(m_textServices->TxSendMessage(
          EM_SELCHANGE, 0 , reinterpret_cast<WPARAM>(&sc), &res));
          */
      winrt::check_hresult(
          m_textServices->TxSendMessage(EM_SETSEL, static_cast<WPARAM>(begin), static_cast<LPARAM>(end), &res));

      m_comingFromJS = false;
    }
  } else {
    Super::handleCommand(commandName, arg);
  }
}

int64_t WindowsTextInputComponentView::SendMessage(uint32_t msg, uint64_t wParam, int64_t lParam) noexcept {
  if (m_textServices) {
    LRESULT lresult;
    DrawBlock db(*this);
    auto hr = m_textServices->TxSendMessage(msg, static_cast<WPARAM>(wParam), static_cast<LPARAM>(lParam), &lresult);
    if (hr >= 0 && lresult) {
      return lresult;
    }
  }
  return Super::SendMessage(msg, wParam, lParam);
}

std::vector<facebook::react::ComponentDescriptorProvider>
WindowsTextInputComponentView::supplementalComponentDescriptorProviders() noexcept {
  return {};
}

void WindowsTextInputComponentView::parent(IComponentView *parent) noexcept {
  Super::parent(parent);

  if (!parent && GetFocusedComponent() == this) {
    SetFocusedComponent(nullptr); // TODO need move focus logic - where should focus go?
  }
}

void WindowsTextInputComponentView::mountChildComponentView(
    const IComponentView &childComponentView,
    uint32_t index) noexcept {
  assert(false);
  // m_element.Children().InsertAt(index, v.Element());
}

void WindowsTextInputComponentView::unmountChildComponentView(
    const IComponentView &childComponentView,
    uint32_t index) noexcept {
  assert(false);
  // m_element.Children().RemoveAt(index);
}

void WindowsTextInputComponentView::onFocusLost() noexcept {
  Super::onFocusLost();
  SendMessage(WM_KILLFOCUS, 0, 0);
}

void WindowsTextInputComponentView::onFocusGained() noexcept {
  Super::onFocusGained();
  SendMessage(WM_SETFOCUS, 0, 0);
}

void WindowsTextInputComponentView::updateProps(
    facebook::react::Props::Shared const &props,
    facebook::react::Props::Shared const &oldProps) noexcept {
  const auto &oldTextInputProps = *std::static_pointer_cast<const facebook::react::WindowsTextInputProps>(m_props);
  const auto &newTextInputProps = *std::static_pointer_cast<const facebook::react::WindowsTextInputProps>(props);

  DWORD propBitsMask = 0;
  DWORD propBits = 0;

  ensureVisual();

  updateBorderProps(oldTextInputProps, newTextInputProps);

  if (!facebook::react::floatEquality(
          oldTextInputProps.textAttributes.fontSize, newTextInputProps.textAttributes.fontSize) ||
      oldTextInputProps.textAttributes.fontWeight != newTextInputProps.textAttributes.fontWeight) {
    propBitsMask |= TXTBIT_CHARFORMATCHANGE;
    propBits |= TXTBIT_CHARFORMATCHANGE;
  }

  /*
  if (oldTextInputProps.textAttributes.foregroundColor != newTextInputProps.textAttributes.foregroundColor) {
    if (newTextInputProps.textAttributes.foregroundColor)
      m_element.Foreground(newTextInputProps.textAttributes.foregroundColor.AsWindowsBrush());
    else
      m_element.ClearValue(::xaml::Controls::TextBlock::ForegroundProperty());
  }

  if (oldTextInputProps.textAttributes.fontStyle != newTextInputProps.textAttributes.fontStyle) {
    switch (newTextInputProps.textAttributes.fontStyle.value_or(facebook::react::FontStyle::Normal)) {
      case facebook::react::FontStyle::Italic:
        m_element.FontStyle(winrt::Windows::UI::Text::FontStyle::Italic);
        break;
      case facebook::react::FontStyle::Normal:
        m_element.FontStyle(winrt::Windows::UI::Text::FontStyle::Normal);
        break;
      case facebook::react::FontStyle::Oblique:
        m_element.FontStyle(winrt::Windows::UI::Text::FontStyle::Oblique);
        break;
      default:
        assert(false);
    }
  }

  if (oldTextInputProps.textAttributes.fontFamily != newTextInputProps.textAttributes.fontFamily) {
    if (newTextInputProps.textAttributes.fontFamily.empty())
      m_element.FontFamily(xaml::Media::FontFamily(L"Segoe UI"));
    else
      m_element.FontFamily(xaml::Media::FontFamily(
          Microsoft::Common::Unicode::Utf8ToUtf16(newTextInputProps.textAttributes.fontFamily)));
  }

  if (oldTextInputProps.allowFontScaling != newTextInputProps.allowFontScaling) {
    m_element.IsTextScaleFactorEnabled(newTextInputProps.allowFontScaling);
  }

  if (oldTextInputProps.placeholder != newTextInputProps.placeholder) {
    m_element.PlaceholderText(winrt::to_hstring(newTextInputProps.placeholder));
  }

  if (oldTextInputProps.editable != newTextInputProps.editable) {
    m_element.IsReadOnly(!newTextInputProps.editable);
  }


    if (oldTextInputProps.multiline != newTextInputProps.multiline) {
      m_element.TextWrapping(newTextInputProps.multiline ? xaml::TextWrapping::Wrap : xaml::TextWrapping::NoWrap);
      m_element.AcceptsReturn(newTextInputProps.multiline);
    }


  if (oldTextInputProps.selection.start != newTextInputProps.selection.start ||
      oldTextInputProps.selection.end != newTextInputProps.selection.end) {
    m_element.Select(
        newTextInputProps.selection.start, newTextInputProps.selection.end - newTextInputProps.selection.start);
  }

  if (oldTextInputProps.autoCapitalize != newTextInputProps.autoCapitalize) {
    if (newTextInputProps.autoCapitalize == "characters") {
      m_element.CharacterCasing(xaml::Controls::CharacterCasing::Upper);
    } else { // anything else turns off autoCap (should be "None" but
             // we don't support "words"/"senetences" yet)
      m_element.CharacterCasing(xaml::Controls::CharacterCasing::Normal);
    }
  }

  if (oldViewProps.backgroundColor != newViewProps.backgroundColor) {
    auto color = *newViewProps.backgroundColor;

    if (newViewProps.backgroundColor) {
      m_element.ViewBackground(SolidColorBrushFrom(newViewProps.backgroundColor));
    } else {
      m_element.ClearValue(winrt::Microsoft::ReactNative::ViewPanel::ViewBackgroundProperty());
    }
  }
  */

  m_props = std::static_pointer_cast<facebook::react::WindowsTextInputProps const>(props);

  if (propBitsMask != 0) {
    DrawBlock db(*this);
    winrt::check_hresult(m_textServices->OnTxPropertyBitsChange(propBitsMask, propBits));
  }
}

void WindowsTextInputComponentView::updateState(
    facebook::react::State::Shared const &state,
    facebook::react::State::Shared const &oldState) noexcept {
  m_state = std::static_pointer_cast<facebook::react::WindowsTextInputShadowNode::ConcreteState const>(state);

  if (!m_state) {
    assert(false && "State is `null` for <TextInput> component.");
    // m_element.Text(L"");
    return;
  }

  auto data = m_state->getData();

  if (!oldState) {
    m_mostRecentEventCount = m_state->getData().mostRecentEventCount;
  }

  if (m_mostRecentEventCount == m_state->getData().mostRecentEventCount) {
    m_comingFromState = true;
    // Only handle single/empty fragments right now -- ignore the other fragments

    LRESULT res;
    CHARRANGE cr;
    cr.cpMin = cr.cpMax = 0;
    winrt::check_hresult(m_textServices->TxSendMessage(EM_EXGETSEL, 0, reinterpret_cast<LPARAM>(&cr), &res));

    UpdateText(
        m_state->getData().attributedString.getFragments().size()
            ? m_state->getData().attributedString.getFragments()[0].string
            : "");

    winrt::check_hresult(
        m_textServices->TxSendMessage(EM_SETSEL, static_cast<WPARAM>(cr.cpMin), static_cast<LPARAM>(cr.cpMax), &res));

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

  winrt::check_hresult(m_textServices->TxSendMessage(EM_EXGETSEL, 0, reinterpret_cast<LPARAM>(&cr), &res));
}

void WindowsTextInputComponentView::updateLayoutMetrics(
    facebook::react::LayoutMetrics const &layoutMetrics,
    facebook::react::LayoutMetrics const &oldLayoutMetrics) noexcept {
  // Set Position & Size Properties

  if ((layoutMetrics.displayType != m_layoutMetrics.displayType)) {
    m_visual.IsVisible(layoutMetrics.displayType != facebook::react::DisplayType::None);
  }

  if ((layoutMetrics.pointScaleFactor != m_layoutMetrics.pointScaleFactor)) {
    LRESULT res;
    winrt::check_hresult(m_textServices->TxSendMessage(
        (WM_USER + 328), // EM_SETDPI
        static_cast<WPARAM>(layoutMetrics.pointScaleFactor * 96.0f),
        static_cast<LPARAM>(layoutMetrics.pointScaleFactor * 96.0f),
        &res));
  }

  updateBorderLayoutMetrics(layoutMetrics, *m_props);

  m_layoutMetrics = layoutMetrics;

  // TODO should ceil?
  unsigned int newWidth = static_cast<unsigned int>(layoutMetrics.frame.size.width * layoutMetrics.pointScaleFactor);
  unsigned int newHeight = static_cast<unsigned int>(layoutMetrics.frame.size.height * layoutMetrics.pointScaleFactor);

  if (newWidth != m_imgWidth || newHeight != m_imgHeight) {
    m_drawingSurface = nullptr; // Invalidate surface if we get a size change
  }

  m_imgWidth = newWidth;
  m_imgHeight = newHeight;

  UpdateCenterPropertySet();
  m_visual.Size(
      {layoutMetrics.frame.size.width * layoutMetrics.pointScaleFactor,
       layoutMetrics.frame.size.height * layoutMetrics.pointScaleFactor});
  m_visual.Offset({
      layoutMetrics.frame.origin.x * layoutMetrics.pointScaleFactor,
      layoutMetrics.frame.origin.y * layoutMetrics.pointScaleFactor,
      0.0f,
  });
}

// When we are notified by RichEdit that the text changed, we need to notify JS
void WindowsTextInputComponentView::OnTextUpdated() noexcept {
  auto data = m_state->getData();
  // auto newAttributedString = getAttributedString();
  // if (data.attributedString == newAttributedString)
  //    return;
  data.attributedString = getAttributedString();
  data.mostRecentEventCount = m_nativeEventCount;
  m_state->updateState(std::move(data));

  if (m_eventEmitter && !m_comingFromJS) {
    auto emitter = std::static_pointer_cast<const facebook::react::WindowsTextInputEventEmitter>(m_eventEmitter);
    facebook::react::WindowsTextInputEventEmitter::OnChange onChangeArgs;
    onChangeArgs.text = GetTextFromRichEdit();
    onChangeArgs.eventCount = ++m_nativeEventCount;
    emitter->onChange(onChangeArgs);
  }
}

void WindowsTextInputComponentView::OnSelectionChanged(LONG start, LONG end) noexcept {
  if (m_eventEmitter /* && !m_comingFromJS ?? */) {
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
  if (*(str.end() - 1) == '\0') {
    str.pop_back();
  }
  if (*(str.end() - 1) == '\r') {
    str.pop_back();
  }
  SysFreeString(bstr);
  return str;
}

void WindowsTextInputComponentView::finalizeUpdates(RNComponentViewUpdateMask updateMask) noexcept {
  // m_element.FinalizeProperties();

  ensureDrawingSurface();
}
void WindowsTextInputComponentView::prepareForRecycle() noexcept {}
facebook::react::Props::Shared WindowsTextInputComponentView::props() noexcept {
  return m_props;
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
  float fontSize = m_props->textAttributes.fontSize;
  if (std::isnan(fontSize))
    fontSize = facebook::react::TextAttributes::defaultTextAttributes().fontSize;
  // TODO get fontSize from m_props->textAttributes, or defaultTextAttributes, or fragment?
  cfNew.dwMask |= CFM_SIZE;
  cfNew.yHeight = static_cast<LONG>(fontSize * 15);

  // set bold
  cfNew.dwMask |= CFM_WEIGHT;
  cfNew.wWeight = m_props->textAttributes.fontWeight ? static_cast<WORD>(*m_props->textAttributes.fontWeight)
                                                     : DWRITE_FONT_WEIGHT_NORMAL;

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

  // set char offset
  cfNew.dwMask |= CFM_OFFSET;
  cfNew.yOffset = 0;

  // set charset
  cfNew.dwMask |= CFM_CHARSET;
  cfNew.bCharSet = DEFAULT_CHARSET;

  m_cf = cfNew;
}

void WindowsTextInputComponentView::UpdateParaFormat() noexcept {
  ZeroMemory(&m_pf, sizeof(m_pf));

  m_pf.cbSize = sizeof(PARAFORMAT2);
  m_pf.dwMask = PFM_ALL;

  m_pf.wAlignment = PFA_LEFT;

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
  assert(m_context.UIDispatcher().HasThreadAccess());

  if (!m_drawingSurface) {
    m_drawingSurface = m_compContext.CreateDrawingSurface(
        {static_cast<float>(m_imgWidth), static_cast<float>(m_imgHeight)},
        winrt::Windows::Graphics::DirectX::DirectXPixelFormat::B8G8R8A8UIntNormalized,
        winrt::Windows::Graphics::DirectX::DirectXAlphaMode::Premultiplied);

    auto rcClient = getClientRect();
    winrt::check_hresult(m_textServices->OnTxInPlaceActivate(&rcClient));

    LRESULT lresult;
    winrt::check_hresult(
        m_textServices->TxSendMessage(EM_SETEVENTMASK, 0, ENM_CHANGE | ENM_SELCHANGE | ENM_ENDCOMPOSITION, &lresult));

    DrawText();

    auto surfaceBrush = m_compContext.CreateSurfaceBrush(m_drawingSurface);
    surfaceBrush.HorizontalAlignmentRatio(0.f);
    surfaceBrush.VerticalAlignmentRatio(0.f);
    surfaceBrush.Stretch(winrt::Microsoft::ReactNative::Composition::CompositionStretch::None);
    m_visual.Brush(surfaceBrush);
  }
}

void WindowsTextInputComponentView::ShowCaret(bool show) noexcept {
  ensureVisual();
  m_caretVisual.IsVisible(show);
}

void WindowsTextInputComponentView::DrawText() noexcept {
  m_needsRedraw = true;
  if (m_cDrawBlock) {
    return;
  }

  if (!m_drawingSurface)
    return;

  // Begin our update of the surface pixels. If this is our first update, we are required
  // to specify the entire surface, which nullptr is shorthand for (but, as it works out,
  // any time we make an update we touch the entire surface, so we always pass nullptr).
  winrt::com_ptr<ID2D1DeviceContext> d2dDeviceContext;
  POINT offset;

  assert(m_context.UIDispatcher().HasThreadAccess());

  winrt::com_ptr<Composition::ICompositionDrawingSurfaceInterop> drawingSurfaceInterop;
  m_drawingSurface.as(drawingSurfaceInterop);

  m_drawing = true;
  if (CheckForDeviceRemoved(drawingSurfaceInterop->BeginDraw(d2dDeviceContext.put(), &offset))) {
    d2dDeviceContext->Clear(D2D1::ColorF(D2D1::ColorF::Black, 0.0f));
    assert(d2dDeviceContext->GetUnitMode() == D2D1_UNIT_MODE_DIPS);
    const auto dpi = m_layoutMetrics.pointScaleFactor * 96.0f;
    float oldDpiX, oldDpiY;
    d2dDeviceContext->GetDpi(&oldDpiX, &oldDpiY);
    d2dDeviceContext->SetDpi(dpi, dpi);

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

    winrt::check_hresult(m_textServices->OnTxInPlaceActivate(&rcClient));

    // TODO keep track of proper invalid rect
    auto hrDraw = m_textServices->TxDrawD2D(d2dDeviceContext.get(), &rc, nullptr, TXTVIEW_ACTIVE);
    winrt::check_hresult(hrDraw);

    // restore dpi state
    d2dDeviceContext->SetDpi(oldDpiX, oldDpiY);

    // Our update is done. EndDraw never indicates rendering device removed, so any
    // failure here is unexpected and, therefore, fatal.
    auto hrEndDraw = drawingSurfaceInterop->EndDraw();
    winrt::check_hresult(hrEndDraw);
  }
  m_drawing = false;
  m_needsRedraw = false;
}

facebook::react::Tag WindowsTextInputComponentView::hitTest(facebook::react::Point pt, facebook::react::Point &localPt)
    const noexcept {
  facebook::react::Point ptLocal{pt.x - m_layoutMetrics.frame.origin.x, pt.y - m_layoutMetrics.frame.origin.y};

  facebook::react::Tag targetTag;

  /*
    if ((m_props.pointerEvents == facebook::react::PointerEventsMode::Auto ||
        m_props.pointerEvents == facebook::react::PointerEventsMode::BoxNone) && std::any_of(m_children.rbegin(),
    m_children.rend(), [&targetTag, &ptLocal, &localPt](auto child) { targetTag = static_cast<const
    CompositionBaseComponentView
    *>(child)->hitTest(ptLocal, localPt); return targetTag != -1;
        }))
      return targetTag;
      */

  if ((m_props->pointerEvents == facebook::react::PointerEventsMode::Auto ||
       m_props->pointerEvents == facebook::react::PointerEventsMode::BoxOnly) &&
      ptLocal.x >= 0 && ptLocal.x <= m_layoutMetrics.frame.size.width && ptLocal.y >= 0 &&
      ptLocal.y <= m_layoutMetrics.frame.size.height) {
    localPt = ptLocal;
    return tag();
  }

  return -1;
}

facebook::react::SharedTouchEventEmitter WindowsTextInputComponentView::touchEventEmitter() noexcept {
  return m_eventEmitter;
}

void WindowsTextInputComponentView::ensureVisual() noexcept {
  if (!m_visual) {
    HrEnsureRichEd20Loaded();
    m_visual = m_compContext.CreateSpriteVisual();
    m_textHost = winrt::make<CompTextHost>(this);
    winrt::com_ptr<IUnknown> spUnk;
    winrt::check_hresult(g_pfnCreateTextServices(nullptr, m_textHost.get(), spUnk.put()));
    spUnk.as(m_textServices);
  }

  if (!m_caretVisual) {
    m_caretVisual = m_compContext.CreateCaretVisual();
    m_visual.InsertAt(m_caretVisual.InnerVisual(), 0);
    m_caretVisual.IsVisible(false);
  }
}

winrt::Microsoft::ReactNative::Composition::IVisual WindowsTextInputComponentView::Visual() const noexcept {
  return m_visual;
}

} // namespace Microsoft::ReactNative
