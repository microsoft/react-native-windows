#pragma once
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "Bridge.ReactInstance.g.h"

#include "NativeModuleRegistry.h"

#include <ReactUWP/IReactInstance.h>
#include <winrt/Microsoft.ReactNative.h>

using namespace winrt;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;

namespace winrt::Microsoft::ReactNative::Bridge::implementation {

struct ReactInstance : ReactInstanceT<ReactInstance> {
  ReactInstance() = default;
  ReactInstance(
      std::shared_ptr<react::uwp::IReactInstance> instance,
      IVectorView<Bridge::INativeModule> const &modules)
      : m_instance(instance) {
    if (instance == nullptr) {
      throw hresult_null_argument(L"instance");
    }

    if (modules == nullptr)
      throw hresult_null_argument(L"modules");

    m_moduleRegistry = Bridge::NativeModuleRegistry(modules);
  }

  IVectorView<Bridge::INativeModule> NativeModules() {
    // TODO: Retrieve the native modules.  The m_instance->GetInnerInstance() is
    // a facebook::react::Instance that already has a module registry.  Could we
    // use that? It doesn't provide a list of registered modules and even if it
    // did we'd need to map it back to modules at this layer on which we can
    // raise notification and invoke their Initialize method.
    // std::shared_ptr<facebook::react::Instance> instance =
    // m_instance->GetInnerInstance(); auto registry =
    // instance->getModuleRegistry();
    assert(m_moduleRegistry != nullptr);

    return m_moduleRegistry.Modules();
  };

  IAsyncAction InitializeAsync();
  void InvokeFunction(
      hstring const &moduleName,
      hstring const &method,
      IVectorView<IInspectable> const &arguments);

 private:
  std::shared_ptr<react::uwp::IReactInstance> m_instance{nullptr};
  Bridge::NativeModuleRegistry m_moduleRegistry{nullptr};
};
} // namespace winrt::Microsoft::ReactNative::Bridge::implementation

namespace winrt::Microsoft::ReactNative::Bridge::factory_implementation {
struct ReactInstance
    : ReactInstanceT<ReactInstance, implementation::ReactInstance> {};
} // namespace winrt::Microsoft::ReactNative::Bridge::factory_implementation
