// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include <pch.h>

#include "HStringHelper.h"
#include "Instance_rt.h"
#include "comTemplateLibrary.h"

#include "XamlView.h"

namespace ABI {
namespace react {
namespace uwp {

ActivatableClassWithFactory(Instance, InstanceStatics);

struct InstanceReactInstanceCreator : ::react::uwp::IReactInstanceCreator {
  InstanceReactInstanceCreator(
      Microsoft::WRL::ComPtr<ABI::react::uwp::IInstance> &outer) {
    outer.AsWeak(&m_wrInstance);
  }

  std::shared_ptr<::react::uwp::IReactInstance> getInstance() {
    Microsoft::WRL::ComPtr<ABI::react::uwp::IInstance> spInstance;
    m_wrInstance.As(&spInstance);
    if (!spInstance)
      return nullptr;
    return reinterpret_cast<Instance *>(spInstance.Get())->getInstance();
  }

  void markAsNeedsReload() {
    Microsoft::WRL::ComPtr<ABI::react::uwp::IInstance> spInstance;
    m_wrInstance.As(&spInstance);
    reinterpret_cast<Instance *>(spInstance.Get())->markAsNeedsReload();
  }

  void persistUseWebDebugger(bool useWebDebugger) {
    Microsoft::WRL::ComPtr<ABI::react::uwp::IInstance> spInstance;
    m_wrInstance.As(&spInstance);
    reinterpret_cast<Instance *>(spInstance.Get())
        ->persistUseWebDebugger(useWebDebugger);
  }

  void persistUseLiveReload(bool useLiveReload) {
    Microsoft::WRL::ComPtr<ABI::react::uwp::IInstance> spInstance;
    m_wrInstance.As(&spInstance);
    reinterpret_cast<Instance *>(spInstance.Get())
        ->persistUseLiveReload(useLiveReload);
  }

 private:
  Microsoft::WRL::WeakRef m_wrInstance;
  Microsoft::WRL::ComPtr<ABI::react::uwp::IInstance> m_instance;
};

Instance::Instance(HSTRING bundleFileName) {
  m_jsBundleName = HSTRINGToString(bundleFileName);
  Microsoft::WRL::ComPtr<IInstance> spThis(this);
  m_instanceCreator = std::make_shared<InstanceReactInstanceCreator>(spThis);
}

Instance::Instance(const ::react::uwp::ReactInstanceCreator &instanceCreator) {
  m_outerInstanceCreator = instanceCreator;
  Microsoft::WRL::ComPtr<IInstance> spThis(this);
  m_instanceCreator = std::make_shared<InstanceReactInstanceCreator>(spThis);
}

std::shared_ptr<::react::uwp::IReactInstance> Instance::getInstance() {
  if (m_outerInstanceCreator)
    return m_outerInstanceCreator->getInstance();

  if (!m_instance) {
    m_instance =
        ::react::uwp::CreateReactInstance(m_spModuleProvider /*moduleLoader*/);
    ::react::uwp::ReactInstanceSettings innerSettings;
    innerSettings.UseLiveReload = m_settings.UseLiveReload;
    innerSettings.UseWebDebugger = m_settings.UseWebDebugger;
    m_instance->Start(m_instance, innerSettings);
    m_instance->loadBundle(std::string(m_jsBundleName));
  }
  return m_instance;
}

void Instance::markAsNeedsReload() {
  if (m_outerInstanceCreator)
    return m_outerInstanceCreator->markAsNeedsReload();

  m_instance->SetAsNeedsReload();
  m_instance = nullptr;
}

void Instance::persistUseWebDebugger(bool useWebDebugger) {
  m_settings.UseWebDebugger = useWebDebugger;
}

void Instance::persistUseLiveReload(bool useLiveReload) {
  m_settings.UseLiveReload = useLiveReload;
}

HRESULT Instance::Start(ABI::react::uwp::InstanceSettings settings) {
  if (m_outerInstanceCreator || m_instance)
    return E_FAIL;

  m_settings.UseLiveReload = settings.UseLiveReload;
  m_settings.UseWebDebugger = settings.UseWebDebugger;
  getInstance();

  return S_OK;
}

HRESULT Instance::RegisterModule(ABI::react::uwp::IModule *pModule) {
  if (m_outerInstanceCreator || m_instance)
    return E_FAIL;

  if (m_spModuleProvider == nullptr)
    m_spModuleProvider = std::make_shared<ABIModuleLoader>();

  Microsoft::WRL::ComPtr<ABI::react::uwp::IModule> spModule(pModule);
  m_spModuleProvider->RegisterModule(spModule);
  return S_OK;
}

HRESULT Instance::SetXamlViewCreatedTestHook(
    ABI::react::uwp::IXamlTestHookDelegate *pXamlTestHookDelegate) {
  if (m_instance == nullptr)
    return E_FAIL;

  Microsoft::WRL::ComPtr<ABI::react::uwp::IXamlTestHookDelegate>
      spXamlTestHookDelegate(pXamlTestHookDelegate);
  std::function<void(::react::uwp::XamlView)> f =
      [spXamlTestHookDelegate](::react::uwp::XamlView params) {
        auto spParams = params.as<ABI::Windows::UI::Xaml::IDependencyObject>();
        spXamlTestHookDelegate->Invoke(spParams.get());
      };

  m_instance->SetXamlViewCreatedTestHook(std::move(f));

  return S_OK;
}

const ::react::uwp::ReactInstanceCreator &Instance::GetReactInstanceCreator() {
  return m_instanceCreator;
}

HRESULT InstanceStatics::Create(
    _In_ HSTRING bundleFileName,
    _Outptr_ ABI::react::uwp::IInstance **ppInstance) {
  auto instance = Microsoft::WRL::Make<Instance>(bundleFileName);
  return instance.CopyTo(ppInstance);
}

} // namespace uwp
} // namespace react
} // namespace ABI
