
/**
 * This file is auto-generated from a TurboModule Spec file
 */

#pragma once

#include "NativeModules.h"

namespace SampleLibraryCpp {

/*
 * This function registers a class as implementing a TurboModule Spec
 * The class must implement all the methods as required by the Spec.
 * Methods may be implemented using one of the method signatures provided above the RegisterMethod calls
 *
 * Note that methods can be optionally written using a veriety of method signatures which may be preferable
 * depending on your scenario.
 * 
 * An example implementation you could copy/paste is provided here:

class MyModule {
  void Initialize() {
  }

  void MyModule::voidFunc() noexcept {
    throw std::logic_error("MyModule::voidFunc not yet implemented.");
  }

  bool MyModule::getBool(bool) noexcept {
    throw std::logic_error("MyModule::getBool not yet implemented.");
  }

  double MyModule::getNumber(double) noexcept {
    throw std::logic_error("MyModule::getNumber not yet implemented.");
  }

  std::string MyModule::getString(std::string) noexcept {
    throw std::logic_error("MyModule::getString not yet implemented.");
  }
};

 */
template <typename TMyModuleImpl>
void RegisterMyModuleModule(ReactModuleBuilder<TMyModuleImpl> &moduleBuilder) noexcept {
  moduleBuilder.RegisterModuleName(L"MyModule");
  moduleBuilder.RegisterInitMethod(&TMyModuleImpl::Initialize);

  // This method can be implemented using the following method signature:
  // void TMyModuleImpl::voidFunc() noexcept;
  moduleBuilder.RegisterMethod/*<void()>*/(&TMyModuleImpl::voidFunc, L"voidFunc");

  // This method can be implemented using the following method signature:
  // bool TMyModuleImpl::getBool(bool) noexcept;
  moduleBuilder.RegisterSyncMethod/*<bool(bool)>*/(&TMyModuleImpl::getBool, L"getBool");

  // This method can be implemented using the following method signature:
  // double TMyModuleImpl::getNumber(double) noexcept;
  moduleBuilder.RegisterSyncMethod/*<double(double)>*/(&TMyModuleImpl::getNumber, L"getNumber");

  // This method can be implemented using the following method signature:
  // std::string TMyModuleImpl::getString(std::string) noexcept;
  moduleBuilder.RegisterSyncMethod/*<std::string(std::string)>*/(&TMyModuleImpl::getString, L"getString");
}

} // namespace SampleLibraryCpp
