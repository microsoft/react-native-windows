// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once
#ifndef MICROSOFT_REACTNATIVE_JSVALUE
#define MICROSOFT_REACTNATIVE_JSVALUE

#include "Crash.h"
#include "winrt/Microsoft.ReactNative.h"

namespace winrt::Microsoft::ReactNative {

// Forward declarations
struct JSValue;
struct JSValueObjectKeyValue;
struct JSValueArrayItem;
IJSValueReader MakeJSValueTreeReader(const JSValue &root) noexcept;
IJSValueReader MakeJSValueTreeReader(JSValue &&root) noexcept;
IJSValueWriter MakeJSValueTreeWriter() noexcept;
JSValue TakeJSValue(IJSValueWriter const &writer) noexcept;

// JSValueObject is based on std::map and has a custom constructor with std::intializer_list.
// It is possible to write: JSValueObject{{"X", 4}, {"Y", 5}} and pass it as JSValue.
struct JSValueObject : std::map<std::string, JSValue, std::less<>> {
#pragma region Constructors

  //! Default constructor.
  JSValueObject() = default;

  //! Construct JSValueObject from the move iterator.
  template <class TMoveInputIterator>
  JSValueObject(TMoveInputIterator first, TMoveInputIterator last) noexcept;

  //! Move-construct JSValueObject from the initializer list.
  JSValueObject(std::initializer_list<JSValueObjectKeyValue> initObject) noexcept;

  //! Move-construct JSValueObject from the string-JSValue map.
  JSValueObject(std::map<std::string, JSValue, std::less<>> &&other) noexcept;

#pragma endregion

#pragma region Copy semantic

  //! Delete copy constructor to avoid unexpected copies. Use the Copy method instead.
  JSValueObject(JSValueObject const &) = delete;

  //! Delete copy assignment to avoid unexpected copies. Use the Copy method instead.
  JSValueObject &operator=(JSValueObject const &) = delete;

  //! Do a deep copy of JSValueObject.
  JSValueObject Copy() const noexcept;

#pragma endregion

#pragma region Move semantic

  // Default move constructor.
  JSValueObject(JSValueObject &&) = default;

  // Default move assignment.
  JSValueObject &operator=(JSValueObject &&) = default;

#pragma endregion

#pragma region Change JSValueObject
  //! Get a reference to object property value if the property is found,
  //! or a reference to a new property created with JSValue::Null value otherwise.
  JSValue &operator[](std::string_view propertyName) noexcept;

#pragma endregion

#pragma region Inspect JSValueObject

  //! Get a reference to object property value if the property is found,
  //! or a reference to JSValue::Null otherwise.
  JSValue const &operator[](std::string_view propertyName) const noexcept;

  //! Return true if this JSValueObject is strictly equal to other JSValueObject.
  //! Both objects must have the same set of equal properties.
  //! Property values must have the same type and value.
  bool Equals(JSValueObject const &other) const noexcept;

  //! Return true if this JSValueObject is strictly equal to other JSValueObject
  //! after their property values are converted to the same type.
  //! See JSValue::EqualsAfterConversion for details about the conversion.
  bool EqualsAfterConversion(const JSValueObject &other) const noexcept;

#pragma endregion

#pragma region JSValueObject serialization and deserialization.

  //! Create JSValueObject from IJSValueReader.
  static JSValueObject ReadFrom(IJSValueReader const &reader) noexcept;

  //! Write this JSValueObject to IJSValueWriter.
  void WriteTo(IJSValueWriter const &writer) const noexcept;

#pragma endregion
};

#pragma region Standalone JSValueObject functions

bool operator==(const JSValueObject &left, const JSValueObject &right) noexcept;
bool operator!=(const JSValueObject &left, const JSValueObject &right) noexcept;

#pragma endregion

//! JSValueArray is based on std::vector<JSValue> and has a custom constructor with std::intializer_list.
//! It is possible to write: JSValueArray{"X", 4, true} and pass it as JSValue.
struct JSValueArray : std::vector<JSValue> {
#pragma region Constructors

  //! Default constructor.
  JSValueArray() = default;

  //! Constructs JSValueArray with size JSValue::Null elements.
  explicit JSValueArray(size_type size) noexcept;

