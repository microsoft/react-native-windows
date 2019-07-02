// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"

// react-native-uwp.dll never links JavaScriptCore (we always use Chakra)
// however, since we share ReactCommon with react-native-win32, which can
// optionally use JavaScriptCore (temporarily), we need to stub some stuff out
// so we can link. This file should go away when we stop comparing perf
// of JavaScriptCore and Chakra so we can remove JavaScriptCore entirely.

extern "C" {

__declspec(dllexport) void *__imp_JSValueMakeString = nullptr;
__declspec(dllexport) void *__imp_JSValueMakeFromJSONString = nullptr;
__declspec(dllexport) void *__imp_JSValueCreateJSONString = nullptr;
__declspec(dllexport) void *__imp_JSValueToStringCopy = nullptr;
__declspec(dllexport) void *__imp_JSValueToObject = nullptr;
__declspec(dllexport) void *__imp_JSValueUnprotect = nullptr;
__declspec(dllexport) void *__imp_JSObjectMake = nullptr;
__declspec(dllexport) void *__imp_JSObjectMakeError = nullptr;
__declspec(dllexport) void *__imp_JSObjectGetProperty = nullptr;
__declspec(dllexport) void *__imp_JSObjectSetProperty = nullptr;
__declspec(dllexport) void *__imp_JSObjectGetPropertyAtIndex = nullptr;
__declspec(dllexport) void *__imp_JSObjectSetPropertyAtIndex = nullptr;
__declspec(dllexport) void *__imp_JSObjectCallAsFunction = nullptr;
__declspec(dllexport) void *__imp_JSObjectCallAsConstructor = nullptr;
__declspec(dllexport) void *__imp_JSObjectCopyPropertyNames = nullptr;
__declspec(dllexport) void *__imp_JSPropertyNameArrayRelease = nullptr;
__declspec(dllexport) void *__imp_JSPropertyNameArrayGetCount = nullptr;
__declspec(dllexport) void *__imp_JSPropertyNameArrayGetNameAtIndex = nullptr;
__declspec(dllexport) void *__imp_JSContextGetGlobalObject = nullptr;
__declspec(dllexport) void *__imp_JSStringCreateWithUTF8CString = nullptr;
__declspec(dllexport) void *__imp_JSStringRetain = nullptr;
__declspec(dllexport) void *__imp_JSStringRelease = nullptr;
__declspec(dllexport) void *__imp_JSStringGetLength = nullptr;
__declspec(dllexport) void *__imp_JSStringGetCharactersPtr = nullptr;
__declspec(dllexport) void *__imp_JSValueGetType = nullptr;
__declspec(dllexport) void *__imp_JSValueMakeUndefined = nullptr;
__declspec(dllexport) void *__imp_JSValueMakeNull = nullptr;
__declspec(dllexport) void *__imp_JSValueMakeNumber = nullptr;
__declspec(dllexport) void *__imp_JSClassCreate = nullptr;
__declspec(dllexport) void *__imp_JSClassRelease = nullptr;
__declspec(dllexport) void *__imp_JSObjectGetPrivate = nullptr;
__declspec(dllexport) void *__imp_JSGlobalContextCreateInGroup = nullptr;
__declspec(dllexport) void *__imp_JSGlobalContextRelease = nullptr;
__declspec(dllexport) void *__imp_JSGlobalContextSetName = nullptr;
__declspec(dllexport) void *__imp_JSStringIsEqualToUTF8CString = nullptr;
__declspec(dllexport) void *__imp_JSEvaluateScript = nullptr;
__declspec(dllexport) void *__imp_JSObjectMakeFunctionWithCallback = nullptr;
__declspec(dllexport) void *__imp_JSValueProtect = nullptr;
__declspec(dllexport) void *__imp_JSValueToNumber = nullptr;
__declspec(dllexport) void *__imp_JSObjectSetPrivate = nullptr;
}
