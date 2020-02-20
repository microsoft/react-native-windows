// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include "ABICxxModule.h"
#include "DynamicWriter.h"

using namespace facebook::xplat::module;

namespace winrt::Microsoft::ReactNative {

ABICxxModule::ABICxxModule(
    winrt::Windows::Foundation::IInspectable const &nativeModule,
    std::string &&name,
    std::vector<ConstantProviderDelegate> &&constantProviders,
    std::vector<facebook::xplat::module::CxxModule::Method> &&methods) noexcept
    : m_nativeModule{nativeModule},
      m_name{std::move(name)},
      m_constantProviders(std::move(constantProviders)),
      m_methods(std::move(methods)) {}

std::string ABICxxModule::getName() noexcept {
  return m_name;
}

std::map<std::string, folly::dynamic> ABICxxModule::getConstants() noexcept {
  std::map<std::string, folly::dynamic> result;

  IJSValueWriter argWriter = winrt::make<DynamicWriter>();
  argWriter.WriteObjectBegin();
  for (auto &constWriter : m_constantProviders) {
    constWriter(argWriter);
  }
  argWriter.WriteObjectEnd();

  folly::dynamic constants = argWriter.as<DynamicWriter>()->TakeValue();
  if (constants.isObject()) {
    for (auto &item : constants.items()) {
      result[item.first.asString()] = std::move(item.second);
    }
  }

  return result;
}

std::vector<CxxModule::Method> ABICxxModule::getMethods() noexcept {
  auto result = std::move(m_methods);
  if (result.empty()) {
    // Module without methods will be not registered in JS configuration.
    // It will cause index mismatch between JS and C++ code.
    // As a result, JS code will code methods on a wrong native module.
    // See for details ReactCommon\cxxreact\ModuleRegistry.cpp, line 133: that reads 'if (!methodNames.empty()) {'
    // To work around this issue we add a Dummy method.
    result.emplace_back("Dummy", []() {});
  }

  return result;
}

} // namespace winrt::Microsoft::ReactNative
