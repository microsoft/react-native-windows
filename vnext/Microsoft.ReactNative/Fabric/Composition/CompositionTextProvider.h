#pragma once

#include <Fabric/Composition/CompositionDynamicAutomationProvider.h>
#include <Fabric/Composition/CompositionViewComponentView.h>
#include <Fabric/ReactTaggedView.h>
#include <UIAutomation.h>
#include <inspectable.h>
#include <uiautomationcore.h>

namespace winrt::Microsoft::ReactNative::implementation {

class CompositionTextProvider : public winrt::implements<CompositionTextProvider, ITextProvider> {
 public:
  CompositionTextProvider(
      const winrt::Microsoft::ReactNative::Composition::ComponentView &componentView,
      CompositionDynamicAutomationProvider *parentProvider) noexcept;

  // inherited via ITextProvider
  virtual HRESULT __stdcall get_DocumentRange(ITextRangeProvider **pRetVal) override;
  virtual HRESULT __stdcall get_SupportedTextSelection(SupportedTextSelection *pRetVal) override;
  virtual HRESULT __stdcall GetSelection(SAFEARRAY **pRetVal) override;
  virtual HRESULT __stdcall GetVisibleRanges(SAFEARRAY **pRetVal) override;
  virtual HRESULT __stdcall RangeFromChild(IRawElementProviderSimple *childElement, ITextRangeProvider **pRetVal)
      override;
  virtual HRESULT __stdcall RangeFromPoint(UiaPoint point, ITextRangeProvider **pRetVal) override;

  void EnsureTextRangeProvider();

 private:
  ::Microsoft::ReactNative::ReactTaggedView m_view;
  winrt::com_ptr<ITextRangeProvider> m_textRangeProvider;
  winrt::com_ptr<CompositionDynamicAutomationProvider> m_parentProvider;
};

} // namespace winrt::Microsoft::ReactNative::implementation
