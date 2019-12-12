// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once
#ifndef MICROSOFT_REACTNATIVE_JSVALUE
#define MICROSOFT_REACTNATIVE_JSVALUE

#include "Crash.h"
#include "winrt/Microsoft.ReactNative.Bridge.h"

namespace winrt::Microsoft::ReactNative::Bridge {

// Forward declarations
struct JSValue;
IJSValueReader MakeJSValueTreeReader(const JSValue &root) noexcept;
IJSValueReader MakeJSValueTreeReader(JSValue &&root) noexcept;
IJSValueWriter MakeJSValueTreeWriter(JSValue &resultValue) noexcept;

// Type alias for JSValue object type
using JSValueObject = std::map<std::string, JSValue, std::less<>>;

// Type alias for JSValue array type
using JSValueArray = std::vector<JSValue>;

// A view for JSValueObject
struct JSValueObjectView {
  JSValueObjectView(const JSValueObject &object) noexcept;
  auto begin() const noexcept;
  auto end() const noexcept;

 private:
  const JSValueObject &m_object;
};

// A view for JSObjectArray
struct JSValueArrayView {
  JSValueArrayView(const JSValueArray &array) noexcept;
  auto begin() const noexcept;
  auto end() const noexcept;

 private:
  const JSValueArray &m_array;
};

// JSValue represents an immutable JavaScript value that can be passed as a parameter.
// It is created to simplify working with IJSValueReader in some complex cases.
// It takes more resources than direct use of IJSValueReader, but provides more flexibility.
// The JSValue is an immutable and is safe to be used from multiple threads.
// It is move-only to avoid unnecessary or unexpected copying of values.
struct JSValue {
  static const JSValue Null;
  static const JSValueObject EmptyObject;
  static const JSValueArray EmptyArray;
  static const std::string EmptyString;

  JSValue() noexcept;
  JSValue(std::nullptr_t) noexcept;
  JSValue(JSValueObject &&value) noexcept;
  JSValue(JSValueArray &&value) noexcept;
  JSValue(std::string &&value) noexcept;
  template <class TStringView, std::enable_if_t<std::is_convertible_v<TStringView, std::string_view>, int> = 1>
  JSValue(TStringView value) noexcept;
  template <class TBool, std::enable_if_t<std::is_same_v<TBool, bool>, int> = 1>
  JSValue(TBool value) noexcept;
  template <class TInt, std::enable_if_t<std::is_integral_v<TInt> && !std::is_same_v<TInt, bool>, int> = 1>
  JSValue(TInt value) noexcept;
  JSValue(double value) noexcept;

  JSValue(const JSValue &other) = delete;
  JSValue &operator=(const JSValue &other) = delete;

  JSValue(JSValue &&other) noexcept;
  JSValue &operator=(JSValue &&other) noexcept;

  ~JSValue() noexcept;

  JSValue Copy() const noexcept;
  static JSValueObject CopyObject(const JSValueObject &other) noexcept;
  static JSValueArray CopyArray(const JSValueArray &other) noexcept;

  JSValueType Type() const noexcept;
  bool IsNull() const noexcept;
  const JSValueObject &Object() const noexcept;
  const JSValueArray &Array() const noexcept;
  const std::string &String() const noexcept;
  bool Boolean() const noexcept;
  int64_t Int64() const noexcept;
  double Double() const noexcept;

  JSValueObject TakeObject() noexcept;
  JSValueArray TakeArray() noexcept;

  size_t PropertyCount() const noexcept;
  size_t ItemCount() const noexcept;

  template <typename T>
  T To() const noexcept;
  template <class T>
  static JSValue From(const T &value) noexcept;

  const JSValue &GetObjectProperty(std::string_view propertyName) const noexcept;
  const JSValue &GetArrayItem(JSValueArray::size_type index) const noexcept;
  const JSValue &operator[](std::string_view propertyName) const noexcept;
  const JSValue &operator[](JSValueArray::size_type index) const noexcept;

  bool Equals(const JSValue &other) const noexcept;

  static JSValue ReadFrom(IJSValueReader const &reader) noexcept;
  static JSValueObject ReadObjectFrom(IJSValueReader const &reader) noexcept;
  static JSValueArray ReadArrayFrom(IJSValueReader const &reader) noexcept;

  void WriteTo(IJSValueWriter const &writer) const noexcept;
  static void WriteObjectTo(IJSValueWriter const &writer, JSValueObjectView object) noexcept;
  static void WriteArrayTo(IJSValueWriter const &writer, JSValueArrayView value) noexcept;

 private:
  bool ObjectEquals(const JSValueObject &other) const noexcept;
  bool ArrayEquals(const JSValueArray &other) const noexcept;
  static JSValueObject ReadObjectProperties(IJSValueReader const &reader) noexcept;
  static JSValueArray ReadArrayItems(IJSValueReader const &reader) noexcept;

