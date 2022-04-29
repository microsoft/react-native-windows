// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.
// IMPORTANT: Before updating this file
// please read react-native-windows repo:
// vnext/Microsoft.ReactNative.Cxx/README.md

#include "pch.h"
#include "TurboModulesProvider.h"
#include <ReactCommon/TurboModuleUtils.h>
#include "JSDispatcherWriter.h"
#include "JsiApi.h"
#include "JsiReader.h"
#include "JsiWriter.h"
#ifdef __APPLE__
#include "Crash.h"
#else
#include <crash/verifyElseCrash.h>
#endif

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
    EnsureMemberNotSet("getConstants", false);
    m_constantProviders.push_back(constantProvider);
  }

  void AddMethod(hstring const &name, MethodReturnType returnType, MethodDelegate const &method) noexcept {
    auto key = to_string(name);
    EnsureMemberNotSet(key, true);
    m_methods.insert({key, {returnType, method}});
  }

  void AddSyncMethod(hstring const &name, SyncMethodDelegate const &method) noexcept {
    auto key = to_string(name);
    EnsureMemberNotSet(key, true);
    m_syncMethods.insert({key, method});
  }

 public:
  std::unordered_map<std::string, TurboModuleMethodInfo> m_methods;
  std::unordered_map<std::string, SyncMethodDelegate> m_syncMethods;
  std::vector<ConstantProviderDelegate> m_constantProviders;
  bool m_constantsEvaluated{false};

 private:
  void EnsureMemberNotSet(const std::string &key, bool checkingMethod) noexcept {
    VerifyElseCrash(m_methods.find(key) == m_methods.end());
    VerifyElseCrash(m_syncMethods.find(key) == m_syncMethods.end());
    if (checkingMethod && key == "getConstants") {
      VerifyElseCrash(m_constantProviders.size() == 0);
    }
  }

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
      const std::shared_ptr<facebook::react::CallInvoker> &jsInvoker,
      const ReactModuleProvider &reactModuleProvider)
      : facebook::react::TurboModule(name, jsInvoker),
        m_reactContext(reactContext),
        m_moduleBuilder(winrt::make<TurboModuleBuilder>(reactContext)),
        m_providedModule(reactModuleProvider(m_moduleBuilder)) {
    if (auto hostObject = m_providedModule.try_as<IJsiHostObject>()) {
      m_hostObjectWrapper = std::make_shared<implementation::HostObjectWrapper>(hostObject);
    }
  }

  std::vector<facebook::jsi::PropNameID> getPropertyNames(facebook::jsi::Runtime &rt) override {
    if (m_hostObjectWrapper) {
      return m_hostObjectWrapper->getPropertyNames(rt);
    }

    auto turboModuleBuilder = m_moduleBuilder.as<TurboModuleBuilder>();
    std::vector<facebook::jsi::PropNameID> propertyNames;
    propertyNames.reserve(turboModuleBuilder->m_methods.size());
    for (auto &methodInfo : turboModuleBuilder->m_methods) {
      propertyNames.push_back(facebook::jsi::PropNameID::forAscii(rt, methodInfo.first));
    }
    return propertyNames;
  };

  facebook::jsi::Value get(facebook::jsi::Runtime &runtime, const facebook::jsi::PropNameID &propName) override {
    if (m_hostObjectWrapper) {
      return m_hostObjectWrapper->get(runtime, propName);
    }

    // it is not safe to assume that "runtime" never changes, so members are not cached here
    auto moduleBuilder = m_moduleBuilder.as<TurboModuleBuilder>();
    std::string key = propName.utf8(runtime);

    if (key == "getConstants" && !moduleBuilder->m_constantProviders.empty()) {
      // try to find getConstants if there is any constant
      return facebook::jsi::Function::createFromHostFunction(
          runtime,
          propName,
          0,
          [moduleBuilder](
              facebook::jsi::Runtime &rt,
              const facebook::jsi::Value &thisVal,
              const facebook::jsi::Value *args,
              size_t count) {
            // collect all constants to an object
            auto writer = winrt::make<JsiWriter>(rt);
            writer.WriteObjectBegin();
            for (auto constantProvider : moduleBuilder->m_constantProviders) {
              constantProvider(writer);
            }
            writer.WriteObjectEnd();
            return writer.as<JsiWriter>()->MoveResult();
          });
    }

    {
      // try to find a Method
      auto it = moduleBuilder->m_methods.find(key);
      if (it != moduleBuilder->m_methods.end()) {
        TurboModuleMethodInfo methodInfo = it->second;
        switch (methodInfo.ReturnType) {
          case MethodReturnType::Void:
            return facebook::jsi::Function::createFromHostFunction(
                runtime,
                propName,
                0,
                [methodInfo](
                    facebook::jsi::Runtime &rt,
                    const facebook::jsi::Value & /*thisVal*/,
                    const facebook::jsi::Value *args,
                    size_t argCount) {
                  methodInfo.Method(winrt::make<JsiReader>(rt, args, argCount), nullptr, nullptr, nullptr);
                  return facebook::jsi::Value::undefined();
                });
          case MethodReturnType::Callback:
            return facebook::jsi::Function::createFromHostFunction(
                runtime,
                propName,
                0,
                [jsDispatcher = m_reactContext.JSDispatcher(), methodInfo](
                    facebook::jsi::Runtime &rt,
                    const facebook::jsi::Value & /*thisVal*/,
                    const facebook::jsi::Value *args,
                    size_t argCount) {
                  VerifyElseCrash(argCount > 0);
                  methodInfo.Method(
                      winrt::make<JsiReader>(rt, args, argCount - 1),
                      winrt::make<JSDispatcherWriter>(jsDispatcher, rt),
                      MakeCallback(rt, {rt, args[argCount - 1]}),
                      nullptr);
                  return facebook::jsi::Value::undefined();
                });
          case MethodReturnType::TwoCallbacks:
            return facebook::jsi::Function::createFromHostFunction(
                runtime,
                propName,
                0,
                [jsDispatcher = m_reactContext.JSDispatcher(), methodInfo](
                    facebook::jsi::Runtime &rt,
                    const facebook::jsi::Value & /*thisVal*/,
                    const facebook::jsi::Value *args,
                    size_t argCount) {
                  VerifyElseCrash(argCount > 1);
                  methodInfo.Method(
                      winrt::make<JsiReader>(rt, args, argCount - 2),
                      winrt::make<JSDispatcherWriter>(jsDispatcher, rt),
                      MakeCallback(rt, {rt, args[argCount - 2]}),
                      MakeCallback(rt, {rt, args[argCount - 1]}));
                  return facebook::jsi::Value::undefined();
                });
          case MethodReturnType::Promise:
            return facebook::jsi::Function::createFromHostFunction(
                runtime,
                propName,
                0,
                [jsDispatcher = m_reactContext.JSDispatcher(), methodInfo](
                    facebook::jsi::Runtime &rt,
                    const facebook::jsi::Value & /*thisVal*/,
                    const facebook::jsi::Value *args,
                    size_t count) {
                  auto argReader = winrt::make<JsiReader>(rt, args, count);
                  auto argWriter = winrt::make<JSDispatcherWriter>(jsDispatcher, rt);
                  return facebook::react::createPromiseAsJSIValue(
                      rt,
                      [methodInfo, argReader, argWriter](
                          facebook::jsi::Runtime &runtime, std::shared_ptr<facebook::react::Promise> promise) {
                        methodInfo.Method(
                            argReader,
                            argWriter,
                            [promise](const IJSValueWriter &writer) {
                              writer.as<JSDispatcherWriter>()->WithResultArgs([promise](
                                                                                  facebook::jsi::Runtime &runtime,
                                                                                  facebook::jsi::Value const *args,
                                                                                  size_t argCount) {
                                VerifyElseCrash(argCount == 1);
                                promise->resolve(args[0]);
                              });
                            },
                            [promise](const IJSValueWriter &writer) {
                              writer.as<JSDispatcherWriter>()->WithResultArgs([promise](
                                                                                  facebook::jsi::Runtime &runtime,
                                                                                  facebook::jsi::Value const *args,
                                                                                  size_t argCount) {
                                VerifyElseCrash(argCount == 1);
                                // To match the Android and iOS TurboModule behavior we create the Error object for
                                // the Promise rejection the same way as in updateErrorWithErrorData method.
                                // See react-native/Libraries/BatchedBridge/NativeModules.js for details.
                                auto error = runtime.global()
                                                 .getPropertyAsFunction(runtime, "Error")
                                                 .callAsConstructor(runtime, {});
                                auto &errorData = args[0];
                                if (errorData.isObject()) {
                                  runtime.global()
                                      .getPropertyAsObject(runtime, "Object")
                                      .getPropertyAsFunction(runtime, "assign")
                                      .call(runtime, error, errorData.getObject(runtime));
                                }
                                promise->reject_.call(runtime, error);
                              });
                            });
                      });
                });
          default:
            VerifyElseCrash(false);
        }
      }
    }

    {
      // try to find a SyncMethod
      auto it = moduleBuilder->m_syncMethods.find(key);
      if (it != moduleBuilder->m_syncMethods.end()) {
        return facebook::jsi::Function::createFromHostFunction(
            runtime,
            propName,
            0,
            [method = it->second](
                facebook::jsi::Runtime &rt,
                const facebook::jsi::Value &thisVal,
                const facebook::jsi::Value *args,
                size_t count) {
              auto argReader = winrt::make<JsiReader>(rt, args, count);
              auto argWriter = winrt::make<JsiWriter>(rt);
              method(argReader, argWriter);
              return argWriter.as<JsiWriter>()->MoveResult();
            });
      }
    }

    // returns undefined if the expected member is not found
    return facebook::jsi::Value::undefined();
  }

  void set(facebook::jsi::Runtime &rt, const facebook::jsi::PropNameID &name, const facebook::jsi::Value &value)
      override {
    if (m_hostObjectWrapper) {
      return m_hostObjectWrapper->set(rt, name, value);
    }

    facebook::react::TurboModule::set(rt, name, value);
  }

 private:
  static MethodResultCallback MakeCallback(facebook::jsi::Runtime &runtime, facebook::jsi::Value callback) noexcept {
    auto sharedCallback =
        std::make_shared<facebook::jsi::Function>(std::move(callback).asObject(runtime).asFunction(runtime));
    return [sharedCallback = std::move(sharedCallback)](const IJSValueWriter &writer) noexcept {
      writer.as<JSDispatcherWriter>()->WithResultArgs(
          [sharedCallback](facebook::jsi::Runtime &rt, facebook::jsi::Value const *args, size_t count) {
            sharedCallback->call(rt, args, count);
          });
    };
  }

 private:
  IReactContext m_reactContext;
  IReactModuleBuilder m_moduleBuilder;
  IInspectable m_providedModule;
  std::shared_ptr<implementation::HostObjectWrapper> m_hostObjectWrapper;
};

