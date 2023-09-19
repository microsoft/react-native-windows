/**
 * This code was generated by [react-native-codegen](https://www.npmjs.com/package/react-native-codegen).
 *
 * Do not edit this file as changes may cause incorrect behavior and will be lost
 * once the code is regenerated.
 *
 * @generated by codegen project: GenerateModuleH.js
 */

#include "msrnIntegrationTestsJSI.h"

namespace facebook {
namespace react {

static jsi::Value __hostFunction_NativeMySimpleTurboModuleCxxCxxSpecJSI_getConstants(jsi::Runtime &rt, TurboModule &turboModule, const jsi::Value* args, size_t count) {
  return static_cast<NativeMySimpleTurboModuleCxxCxxSpecJSI *>(&turboModule)->getConstants(
    rt
  );
}
static jsi::Value __hostFunction_NativeMySimpleTurboModuleCxxCxxSpecJSI_logAction(jsi::Runtime &rt, TurboModule &turboModule, const jsi::Value* args, size_t count) {
  static_cast<NativeMySimpleTurboModuleCxxCxxSpecJSI *>(&turboModule)->logAction(
    rt,
    args[0].asString(rt),
    jsi::Value(rt, args[1])
  );
  return jsi::Value::undefined();
}
static jsi::Value __hostFunction_NativeMySimpleTurboModuleCxxCxxSpecJSI_voidFunc(jsi::Runtime &rt, TurboModule &turboModule, const jsi::Value* args, size_t count) {
  static_cast<NativeMySimpleTurboModuleCxxCxxSpecJSI *>(&turboModule)->voidFunc(
    rt
  );
  return jsi::Value::undefined();
}
static jsi::Value __hostFunction_NativeMySimpleTurboModuleCxxCxxSpecJSI_getBool(jsi::Runtime &rt, TurboModule &turboModule, const jsi::Value* args, size_t count) {
  return static_cast<NativeMySimpleTurboModuleCxxCxxSpecJSI *>(&turboModule)->getBool(
    rt,
    args[0].asBool()
  );
}
static jsi::Value __hostFunction_NativeMySimpleTurboModuleCxxCxxSpecJSI_getNumber(jsi::Runtime &rt, TurboModule &turboModule, const jsi::Value* args, size_t count) {
  return static_cast<NativeMySimpleTurboModuleCxxCxxSpecJSI *>(&turboModule)->getNumber(
    rt,
    args[0].asNumber()
  );
}
static jsi::Value __hostFunction_NativeMySimpleTurboModuleCxxCxxSpecJSI_getString(jsi::Runtime &rt, TurboModule &turboModule, const jsi::Value* args, size_t count) {
  return static_cast<NativeMySimpleTurboModuleCxxCxxSpecJSI *>(&turboModule)->getString(
    rt,
    args[0].asString(rt)
  );
}
static jsi::Value __hostFunction_NativeMySimpleTurboModuleCxxCxxSpecJSI_getArray(jsi::Runtime &rt, TurboModule &turboModule, const jsi::Value* args, size_t count) {
  return static_cast<NativeMySimpleTurboModuleCxxCxxSpecJSI *>(&turboModule)->getArray(
    rt,
    args[0].asObject(rt).asArray(rt)
  );
}
static jsi::Value __hostFunction_NativeMySimpleTurboModuleCxxCxxSpecJSI_getObject(jsi::Runtime &rt, TurboModule &turboModule, const jsi::Value* args, size_t count) {
  return static_cast<NativeMySimpleTurboModuleCxxCxxSpecJSI *>(&turboModule)->getObject(
    rt,
    args[0].asObject(rt)
  );
}
static jsi::Value __hostFunction_NativeMySimpleTurboModuleCxxCxxSpecJSI_getValue(jsi::Runtime &rt, TurboModule &turboModule, const jsi::Value* args, size_t count) {
  return static_cast<NativeMySimpleTurboModuleCxxCxxSpecJSI *>(&turboModule)->getValue(
    rt,
    args[0].asNumber(),
    args[1].asString(rt),
    args[2].asObject(rt)
  );
}
static jsi::Value __hostFunction_NativeMySimpleTurboModuleCxxCxxSpecJSI_getValueWithCallback(jsi::Runtime &rt, TurboModule &turboModule, const jsi::Value* args, size_t count) {
  static_cast<NativeMySimpleTurboModuleCxxCxxSpecJSI *>(&turboModule)->getValueWithCallback(
    rt,
    args[0].asObject(rt).asFunction(rt)
  );
  return jsi::Value::undefined();
}
static jsi::Value __hostFunction_NativeMySimpleTurboModuleCxxCxxSpecJSI_getValueWithPromise(jsi::Runtime &rt, TurboModule &turboModule, const jsi::Value* args, size_t count) {
  return static_cast<NativeMySimpleTurboModuleCxxCxxSpecJSI *>(&turboModule)->getValueWithPromise(
    rt,
    args[0].asBool()
  );
}

NativeMySimpleTurboModuleCxxCxxSpecJSI::NativeMySimpleTurboModuleCxxCxxSpecJSI(std::shared_ptr<CallInvoker> jsInvoker)
  : TurboModule("MySimpleTurboModuleCxx", jsInvoker) {
  methodMap_["getConstants"] = MethodMetadata {0, __hostFunction_NativeMySimpleTurboModuleCxxCxxSpecJSI_getConstants};
  methodMap_["logAction"] = MethodMetadata {2, __hostFunction_NativeMySimpleTurboModuleCxxCxxSpecJSI_logAction};
  methodMap_["voidFunc"] = MethodMetadata {0, __hostFunction_NativeMySimpleTurboModuleCxxCxxSpecJSI_voidFunc};
  methodMap_["getBool"] = MethodMetadata {1, __hostFunction_NativeMySimpleTurboModuleCxxCxxSpecJSI_getBool};
  methodMap_["getNumber"] = MethodMetadata {1, __hostFunction_NativeMySimpleTurboModuleCxxCxxSpecJSI_getNumber};
  methodMap_["getString"] = MethodMetadata {1, __hostFunction_NativeMySimpleTurboModuleCxxCxxSpecJSI_getString};
  methodMap_["getArray"] = MethodMetadata {1, __hostFunction_NativeMySimpleTurboModuleCxxCxxSpecJSI_getArray};
  methodMap_["getObject"] = MethodMetadata {1, __hostFunction_NativeMySimpleTurboModuleCxxCxxSpecJSI_getObject};
  methodMap_["getValue"] = MethodMetadata {3, __hostFunction_NativeMySimpleTurboModuleCxxCxxSpecJSI_getValue};
  methodMap_["getValueWithCallback"] = MethodMetadata {1, __hostFunction_NativeMySimpleTurboModuleCxxCxxSpecJSI_getValueWithCallback};
  methodMap_["getValueWithPromise"] = MethodMetadata {1, __hostFunction_NativeMySimpleTurboModuleCxxCxxSpecJSI_getValueWithPromise};
}
static jsi::Value __hostFunction_NativeMyTrivialTurboModuleCxxSpecJSI_startFromJS(jsi::Runtime &rt, TurboModule &turboModule, const jsi::Value* args, size_t count) {
  static_cast<NativeMyTrivialTurboModuleCxxSpecJSI *>(&turboModule)->startFromJS(
    rt
  );
  return jsi::Value::undefined();
}

NativeMyTrivialTurboModuleCxxSpecJSI::NativeMyTrivialTurboModuleCxxSpecJSI(std::shared_ptr<CallInvoker> jsInvoker)
  : TurboModule("MyTrivialTurboModule", jsInvoker) {
  methodMap_["startFromJS"] = MethodMetadata {0, __hostFunction_NativeMyTrivialTurboModuleCxxSpecJSI_startFromJS};
}


} // namespace react
} // namespace facebook