 private: // Instance fields
  JSValueType m_type;
  union {
    JSValueObject m_object;
    JSValueArray m_array;
    std::string m_string;
    bool m_bool;
    int64_t m_int64;
    double m_double;
  };
};

bool operator==(const JSValue &left, const JSValue &right) noexcept;
bool operator!=(const JSValue &left, const JSValue &right) noexcept;
void swap(JSValue &left, JSValue &right) noexcept;

//===========================================================================
// JSValueObjectView inline implementation
//===========================================================================

inline JSValueObjectView::JSValueObjectView(const JSValueObject &object) noexcept : m_object{object} {}

inline auto JSValueObjectView::begin() const noexcept {
  return m_object.begin();
}

inline auto JSValueObjectView::end() const noexcept {
  return m_object.end();
}

//===========================================================================
// JSValueArrayView inline implementation
//===========================================================================

inline JSValueArrayView::JSValueArrayView(const JSValueArray &array) noexcept : m_array{array} {}

inline auto JSValueArrayView::begin() const noexcept {
  return m_array.begin();
}

inline auto JSValueArrayView::end() const noexcept {
  return m_array.end();
}

//===========================================================================
// JSValue inline implementation
//===========================================================================

inline JSValue::JSValue() noexcept : m_type{JSValueType::Null}, m_int64{0} {}
inline JSValue::JSValue(std::nullptr_t) noexcept : m_type{JSValueType::Null}, m_int64{0} {}
inline JSValue::JSValue(JSValueObject &&value) noexcept : m_type{JSValueType::Object}, m_object{std::move(value)} {}
inline JSValue::JSValue(JSValueArray &&value) noexcept : m_type{JSValueType::Array}, m_array(std::move(value)) {}
inline JSValue::JSValue(std::string &&value) noexcept : m_type{JSValueType::String}, m_string{std::move(value)} {}
template <class TStringView, std::enable_if_t<std::is_convertible_v<TStringView, std::string_view>, int>>
inline JSValue::JSValue(TStringView value) noexcept : m_type{JSValueType::String}, m_string{value} {}
template <class TBool, std::enable_if_t<std::is_same_v<TBool, bool>, int>>
inline JSValue::JSValue(TBool value) noexcept : m_type{JSValueType::Boolean}, m_bool{value} {}
template <class TInt, std::enable_if_t<std::is_integral_v<TInt> && !std::is_same_v<TInt, bool>, int>>
inline JSValue::JSValue(TInt value) noexcept : m_type{JSValueType::Int64}, m_int64{static_cast<int64_t>(value)} {}
inline JSValue::JSValue(double value) noexcept : m_type{JSValueType::Double}, m_double{value} {}

inline JSValueType JSValue::Type() const noexcept {
  return m_type;
}

inline bool JSValue::IsNull() const noexcept {
  return m_type == JSValueType::Null;
}

inline const JSValueObject &JSValue::Object() const noexcept {
  return (m_type == JSValueType::Object) ? m_object : EmptyObject;
}

inline const JSValueArray &JSValue::Array() const noexcept {
  return (m_type == JSValueType::Array) ? m_array : EmptyArray;
}

inline const std::string &JSValue::String() const noexcept {
  return (m_type == JSValueType::String) ? m_string : EmptyString;
}

inline bool JSValue::Boolean() const noexcept {
  return (m_type == JSValueType::Boolean) ? m_bool : false;
}

inline int64_t JSValue::Int64() const noexcept {
  return (m_type == JSValueType::Int64) ? m_int64 : 0;
}

inline double JSValue::Double() const noexcept {
  return (m_type == JSValueType::Double) ? m_double : 0;
}

template <typename T>
inline T JSValue::To() const noexcept {
  T result;
  ReadValue(MakeJSValueTreeReader(*this), /*out*/ result);
  return result;
}

template <class T>
static JSValue From(const T &value) noexcept {
  JSValue result;
  auto writer = MakeJSValueTreeWriter(result);
  WriteValue(writer, value);
  return result;
}

inline const JSValue &JSValue::operator[](std::string_view propertyName) const noexcept {
  return GetObjectProperty(propertyName);
}

inline const JSValue &JSValue::operator[](JSValueArray::size_type index) const noexcept {
  return GetArrayItem(index);
}

//===========================================================================
// Standalone inline functions implementation
//===========================================================================

inline bool operator==(const JSValue &left, const JSValue &right) noexcept {
  return left.Equals(right);
}

inline bool operator!=(const JSValue &left, const JSValue &right) noexcept {
  return !left.Equals(right);
}

} // namespace winrt::Microsoft::ReactNative::Bridge

#endif // MICROSOFT_REACTNATIVE_JSVALUE
