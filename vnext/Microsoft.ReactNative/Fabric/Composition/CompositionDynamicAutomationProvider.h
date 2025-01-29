#pragma once

#include <Fabric/Composition/CompositionViewComponentView.h>
#include <Fabric/ReactTaggedView.h>
#include <UIAutomation.h>
#include <inspectable.h>
#include <uiautomationcore.h>

namespace winrt::Microsoft::ReactNative::implementation {

class CompositionDynamicAutomationProvider : public winrt::implements<
                                                 CompositionDynamicAutomationProvider,
                                                 IInspectable,
                                                 IRawElementProviderFragment,
                                                 IRawElementProviderSimple,
                                                 IInvokeProvider,
                                                 IScrollItemProvider,
                                                 IValueProvider,
                                                 IRangeValueProvider,
                                                 IToggleProvider,
                                                 IExpandCollapseProvider,
                                                 ISelectionProvider,
                                                 ISelectionItemProvider,
                                                 ITextProvider> {
 public:
  CompositionDynamicAutomationProvider(
      const winrt::Microsoft::ReactNative::Composition::ComponentView &componentView) noexcept;

  // inherited via IRawElementProviderFragment
  virtual HRESULT __stdcall Navigate(NavigateDirection direction, IRawElementProviderFragment **pRetVal) override;
  virtual HRESULT __stdcall GetRuntimeId(SAFEARRAY **pRetVal) override;
  virtual HRESULT __stdcall get_BoundingRectangle(UiaRect *pRetVal) override;
  virtual HRESULT __stdcall GetEmbeddedFragmentRoots(SAFEARRAY **pRetVal) override;
  virtual HRESULT __stdcall SetFocus(void) override;
  virtual HRESULT __stdcall get_FragmentRoot(IRawElementProviderFragmentRoot **pRetVal) override;

  // inherited via IRawElementProviderSimple2
  virtual HRESULT __stdcall get_ProviderOptions(ProviderOptions *pRetVal) override;
  virtual HRESULT __stdcall GetPatternProvider(PATTERNID patternId, IUnknown **pRetVal) override;
  virtual HRESULT __stdcall GetPropertyValue(PROPERTYID propertyId, VARIANT *pRetVal) override;
  virtual HRESULT __stdcall get_HostRawElementProvider(IRawElementProviderSimple **pRetVal) override;
  // virtual HRESULT __stdcall ShowContextMenu() noexcept override;

  // inherited via IInvokeProvider
  virtual HRESULT __stdcall Invoke() override;

  // inherited via IScrollItemProvider
  HRESULT __stdcall ScrollIntoView() override;

  // inherited via IValueProvider
  virtual HRESULT __stdcall SetValue(LPCWSTR val) override;
  virtual HRESULT __stdcall get_Value(BSTR *pRetVal) override;
  virtual HRESULT __stdcall get_IsReadOnly(BOOL *pRetVal) override;

  // inherited via IRangeValueProvider
  virtual HRESULT __stdcall get_LargeChange(double *pRetVal) override;
  virtual HRESULT __stdcall get_Maximum(double *pRetVal) override;
  virtual HRESULT __stdcall get_Minimum(double *pRetVal) override;
  virtual HRESULT __stdcall get_SmallChange(double *pRetVal) override;
  virtual HRESULT __stdcall get_Value(double *pRetVal) override;
  virtual HRESULT __stdcall SetValue(double val) override;

  // inherited via IToggleProvider
  virtual HRESULT __stdcall get_ToggleState(ToggleState *pRetVal) override;
  virtual HRESULT __stdcall Toggle() override;

  // inherited via IExpandCollapseProvider
  virtual HRESULT __stdcall get_ExpandCollapseState(ExpandCollapseState *pRetVal) override;
  virtual HRESULT __stdcall Expand() override;
  virtual HRESULT __stdcall Collapse() override;

  // inherited via ISelectionProvider
  virtual HRESULT __stdcall get_CanSelectMultiple(BOOL *pRetVal) override;
  virtual HRESULT __stdcall get_IsSelectionRequired(BOOL *pRetVal) override;
  virtual HRESULT __stdcall GetSelection(SAFEARRAY **pRetVal) override;

  // inherited via ISelectionItemProvider
  virtual HRESULT __stdcall AddToSelection() override;
  virtual HRESULT __stdcall get_IsSelected(BOOL *pRetVal) override;
  virtual HRESULT __stdcall get_SelectionContainer(IRawElementProviderSimple **pRetVal) override;
  virtual HRESULT __stdcall RemoveFromSelection() override;
  virtual HRESULT __stdcall Select() override;

  // inherited via ITextProvider
  virtual HRESULT __stdcall get_DocumentRange(ITextRangeProvider **pRetVal) override;
  virtual HRESULT __stdcall get_SupportedTextSelection(SupportedTextSelection *pRetVal) override;
  // virtual HRESULT __stdcall GetSelection(SAFEARRAY **pRetVal) override;
  virtual HRESULT __stdcall GetVisibleRanges(SAFEARRAY **pRetVal) override;
  virtual HRESULT __stdcall RangeFromChild(IRawElementProviderSimple *childElement, ITextRangeProvider **pRetVal)
      override;
  virtual HRESULT __stdcall RangeFromPoint(UiaPoint point, ITextRangeProvider **pRetVal) override;

  // inherited via ITextProvider2
  /* virtual HRESULT __stdcall GetCaretRange(BOOL *isActive, ITextRangeProvider **pRetVal) override;
  virtual HRESULT __stdcall RangeFromAnnotation(
      IRawElementProviderSimple *annotationElement,
      ITextRangeProvider **pRetVal) override;*/

  void AddToSelectionItems(winrt::com_ptr<IRawElementProviderSimple> &item);
  void RemoveFromSelectionItems(winrt::com_ptr<IRawElementProviderSimple> &item);
  void EnsureTextRangeProvider();

 private:
  ::Microsoft::ReactNative::ReactTaggedView m_view;
  winrt::com_ptr<ITextRangeProvider> m_textRangeProvider;
  std::vector<winrt::com_ptr<IRawElementProviderSimple>> m_selectionItems;
};

} // namespace winrt::Microsoft::ReactNative::implementation
