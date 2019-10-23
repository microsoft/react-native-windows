// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include <pch.h>

#include "ABIModuleLoader.h"
#include "HStringHelper.h"

#include <folly/json.h>

namespace ABI {
namespace react {
namespace uwp {

ABIModule::ABIModule(const Microsoft::WRL::ComPtr<ABI::react::uwp::IModule> &module) : m_module(module) {
  assert(module != nullptr);
}

std::string ABIModule::getName() {
  HSTRING name;
  m_module->get_Name(&name);

  return HSTRINGToString(name);
}

std::map<std::string, folly::dynamic> ABIModule::getConstants() {
  using namespace ABI::Windows::Foundation;
  using namespace ABI::Windows::Foundation::Collections;

  Microsoft::WRL::ComPtr<IMapView<HSTRING, HSTRING>> constantsMapView;
  Microsoft::WRL::ComPtr<IIterable<IKeyValuePair<HSTRING, HSTRING> *>> constantsIterable;
  Microsoft::WRL::ComPtr<IIterator<IKeyValuePair<HSTRING, HSTRING> *>> constantsIter;

  m_module->get_Constants(constantsMapView.GetAddressOf());
  constantsMapView.As(&constantsIterable);
  constantsIterable->First(&constantsIter);

  std::map<std::string, folly::dynamic> ret;
  boolean hasCurrent;

  do {
    constantsIter->get_HasCurrent(&hasCurrent);

    if (!hasCurrent)
      break;

    Microsoft::WRL::ComPtr<IKeyValuePair<HSTRING, HSTRING>> pair;
    constantsIter->get_Current(pair.GetAddressOf());

    HSTRING key, value;
    pair->get_Key(&key);
    pair->get_Value(&value);

    ret.emplace(HSTRINGToString(key), folly::parseJson(HSTRINGToString(value)));
    constantsIter->MoveNext(&hasCurrent);
  } while (hasCurrent);

  return ret;
}

std::vector<facebook::xplat::module::CxxModule::Method> ABIModule::getMethods() {
  using namespace ABI::Windows::Foundation;
  using namespace ABI::Windows::Foundation::Collections;

  std::vector<facebook::xplat::module::CxxModule::Method> ret;
  boolean hasCurrent;

  // Add methods without callback
  Microsoft::WRL::ComPtr<IMapView<HSTRING, ABI::react::uwp::IMethodDelegate *>> methodsMapView;
  Microsoft::WRL::ComPtr<IIterable<IKeyValuePair<HSTRING, ABI::react::uwp::IMethodDelegate *> *>> methodsIterable;
  Microsoft::WRL::ComPtr<IIterator<IKeyValuePair<HSTRING, ABI::react::uwp::IMethodDelegate *> *>> methodsIter;

  m_module->get_Methods(&methodsMapView);
  methodsMapView.As(&methodsIterable);
  methodsIterable->First(&methodsIter);

  do {
    methodsIter->get_HasCurrent(&hasCurrent);

    if (!hasCurrent)
      break;

    Microsoft::WRL::ComPtr<IKeyValuePair<HSTRING, ABI::react::uwp::IMethodDelegate *>> pair;
    methodsIter->get_Current(pair.GetAddressOf());

    HSTRING key;
    ABI::react::uwp::IMethodDelegate *value;
    pair->get_Key(&key);
    pair->get_Value(&value);

    ret.push_back(facebook::xplat::module::CxxModule::Method(HSTRINGToString(key), [this, value](folly::dynamic args) {
      std::string str = folly::toJson(args);

      HSTRING para = StringToHSTRING(str);
      value->Invoke(para);
      WindowsDeleteString(para);
    }));

    methodsIter->MoveNext(&hasCurrent);

  } while (hasCurrent);

  // Add methods with callback
  Microsoft::WRL::ComPtr<IMapView<HSTRING, ABI::react::uwp::IMethodWithCallbackDelegate *>> methodsWithCallBackMapView;
  Microsoft::WRL::ComPtr<IIterable<IKeyValuePair<HSTRING, ABI::react::uwp::IMethodWithCallbackDelegate *> *>>
      methodsWithCallBackIterable;
  Microsoft::WRL::ComPtr<IIterator<IKeyValuePair<HSTRING, ABI::react::uwp::IMethodWithCallbackDelegate *> *>>
      methodsWithCallBackIter;

  m_module->get_MethodsWithCallback(&methodsWithCallBackMapView);
  methodsWithCallBackMapView.As(&methodsWithCallBackIterable);
  methodsWithCallBackIterable->First(&methodsWithCallBackIter);

  do {
    boolean hasCurrent;
    methodsWithCallBackIter->get_HasCurrent(&hasCurrent);

    if (!hasCurrent)
      break;

    Microsoft::WRL::ComPtr<IKeyValuePair<HSTRING, ABI::react::uwp::IMethodWithCallbackDelegate *>> pair;
    methodsWithCallBackIter->get_Current(pair.GetAddressOf());

    HSTRING key;
    ABI::react::uwp::IMethodWithCallbackDelegate *value;
    pair->get_Key(&key);
    pair->get_Value(&value);

    ret.push_back(facebook::xplat::module::CxxModule::Method(
        HSTRINGToString(key), [this, value](folly::dynamic args, Callback cb) {
          std::string str = folly::toJson(args);

          HSTRING para = StringToHSTRING(str);

          auto callback =
              Microsoft::WRL::Callback<ABI::react::uwp::IMethodCallback>([cb](IVectorView<HSTRING> *argVectorView) {
                std::vector<folly::dynamic> args;

                if (argVectorView != nullptr) {
                  unsigned int size = 0;
                  argVectorView->get_Size(&size);

                  for (unsigned int i = 0; i < size; i++) {
                    HSTRING itemString;
                    argVectorView->GetAt(i, &itemString);
                    if (itemString == NULL)
                      args.push_back(nullptr);
                    else
                      args.push_back(folly::parseJson(HSTRINGToString(itemString)));
                  }
                }

                cb(args);
                return S_OK;
              });

          value->Invoke(para, callback.Get());
          WindowsDeleteString(para);
        }));

    methodsWithCallBackIter->MoveNext(&hasCurrent);
  } while (true);

  return ret;
}

ABIModuleLoader::ABIModuleLoader() {}

void ABIModuleLoader::RegisterModule(Microsoft::WRL::ComPtr<ABI::react::uwp::IModule> &module) {
  m_modules.push_back(module);
}

std::vector<facebook::react::NativeModuleDescription> ABIModuleLoader::GetModules(
    const std::shared_ptr<facebook::react::MessageQueueThread> &defaultQueueThread) {
  std::shared_ptr<facebook::react::MessageQueueThread> queueThread(defaultQueueThread);
  std::vector<facebook::react::NativeModuleDescription> modules;
  modules.reserve(m_modules.size());

  for (auto &module : m_modules) {
    HSTRING name;
    module->get_Name(&name);

    modules.emplace_back(
        HSTRINGToString(name), [module, queueThread]() { return std::make_unique<ABIModule>(module); }, queueThread);
  }

  return modules;
}

} // namespace uwp
} // namespace react
} // namespace ABI
