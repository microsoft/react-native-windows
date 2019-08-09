// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include <folly/dynamic.h>
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

#define GET_ARGUMENT_TYPE_IMPL(CALL_OPT)                                      \
  template <size_t TIndex, typename TRet, typename... TArgs>                  \
  struct get_argument_type_impl<TIndex, TRet CALL_OPT(TArgs...)> {            \
    typedef                                                                   \
        typename std::tuple_element<TIndex, std::tuple<TArgs...>>::type type; \
  };

CALL_NON_MEMBER(GET_ARGUMENT_TYPE_IMPL)
#undef GET_ARGUMENT_TYPE_IMPL

#define GET_ARGUMENT_TYPE_IMPL(CALL_OPT)                                      \
  template <size_t TIndex, typename TType, typename TRet, typename... TArgs>  \
  struct get_argument_type_impl<TIndex, TRet (CALL_OPT TType::*)(TArgs...)> { \
    typedef                                                                   \
        typename std::tuple_element<TIndex, std::tuple<TArgs...>>::type type; \
  };

CALL_MEMBER(GET_ARGUMENT_TYPE_IMPL)
#undef GET_ARGUMENT_TYPE_IMPL

template <size_t TIndex, typename TFunc>
struct get_argument_type {
  typedef typename get_argument_type_impl<TIndex, TFunc>::type type;
};

template <typename T>
struct json_type_traits;
//{
//  static_assert(false, "implement type traits to support parsing of your type
//  form folly::dynamic");
//};

template <typename T>
struct json_type_traits<std::vector<T>> {
  static std::vector<T> parseJson(const folly::dynamic &obj) {
    std::vector<T> result;
    for (const auto &item : obj) {
      result.push_back(json_type_traits<T>::parseJson(item));
    }
    return result;
  }
};

template <>
struct json_type_traits<folly::dynamic> {
  static const folly::dynamic &parseJson(const folly::dynamic &obj) {
    return obj;
  }
};

template <>
struct json_type_traits<double> {
  static double parseJson(const folly::dynamic &obj) {
    return obj.asDouble();
  }
};

template <>
struct json_type_traits<std::string> {
  static std::string parseJson(const folly::dynamic &obj) {
    return obj.asString();
  }
};

#define RCT_BEGIN_PROPERTY_MAP(className)                                  \
  std::unordered_map<                                                      \
      std::string,                                                         \
      std::function<void(react::uwp::XamlView &, const folly::dynamic &)>> \
      m_propertyHandlers;                                                  \
  void SetupPropertyHandlersInternal() {                                   \
    typedef className class_name;

#define RCT_PROPERTY(name, handler)                                            \
  {                                                                            \
    typedef                                                                    \
        typename get_argument_type<1, decltype(&class_name::handler)>::type    \
            second_argument_t;                                                 \
    typedef typename std::remove_const_t<                                      \
        std::remove_reference_t<second_argument_t>>                            \
        argument_type_t;                                                       \
    m_propertyHandlers[name] = [this](                                         \
                                   react::uwp::XamlView &view,                 \
                                   const folly::dynamic &prop) {               \
      typedef decltype(                                                        \
          json_type_traits<argument_type_t>::parseJson(prop)) value_type_t;    \
      value_type_t value = json_type_traits<argument_type_t>::parseJson(prop); \
      handler(view, value);                                                    \
    };                                                                         \
  }

#define RCT_END_PROPERTY_MAP()                                           \
  }                                                                      \
  void UpdatePropertiesInternal(                                         \
      react::uwp::XamlView &view, const folly::dynamic &diffMap) {       \
    for (auto &pair : diffMap.items()) {                                 \
      auto propHandler = m_propertyHandlers.find(pair.first.asString()); \
      if (propHandler != m_propertyHandlers.end()) {                     \
        propHandler->second(view, pair.second);                          \
      }                                                                  \
    }                                                                    \
  }
