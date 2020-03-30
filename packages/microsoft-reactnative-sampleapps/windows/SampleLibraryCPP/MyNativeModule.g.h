// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

// TODO: This file should be generated from the spec file

#pragma once

#include "NativeModules.h"
#include "MyNativeModule.h"

namespace SampleLibraryCpp {

void RegisterModule(ReactModuleBuilder<MyNativeModule> &moduleBuilder) noexcept {
  moduleBuilder.RegisterModuleName(L"MyNativeModule");
  moduleBuilder.RegisterInitMethod(&MyNativeModule::Initialize);
  moduleBuilder.RegisterConstant/*<bool>*/(&MyNativeModule::const1, L"const1");
  moduleBuilder.RegisterConstant/*<number>*/(&MyNativeModule::const2, L"const2");
  moduleBuilder.RegisterConstant/*<string>*/(&MyNativeModule::const3, L"const3");
  moduleBuilder.RegisterMethod/*<void()>*/(&MyNativeModule::voidFunc, L"voidFunc");
  moduleBuilder.RegisterMethod/*<bool(bool)>*/(&MyNativeModule::getBool, L"getBool");
  moduleBuilder.RegisterMethod/*<number(number)>*/(&MyNativeModule::getNumber, L"getNumber");
  moduleBuilder.RegisterMethod/*<string(string)>*/(&MyNativeModule::getString, L"getString");

//  moduleBuilder.RegisterMethod/*<Array<any>(Array<any>)>*/(&MyNativeModule::getArray, L"getArray");
//  moduleBuilder.RegisterMethod/*<Object(Object)>*/(&MyNativeModule::getObject, L"getObject");
//  moduleBuilder.RegisterMethod/*<Object(number, string, Object)>*/(&MyNativeModule::getValue, L"getValue");
//  moduleBuilder.RegisterMethod/*<void(callback)>*/(&MyNativeModule::getValueWithCallback, L"getValueWithCallback");
//  moduleBuilder.RegisterMethod/*<promise(bool)>*/(&MyNativeModule::getValueWithPromise, L"getValueWithPromise");
}

} // namespace SampleLibraryCpp
