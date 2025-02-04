#include "pch.h"
#include "CompositionTextRangeProvider.h"
#include <Fabric/ComponentView.h>
#include <Fabric/Composition/ParagraphComponentView.h>
#include <Fabric/Composition/TextInput/WindowsTextInputComponentView.h>
#include <Fabric/platform/react/renderer/graphics/HostPlatformColor.h>
#include <Unicode.h>
#include "RootComponentView.h"
#include "UiaHelpers.h"

namespace winrt::Microsoft::ReactNative::implementation {

CompositionTextRangeProvider::CompositionTextRangeProvider(
    const winrt::Microsoft::ReactNative::Composition::ComponentView &componentView,
    CompositionDynamicAutomationProvider *parentProvider) noexcept
    : m_view{componentView} {
  m_parentProvider.copy_from(parentProvider);
}

HRESULT __stdcall CompositionTextRangeProvider::Clone(ITextRangeProvider **pRetVal) {
  // no-op
  *pRetVal = nullptr;
  return S_OK;
}

HRESULT __stdcall CompositionTextRangeProvider::Compare(ITextRangeProvider *range, BOOL *pRetVal) {
  // no-op
  *pRetVal = false;
  return S_OK;
}

HRESULT __stdcall CompositionTextRangeProvider::CompareEndpoints(
    TextPatternRangeEndpoint endpoint,
    ITextRangeProvider *targetRange,
    TextPatternRangeEndpoint targetEndpoint,
    int *pRetVal) {
  // no-op
  *pRetVal = 0;
  return S_OK;
}

HRESULT __stdcall CompositionTextRangeProvider::ExpandToEnclosingUnit(TextUnit unit) {
  // no-op
  return S_OK;
}

HRESULT __stdcall CompositionTextRangeProvider::FindAttribute(
    TEXTATTRIBUTEID attributeId,
    VARIANT val,
    BOOL backward,
    ITextRangeProvider **pRetVal) {
  // no-op
  *pRetVal = nullptr;
  return S_OK;
}

HRESULT __stdcall CompositionTextRangeProvider::FindText(
    BSTR text,
    BOOL backward,
    BOOL ignoreCase,
    ITextRangeProvider **pRetVal) {
  // no-op
  *pRetVal = nullptr;
  return S_OK;
}

HRESULT __stdcall CompositionTextRangeProvider::GetAttributeValue(TEXTATTRIBUTEID attributeId, VARIANT *pRetVal) {
  if (pRetVal == nullptr)
    return E_POINTER;
  auto strongView = m_view.view();

  if (!strongView)
    return UIA_E_ELEMENTNOTAVAILABLE;

  auto props = std::static_pointer_cast<const facebook::react::ParagraphProps>(
      winrt::get_self<ComponentView>(strongView)->props());

  auto textinputProps = std::static_pointer_cast<const facebook::react::WindowsTextInputProps>(
      winrt::get_self<ComponentView>(strongView)->props());

  auto isTextInput =
      strongView.try_as<winrt::Microsoft::ReactNative::Composition::implementation::WindowsTextInputComponentView>();

  if (props == nullptr)
    return UIA_E_ELEMENTNOTAVAILABLE;

  if (attributeId == UIA_BackgroundColorAttributeId) {
    pRetVal->vt = VT_I4;
    pRetVal->lVal = (*props->backgroundColor).AsColorRefWithAlpha();
  } else if (attributeId == UIA_CapStyleAttributeId) {
    pRetVal->vt = VT_I4;
    auto fontVariant = facebook::react::FontVariant::Default;
    auto textTransform = facebook::react::TextTransform::None;
    if (props->textAttributes.fontVariant.has_value()) {
      fontVariant = props->textAttributes.fontVariant.value();
    }
    if (props->textAttributes.textTransform.has_value()) {
      textTransform = props->textAttributes.textTransform.value();
    }
    if (fontVariant == facebook::react::FontVariant::SmallCaps) {
      return CapStyle_SmallCap;
    } else if (textTransform == facebook::react::TextTransform::Capitalize) {
      return CapStyle_Titling;
    } else if (textTransform == facebook::react::TextTransform::Lowercase) {
      return CapStyle_None;
    } else if (textTransform == facebook::react::TextTransform::Uppercase) {
      return CapStyle_AllCap;
    }
  } else if (attributeId == UIA_FontNameAttributeId) {
    pRetVal->vt = VT_BSTR;
    auto fontName = props->textAttributes.fontFamily;
    if (fontName.empty()) {
      fontName = "Segoe UI";
    }
    std::wstring wfontName(fontName.begin(), fontName.end());
    pRetVal->bstrVal = SysAllocString(wfontName.c_str());
  } else if (attributeId == UIA_FontSizeAttributeId) {
    pRetVal->vt = VT_R8;
    pRetVal->dblVal = props->textAttributes.fontSize;
  } else if (attributeId == UIA_FontWeightAttributeId) {
    if (props->textAttributes.fontWeight.has_value()) {
      pRetVal->vt = VT_I4;
      pRetVal->lVal = static_cast<long>(props->textAttributes.fontWeight.value());
    }
  } else if (attributeId == UIA_ForegroundColorAttributeId) {
    pRetVal->vt = VT_I4;
    pRetVal->lVal = (*props->textAttributes.foregroundColor).AsColorRefWithAlpha();
  } else if (attributeId == UIA_IsItalicAttributeId) {
    pRetVal->vt = VT_BOOL;
    pRetVal->boolVal = (props->textAttributes.fontStyle.has_value() &&
                        props->textAttributes.fontStyle.value() == facebook::react::FontStyle::Italic)
        ? VARIANT_TRUE
        : VARIANT_FALSE;
  } else if (attributeId == UIA_IsReadOnlyAttributeId) {
    pRetVal->vt = VT_BOOL;
    pRetVal->boolVal = isTextInput ? textinputProps->editable ? VARIANT_FALSE : VARIANT_TRUE : VARIANT_TRUE;
  } else if (attributeId == UIA_HorizontalTextAlignmentAttributeId) {
    pRetVal->vt = VT_I4;
    auto textAlign = facebook::react::TextAlignment::Center;
    if (props->textAttributes.alignment.has_value()) {
      textAlign = props->textAttributes.alignment.value();
    }
    if (textAlign == facebook::react::TextAlignment::Left) {
      pRetVal->lVal = HorizontalTextAlignment_Left;
    } else if (textAlign == facebook::react::TextAlignment::Right) {
      pRetVal->lVal = HorizontalTextAlignment_Right;
    } else if (textAlign == facebook::react::TextAlignment::Center) {
      pRetVal->lVal = HorizontalTextAlignment_Centered;
    } else if (textAlign == facebook::react::TextAlignment::Justified) {
      pRetVal->lVal = HorizontalTextAlignment_Justified;
    } else if (textAlign == facebook::react::TextAlignment::Natural) {
      pRetVal->lVal = HorizontalTextAlignment_Left;
    }
  } else if (attributeId == UIA_StrikethroughColorAttributeId) {
    if (props->textAttributes.textDecorationLineType.has_value() &&
        (props->textAttributes.textDecorationLineType.value() ==
             facebook::react::TextDecorationLineType::Strikethrough ||
         props->textAttributes.textDecorationLineType.value() ==
             facebook::react::TextDecorationLineType::UnderlineStrikethrough)) {
      pRetVal->vt = VT_I4;
      pRetVal->lVal = (*props->textAttributes.textDecorationColor).AsColorRefWithAlpha();
    }
  } else if (attributeId == UIA_StrikethroughStyleAttributeId) {
    if (props->textAttributes.textDecorationLineType.has_value() &&
        (props->textAttributes.textDecorationLineType.value() ==
             facebook::react::TextDecorationLineType::Strikethrough ||
         props->textAttributes.textDecorationLineType.value() ==
             facebook::react::TextDecorationLineType::UnderlineStrikethrough)) {
      pRetVal->vt = VT_I4;
      auto style = props->textAttributes.textDecorationStyle.value();
      pRetVal->lVal = GetTextDecorationLineStyle(style);
    }
  } else if (attributeId == UIA_UnderlineColorAttributeId) {
    if (props->textAttributes.textDecorationLineType.has_value() &&
        (props->textAttributes.textDecorationLineType.value() == facebook::react::TextDecorationLineType::Underline ||
         props->textAttributes.textDecorationLineType.value() ==
             facebook::react::TextDecorationLineType::UnderlineStrikethrough)) {
      pRetVal->vt = VT_I4;
      pRetVal->lVal = (*props->textAttributes.textDecorationColor).AsColorRefWithAlpha();
    }
  } else if (attributeId == UIA_UnderlineStyleAttributeId) {
    if (props->textAttributes.textDecorationLineType.has_value() &&
        (props->textAttributes.textDecorationLineType.value() == facebook::react::TextDecorationLineType::Underline ||
         props->textAttributes.textDecorationLineType.value() ==
             facebook::react::TextDecorationLineType::UnderlineStrikethrough)) {
      pRetVal->vt = VT_I4;
      auto style = props->textAttributes.textDecorationStyle.value();
      pRetVal->lVal = GetTextDecorationLineStyle(style);
    }
  }
  return S_OK;
}

HRESULT __stdcall CompositionTextRangeProvider::GetBoundingRectangles(SAFEARRAY **pRetVal) {
  if (pRetVal == nullptr)
    return E_POINTER;
  UiaRect rect;
  auto hr = m_parentProvider->get_BoundingRectangle(&rect);
  if (FAILED(hr))
    return hr;
  *pRetVal = SafeArrayCreateVector(VT_R8, 0, 4);
  double *pData = nullptr;
  hr = SafeArrayAccessData(*pRetVal, reinterpret_cast<void **>(&pData));
  if (FAILED(hr))
    return hr;
  pData[0] = rect.left;
  pData[1] = rect.top;
  pData[2] = rect.width;
  pData[3] = rect.height;
  hr = SafeArrayUnaccessData(*pRetVal);
  if (FAILED(hr))
    return hr;
  return S_OK;
}

HRESULT __stdcall CompositionTextRangeProvider::GetChildren(SAFEARRAY **pRetVal) {
  // no-op
  *pRetVal = SafeArrayCreateVector(VT_UNKNOWN, 0, 0);
  return S_OK;
}

HRESULT __stdcall CompositionTextRangeProvider::GetEnclosingElement(IRawElementProviderSimple **pRetVal) {
  // no-op
  *pRetVal = nullptr;
  return S_OK;
}

HRESULT __stdcall CompositionTextRangeProvider::GetText(int maxLength, BSTR *pRetVal) {
  if (pRetVal == nullptr)
    return E_POINTER;
  auto strongView = m_view.view();

  if (!strongView)
    return UIA_E_ELEMENTNOTAVAILABLE;
  auto paragraphView =
      strongView.try_as<winrt::Microsoft::ReactNative::Composition::implementation::ParagraphComponentView>();
  std::string text = "";
  if (paragraphView) {
    text = paragraphView->DefaultAccessibleName();
  } else {
    auto textInputView =
        strongView.try_as<winrt::Microsoft::ReactNative::Composition::implementation::WindowsTextInputComponentView>();
    if (textInputView) {
      text = textInputView->getAccessiblityValue().value().empty() ? textInputView->DefaultAccessibleName()
                                                                   : textInputView->getAccessiblityValue().value();
    } else {
      return UIA_E_NOTSUPPORTED;
    }
  }

  std::wstring wtext(text.begin(), text.end());
  *pRetVal = SysAllocString(wtext.c_str());
  return S_OK;
}

HRESULT __stdcall CompositionTextRangeProvider::Move(TextUnit unit, int count, int *pRetVal) {
  // no-op
  *pRetVal = 0;
  return S_OK;
}

HRESULT __stdcall CompositionTextRangeProvider::MoveEndpointByRange(
    TextPatternRangeEndpoint endpoint,
    ITextRangeProvider *targetRange,
    TextPatternRangeEndpoint targetEndpoint) {
  // no-op
  return S_OK;
}

HRESULT __stdcall CompositionTextRangeProvider::MoveEndpointByUnit(
    TextPatternRangeEndpoint endpoint,
    TextUnit unit,
    int count,
    int *pRetVal) {
  // no-op
  *pRetVal = 0;
  return S_OK;
}

HRESULT __stdcall CompositionTextRangeProvider::ScrollIntoView(BOOL alignToTop) {
  // no-op
  return S_OK;
}

HRESULT __stdcall CompositionTextRangeProvider::AddToSelection() {
  // no-op
  return S_OK;
}
HRESULT __stdcall CompositionTextRangeProvider::RemoveFromSelection() {
  // no-op
  return S_OK;
}
HRESULT __stdcall CompositionTextRangeProvider::Select() {
  // no-op
  return S_OK;
}

} // namespace winrt::Microsoft::ReactNative::implementation
