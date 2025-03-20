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
                                                 IScrollProvider,
                                                 IValueProvider,
                                                 IRangeValueProvider,
                                                 IToggleProvider,
                                                 IExpandCollapseProvider,
                                                 ISelectionProvider,
                                                 ISelectionItemProvider> {
 public:
  CompositionDynamicAutomationProvider(
      const winrt::Microsoft::ReactNative::Composition::ComponentView &componentView) noexcept;

#ifdef USE_EXPERIMENTAL_WINUI3
  CompositionDynamicAutomationProvider(
      const winrt::Microsoft::ReactNative::Composition::ComponentView &componentView,
      const winrt::Microsoft::UI::Content::ChildSiteLink &childContentLink) noexcept;
#endif // USE_EXPERIMENTAL_WINUI3

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

  // inherited via IScrollProvider
  HRESULT __stdcall get_HorizontalScrollPercent(double *pRetVal) override;
  HRESULT __stdcall get_VerticalScrollPercent(double *pRetVal) override;
  HRESULT __stdcall get_HorizontalViewSize(double *pRetVal) override;
  HRESULT __stdcall get_VerticalViewSize(double *pRetVal) override;
  HRESULT __stdcall get_HorizontallyScrollable(BOOL *pRetVal) override;
  HRESULT __stdcall get_VerticallyScrollable(BOOL *pRetVal) override;
  HRESULT __stdcall Scroll(ScrollAmount horizontalAmount, ScrollAmount verticalAmount) override;
  HRESULT __stdcall SetScrollPercent(double horiztonalPercent, double verticalPercent) override;

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

  void AddToSelectionItems(winrt::com_ptr<IRawElementProviderSimple> &item);
  void RemoveFromSelectionItems(winrt::com_ptr<IRawElementProviderSimple> &item);

 private:
  ::Microsoft::ReactNative::ReactTaggedView m_view;
  winrt::com_ptr<ITextProvider2> m_textProvider;
  std::vector<winrt::com_ptr<IRawElementProviderSimple>> m_selectionItems;
#ifdef USE_EXPERIMENTAL_WINUI3
  // Non-null when this UIA node is the peer of a ContentIslandComponentView.
  winrt::Microsoft::UI::Content::ChildSiteLink m_childSiteLink{nullptr};
#endif
};

} // namespace winrt::Microsoft::ReactNative::implementation
