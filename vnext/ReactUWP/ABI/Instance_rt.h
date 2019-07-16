// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once
#include <wrl.h>

#include <Instance.h>
#include "ABIModuleLoader.h"

#include <wrl\client.h>
#include <wrl\wrappers\corewrappers.h>
#include <functional>

#include <IReactInstance.h>
#include "InstanceFactory.h"

namespace react {
namespace uwp {
struct IInstanceManager;
}
} // namespace react

namespace ABI {
namespace react {
namespace uwp {

class Instance
    : public Microsoft::WRL::RuntimeClass<ABI::react::uwp::IInstance> {
  InspectableClass(RuntimeClass_react_uwp_Instance, BaseTrust);

 public:
  Instance(HSTRING bundleFileName);
  Instance(const ::react::uwp::ReactInstanceCreator &instanceCreator);

  virtual HRESULT STDMETHODCALLTYPE Start(InstanceSettings settings) override;
  virtual HRESULT STDMETHODCALLTYPE
  RegisterModule(ABI::react::uwp::IModule *pModule) override;

  virtual HRESULT STDMETHODCALLTYPE SetXamlViewCreatedTestHook(
      ABI::react::uwp::IXamlTestHookDelegate *pXamlTestHookDelegate) override;

  const ::react::uwp::ReactInstanceCreator &GetReactInstanceCreator();

  std::shared_ptr<::react::uwp::IReactInstance> getInstance();
  void markAsNeedsReload();

 private:
  std::string m_jsBundleName;
  std::shared_ptr<ABIModuleLoader> m_spModuleProvider;
  ::react::uwp::ReactInstanceCreator
      m_instanceCreator; // Internal that just forwards impl to this
  ::react::uwp::ReactInstanceCreator m_outerInstanceCreator; // Provided by user

  ::react::uwp::ReactInstanceSettings m_settings;
  std::shared_ptr<::react::uwp::IReactInstance> m_instance;
};

class InstanceStatics
    : public Microsoft::WRL::AgileActivationFactory<IInstanceStatics> {
  InspectableClassStatic(RuntimeClass_react_uwp_Instance, BaseTrust);

 public:
  virtual HRESULT STDMETHODCALLTYPE Create(
      _In_ HSTRING bundleFileName,
      _Outptr_ ABI::react::uwp::IInstance **ppInstance) override;
};

} // namespace uwp
} // namespace react
} // namespace ABI