  //! Constructs JSValueArray with size elements.
  //! Each element is a copy of defaultValue.
  JSValueArray(size_type size, JSValue const &defaultValue) noexcept;

  //! Construct JSValueArray from the move iterator.
  template <class TMoveInputIterator>
  JSValueArray(TMoveInputIterator first, TMoveInputIterator last) noexcept;

  //! Move-construct JSValueArray from the initializer list.
  JSValueArray(std::initializer_list<JSValueArrayItem> initObject) noexcept;

  //! Move-construct JSValueArray from the JSValue vector.
  JSValueArray(std::vector<JSValue> &&other) noexcept;

#pragma endregion

#pragma region Copy semantic

  //! Delete copy constructor to avoid unexpected copies. Use the Copy method instead.
  JSValueArray(JSValueArray const &) = delete;

  //! Delete copy assignment to avoid unexpected copies. Use the Copy method instead.
  JSValueArray &operator=(JSValueArray const &) = delete;

  //! Do a deep copy of JSValueArray.
  JSValueArray Copy() const noexcept;

#pragma endregion

#pragma region Move semantic

  // Default move constructor.
  JSValueArray(JSValueArray &&) = default;

  // Default move assignment.
  JSValueArray &operator=(JSValueArray &&) = default;

#pragma endregion

#pragma region Inspect JSValueArray

  //! Return true if this JSValueArray is strictly equal to other JSValueArray.
  //! Both arrays must have the same set of items.
  //! Items must have the same type and value.
  bool Equals(JSValueArray const &other) const noexcept;

  //! Return true if this JSValueArray is strictly equal to other JSValueArray
  //! after their items are converted to the same type.
  //! See JSValue::EqualsAfterConversion for details about the conversion.
  bool EqualsAfterConversion(const JSValueArray &other) const noexcept;

#pragma endregion

#pragma region JSValueArray serialization and deserialization.

  //! Create JSValueArray from IJSValueReader.
  static JSValueArray ReadFrom(IJSValueReader const &reader) noexcept;

  //! Write this JSValueArray to IJSValueWriter.
  void WriteTo(IJSValueWriter const &writer) const noexcept;

#pragma endregion
};

#pragma region Standalone JSValueArray functions

bool operator==(const JSValueArray &left, const JSValueArray &right) noexcept;
bool operator!=(const JSValueArray &left, const JSValueArray &right) noexcept;

#pragma endregion

//! JSValue represents an immutable JavaScript value that can be passed as a parameter.
//! It is created to simplify working with IJSValueReader in some complex cases.
//! It takes more resources than direct use of IJSValueReader, but provides more flexibility.
//! The JSValue is an immutable and is safe to be used from multiple threads.
//! It is move-only to avoid unnecessary or unexpected copying of values.
//! For copy operations the explicit Copy() method must be used.
//! Note that the move operations are not thread safe.
struct JSValue {
#pragma region Predefined constants

  static const JSValue Null;
  static const JSValueObject EmptyObject;
  static const JSValueArray EmptyArray;
  static const std::string EmptyString;

#pragma endregion

#pragma region Constructors

  //! Create a Null JSValue.
  JSValue() noexcept;

  //! Create a Null JSValue.
  JSValue(std::nullptr_t) noexcept;

  //! Create an Object JSValue.
  JSValue(JSValueObject &&value) noexcept;

  //! Create an Array JSValue.
  JSValue(JSValueArray &&value) noexcept;

  //! Create a String JSValue.
  JSValue(std::string &&value) noexcept;

  //! Create a String JSValue from any type that can be converted to std::string_view.
  template <class TStringView, std::enable_if_t<std::is_convertible_v<TStringView, std::string_view>, int> = 1>
  JSValue(TStringView value) noexcept;

  //! Create a Boolean JSValue.
  template <class TBool, std::enable_if_t<std::is_same_v<TBool, bool>, int> = 1>
  JSValue(TBool value) noexcept;

  //! Create an Int64 JSValue from any integral type except bool.
  template <class TInt, std::enable_if_t<std::is_integral_v<TInt> && !std::is_same_v<TInt, bool>, int> = 1>
  JSValue(TInt value) noexcept;

  //! Create a Double JSValue.
  JSValue(double value) noexcept;

