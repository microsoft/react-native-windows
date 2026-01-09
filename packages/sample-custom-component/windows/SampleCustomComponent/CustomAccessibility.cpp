// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.
#include "pch.h"

#include "codegen/react/components/SampleCustomComponent/CustomAccessibility.g.h"

#ifdef RNW_NEW_ARCH
#include <winrt/Microsoft.ReactNative.Composition.Input.h>
#include <winrt/Microsoft.UI.Composition.h>
#include <winrt/Windows.UI.h>
#include <uiautomationcore.h>
#include <uiautomationclient.h>

namespace winrt::SampleCustomComponent {


struct CustomAccessibilityAutomationPeer : public winrt::implements<CustomAccessibilityAutomationPeer,
     winrt::IInspectable,
     IRawElementProviderFragment,
     IRawElementProviderSimple>
{

  CustomAccessibilityAutomationPeer(const winrt::Microsoft::ReactNative::CreateAutomationPeerArgs& args)
    : m_inner(args.DefaultAutomationPeer())
  {
  }

  virtual HRESULT __stdcall Navigate(NavigateDirection direction, IRawElementProviderFragment **pRetVal) override
  {
    winrt::com_ptr<IRawElementProviderFragment> innerAsREPF = m_inner.try_as<IRawElementProviderFragment>();
    if (!innerAsREPF)
      return E_FAIL;
    return innerAsREPF->Navigate(direction, pRetVal);
  }

  virtual HRESULT __stdcall GetRuntimeId(SAFEARRAY **pRetVal) override
  {
    winrt::com_ptr<IRawElementProviderFragment> innerAsREPF = m_inner.try_as<IRawElementProviderFragment>();
    if (!innerAsREPF)
      return E_FAIL;
    return innerAsREPF->GetRuntimeId(pRetVal);
  }

  virtual HRESULT __stdcall get_BoundingRectangle(UiaRect *pRetVal) override {
    winrt::com_ptr<IRawElementProviderFragment> innerAsREPF = m_inner.try_as<IRawElementProviderFragment>();
    if (!innerAsREPF)
      return E_FAIL;
    return innerAsREPF->get_BoundingRectangle(pRetVal);
  }

  virtual HRESULT __stdcall GetEmbeddedFragmentRoots(SAFEARRAY **pRetVal) override {
    winrt::com_ptr<IRawElementProviderFragment> innerAsREPF = m_inner.try_as<IRawElementProviderFragment>();
    if (!innerAsREPF)
      return E_FAIL;
    return innerAsREPF->GetEmbeddedFragmentRoots(pRetVal);
  }

  virtual HRESULT __stdcall SetFocus(void) override {
    winrt::com_ptr<IRawElementProviderFragment> innerAsREPF = m_inner.try_as<IRawElementProviderFragment>();
    if (!innerAsREPF)
      return E_FAIL;
    return innerAsREPF->SetFocus();
  }

  virtual HRESULT __stdcall get_FragmentRoot(IRawElementProviderFragmentRoot **pRetVal) override {
    winrt::com_ptr<IRawElementProviderFragment> innerAsREPF = m_inner.try_as<IRawElementProviderFragment>();
    if (!innerAsREPF)
      return E_FAIL;
    return innerAsREPF->get_FragmentRoot(pRetVal);
  }

  // inherited via IRawElementProviderSimple
  virtual HRESULT __stdcall get_ProviderOptions(ProviderOptions *pRetVal) override {
    winrt::com_ptr<IRawElementProviderSimple> innerAsREPS = m_inner.try_as<IRawElementProviderSimple>();
    if (!innerAsREPS)
      return E_FAIL;
    return innerAsREPS->get_ProviderOptions(pRetVal);
  }

  virtual HRESULT __stdcall GetPatternProvider(PATTERNID patternId, IUnknown **pRetVal) override {
    winrt::com_ptr<IRawElementProviderSimple> innerAsREPS = m_inner.try_as<IRawElementProviderSimple>();
    if (!innerAsREPS)
      return E_FAIL;
    return innerAsREPS->GetPatternProvider(patternId, pRetVal);
  }

  virtual HRESULT __stdcall GetPropertyValue(PROPERTYID propertyId, VARIANT *pRetVal) override {
    winrt::com_ptr<IRawElementProviderSimple> innerAsREPS = m_inner.try_as<IRawElementProviderSimple>();
    if (!innerAsREPS)
      return E_FAIL;

    if (propertyId == UIA_NamePropertyId) {
      pRetVal->vt = VT_BSTR;
      pRetVal->bstrVal = SysAllocString(L"accessiblity label from native");
      return pRetVal->bstrVal != nullptr ? S_OK : E_OUTOFMEMORY;
    }

    return innerAsREPS->GetPropertyValue(propertyId, pRetVal);
  }

  virtual HRESULT __stdcall get_HostRawElementProvider(IRawElementProviderSimple **pRetVal) override {
    winrt::com_ptr<IRawElementProviderSimple> innerAsREPS = m_inner.try_as<IRawElementProviderSimple>();
    if (!innerAsREPS)
      return E_FAIL;
    return innerAsREPS->get_HostRawElementProvider(pRetVal);
  }

private:
  winrt::Windows::Foundation::IInspectable m_inner;
};

struct CustomAccessibility : public winrt::implements<CustomAccessibility, winrt::IInspectable>, Codegen::BaseCustomAccessibility<CustomAccessibility> {

  virtual winrt::Windows::Foundation::IInspectable CreateAutomationPeer(
      const winrt::Microsoft::ReactNative::ComponentView & /*view*/,
      const winrt::Microsoft::ReactNative::CreateAutomationPeerArgs & args) noexcept override {
    return winrt::make<winrt::SampleCustomComponent::CustomAccessibilityAutomationPeer>(args);
  }
};

} // namespace winrt::SampleCustomComponent

void RegisterCustomAccessibilityComponentView(
    winrt::Microsoft::ReactNative::IReactPackageBuilder const &packageBuilder) noexcept {
  winrt::SampleCustomComponent::Codegen::RegisterCustomAccessibilityNativeComponent<
      winrt::SampleCustomComponent::CustomAccessibility>(packageBuilder, {});
}

#endif // #ifdef RNW_NEW_ARCH
