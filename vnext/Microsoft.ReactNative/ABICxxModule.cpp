// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include "ABICxxModule.h"
#include "DynamicWriter.h"
#include "cxxreact/Instance.h"

using namespace facebook::xplat::module;

namespace winrt::Microsoft::ReactNative::Bridge {

ABICxxModule::ABICxxModule(
    winrt::Windows::Foundation::IInspectable &nativeModule,
    std::string name,
    std::string eventEmitterName,
    std::vector<CxxModule::Method> methods,
    std::vector<ConstantProvider> constants,
    std::vector<ABICxxModuleEventHandlerSetter> eventHandlerSetters) noexcept
    : m_nativeModule{nativeModule},
      m_name{std::move(name)},
      m_eventEmitterName{std::move(eventEmitterName)},
      m_methods{std::move(methods)},
      m_constants(std::move(constants)) {
  InitEvents(std::move(eventHandlerSetters));
}

std::string ABICxxModule::getName() {
  return m_name;
}

std::map<std::string, folly::dynamic> ABICxxModule::getConstants() {
  std::map<std::string, folly::dynamic> result;

  IJSValueWriter argWriter = winrt::make<DynamicWriter>();
  argWriter.WriteObjectBegin();
  for (auto &constWriter : m_constants) {
    constWriter(argWriter);
  }
  argWriter.WriteObjectEnd();

  folly::dynamic constants = argWriter.as<DynamicWriter>()->TakeValue();
  for (auto &item : constants.items()) {
    result[item.first.asString()] = std::move(item.second);
  }

  return result;
}

std::vector<CxxModule::Method> ABICxxModule::getMethods() {
  return m_methods;
}

void ABICxxModule::InitEvents(std::vector<ABICxxModuleEventHandlerSetter> eventHandlerSetters) noexcept {
  for (auto &eventHandler : eventHandlerSetters) {
    eventHandler.EventHandlerSetter([ this, name = eventHandler.Name ](ReactArgWriter const &argWriter) noexcept {
      if (std::shared_ptr<facebook::react::Instance> instance = getInstance().lock()) {
        DynamicWriter writer;
        writer.WriteArrayBegin();
        writer.WriteString(winrt::to_hstring(name));
        argWriter(writer);
        writer.WriteArrayEnd();

        std::string emitterName = m_eventEmitterName.empty() ? DefaultEventEmitterName : m_eventEmitterName;
        instance->callJSFunction(std::move(emitterName), "emit", writer.TakeValue());
      }
    });
  }
}

} // namespace winrt::Microsoft::ReactNative::Bridge
