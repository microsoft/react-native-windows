// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.
//
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
  const std::unordered_map<std::string, TurboModuleMethodInfo> &Methods() const noexcept {
    return m_methods;
  }

  const std::unordered_map<std::string, SyncMethodDelegate> &SyncMethods() const noexcept {
    return m_syncMethods;
  }

  const std::vector<ConstantProviderDelegate> &ConstantProviders() const noexcept {
    return m_constantProviders;
  }

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
  std::unordered_map<std::string, TurboModuleMethodInfo> m_methods;
  std::unordered_map<std::string, SyncMethodDelegate> m_syncMethods;
  std::vector<ConstantProviderDelegate> m_constantProviders;
  bool m_constantsEvaluated{false};
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
      std::weak_ptr<facebook::react::LongLivedObjectCollection> longLivedObjectCollection,
      const ReactModuleProvider &reactModuleProvider)
      : facebook::react::TurboModule(name, jsInvoker),
        m_reactContext(reactContext),
        m_longLivedObjectCollection(std::move(longLivedObjectCollection)),
        m_moduleBuilder(winrt::make_self<TurboModuleBuilder>(reactContext)),
        m_providedModule(reactModuleProvider(m_moduleBuilder.as<IReactModuleBuilder>())) {
    if (auto hostObject = m_providedModule.try_as<IJsiHostObject>()) {
      m_hostObjectWrapper = std::make_shared<implementation::HostObjectWrapper>(hostObject);
    }
  }

  std::vector<facebook::jsi::PropNameID> getPropertyNames(facebook::jsi::Runtime &rt) override {
    if (m_hostObjectWrapper) {
      return m_hostObjectWrapper->getPropertyNames(rt);
    }

    std::vector<facebook::jsi::PropNameID> propertyNames;
    propertyNames.reserve(
        m_moduleBuilder->Methods().size() + m_moduleBuilder->SyncMethods().size() +
        (m_moduleBuilder->ConstantProviders().empty() ? 0 : 1));

    for (auto &methodInfo : m_moduleBuilder->Methods()) {
      propertyNames.push_back(facebook::jsi::PropNameID::forAscii(rt, methodInfo.first));
    }

    for (auto &syncMethodInfo : m_moduleBuilder->SyncMethods()) {
      propertyNames.push_back(facebook::jsi::PropNameID::forAscii(rt, syncMethodInfo.first));
    }

    if (!m_moduleBuilder->ConstantProviders().empty()) {
      propertyNames.push_back(facebook::jsi::PropNameID::forAscii(rt, "getConstants"));
    }

    return propertyNames;
  };

  facebook::jsi::Value get(facebook::jsi::Runtime &runtime, const facebook::jsi::PropNameID &propName) override {
    if (m_hostObjectWrapper) {
      return m_hostObjectWrapper->get(runtime, propName);
    }

    // it is not safe to assume that "runtime" never changes, so members are not cached here
    std::string key = propName.utf8(runtime);

    if (key == "getConstants" && !m_moduleBuilder->ConstantProviders().empty()) {
      // try to find getConstants if there is any constant
      return facebook::jsi::Function::createFromHostFunction(
          runtime,
          propName,
          0,
          [moduleBuilder = m_moduleBuilder](
              facebook::jsi::Runtime &rt,
              const facebook::jsi::Value & /*thisVal*/,
              const facebook::jsi::Value * /*args*/,
              size_t /*count*/) {
            // collect all constants to an object
            auto writer = winrt::make<JsiWriter>(rt);
            writer.WriteObjectBegin();
            for (auto const &constantProvider : moduleBuilder->ConstantProviders()) {
              constantProvider(writer);
            }
            writer.WriteObjectEnd();
            return writer.as<JsiWriter>()->MoveResult();
          });
    }

    {
      // try to find a Method
      auto it = m_moduleBuilder->Methods().find(key);
      if (it != m_moduleBuilder->Methods().end()) {
        TurboModuleMethodInfo const &methodInfo = it->second;
        switch (methodInfo.ReturnType) {
          case MethodReturnType::Void:
            return facebook::jsi::Function::createFromHostFunction(
                runtime,
                propName,
                0,
                [method = methodInfo.Method](
                    facebook::jsi::Runtime &rt,
                    const facebook::jsi::Value & /*thisVal*/,
                    const facebook::jsi::Value *args,
                    size_t argCount) {
                  method(winrt::make<JsiReader>(rt, args, argCount), nullptr, nullptr, nullptr);
                  return facebook::jsi::Value::undefined();
                });
          case MethodReturnType::Callback:
            return facebook::jsi::Function::createFromHostFunction(
                runtime,
                propName,
                0,
                [jsDispatcher = m_reactContext.JSDispatcher(),
                 method = methodInfo.Method,
                 longLivedObjectCollection = m_longLivedObjectCollection](
                    facebook::jsi::Runtime &rt,
                    const facebook::jsi::Value & /*thisVal*/,
                    const facebook::jsi::Value *args,
                    size_t argCount) {
                  VerifyElseCrash(argCount > 0);
                  if (auto strongLongLivedObjectCollection = longLivedObjectCollection.lock()) {
                    auto jsiRuntimeHolder = LongLivedJsiRuntime::CreateWeak(strongLongLivedObjectCollection, rt);
                    method(
                        winrt::make<JsiReader>(rt, args, argCount - 1),
                        winrt::make<JSDispatcherWriter>(jsDispatcher, jsiRuntimeHolder),
                        MakeCallback(rt, strongLongLivedObjectCollection, args[argCount - 1]),
                        nullptr);
                  }
                  return facebook::jsi::Value::undefined();
                });
          case MethodReturnType::TwoCallbacks:
            return facebook::jsi::Function::createFromHostFunction(
                runtime,
                propName,
                0,
                [jsDispatcher = m_reactContext.JSDispatcher(),
                 method = methodInfo.Method,
                 longLivedObjectCollection = m_longLivedObjectCollection](
                    facebook::jsi::Runtime &rt,
                    const facebook::jsi::Value & /*thisVal*/,
                    const facebook::jsi::Value *args,
                    size_t argCount) {
                  VerifyElseCrash(argCount > 1);
                  if (auto strongLongLivedObjectCollection = longLivedObjectCollection.lock()) {
                    auto jsiRuntimeHolder = LongLivedJsiRuntime::CreateWeak(strongLongLivedObjectCollection, rt);
                    method(
                        winrt::make<JsiReader>(rt, args, argCount - 2),
                        winrt::make<JSDispatcherWriter>(jsDispatcher, jsiRuntimeHolder),
                        MakeCallback(rt, strongLongLivedObjectCollection, args[argCount - 2]),
                        MakeCallback(rt, strongLongLivedObjectCollection, args[argCount - 1]));
                  }
                  return facebook::jsi::Value::undefined();
                });
          case MethodReturnType::Promise:
            return facebook::jsi::Function::createFromHostFunction(
                runtime,
                propName,
                0,
                [jsDispatcher = m_reactContext.JSDispatcher(),
                 method = methodInfo.Method,
                 longLivedObjectCollection = m_longLivedObjectCollection](
                    facebook::jsi::Runtime &rt,
                    const facebook::jsi::Value & /*thisVal*/,
                    const facebook::jsi::Value *args,
                    size_t count) {
                  if (auto strongLongLivedObjectCollection = longLivedObjectCollection.lock()) {
                    auto jsiRuntimeHolder = LongLivedJsiRuntime::CreateWeak(strongLongLivedObjectCollection, rt);
                    auto argReader = winrt::make<JsiReader>(rt, args, count);
                    auto argWriter = winrt::make<JSDispatcherWriter>(jsDispatcher, jsiRuntimeHolder);
                    return facebook::react::createPromiseAsJSIValue(
                        rt,
                        [method, argReader, argWriter, strongLongLivedObjectCollection](
                            facebook::jsi::Runtime &runtime, std::shared_ptr<facebook::react::Promise> promise) {
                          method(
                              argReader,
                              argWriter,
                              [weakResolve = LongLivedJsiFunction::CreateWeak(
                                   strongLongLivedObjectCollection, runtime, std::move(promise->resolve_))](
                                  const IJSValueWriter &writer) {
                                writer.as<JSDispatcherWriter>()->WithResultArgs([weakResolve](
                                                                                    facebook::jsi::Runtime &runtime,
                                                                                    facebook::jsi::Value const *args,
                                                                                    size_t argCount) {
                                  VerifyElseCrash(argCount == 1);
                                  if (auto resolveHolder = weakResolve.lock()) {
                                    resolveHolder->Value().call(runtime, args[0]);
                                  }
                                });
                              },
                              [weakReject = LongLivedJsiFunction::CreateWeak(
                                   strongLongLivedObjectCollection, runtime, std::move(promise->reject_))](
                                  const IJSValueWriter &writer) {
                                writer.as<JSDispatcherWriter>()->WithResultArgs([weakReject](
                                                                                    facebook::jsi::Runtime &runtime,
                                                                                    facebook::jsi::Value const *args,
                                                                                    size_t argCount) {
                                  VerifyElseCrash(argCount == 1);
                                  if (auto rejectHolder = weakReject.lock()) {
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
                                    rejectHolder->Value().call(runtime, args[0]);
                                  }
                                });
                              });
                        });
                  }
                  return facebook::jsi::Value::undefined();
                });
          default:
            VerifyElseCrash(false);
        }
      }
    }

    {
      // try to find a SyncMethod
      auto it = m_moduleBuilder->SyncMethods().find(key);
      if (it != m_moduleBuilder->SyncMethods().end()) {
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
  static MethodResultCallback MakeCallback(
      facebook::jsi::Runtime &rt,
      const std::shared_ptr<facebook::react::LongLivedObjectCollection> &longLivedObjectCollection,
      const facebook::jsi::Value &callback) noexcept {
    auto weakCallback =
        LongLivedJsiFunction::CreateWeak(longLivedObjectCollection, rt, callback.getObject(rt).getFunction(rt));
    return [weakCallback = std::move(weakCallback)](const IJSValueWriter &writer) noexcept {
      writer.as<JSDispatcherWriter>()->WithResultArgs(
          [weakCallback](facebook::jsi::Runtime &rt, facebook::jsi::Value const *args, size_t count) {
            if (auto callback = weakCallback.lock()) {
              callback->Value().call(rt, args, count);
            }
          });
    };
  }

 private:
  IReactContext m_reactContext;
  winrt::com_ptr<TurboModuleBuilder> m_moduleBuilder;
  IInspectable m_providedModule;
  std::shared_ptr<implementation::HostObjectWrapper> m_hostObjectWrapper;
  std::weak_ptr<facebook::react::LongLivedObjectCollection> m_longLivedObjectCollection;
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

  auto tm = std::make_shared<TurboModuleImpl>(
      m_reactContext, moduleName, callInvoker, m_longLivedObjectCollection, /*reactModuleProvider*/ it->second);
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

std::shared_ptr<facebook::react::LongLivedObjectCollection> const &
TurboModulesProvider::LongLivedObjectCollection() noexcept {
  return m_longLivedObjectCollection;
}

} // namespace winrt::Microsoft::ReactNative