  //! Create JSValue from a type that has WriteValue method defined to write to IJSValueWriter.
  template <class T>
  static JSValue From(T const &value) noexcept;

#pragma endregion

#pragma region Destructor

  ~JSValue() noexcept;

#pragma endregion

#pragma region JSValue copy semantic

  //! Delete the copy constructor to avoid unexpected copies. Use the Copy method instead.
  JSValue(const JSValue &other) = delete;

  //! Delete the copy assignment to avoid unexpected copies. Use the Copy method instead.
  JSValue &operator=(const JSValue &other) = delete;

  //! Do a deep copy of JSValue.
  JSValue Copy() const noexcept;

#pragma endregion

#pragma region JSValue move semantic

  //! Move constructor. The 'other' JSValue becomes JSValue::Null.
  JSValue(JSValue &&other) noexcept;

  //! Move assignment. The 'other' JSValue becomes JSValue::Null.
  JSValue &operator=(JSValue &&other) noexcept;

  //! Move out Object and set this to JSValue::Null. It returns JSValue::EmptyObject
  //! and keeps this JSValue unchanged if current type is not an object.
  JSValueObject MoveObject() noexcept;

  //! Move out Array and set this to JSValue::Null. It returns JSValue::EmptyArray
  //! and keeps this JSValue unchanged if current type is not an array.
  JSValueArray MoveArray() noexcept;

#pragma endregion

#pragma region Inspect JSValue

  //! Get JSValue type.
  JSValueType Type() const noexcept;

  //! Return true if JSValue type is Null.
  bool IsNull() const noexcept;

  //! Return pointer to JSValueObject if JSValue type is Object, or nullptr otherwise.
  JSValueObject const *TryGetObject() const noexcept;

  //! Return pointer to JSValueArray if JSValue type is Array, or nullptr otherwise.
  JSValueArray const *TryGetArray() const noexcept;

  //! Return pointer to string if JSValue type is String, or nullptr otherwise.
  std::string const *TryGetString() const noexcept;

  //! Return pointer to bool value if JSValue type is Boolean, or nullptr otherwise.
  bool const *TryGetBoolean() const noexcept;

  //! Return pointer to int64_t value if JSValue type is Int64, or nullptr otherwise.
  int64_t const *TryGetInt64() const noexcept;

  //! Return pointer to double value if JSValue type is Double, or nullptr otherwise.
  double const *TryGetDouble() const noexcept;

  //! Return true if this JSValue is strictly equal to JSValue.
  //! Compared values must have the same type and value.
  //!
  //! The behavior is similar to JavaScript === operator except for Object and Array for which
  //! this functions does a deep structured comparison instead of pointer equality.
  bool Equals(const JSValue &other) const noexcept;

  //! Return true if this JSValue is strictly equal to JSValue after they are converted to the same type.
  //!
  //! Null is not converted to any other type before comparison.
  //! Object and Array types are converted first to a String type using AsString() before comparing
  //! with other types, and then we apply the same rules as for the String type.
  //! String is converted to Double before comparing with Boolean, Int64, or Double.
  //! Boolean is converted to 1.0 and +0.0 when comparing with String or Double.
  //! Boolean is converted to 1 and 0 when comparing with Int64.
  //! Int64 is converted to Double when comparing with Double.
  //!
  //! The behavior is similar to JavaScript == operator except for Object and Array for which
  //! this functions does a deep structured comparison using EqualsAfterConversion instead
  //! of pointer equality.
  bool EqualsAfterConversion(const JSValue &other) const noexcept;

#pragma endregion

#pragma region Inspect JSValueObject

  //! Return property count if JSValue is Object, or 0 otherwise.
  size_t PropertyCount() const noexcept;

  //! Get a reference to object property value if JSValue type is Object and the property is found,
  //! or a reference to JSValue::Null otherwise.
  JSValue const &GetObjectProperty(std::string_view propertyName) const noexcept;

  //! Get a reference to object property value if JSValue type is Object and the property is found,
  //! or a reference to JSValue::Null otherwise.
  JSValue const &operator[](std::string_view propertyName) const noexcept;

#pragma endregion

#pragma region Inspect JSValueArray

  //! Return item count if JSValue is Array, or 0 otherwise.
  size_t ItemCount() const noexcept;

