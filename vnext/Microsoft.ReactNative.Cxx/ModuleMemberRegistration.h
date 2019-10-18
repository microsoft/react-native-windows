// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once
#include "winrt/Microsoft.ReactNative.Bridge.h"
#include "winrt/Microsoft.ReactNative.h"

// Internal implementation details.
// Registration of a method relies on a TLS context that is setup when object is
// created. The advantage is that we do zero work during static initialization.
// The disadvantage is that we require to have one bool field per registration.
#define INTERNAL_REACT_METHOD_4_ARGS(methodType, isAsync, method, methodName) \
  bool REACT_reg##method{                                                     \
      ::Microsoft::ReactNative::Module##methodType##MethodInfo<decltype(      \
          &std::remove_pointer_t<decltype(this)>::method)>::                  \
          Register(                                                           \
              this,                                                           \
              methodName,                                                     \
              &std::remove_pointer_t<decltype(this)>::method,                 \
              isAsync)};

#define INTERNAL_REACT_METHOD_3_ARGS(methodType, isAsync, method) \
  INTERNAL_REACT_METHOD_4_ARGS(methodType, isAsync, method, #method)

#define INTERNAL_REACT_MEMBER_3RD_ARG(arg1, arg2, arg3, ...) arg3

#define INTERNAL_REACT_MEMBER_RECOMPOSER(argsWithParentheses) \
  INTERNAL_REACT_MEMBER_3RD_ARG argsWithParentheses

#define INTERNAL_REACT_METHOD_MACRO_CHOOSER(...) \
  INTERNAL_REACT_MEMBER_RECOMPOSER(              \
      (__VA_ARGS__,                              \
       INTERNAL_REACT_METHOD_4_ARGS,             \
       INTERNAL_REACT_METHOD_3_ARGS, ))

#define INTERNAL_REACT_CONSTANT_2_ARGS(field, constantName)    \
  bool REACT_reg##field{                                       \
      ::Microsoft::ReactNative::ModuleConstFieldInfo<decltype( \
          &std::remove_pointer_t<decltype(this)>::field)>::    \
          Register(                                            \
              this,                                            \
              constantName,                                    \
              &std::remove_pointer_t<decltype(this)>::field)};

#define INTERNAL_REACT_CONSTANT_1_ARGS(field) \
  INTERNAL_REACT_CONSTANT_2_ARGS(field, #field)

#define INTERNAL_REACT_CONSTANT_MACRO_CHOOSER(...) \
  INTERNAL_REACT_MEMBER_RECOMPOSER(                \
      (__VA_ARGS__,                                \
       INTERNAL_REACT_CONSTANT_2_ARGS,             \
       INTERNAL_REACT_CONSTANT_1_ARGS, ))

#define INTERNAL_REACT_EVENT_2_ARGS(field, eventName)          \
  bool REACT_reg##field{                                       \
      ::Microsoft::ReactNative::ModuleEventFieldInfo<decltype( \
          &std::remove_pointer_t<decltype(this)>::field)>::    \
          Register(                                            \
              this,                                            \
              eventName,                                       \
              &std::remove_pointer_t<decltype(this)>::field)};

#define INTERNAL_REACT_EVENT_1_ARGS(field) \
  INTERNAL_REACT_EVENT_2_ARGS(field, #field)

#define INTERNAL_REACT_EVENT_MACRO_CHOOSER(...) \
  INTERNAL_REACT_MEMBER_RECOMPOSER(             \
      (__VA_ARGS__,                             \
       INTERNAL_REACT_EVENT_2_ARGS,             \
       INTERNAL_REACT_EVENT_1_ARGS, ))
