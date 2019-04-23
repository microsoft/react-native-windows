// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include <pch.h>

#include "comTemplateLibrary.h"
#include "HStringHelper.h"
#include "ReactControl_rt.h"
#include <Windows.Foundation.h>
#include <windows.ui.xaml.controls.h>
#include <folly/json.h>

namespace ABI {
namespace react {
namespace uwp {

ActivatableClassWithFactory(ReactControl, ReactControlFactory);
ActivatableClassWithFactory(Instance, InstanceStatics);

struct InstanceReactInstanceCreator : ::react::uwp::IReactInstanceCreator
{
  InstanceReactInstanceCreator(Microsoft::WRL::ComPtr<ABI::react::uwp::IInstance>& outer)
  {
    outer.AsWeak(&m_wrInstance);
  }

  std::shared_ptr<::react::uwp::IReactInstance> getInstance()
  {
    Microsoft::WRL::ComPtr<ABI::react::uwp::IInstance> spInstance;
    m_wrInstance.As(&spInstance);
    if (!spInstance)
      return nullptr;
    return reinterpret_cast<Instance*>(spInstance.Get())->getInstance();
  }

  void markAsNeedsReload()
  {
    Microsoft::WRL::ComPtr<ABI::react::uwp::IInstance> spInstance;
    m_wrInstance.As(&spInstance);
    reinterpret_cast<Instance*>(spInstance.Get())->markAsNeedsReload();
  }

private:
  Microsoft::WRL::WeakRef m_wrInstance;
  Microsoft::WRL::ComPtr<ABI::react::uwp::IInstance> m_instance;
};

Instance::Instance(HSTRING bundleFileName)
{
  m_jsBundleName = HSTRINGToString(bundleFileName);
  Microsoft::WRL::ComPtr<IInstance> spThis(this);
  m_instanceCreator = std::make_shared<InstanceReactInstanceCreator>(spThis);
}

Instance::Instance(const ::react::uwp::ReactInstanceCreator& instanceCreator)
{
  m_outerInstanceCreator = instanceCreator;
  Microsoft::WRL::ComPtr<IInstance>spThis(this);
  m_instanceCreator = std::make_shared<InstanceReactInstanceCreator>(spThis);
}

std::shared_ptr<::react::uwp::IReactInstance> Instance::getInstance()
{
  if (m_outerInstanceCreator)
    return m_outerInstanceCreator->getInstance();

  if (!m_instance)
  {
    m_instance = ::react::uwp::CreateReactInstance(m_spModuleProvider /*moduleLoader*/);
    ::react::uwp::ReactInstanceSettings innerSettings;
    innerSettings.UseLiveReload = m_settings.UseLiveReload;
    innerSettings.UseWebDebugger = m_settings.UseWebDebugger;
    m_instance->Start(m_instance, innerSettings);
    m_instance->loadBundle(std::string(m_jsBundleName));
  }
  return m_instance;
}

void Instance::markAsNeedsReload()
{
  if (m_outerInstanceCreator)
    return m_outerInstanceCreator->markAsNeedsReload();

  m_instance->SetAsNeedsReload();
  m_instance = nullptr;
}

HRESULT Instance::Start(ABI::react::uwp::InstanceSettings settings)
{
  if (m_outerInstanceCreator || m_instance)
    return E_FAIL;

  m_settings.UseLiveReload = settings.UseLiveReload;
  m_settings.UseWebDebugger = settings.UseWebDebugger;
  getInstance();

  return S_OK;
}

HRESULT Instance::RegisterModule(ABI::react::uwp::IModule* pModule)
{
  if (m_outerInstanceCreator || m_instance)
    return E_FAIL;

  if (m_spModuleProvider == nullptr)
    m_spModuleProvider = std::make_shared<ABIModuleLoader>();

  Microsoft::WRL::ComPtr<ABI::react::uwp::IModule> spModule(pModule);
  m_spModuleProvider->RegisterModule(spModule);
  return S_OK;
}

const ::react::uwp::ReactInstanceCreator& Instance::GetReactInstanceCreator()
{
  return m_instanceCreator;
}

HRESULT InstanceStatics::Create(_In_ HSTRING bundleFileName, _Outptr_ ABI::react::uwp::IInstance** ppInstance)
{
  auto instance = Microsoft::WRL::Make<Instance>(bundleFileName);
  return instance.CopyTo(ppInstance);
}


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

  m_pInnerControl = std::make_unique<::react::uwp::ReactControl>(this, pXamlView);
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
