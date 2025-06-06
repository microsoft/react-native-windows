#pragma once

#include <Fabric/Composition/CompositionDynamicAutomationProvider.h>
#include <Fabric/Composition/CompositionViewComponentView.h>
#include <Fabric/ReactTaggedView.h>
#include <UIAutomation.h>
#include <inspectable.h>
#include <uiautomationcore.h>

namespace winrt::Microsoft::ReactNative::implementation {

class CompositionAnnotationProvider : public winrt::implements<CompositionAnnotationProvider, IAnnotationProvider> {
 public:
  CompositionAnnotationProvider(
      const winrt::Microsoft::ReactNative::Composition::ComponentView &componentView,
      CompositionDynamicAutomationProvider *parentProvider) noexcept;

  // inherited via IAnnotationProvider
  virtual HRESULT __stdcall get_AnnotationTypeId(int *retVal) override;
  virtual HRESULT __stdcall get_AnnotationTypeName(BSTR *retVal) override;
  virtual HRESULT __stdcall get_Author(BSTR *retVal) override;
  virtual HRESULT __stdcall get_DateTime(BSTR *retVal) override;
  virtual HRESULT __stdcall get_Target(IRawElementProviderSimple **retVal) override;

 private:
  ::Microsoft::ReactNative::ReactTaggedView m_view;
  winrt::com_ptr<IAnnotationProvider> m_annotationProvider;
  winrt::com_ptr<CompositionDynamicAutomationProvider> m_parentProvider;
};

} // namespace winrt::Microsoft::ReactNative::implementation
