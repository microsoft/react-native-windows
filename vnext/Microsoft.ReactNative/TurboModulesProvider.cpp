// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"
#include "TurboModulesProvider.h"
#include <ReactCommon/TurboModuleUtils.h>
#include "JsiReader.h"
#include "JsiWriter.h"

using namespace winrt;
using namespace Windows::Foundation;

namespace winrt::Microsoft::ReactNative {
/*-------------------------------------------------------------------------------
  TurboModuleBuilder
-------------------------------------------------------------------------------*/

struct TurboModuleMethodInfo {
  MethodReturnType ReturnType;
  MethodDelegate Method;
};

struct TurboModuleBuilder : winrt::implements<TurboModuleBuilder, IReactModuleBuilder> {
  TurboModuleBuilder(const IReactContext &reactContext) noexcept : m_reactContext(reactContext) {}

 public: // IReactModuleBuilder
  void AddInitializer(InitializerDelegate const &initializer) noexcept {
    initializer(m_reactContext);
  }

  void AddConstantProvider(ConstantProviderDelegate const &constantProvider) noexcept {
    VerifyElseCrash(false);
  }

  void AddMethod(hstring const &name, MethodReturnType returnType, MethodDelegate const &method) noexcept {
    auto key = to_string(name);
    VerifyElseCrash(m_methods.find(key) == m_methods.end());
    m_methods.insert({key, {returnType, method}});
  }

  void AddSyncMethod(hstring const &name, SyncMethodDelegate const &method) noexcept {
    VerifyElseCrash(false);
  }

 public:
  std::unordered_map<std::string, TurboModuleMethodInfo> m_methods;

 private:
  IReactContext m_reactContext;
};

/*-------------------------------------------------------------------------------
  TurboModuleImpl
-------------------------------------------------------------------------------*/

class TurboModuleImpl : public facebook::react::TurboModule {
 public:
  TurboModuleImpl(
      const IReactContext &reactContext,
      const std::string &name,
      std::shared_ptr<facebook::react::CallInvoker> jsInvoker,
      ReactModuleProvider reactModuleProvider)
      : facebook::react::TurboModule(name, jsInvoker), m_moduleBuilder(winrt::make<TurboModuleBuilder>(reactContext)) {
    providedModule = reactModuleProvider(m_moduleBuilder);
  }

  facebook::jsi::Value get(facebook::jsi::Runtime &runtime, const facebook::jsi::PropNameID &propName) override {
    // it is not safe to assume that "runtime" never changes, so members are not cached here
    auto tmb = m_moduleBuilder.as<TurboModuleBuilder>();
    auto key = propName.utf8(runtime);
    {
      // try to find a Method
      auto it = tmb->m_methods.find(key);
      if (it != tmb->m_methods.end()) {
        return facebook::jsi::Function::createFromHostFunction(
            runtime,
            propName,
            0,
            [&runtime, method = it->second](
                facebook::jsi::Runtime &rt,
                const facebook::jsi::Value &thisVal,
                const facebook::jsi::Value *args,
                size_t count) {
              // prepare input arguments
              facebook::jsi::Array argsArray(runtime, count);
              for (size_t i = 0; i < count; i++) {
                argsArray.setValueAtIndex(runtime, i, args[i]);
              }
              auto argReader = winrt::make<JsiReader>(runtime, facebook::jsi::Value(runtime, argsArray));

              // prepare output value
              auto argWriter = winrt::make<JsiWriter>(runtime);

              // call the function
              switch (method.ReturnType) {
                case MethodReturnType::Void: {
                  method.Method(argReader, argWriter, nullptr, nullptr);
                  return facebook::jsi::Value::undefined();
                }
                case MethodReturnType::Promise: {
                  return facebook::react::createPromiseAsJSIValue(
                      runtime, [=](facebook::jsi::Runtime &runtime, std::shared_ptr<facebook::react::Promise> promise) {
                        method.Method(
                            argReader,
                            argWriter,
                            [promise, &runtime](const IJSValueWriter &writer) {
                              auto result = writer.as<JsiWriter>()->MoveResult();
                              promise->resolve(result);
                            },
                            [promise, &runtime](const IJSValueWriter &writer) {
                              auto result = writer.as<JsiWriter>()->MoveResult();
                              VerifyElseCrash(result.isString());
                              promise->reject(result.getString(runtime).utf8(runtime));
                            });
                      });
                }
                case MethodReturnType::Callback:
                case MethodReturnType::TwoCallbacks:
                  VerifyElseCrash(false);
                default:
                  VerifyElseCrash(false);
              }
            });
      }
    }

    // missing member, return undefined
    return facebook::jsi::Value::undefined();
  }

 private:
  IReactModuleBuilder m_moduleBuilder;
  IInspectable providedModule;
};

/*-------------------------------------------------------------------------------
  TurboModulesProvider
-------------------------------------------------------------------------------*/
TurboModulesProvider::TurboModulePtr TurboModulesProvider::getModule(
    const std::string &moduleName,
    const CallInvokerPtr &callInvoker) noexcept {
  // see if the expected turbo module has been cached
  auto pair = std::make_pair(moduleName, callInvoker);
  auto itCached = m_cachedModules.find(pair);
  if (itCached != m_cachedModules.end()) {
    return itCached->second;
  }

  // fail if the expected turbo module has not been registered
  auto it = m_moduleProviders.find(moduleName);
  if (it == m_moduleProviders.end()) {
    return nullptr;
  }

  // cache and return the turbo module
  auto tm = std::make_shared<TurboModuleImpl>(m_reactContext, moduleName, callInvoker, it->second);
  m_cachedModules.insert({pair, tm});
  return tm;
}

std::vector<std::string> TurboModulesProvider::getEagerInitModuleNames() noexcept {
  return {};
}

void TurboModulesProvider::SetReactContext(const IReactContext &reactContext) noexcept {
  m_reactContext = reactContext;
}

void TurboModulesProvider::AddModuleProvider(
    winrt::hstring const &moduleName,
    ReactModuleProvider const &moduleProvider) noexcept {
  auto key = to_string(moduleName);
  VerifyElseCrash(m_moduleProviders.find(key) == m_moduleProviders.end());
  m_moduleProviders.insert({key, moduleProvider});
}

} // namespace winrt::Microsoft::ReactNative
