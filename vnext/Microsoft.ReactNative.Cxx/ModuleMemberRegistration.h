// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

// We implement optional parameter macros based on the StackOverflow discussion:
// https://stackoverflow.com/questions/3046889/optional-parameters-with-c-macros
// Please refer to it if you want to understand it works.

#pragma once
#include "ReactMemberInfo.h"

// Internal implementation details.
// Registration of a method relies on a TLS context that is setup when object is
// created. The advantage is that we do zero work during static initialization.
// The disadvantage is that we require to have one bool field per registration.
#define INTERNAL_REACT_METHOD_3_ARGS(methodType, method, methodName)                                     \
  template <class TClass, class TRegistry>                                                               \
  static void RegisterMember(                                                                            \
      TRegistry &registry, winrt::Microsoft::ReactNative::Bridge::ReactMemberId<__COUNTER__>) noexcept { \
    registry.Register##methodType##Method<TClass>(&TClass::method, methodName);                          \
  }

#define INTERNAL_REACT_METHOD_2_ARGS(methodType, method) INTERNAL_REACT_METHOD_3_ARGS(methodType, method, L## #method)

#define INTERNAL_REACT_MEMBER_3RD_ARG(arg1, arg2, arg3, ...) arg3

#define INTERNAL_REACT_MEMBER_RECOMPOSER(argsWithParentheses) INTERNAL_REACT_MEMBER_3RD_ARG argsWithParentheses

#define INTERNAL_REACT_METHOD(...) \
  INTERNAL_REACT_MEMBER_RECOMPOSER((__VA_ARGS__, INTERNAL_REACT_METHOD_3_ARGS, INTERNAL_REACT_METHOD_2_ARGS, ))

#define INTERNAL_REACT_CONSTANT_2_ARGS(field, constantName)                                              \
  template <class TClass, class TRegistry>                                                               \
  static void RegisterMember(                                                                            \
      TRegistry &registry, winrt::Microsoft::ReactNative::Bridge::ReactMemberId<__COUNTER__>) noexcept { \
    registry.RegisterConstant<TClass>(&TClass::field, constantName);                                     \
  }

#define INTERNAL_REACT_CONSTANT_1_ARGS(field) INTERNAL_REACT_CONSTANT_2_ARGS(field, L## #field)

#define INTERNAL_REACT_CONSTANT(...) \
  INTERNAL_REACT_MEMBER_RECOMPOSER((__VA_ARGS__, INTERNAL_REACT_CONSTANT_2_ARGS, INTERNAL_REACT_CONSTANT_1_ARGS, ))

#define INTERNAL_REACT_EVENT_2_ARGS(field, eventName)                                                    \
  template <class TClass, class TRegistry>                                                               \
  static void RegisterMember(                                                                            \
      TRegistry &registry, winrt::Microsoft::ReactNative::Bridge::ReactMemberId<__COUNTER__>) noexcept { \
    registry.RegisterEvent<TClass>(&TClass::field, eventName);                                           \
  }

#define INTERNAL_REACT_EVENT_1_ARGS(field) INTERNAL_REACT_EVENT_2_ARGS(field, L## #field)

#define INTERNAL_REACT_EVENT(...) \
  INTERNAL_REACT_MEMBER_RECOMPOSER((__VA_ARGS__, INTERNAL_REACT_EVENT_2_ARGS, INTERNAL_REACT_EVENT_1_ARGS, ))
