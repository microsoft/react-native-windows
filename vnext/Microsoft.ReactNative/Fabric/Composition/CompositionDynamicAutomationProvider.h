#pragma once

#include <UIAutomation.h>
#include <inspectable.h>
#include <Fabric/ComponentView.h>

namespace winrt::Microsoft::ReactNative::implementation {

class CompositionDynamicAutomationProvider : public winrt::implements<
  CompositionDynamicAutomationProvider,
  IInspectable,
  IRawElementProviderFragment,
  IRawElementProviderSimple2> {

public:
  CompositionDynamicAutomationProvider(const std::shared_ptr<IComponentView>& componentView) noexcept;

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
  virtual HRESULT __stdcall ShowContextMenu() noexcept override;


private:

  ReactTaggedView m_view;

};

}