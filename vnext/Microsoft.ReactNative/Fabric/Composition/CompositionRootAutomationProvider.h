#pragma once

#include <Fabric/Composition/RootComponentView.h>
#include <Fabric/ReactTaggedView.h>
#include <UIAutomation.h>
#include <inspectable.h>

namespace winrt::Microsoft::ReactNative::implementation {
struct CompositionRootView;

class CompositionRootAutomationProvider : public winrt::implements<
                                              CompositionRootAutomationProvider,
                                              IInspectable,
                                              IRawElementProviderFragmentRoot,
                                              IRawElementProviderFragment,
                                              IRawElementProviderSimple> {
 public:
  // inherited via IRawElementProviderFragmentRoot
  virtual HRESULT __stdcall ElementProviderFromPoint(double x, double y, IRawElementProviderFragment **pRetVal)
      override;
  virtual HRESULT __stdcall GetFocus(IRawElementProviderFragment **pRetVal) override;

  // inherited via IRawElementProviderFragment
  virtual HRESULT __stdcall Navigate(NavigateDirection direction, IRawElementProviderFragment **pRetVal) override;
  virtual HRESULT __stdcall GetRuntimeId(SAFEARRAY **pRetVal) override;
  virtual HRESULT __stdcall get_BoundingRectangle(UiaRect *pRetVal) override;
  virtual HRESULT __stdcall GetEmbeddedFragmentRoots(SAFEARRAY **pRetVal) override;
  virtual HRESULT __stdcall SetFocus(void) override;
  virtual HRESULT __stdcall get_FragmentRoot(IRawElementProviderFragmentRoot **pRetVal) override;

  // inherited via IRawElementProviderSimple
  virtual HRESULT __stdcall get_ProviderOptions(ProviderOptions *pRetVal) override;
  virtual HRESULT __stdcall GetPatternProvider(PATTERNID patternId, IUnknown **pRetVal) override;
  virtual HRESULT __stdcall GetPropertyValue(PROPERTYID propertyId, VARIANT *pRetVal) override;
  virtual HRESULT __stdcall get_HostRawElementProvider(IRawElementProviderSimple **pRetVal) override;

  CompositionRootAutomationProvider(
      const std::shared_ptr<::Microsoft::ReactNative::RootComponentView> &componentView) noexcept;
  void SetHwnd(HWND hwnd) noexcept;

 private:
  ::Microsoft::ReactNative::ReactTaggedView m_view;
  HWND m_hwnd{nullptr};
};

} // namespace winrt::Microsoft::ReactNative::implementation
