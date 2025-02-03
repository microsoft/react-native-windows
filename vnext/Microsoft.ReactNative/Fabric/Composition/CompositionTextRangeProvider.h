#pragma once

#include <Fabric/Composition/CompositionDynamicAutomationProvider.h>
#include <Fabric/Composition/CompositionTextProvider.h>
#include <Fabric/Composition/CompositionViewComponentView.h>
#include <Fabric/ReactTaggedView.h>
#include <UIAutomation.h>
#include <inspectable.h>
#include <uiautomationcore.h>

namespace winrt::Microsoft::ReactNative::implementation {

class CompositionTextRangeProvider : public winrt::implements<CompositionTextRangeProvider, ITextRangeProvider> {
 public:
  CompositionTextRangeProvider(
      const winrt::Microsoft::ReactNative::Composition::ComponentView &componentView,
      CompositionDynamicAutomationProvider *parentProvider) noexcept;

  // inherited via ITextRangeProvider
  virtual HRESULT __stdcall Clone(ITextRangeProvider **pRetVal) override;
  virtual HRESULT __stdcall Compare(ITextRangeProvider *range, BOOL *pRetVal) override;
  virtual HRESULT __stdcall CompareEndpoints(
      TextPatternRangeEndpoint endpoint,
      ITextRangeProvider *targetRange,
      TextPatternRangeEndpoint targetEndpoint,
      int *pRetVal) override;
  virtual HRESULT __stdcall ExpandToEnclosingUnit(TextUnit unit) override;
  virtual HRESULT __stdcall FindAttribute(
      TEXTATTRIBUTEID attributeId,
      VARIANT val,
      BOOL backward,
      ITextRangeProvider **pRetVal) override;
  virtual HRESULT __stdcall FindText(BSTR text, BOOL backward, BOOL ignoreCase, ITextRangeProvider **pRetVal) override;
  virtual HRESULT __stdcall GetAttributeValue(TEXTATTRIBUTEID attributeId, VARIANT *pRetVal) override;
  virtual HRESULT __stdcall GetBoundingRectangles(SAFEARRAY **pRetVal) override;
  virtual HRESULT __stdcall GetChildren(SAFEARRAY **pRetVal) override;
  virtual HRESULT __stdcall GetEnclosingElement(IRawElementProviderSimple **pRetVal) override;
  virtual HRESULT __stdcall GetText(int maxLength, BSTR *pRetVal) override;
  virtual HRESULT __stdcall Move(TextUnit unit, int count, int *pRetVal) override;
  virtual HRESULT __stdcall MoveEndpointByRange(
      TextPatternRangeEndpoint endpoint,
      ITextRangeProvider *targetRange,
      TextPatternRangeEndpoint targetEndpoint) override;
  virtual HRESULT __stdcall MoveEndpointByUnit(
      TextPatternRangeEndpoint endpoint,
      TextUnit unit,
      int count,
      int *pRetVal) override;
  virtual HRESULT __stdcall ScrollIntoView(BOOL alignToTop) override;
  virtual HRESULT __stdcall AddToSelection() override;
  virtual HRESULT __stdcall RemoveFromSelection() override;
  virtual HRESULT __stdcall Select() override;

 private:
  ::Microsoft::ReactNative::ReactTaggedView m_view;
  winrt::com_ptr<CompositionDynamicAutomationProvider> m_parentProvider;
};

} // namespace winrt::Microsoft::ReactNative::implementation
