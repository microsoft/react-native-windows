// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include <pch.h>

#include "comTemplateLibrary.h"
#include "HStringHelper.h"
#include "Instance_rt.h"
#include "ReactControl_rt.h"
#include <Windows.Foundation.h>
#include <windows.ui.xaml.controls.h>
#include <folly/json.h>

namespace ABI {
namespace react {
namespace uwp {

ActivatableClassWithFactory(ReactControl, ReactControlFactory);

ReactControl::ReactControl()
  : m_rendering(false)
{
  using namespace Microsoft::WRL;
  using namespace Microsoft::WRL::Wrappers;
  using namespace ABI::Windows::UI::Xaml::Controls;
  using namespace ABI::Windows::Foundation;

  ComPtr<IGrid> spControl;
  ComPtr<IGridFactory> spFactory;
  HRESULT hr = GetActivationFactory(
    Wrappers::HStringReference(RuntimeClass_Windows_UI_Xaml_Controls_Grid).Get(), &spFactory);

  spFactory->CreateInstance(static_cast<ABI::react::uwp::IReactControl*>(this), &_spInnerInspectable, &spControl);
  SetComposableBasePointers(_spInnerInspectable.Get(), spFactory.Get());
}

HRESULT ReactControl::get_Instance(ABI::react::uwp::IInstance** value)
{
  *value = _spInstance.Get();
  return S_OK;
}

HRESULT ReactControl::put_Instance(ABI::react::uwp::IInstance* value)
{
  if (m_pInnerControl)
    m_pInnerControl->SetInstanceCreator(static_cast<Instance*>(value)->GetReactInstanceCreator());
  _spInstance = value;
  return S_OK;
}

HRESULT ReactControl::get_JsComponentName(HSTRING * value)
{
  *value = StringToHSTRING(m_jsComponentName);;
  return S_OK;
}

HRESULT ReactControl::put_JsComponentName(HSTRING value)
{
  m_jsComponentName =  HSTRINGToString(value);
  return S_OK;
}

HRESULT ReactControl::get_InitialProps(HSTRING *value)
{
  *value = StringToHSTRING(m_initialProps);
  return S_OK;
}

HRESULT ReactControl::put_InitialProps(HSTRING value)
{
  m_initialProps = HSTRINGToString(value);
  return S_OK;
}

HRESULT ReactControl::StartRender()
{
  m_rendering = true;
  Refresh();

  return S_OK;
}

HRESULT ReactControl::StopRender()
{
  m_rendering = false;
  Refresh();

  return S_OK;
}

HRESULT ReactControlFactory::ActivateInstance(IInspectable ** ppInstance)
{
  return ctl::AggregableComFactory<ReactControl>::ActivateInstance(ppInstance);
}

HRESULT ReactControlFactory::CreateInstance(IInspectable * pOuter, IInspectable ** ppInner, ABI::react::uwp::IReactControl ** ppInstance)
{
  return ctl::AggregableComFactory<ReactControl>::CreateInstance(pOuter, ppInner, ppInstance);
}

void ReactControl::Refresh()
{
  if (m_rendering && !m_jsComponentName.empty())
  {
    if (m_pInnerControl == nullptr)
    {
      CreateInnerInstance();
    }
    else
    {
      m_pInnerControl->DetachRoot();
      m_pInnerControl = nullptr;
      CreateInnerInstance();
    }
  }
  else
  {
    if (m_pInnerControl != nullptr)
    {
      m_pInnerControl->DetachRoot();
      m_pInnerControl = nullptr;
    }
  }
}

void ReactControl::CreateInnerInstance()
{
  Microsoft::WRL::ComPtr<ABI::react::uwp::ReactControl> spThis(this);
  Microsoft::WRL::ComPtr<ABI::Windows::UI::Xaml::IFrameworkElement> spFrameworkElementABI;
  spThis.As(&spFrameworkElementABI);
  ::react::uwp::XamlView pXamlView = reinterpret_cast<const winrt::Windows::UI::Xaml::FrameworkElement&>(spFrameworkElementABI);

  m_pInnerControl = std::make_shared<::react::uwp::ReactControl>(this, pXamlView);
  m_pInnerControl->SetJSComponentName(std::string(m_jsComponentName));
  m_pInnerControl->SetInitialProps(folly::parseJson(m_initialProps));
  m_pInnerControl->SetInstanceCreator(static_cast<Instance*>(_spInstance.Get())->GetReactInstanceCreator());

  m_pInnerControl->AttachRoot();
}

// IReactRootView implementations
std::shared_ptr<::react::uwp::IReactInstance> ReactControl::GetReactInstance() const noexcept
{
  return m_pInnerControl->GetReactInstance();
}

::react::uwp::XamlView ReactControl::GetXamlView() const noexcept
{
  return m_pInnerControl->GetXamlView();
}

void ReactControl::SetJSComponentName(std::string&& jsComponentName) noexcept
{
  m_pInnerControl->SetJSComponentName(std::move(jsComponentName));
}

void ReactControl::SetInstanceCreator(const ::react::uwp::ReactInstanceCreator& instanceCreator) noexcept
{
  m_pInnerControl->SetInstanceCreator(instanceCreator);
  _spInstance = Microsoft::WRL::Make<Instance>(instanceCreator);
}

void ReactControl::SetInitialProps(folly::dynamic&& initialProps) noexcept
{
  m_pInnerControl->SetInitialProps(std::move(initialProps));
}

void ReactControl::AttachRoot() noexcept
{
  m_pInnerControl->AttachRoot();
}

void ReactControl::DetachRoot() noexcept
{
  m_pInnerControl->DetachRoot();
}

void ReactControl::ResetView()
{
  return m_pInnerControl->ResetView();
}

std::string ReactControl::JSComponentName() const noexcept
{
  return m_pInnerControl->JSComponentName();
}

int64_t ReactControl::GetActualHeight() const
{
  return m_pInnerControl->GetActualHeight();
}

int64_t ReactControl::GetActualWidth() const
{
  return m_pInnerControl->GetActualWidth();
}

int64_t ReactControl::GetTag() const
{
  return m_pInnerControl->GetTag();
}

void ReactControl::SetTag(int64_t tag)
{
  return m_pInnerControl->SetTag(tag);
}

} // uwp
} // react
} // ABI