  //! Get a reference to array item if JSValue type is Array and the index is in bounds,
  //! or a reference to JSValue::Null otherwise.
  JSValue const &GetArrayItem(JSValueArray::size_type index) const noexcept;

  //! Get a reference to array item if JSValue type is Array and the index is in bounds,
  //! or a reference to JSValue::Null otherwise.
  JSValue const &operator[](JSValueArray::size_type index) const noexcept;

#pragma endregion

#pragma region Convert JSValue to other type

  //! Return Object representation of JSValue. It is JSValue::EmptyObject if type is not Object.
  JSValueObject const &AsObject() const noexcept;

  //! Return Array representation of JSValue. It is JSValue::EmptyArray if type is not Object.
  JSValueArray const &AsArray() const noexcept;

  //! Return a string representation of JSValue. It is the same as JavaScript String(value) result.
  std::string AsString() const noexcept;

  //! Return a Boolean representation of JSValue. It is the same as JavaScript Boolean(value) result.
  bool AsBoolean() const noexcept;

  //! Return an Int8 representation of JSValue. It is the same as JavaScript Number(value) result casted to int8_t.
  int8_t AsInt8() const noexcept;

  //! Return an Int16 representation of JSValue. It is the same as JavaScript Number(value) result casted to int16_t.
  int16_t AsInt16() const noexcept;

  //! Return an Int32 representation of JSValue. It is the same as JavaScript Number(value) result casted to int32_t.
  int32_t AsInt32() const noexcept;

  //! Return an Int64 representation of JSValue. It is the same as JavaScript Number(value) result casted to int64_t.
  int64_t AsInt64() const noexcept;

  //! Return an UInt8 representation of JSValue. It is the same as JavaScript Number(value) result casted to uint8_t.
  uint8_t AsUInt8() const noexcept;

  //! Return an UInt16 representation of JSValue. It is the same as JavaScript Number(value) result casted to uint16_t.
  uint16_t AsUInt16() const noexcept;

  //! Return an UInt32 representation of JSValue. It is the same as JavaScript Number(value) result casted to uint32_t.
  uint32_t AsUInt32() const noexcept;

  //! Return an UInt64 representation of JSValue. It is the same as JavaScript Number(value) result casted to uint64_t.
  uint64_t AsUInt64() const noexcept;

  //! Return a Double representation of JSValue. It is the same as JavaScript Number(value) result.
  double AsDouble() const noexcept;

  //! Return an Float representation of JSValue. It is the same as JavaScript Number(value) result casted to float.
  float AsFloat() const noexcept;

  //! Return value T that is created from JSValue using the ReadValue function override.
  //! Default T is constructed by using default constructor.
  template <
      class T,
      std::enable_if_t<std::is_default_constructible_v<T> && !std::is_constructible_v<T, std::nullptr_t>, int> = 0>
  T As() const noexcept;

  //! Return value T that is created from JSValue using the ReadValue function override.
  //! Default T is constructed by using constructor that receives nullptr as a parameter.
  template <class T, std::enable_if_t<std::is_constructible_v<T, std::nullptr_t>, int> = 0>
  T As() const noexcept;

  //! Return value T that is created from JSValue using the ReadValue function override.
  //! Default T is constructed from the provided 'default' value.
  template <class T>
  T As(T &&defaultValue) const noexcept;

  //! Convert JSValue to a readable string that can be used for logging.
  std::string ToString() const noexcept;

#pragma endregion

#pragma region JSValue serialization and deserialization.

  //! Create JSValue from IJSValueReader.
  static JSValue ReadFrom(IJSValueReader const &reader) noexcept;

  //! Write this JSValue to IJSValueWriter.
  void WriteTo(IJSValueWriter const &writer) const noexcept;

#pragma endregion

#pragma region Deprecated methods

  // The methods below are deprecated in favor of other methods with clearer semantic
  [[deprecated("Use TryGetObject or AsObject")]] JSValueObject const &Object() const noexcept;
  [[deprecated("Use TryGetArray or As Array")]] JSValueArray const &Array() const noexcept;
  [[deprecated("Use TryGetString or AsString")]] std::string const &String() const noexcept;
  [[deprecated("Use TryGetBoolean or AsBoolean")]] bool Boolean() const noexcept;
  [[deprecated("Use TryGetInt64 or AsInt64")]] int64_t Int64() const noexcept;
  [[deprecated("Use TryGetDouble or AsDouble")]] double Double() const noexcept;

