// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include <JSValue.h>
#include <functional>
#include <tuple>
#include <type_traits>
#include <unordered_map>

#ifdef _M_IX86

#define CALL_MEMBER(FUNC) \
  FUNC(__cdecl)           \
  FUNC(__stdcall)         \
  FUNC(__thiscall)        \
  FUNC(__vectorcall)
#define CALL_NON_MEMBER(FUNC) \
  FUNC(__cdecl)               \
  FUNC(__stdcall)             \
  FUNC(__vectorcall)
#else
#define CALL_MEMBER(FUNC) FUNC(__cdecl)
#define CALL_NON_MEMBER(FUNC) FUNC(__cdecl)
#endif

template <size_t TIndex, typename TFunc>
struct get_argument_type_impl;

#define GET_ARGUMENT_TYPE_IMPL(CALL_OPT)                                          \
  template <size_t TIndex, typename TRet, typename... TArgs>                      \
  struct get_argument_type_impl<TIndex, TRet CALL_OPT(TArgs...)> {                \
    typedef typename std::tuple_element<TIndex, std::tuple<TArgs...>>::type type; \
  };

CALL_NON_MEMBER(GET_ARGUMENT_TYPE_IMPL)
#undef GET_ARGUMENT_TYPE_IMPL

#define GET_ARGUMENT_TYPE_IMPL(CALL_OPT)                                          \
  template <size_t TIndex, typename TType, typename TRet, typename... TArgs>      \
  struct get_argument_type_impl<TIndex, TRet (CALL_OPT TType::*)(TArgs...)> {     \
    typedef typename std::tuple_element<TIndex, std::tuple<TArgs...>>::type type; \
  };

CALL_MEMBER(GET_ARGUMENT_TYPE_IMPL)
#undef GET_ARGUMENT_TYPE_IMPL

template <size_t TIndex, typename TFunc>
struct get_argument_type {
  typedef typename get_argument_type_impl<TIndex, TFunc>::type type;
};

template <typename T>
struct json_type_traits;

template <typename T>
struct json_type_traits<std::vector<T>> {
  static std::vector<T> parseJson(const winrt::Microsoft::ReactNative::JSValue &obj) {
    std::vector<T> result;
    for (const auto &item : obj.AsArray()) {
      result.push_back(json_type_traits<T>::parseJson(item));
    }
    return result;
  }
};

template <>
struct json_type_traits<winrt::Microsoft::ReactNative::JSValue> {
  static const winrt::Microsoft::ReactNative::JSValue &parseJson(const winrt::Microsoft::ReactNative::JSValue &obj) {
    return obj;
  }
};

template <>
struct json_type_traits<double> {
  static double parseJson(const winrt::Microsoft::ReactNative::JSValue &obj) {
    return obj.AsDouble();
  }
};

template <>
struct json_type_traits<std::string> {
  static std::string parseJson(const winrt::Microsoft::ReactNative::JSValue &obj) {
    return obj.AsString();
  }
};
