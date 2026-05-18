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
    const winrt::Microsoft::ReactNative::ComponentView &componentView) noexcept
    : m_view{componentView} {}

HRESULT __stdcall CompositionTextRangeProvider::Clone(ITextRangeProvider **pRetVal) {
  if (pRetVal == nullptr)
    return E_POINTER;

  auto clone = winrt::make<winrt::Microsoft::ReactNative::implementation::CompositionTextRangeProvider>(m_view.view());
  *pRetVal = clone.detach();
  return S_OK;
}

HRESULT __stdcall CompositionTextRangeProvider::Compare(ITextRangeProvider *range, BOOL *pRetVal) {
  if (pRetVal == nullptr)
    return E_POINTER;
  if (range == nullptr) {
    *pRetVal = FALSE;
    return S_OK;
  }

  // Try to cast to our type , considering provider only supports a single range per view
  auto other = dynamic_cast<CompositionTextRangeProvider *>(range);
  if (other && other->m_view.view() == m_view.view()) {
    *pRetVal = TRUE;
  } else {
    *pRetVal = FALSE;
  }
  return S_OK;
}

HRESULT __stdcall CompositionTextRangeProvider::CompareEndpoints(
    TextPatternRangeEndpoint endpoint,
    ITextRangeProvider *targetRange,
    TextPatternRangeEndpoint targetEndpoint,
    int *pRetVal) {
  if (pRetVal == nullptr)
    return E_POINTER;

  // For a single-range provider, always equal:
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

  auto props = std::static_pointer_cast<const facebook::react::BaseViewProps>(
      winrt::get_self<ComponentView>(strongView)->props());

  auto asParagraph =
      strongView.try_as<winrt::Microsoft::ReactNative::Composition::implementation::ParagraphComponentView>();

  auto asTextInput =
      strongView.try_as<winrt::Microsoft::ReactNative::Composition::implementation::WindowsTextInputComponentView>();

  if (props == nullptr)
    return UIA_E_ELEMENTNOTAVAILABLE;

  if (attributeId == UIA_BackgroundColorAttributeId) {
    pRetVal->vt = VT_I4;
    pRetVal->lVal = (*props->backgroundColor).AsColorRefWithAlpha();
  } else if (attributeId == UIA_CapStyleAttributeId && asParagraph) {
    pRetVal->vt = VT_I4;
    auto fontVariant = facebook::react::FontVariant::Default;
    auto textTransform = facebook::react::TextTransform::None;

    if (asParagraph->paragraphProps().textAttributes.fontVariant.has_value()) {
      fontVariant = asParagraph->paragraphProps().textAttributes.fontVariant.value();
    }
    if (asParagraph->paragraphProps().textAttributes.textTransform.has_value()) {
      textTransform = asParagraph->paragraphProps().textAttributes.textTransform.value();
    }
    if (fontVariant == facebook::react::FontVariant::SmallCaps) {
      pRetVal->lVal = CapStyle_SmallCap;
    } else if (textTransform == facebook::react::TextTransform::Capitalize) {
      pRetVal->lVal = CapStyle_Titling;
    } else if (textTransform == facebook::react::TextTransform::Lowercase) {
      pRetVal->lVal = CapStyle_None;
    } else if (textTransform == facebook::react::TextTransform::Uppercase) {
      pRetVal->lVal = CapStyle_AllCap;
    }
  } else if (attributeId == UIA_FontNameAttributeId && asParagraph) {
    pRetVal->vt = VT_BSTR;
    auto fontName = asParagraph->paragraphProps().textAttributes.fontFamily;
    if (fontName.empty()) {
      fontName = "Segoe UI";
    }
    std::wstring wfontName(fontName.begin(), fontName.end());
    pRetVal->bstrVal = SysAllocString(wfontName.c_str());
  } else if (attributeId == UIA_FontSizeAttributeId && asParagraph) {
    pRetVal->vt = VT_R8;
    pRetVal->dblVal = asParagraph->paragraphProps().textAttributes.fontSize;
  } else if (attributeId == UIA_FontWeightAttributeId && asParagraph) {
    if (asParagraph->paragraphProps().textAttributes.fontWeight.has_value()) {
      pRetVal->vt = VT_I4;
      pRetVal->lVal = static_cast<long>(asParagraph->paragraphProps().textAttributes.fontWeight.value());
    }
  } else if (attributeId == UIA_ForegroundColorAttributeId && asParagraph) {
    pRetVal->vt = VT_I4;
    pRetVal->lVal = (*asParagraph->paragraphProps().textAttributes.foregroundColor).AsColorRefWithAlpha();
  } else if (attributeId == UIA_IsItalicAttributeId && asParagraph) {
    pRetVal->vt = VT_BOOL;
    pRetVal->boolVal =
        (asParagraph->paragraphProps().textAttributes.fontStyle.has_value() &&
         asParagraph->paragraphProps().textAttributes.fontStyle.value() == facebook::react::FontStyle::Italic)
        ? VARIANT_TRUE
        : VARIANT_FALSE;
  } else if (attributeId == UIA_IsReadOnlyAttributeId) {
    pRetVal->vt = VT_BOOL;
    if (asTextInput) {
      pRetVal->boolVal = asTextInput->windowsTextInputProps().editable ? VARIANT_FALSE : VARIANT_TRUE;
    } else {
      pRetVal->boolVal = VARIANT_TRUE;
    }
  } else if (attributeId == UIA_HorizontalTextAlignmentAttributeId && asParagraph) {
    pRetVal->vt = VT_I4;
    auto textAlign = facebook::react::TextAlignment::Center;
    if (asParagraph->paragraphProps().textAttributes.alignment.has_value()) {
      textAlign = asParagraph->paragraphProps().textAttributes.alignment.value();
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
  } else if (attributeId == UIA_StrikethroughColorAttributeId && asParagraph) {
    if (asParagraph->paragraphProps().textAttributes.textDecorationLineType.has_value() &&
        (asParagraph->paragraphProps().textAttributes.textDecorationLineType.value() ==
             facebook::react::TextDecorationLineType::Strikethrough ||
         asParagraph->paragraphProps().textAttributes.textDecorationLineType.value() ==
             facebook::react::TextDecorationLineType::UnderlineStrikethrough)) {
      pRetVal->vt = VT_I4;
      pRetVal->lVal = (*asParagraph->paragraphProps().textAttributes.textDecorationColor).AsColorRefWithAlpha();
    }
  } else if (attributeId == UIA_StrikethroughStyleAttributeId && asParagraph) {
    if (asParagraph->paragraphProps().textAttributes.textDecorationLineType.has_value() &&
        (asParagraph->paragraphProps().textAttributes.textDecorationLineType.value() ==
             facebook::react::TextDecorationLineType::Strikethrough ||
         asParagraph->paragraphProps().textAttributes.textDecorationLineType.value() ==
             facebook::react::TextDecorationLineType::UnderlineStrikethrough)) {
      pRetVal->vt = VT_I4;
      auto style = asParagraph->paragraphProps().textAttributes.textDecorationStyle.value();
      pRetVal->lVal = GetTextDecorationLineStyle(style);
    }
  } else if (attributeId == UIA_UnderlineColorAttributeId && asParagraph) {
    if (asParagraph->paragraphProps().textAttributes.textDecorationLineType.has_value() &&
        (asParagraph->paragraphProps().textAttributes.textDecorationLineType.value() ==
             facebook::react::TextDecorationLineType::Underline ||
         asParagraph->paragraphProps().textAttributes.textDecorationLineType.value() ==
             facebook::react::TextDecorationLineType::UnderlineStrikethrough)) {
      pRetVal->vt = VT_I4;
      pRetVal->lVal = (*asParagraph->paragraphProps().textAttributes.textDecorationColor).AsColorRefWithAlpha();
    }
  } else if (attributeId == UIA_UnderlineStyleAttributeId && asParagraph) {
    if (asParagraph->paragraphProps().textAttributes.textDecorationLineType.has_value() &&
        (asParagraph->paragraphProps().textAttributes.textDecorationLineType.value() ==
             facebook::react::TextDecorationLineType::Underline ||
         asParagraph->paragraphProps().textAttributes.textDecorationLineType.value() ==
             facebook::react::TextDecorationLineType::UnderlineStrikethrough)) {
      pRetVal->vt = VT_I4;
      auto style = asParagraph->paragraphProps().textAttributes.textDecorationStyle.value();
      pRetVal->lVal = GetTextDecorationLineStyle(style);
    }
  }
  return S_OK;
}

HRESULT __stdcall CompositionTextRangeProvider::GetBoundingRectangles(SAFEARRAY **pRetVal) {
  if (pRetVal == nullptr)
    return E_POINTER;
  UiaRect rect;

  auto strongView = m_view.view();
  if (!strongView)
    return UIA_E_ELEMENTNOTAVAILABLE;

  auto componentView = strongView.as<winrt::Microsoft::ReactNative::implementation::ComponentView>();
  auto provider = componentView->EnsureUiaProvider();
  auto repf = provider.try_as<IRawElementProviderFragment>();
  if (!repf)
    return E_FAIL;

  auto hr = repf->get_BoundingRectangle(&rect);
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

// All the below methods should be implemented once the selection comes for paragraph and TextInput

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