  // We have renamed or moved the methods below.
  template <class T>
  [[deprecated("Use As<T>")]] T To() const noexcept;
  [[deprecated("Use MoveObject")]] JSValueObject TakeObject() noexcept;
  [[deprecated("Use MoveArray")]] JSValueArray TakeArray() noexcept;
  [[deprecated("Use JSValueObject::Copy")]] static JSValueObject CopyObject(const JSValueObject &other) noexcept;
  [[deprecated("Use JSValueArray::Copy")]] static JSValueArray CopyArray(const JSValueArray &other) noexcept;
  [[deprecated("Use JSValueObject::ReadFrom")]] static JSValueObject ReadObjectFrom(
      IJSValueReader const &reader) noexcept;
  [[deprecated("Use JSValueArray::ReadFrom")]] static JSValueArray ReadArrayFrom(IJSValueReader const &reader) noexcept;
  [[deprecated("Use JSValueObject::WriteTo")]] static void WriteObjectTo(
      IJSValueWriter const &writer,
      JSValueObject const &value) noexcept;
  [[deprecated("Use JSValueArray::WriteTo")]] static void WriteArrayTo(
      IJSValueWriter const &writer,
      JSValueArray const &value) noexcept;

#pragma endregion

#pragma region Private fields

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

#pragma endregion
};

#pragma region Standalone JSValue functions

bool operator==(const JSValue &left, const JSValue &right) noexcept;
bool operator!=(const JSValue &left, const JSValue &right) noexcept;

#pragma endregion

//! Helps initialize key-value pairs for JSValueObject.
//! It creates its own instance of JSValue which then can be moved to JSValueObject.
struct JSValueObjectKeyValue {
  template <class TKey, class TValue>
  JSValueObjectKeyValue(TKey &&key, TValue &&value) noexcept
      : Key(std::forward<TKey>(key)), Value(std::forward<TValue>(value)) {}

  std::string_view Key;
  JSValue Value;
};

//! Helps initialize items for JSValueArray.
//! It creates its own instance of JSValue which then can be moved to JSValueArray.
struct JSValueArrayItem {
  template <class TItem>
  JSValueArrayItem(TItem &&item) noexcept : Item(std::forward<TItem>(item)) {}

