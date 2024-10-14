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
                                                 IToggleProvider,
                                                 IExpandCollapseProvider> {
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

  // inherited via IToggleProvider
  virtual HRESULT __stdcall get_ToggleState(ToggleState *pRetVal) override;
  virtual HRESULT __stdcall Toggle() override;

  // inherited via IExpandCollapseProvider
  virtual HRESULT __stdcall get_ExpandCollapseState(ExpandCollapseState *pRetVal) override;
  virtual HRESULT __stdcall Expand() override;
  virtual HRESULT __stdcall Collapse() override;

 private:
  ::Microsoft::ReactNative::ReactTaggedView m_view;
};

} // namespace winrt::Microsoft::ReactNative::implementation
