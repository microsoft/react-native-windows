#include "pch.h"
#include <Fabric/ComponentView.h>
#include <Fabric/Composition/ParagraphComponentView.h>
#include <Fabric/Composition/TextInput/WindowsTextInputComponentView.h>
#include <Unicode.h>
#include "CompositionTextRangeProvider.h"
#include "RootComponentView.h"
#include "UiaHelpers.h"

namespace winrt::Microsoft::ReactNative::implementation {

CompositionTextProvider::CompositionTextProvider(
    const winrt::Microsoft::ReactNative::Composition::ComponentView &componentView) noexcept
    : m_view{componentView} {
  EnsureTextRangeProvider();
}

void CompositionTextProvider::EnsureTextRangeProvider() {
  auto strongView = m_view.view();

  if (!strongView)
    return;

  if (!m_textRangeProvider) {
    m_textRangeProvider = winrt::make<CompositionTextRangeProvider>(
                              strongView.as<winrt::Microsoft::ReactNative::Composition::ComponentView>())
                              .as<ITextRangeProvider>();
  }
}

HRESULT __stdcall CompositionTextProvider::get_DocumentRange(ITextRangeProvider **pRetVal) {
  if (pRetVal == nullptr)
    return E_POINTER;
  auto strongView = m_view.view();

  if (!strongView)
    return UIA_E_ELEMENTNOTAVAILABLE;

  if (m_textRangeProvider == nullptr)
    return UIA_E_ELEMENTNOTAVAILABLE;

  m_textRangeProvider.copy_to(pRetVal);
  return S_OK;
}

HRESULT __stdcall CompositionTextProvider::get_SupportedTextSelection(SupportedTextSelection *pRetVal) {
  if (pRetVal == nullptr)
    return E_POINTER;
  auto strongView = m_view.view();

  if (!strongView)
    return UIA_E_ELEMENTNOTAVAILABLE;

  if (strongView.try_as<winrt::Microsoft::ReactNative::Composition::implementation::WindowsTextInputComponentView>()) {
    *pRetVal = SupportedTextSelection_Single;
  } else if (
      auto textView =
          strongView.try_as<winrt::Microsoft::ReactNative::Composition::implementation::ParagraphComponentView>()) {
    auto props = std::static_pointer_cast<const facebook::react::ParagraphProps>(textView->props());
    if (props == nullptr)
      return UIA_E_ELEMENTNOTAVAILABLE;
    *pRetVal = props->isSelectable ? SupportedTextSelection_Single : SupportedTextSelection_None;
  } else {
    *pRetVal = SupportedTextSelection_None;
  }

  return S_OK;
}

HRESULT __stdcall CompositionTextProvider::GetSelection(SAFEARRAY **pRetVal) {
  // no-op
  *pRetVal = SafeArrayCreateVector(VT_UNKNOWN, 0, 0);
  return S_OK;
}

HRESULT __stdcall CompositionTextProvider::GetVisibleRanges(SAFEARRAY **pRetVal) {
  *pRetVal = SafeArrayCreateVector(VT_UNKNOWN, 0, 1);
  if (m_textRangeProvider == nullptr)
    return UIA_E_ELEMENTNOTAVAILABLE;
  LONG pos = 0;
  return SafeArrayPutElement(*pRetVal, &pos, m_textRangeProvider.get());
}

HRESULT __stdcall CompositionTextProvider::RangeFromChild(
    IRawElementProviderSimple *childElement,
    ITextRangeProvider **pRetVal) {
  // no-op
  *pRetVal = nullptr;
  return S_OK;
}

HRESULT __stdcall CompositionTextProvider::RangeFromPoint(UiaPoint point, ITextRangeProvider **pRetVal) {
  // no-op
  if (m_textRangeProvider == nullptr)
    return UIA_E_ELEMENTNOTAVAILABLE;
  m_textRangeProvider.copy_to(pRetVal);
  return S_OK;
}
HRESULT __stdcall CompositionTextProvider::GetCaretRange(BOOL *isActive, ITextRangeProvider **pRetVal) {
  // no-op
  *pRetVal = nullptr;
  return S_OK;
}

HRESULT __stdcall CompositionTextProvider::RangeFromAnnotation(
    IRawElementProviderSimple *annotationElement,
    ITextRangeProvider **pRetVal) {
  // no-op
  *pRetVal = nullptr;
  return S_OK;
}
} // namespace winrt::Microsoft::ReactNative::implementation