  JSValue Item;
};

//===========================================================================
// Inline JSValueObject implementation.
//===========================================================================

template <class TMoveInputIterator>
JSValueObject::JSValueObject(TMoveInputIterator first, TMoveInputIterator last) noexcept {
  auto it = first;
  while (it != last) {
    auto pair = *it++;
    try_emplace(std::move(pair.first), std::move(pair.second));
  }
}

//===========================================================================
// Inline JSValueObject standalone function implementations.
//===========================================================================

inline bool operator==(const JSValueObject &left, const JSValueObject &right) noexcept {
  return left.Equals(right);
}

inline bool operator!=(const JSValueObject &left, const JSValueObject &right) noexcept {
  return !left.Equals(right);
}

//===========================================================================
// Inline JSValueArray implementation.
//===========================================================================

template <class TMoveInputIterator>
JSValueArray::JSValueArray(TMoveInputIterator first, TMoveInputIterator last) noexcept {
  auto it = first;
  while (it != last) {
    push_back(*it++);
  }
}

//===========================================================================
// Inline JSValueArray standalone function implementations.
//===========================================================================

inline bool operator==(const JSValueArray &left, const JSValueArray &right) noexcept {
  return left.Equals(right);
}

inline bool operator!=(const JSValueArray &left, const JSValueArray &right) noexcept {
  return !left.Equals(right);
}

//===========================================================================
// Inline JSValue implementation.
//===========================================================================

#pragma warning(push)
#pragma warning(disable : 26495) // False positive for union member not initialized
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
#pragma warning(pop)

template <class T>
static JSValue From(T const &value) noexcept {
  auto writer = MakeJSValueTreeWriter();
  WriteValue(writer, value);
  return TakeJSValue(writer);
}

inline JSValueType JSValue::Type() const noexcept {
  return m_type;
}

inline bool JSValue::IsNull() const noexcept {
  return m_type == JSValueType::Null;
}

inline JSValueObject const *JSValue::TryGetObject() const noexcept {
  return (m_type == JSValueType::Object) ? &m_object : nullptr;
}

inline JSValueArray const *JSValue::TryGetArray() const noexcept {
  return (m_type == JSValueType::Array) ? &m_array : nullptr;
}

inline std::string const *JSValue::TryGetString() const noexcept {
  return (m_type == JSValueType::String) ? &m_string : nullptr;
}

inline bool const *JSValue::TryGetBoolean() const noexcept {
  return (m_type == JSValueType::Boolean) ? &m_bool : nullptr;
}

inline int64_t const *JSValue::TryGetInt64() const noexcept {
  return (m_type == JSValueType::Int64) ? &m_int64 : nullptr;
}

inline double const *JSValue::TryGetDouble() const noexcept {
  return (m_type == JSValueType::Double) ? &m_double : nullptr;
}

inline JSValueObject const &JSValue::AsObject() const noexcept {
  return (m_type == JSValueType::Object) ? m_object : EmptyObject;
}

inline JSValueArray const &JSValue::AsArray() const noexcept {
  return (m_type == JSValueType::Array) ? m_array : EmptyArray;
}

template <>
inline std::string JSValue::As() const noexcept {
  return AsString();
}

template <>
inline bool JSValue::As() const noexcept {
  return AsBoolean();
}

template <>
inline int8_t JSValue::As() const noexcept {
  return AsInt8();
}

template <>
inline int16_t JSValue::As() const noexcept {
  return AsInt16();
}

template <>
inline int32_t JSValue::As() const noexcept {
  return AsInt32();
}

template <>
inline int64_t JSValue::As() const noexcept {
  return AsInt64();
}

template <
    class T,
    std::enable_if_t<std::is_default_constructible_v<T> && !std::is_constructible_v<T, std::nullptr_t>, int>>
inline T JSValue::As() const noexcept {
  T result;
  ReadValue(MakeJSValueTreeReader(*this), /*out*/ result);
  return result;
}

template <class T, std::enable_if_t<std::is_constructible_v<T, std::nullptr_t>, int>>
inline T JSValue::As() const noexcept {
  T result{nullptr};
  ReadValue(MakeJSValueTreeReader(*this), /*out*/ result);
  return result;
}

inline const JSValue &JSValue::operator[](std::string_view propertyName) const noexcept {
  return GetObjectProperty(propertyName);
}

inline const JSValue &JSValue::operator[](JSValueArray::size_type index) const noexcept {
  return GetArrayItem(index);
}

//===========================================================================
// JSValue deprecated methods.
//===========================================================================

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

template <class T>
inline T JSValue::To() const noexcept {
  return As<T>();
}

inline JSValueObject JSValue::TakeObject() noexcept {
  return MoveObject();
}

inline JSValueArray JSValue::TakeArray() noexcept {
  return MoveArray();
}

inline /*static*/ JSValueObject JSValue::CopyObject(const JSValueObject &other) noexcept {
  return other.Copy();
}

inline /*static*/ JSValueArray JSValue::CopyArray(const JSValueArray &other) noexcept {
  return other.Copy();
}

inline /*static*/ JSValueObject JSValue::ReadObjectFrom(IJSValueReader const &reader) noexcept {
  return JSValueObject::ReadFrom(reader);
}

inline /*static*/ JSValueArray JSValue::ReadArrayFrom(IJSValueReader const &reader) noexcept {
  return JSValueArray::ReadFrom(reader);
}

inline /*static*/ void JSValue::WriteObjectTo(IJSValueWriter const &writer, JSValueObject const &value) noexcept {
  value.WriteTo(writer);
}
inline /*static*/ void JSValue::WriteArrayTo(IJSValueWriter const &writer, JSValueArray const &value) noexcept {
  value.WriteTo(writer);
}

//===========================================================================
// Inline JSValue standalone function implementations.
//===========================================================================

inline bool operator==(const JSValue &left, const JSValue &right) noexcept {
  return left.Equals(right);
}

inline bool operator!=(const JSValue &left, const JSValue &right) noexcept {
  return !left.Equals(right);
}

} // namespace winrt::Microsoft::ReactNative

#endif // MICROSOFT_REACTNATIVE_JSVALUE
