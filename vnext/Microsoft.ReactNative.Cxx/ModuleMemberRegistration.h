// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

// We implement optional parameter macros based on the StackOverflow discussion:
// https://stackoverflow.com/questions/3046889/optional-parameters-with-c-macros
// Please refer to it if you want to understand it works.

#pragma once
#include "ReactMemberInfo.h"

// Internal implementation details.
// For each registered member we create a static method that registers it.
// The member Id is generated as a ReactMemberId<__COUNTER__> type.
// To invoke the static registration methods, we increment ReactMemberId while static member exists.
#define INTERNAL_REACT_METHOD_3_ARGS(methodType, method, methodName)                             \
  template <class TClass, class TRegistry>                                                       \
  static void RegisterMember(                                                                    \
      TRegistry &registry, winrt::Microsoft::ReactNative::ReactMemberId<__COUNTER__>) noexcept { \
    registry.Register##methodType##Method(&TClass::method, methodName);                          \
  }

#define INTERNAL_REACT_METHOD_2_ARGS(methodType, method) INTERNAL_REACT_METHOD_3_ARGS(methodType, method, L## #method)

#define INTERNAL_REACT_MEMBER_3RD_ARG(arg1, arg2, arg3, ...) arg3
#define INTERNAL_REACT_MEMBER_4TH_ARG(arg1, arg2, arg3, arg4, ...) arg4

#define INTERNAL_REACT_MEMBER_RECOMPOSER2(argsWithParentheses) INTERNAL_REACT_MEMBER_3RD_ARG argsWithParentheses
#define INTERNAL_REACT_MEMBER_RECOMPOSER3(argsWithParentheses) INTERNAL_REACT_MEMBER_4TH_ARG argsWithParentheses

#define INTERNAL_REACT_METHOD(...) \
  INTERNAL_REACT_MEMBER_RECOMPOSER2((__VA_ARGS__, INTERNAL_REACT_METHOD_3_ARGS, INTERNAL_REACT_METHOD_2_ARGS, ))

#define INTERNAL_REACT_CONSTANT_2_ARGS(field, constantName)                                      \
  template <class TClass, class TRegistry>                                                       \
  static void RegisterMember(                                                                    \
      TRegistry &registry, winrt::Microsoft::ReactNative::ReactMemberId<__COUNTER__>) noexcept { \
    registry.RegisterConstant(&TClass::field, constantName);                                     \
  }

#define INTERNAL_REACT_CONSTANT_1_ARGS(field) INTERNAL_REACT_CONSTANT_2_ARGS(field, L## #field)

#define INTERNAL_REACT_CONSTANT(...) \
  INTERNAL_REACT_MEMBER_RECOMPOSER2((__VA_ARGS__, INTERNAL_REACT_CONSTANT_2_ARGS, INTERNAL_REACT_CONSTANT_1_ARGS, ))

#define INTERNAL_REACT_EVENT_3_ARGS(field, eventName, eventEmitterName)                          \
  template <class TClass, class TRegistry>                                                       \
  static void RegisterMember(                                                                    \
      TRegistry &registry, winrt::Microsoft::ReactNative::ReactMemberId<__COUNTER__>) noexcept { \
    registry.RegisterEvent(&TClass::field, eventName, eventEmitterName);                         \
  }

#define INTERNAL_REACT_EVENT_2_ARGS(field, eventName) \
  INTERNAL_REACT_EVENT_3_ARGS(field, eventName, /*defined by REACT_MODULE*/ nullptr)

#define INTERNAL_REACT_EVENT_1_ARGS(field) INTERNAL_REACT_EVENT_2_ARGS(field, L## #field)

#define INTERNAL_REACT_EVENT(...)    \
  INTERNAL_REACT_MEMBER_RECOMPOSER3( \
      (__VA_ARGS__, INTERNAL_REACT_EVENT_3_ARGS, INTERNAL_REACT_EVENT_2_ARGS, INTERNAL_REACT_EVENT_1_ARGS, ))

#define INTERNAL_REACT_FUNCTION_3_ARGS(field, functionName, moduleName)                          \
  template <class TClass, class TRegistry>                                                       \
  static void RegisterMember(                                                                    \
      TRegistry &registry, winrt::Microsoft::ReactNative::ReactMemberId<__COUNTER__>) noexcept { \
    registry.RegisterFunction(&TClass::field, functionName, moduleName);                         \
  }

#define INTERNAL_REACT_FUNCTION_2_ARGS(field, functionName) \
  INTERNAL_REACT_FUNCTION_3_ARGS(field, functionName, /*defined by REACT_MODULE*/ nullptr)

#define INTERNAL_REACT_FUNCTION_1_ARGS(field) INTERNAL_REACT_FUNCTION_2_ARGS(field, L## #field)

#define INTERNAL_REACT_FUNCTION(...) \
  INTERNAL_REACT_MEMBER_RECOMPOSER3( \
      (__VA_ARGS__, INTERNAL_REACT_FUNCTION_3_ARGS, INTERNAL_REACT_FUNCTION_2_ARGS, INTERNAL_REACT_FUNCTION_1_ARGS, ))