/*-------------------------------------------------------------------------------
  TurboModulesProvider
-------------------------------------------------------------------------------*/
std::shared_ptr<facebook::react::TurboModule> TurboModulesProvider::getModule(
    const std::string &moduleName,
    const std::shared_ptr<facebook::react::CallInvoker> &callInvoker) noexcept {
  // fail if the expected turbo module has not been registered
  auto it = m_moduleProviders.find(moduleName);
  if (it == m_moduleProviders.end()) {
    return nullptr;
  }

  auto tm = std::make_shared<TurboModuleImpl>(m_reactContext, moduleName, callInvoker, it->second);
  return tm;
}

std::vector<std::string> TurboModulesProvider::getEagerInitModuleNames() noexcept {
  std::vector<std::string> eagerModules;
  auto it = m_moduleProviders.find("UIManager");
  if (it != m_moduleProviders.end()) {
    eagerModules.push_back("UIManager");
  }
  return eagerModules;
}

void TurboModulesProvider::SetReactContext(const IReactContext &reactContext) noexcept {
  m_reactContext = reactContext;
}

void TurboModulesProvider::AddModuleProvider(
    winrt::hstring const &moduleName,
    ReactModuleProvider const &moduleProvider,
    bool overwriteExisting) noexcept {
  auto key = to_string(moduleName);
  auto it = m_moduleProviders.find(key);
  if (it == m_moduleProviders.end()) {
    m_moduleProviders.insert({key, moduleProvider});
  } else if (overwriteExisting) {
    // turbo modules should be replaceable before the first time it is requested
    it->second = moduleProvider;
  }
}

} // namespace winrt::Microsoft::ReactNative
