// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include "ABIModule.h"
#include "NativeModulesProvider.h"

namespace winrt::Microsoft::ReactNative::Bridge {

auto ABIModule::getName() -> std::string {
  auto name = m_module.Name();
  return to_string(name);
}

auto ABIModule::getConstants() -> std::map<std::string, folly::dynamic> {
  std::map<std::string, folly::dynamic> constants;
  auto constantsMapView = m_module.Constants();

  for (auto const &pair : constantsMapView) {
    std::string key = to_string(pair.Key());
    folly::dynamic value = ConvertToDynamic(pair.Value());

    constants.emplace(key, value);
  }

  return constants;
}

auto ABIModule::getMethods()
    -> std::vector<facebook::xplat::module::CxxModule::Method> {
  std::vector<facebook::xplat::module::CxxModule::Method> methods;

  // Add methods without callback
  auto methodsView = m_module.Methods();

  for (auto methodInfo : methodsView) {
    hstring key = methodInfo.Name();
    Bridge::Method method = methodInfo.Method();
    auto returnType = methodInfo.ReturnType();

    switch (returnType) {
      case Bridge::ReturnType::Void: {
        auto simpleAction = WrapAction(key, method);
        methods.push_back(simpleAction);
        break;
      }
      case Bridge::ReturnType::Callback: {
        auto methodWithCallback = WrapCallback(key, method);
        methods.push_back(methodWithCallback);
        break;
      }
      case Bridge::ReturnType::Promise: {
        auto methodWithPromise = WrapPromise(key, method);
        methods.push_back(methodWithPromise);
        break;
      }
    }
  }

  return methods;
}

auto ABIModule::WrapAction(
    hstring methodName,
    Bridge::Method const &nativeMethod)
    -> facebook::xplat::module::CxxModule::Method {

  return facebook::xplat::module::CxxModule::Method(
    to_string(methodName),
    [this, nativeMethod = std::move(nativeMethod)]
    (folly::dynamic args) {
      hstring str = to_hstring(folly::toJson(args));
      auto methodArgs =
        single_threaded_vector<IInspectable>({box_value(str)});

        nativeMethod(methodArgs.GetView(), nullptr, nullptr);
    });
}

auto ABIModule::WrapCallback(
    hstring methodName,
    Bridge::Method const &nativeMethod)
    -> facebook::xplat::module::CxxModule::Method {

  return facebook::xplat::module::CxxModule::Method(
      to_string(methodName),
      [this, nativeMethod = std::move(nativeMethod)]
      (folly::dynamic args, Callback jsMethod) {
        std::string strArgs = folly::toJson(args);
        auto strValue = to_hstring(strArgs);
        auto abiParameter =
            single_threaded_vector<IInspectable>({box_value(strValue)});

        auto jsCallbackABI =
            Bridge::Callback(
              [jsMethod = std::move(jsMethod)]
              (IVectorView<IInspectable> argValues) {
              // when invoked convert the args and callback to the
              // javascript method
              std::vector<folly::dynamic> args;
              if (argValues != nullptr) {
                for (auto arg : argValues) {
                  auto dynamicObject = ConvertToDynamic(arg);
                  args.push_back(dynamicObject);
                }
              }

              jsMethod(args);
            });

        nativeMethod(
            abiParameter.GetView(), jsCallbackABI, nullptr);
      });
}

auto ABIModule::WrapPromise(
    hstring methodName,
    Bridge::Method const &nativeMethod)
    -> facebook::xplat::module::CxxModule::Method {

  return facebook::xplat::module::CxxModule::Method(
    to_string(methodName),
    [this, nativeMethod = std::move(nativeMethod)]
    (folly::dynamic args, Callback cbResolve, Callback cbReject) {
      hstring strArgs = to_hstring(folly::toJson(args));
      auto abiParameter =
          single_threaded_vector<IInspectable>({box_value(strArgs)});

      // Create a wrapper for the callback into JS that the native
      // code can invoke.
      auto jsResolveCallback =
        Bridge::Callback(
         [cbResolve = std::move(cbResolve)](
         IVectorView<IInspectable> methodArgs) {
            if (cbResolve == nullptr)
              return;

            if (methodArgs == nullptr || methodArgs.Size() < 1)
              throw hresult_invalid_argument();

            auto object = ConvertToDynamic(methodArgs.GetAt(0));
            cbResolve(std::vector<folly::dynamic>{object});
          });

      auto jsRejectCallback =
        Bridge::Callback(
          [cbReject = std::move(cbReject)](
          IVectorView<IInspectable> methodArgs) {
            if (cbReject == nullptr)
              return;

            std::vector<folly::dynamic> args;
            for (auto arg : methodArgs) {
              auto dynamicObject = ConvertToDynamic(arg);
              args.push_back(dynamicObject);
            }

            cbReject(args);
          });

      nativeMethod(
        abiParameter.GetView(),
        jsResolveCallback,
        jsRejectCallback);
    });
}

} // namespace winrt::Microsoft::ReactNative::Bridge
