// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include "DebugHelpers.h"
#include "..\..\codegen\SampleAppJSI.h"

namespace SampleLibraryCpp {

/**
 * A sample implementation of the C++ spec. In practice, this class can just
 * extend jsi::HostObject directly, but using the spec provides build-time
 * type-safety.
 */
class MyJsiModuleCxx : public facebook::react::NativeMyJsiModuleCxxSpecJSI {
 public:
  MyJsiModuleCxx(std::shared_ptr<facebook::react::CallInvoker> jsInvoker);

  facebook::jsi::Object getConstants(facebook::jsi::Runtime &rt) override;
  void voidFunc(facebook::jsi::Runtime &rt) override;
  bool getBool(facebook::jsi::Runtime &rt, bool arg) override;
  double getNumber(facebook::jsi::Runtime &rt, double arg) override;
  facebook::jsi::String getString(facebook::jsi::Runtime &rt, facebook::jsi::String arg) override;
  facebook::jsi::Array getArray(facebook::jsi::Runtime &rt, facebook::jsi::Array arg) override;
  facebook::jsi::Object getObject(facebook::jsi::Runtime &rt, facebook::jsi::Object arg) override;
  facebook::jsi::Object getValue(facebook::jsi::Runtime &rt, double x, facebook::jsi::String y, facebook::jsi::Object z) override;
  void getValueWithCallback(facebook::jsi::Runtime &rt, facebook::jsi::Function callback) override;
  facebook::jsi::Value getValueWithPromise(facebook::jsi::Runtime &rt, bool error) override;
};

} // namespace SampleLibraryCpp
