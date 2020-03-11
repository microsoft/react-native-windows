// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include "JSValue.h"
#include <iomanip>
#include <sstream>
#include <string_view>

namespace winrt::Microsoft::ReactNative {

//===========================================================================
// JSValue type conversion helpers.
//===========================================================================

namespace {

struct NullConverter {
  static constexpr char const *NullString = "null";
};

struct ObjectConverter {
  static constexpr char const *JavaScriptString = "[object Object]";
};

struct StringConverter {
  static std::string_view TrimString(std::string const &value) noexcept {
    constexpr char const *WhiteSpace = " \n\r\t\f\v";

    size_t start = value.find_first_not_of(WhiteSpace);
    if (start == std::string::npos) {
      return "";
    }

    size_t end = value.find_last_not_of(WhiteSpace);
    return {value.data() + start, end - start + 1};
  }

  static std::ostream &WriteAsJsonString(std::ostream &stream, std::string const &value) noexcept {
    auto writeChar = [](std::ostream & stream, char ch) noexcept->std::ostream & {
      switch (ch) {
        case '"':
          return stream << "\\\"";
        case '\\':
          return stream << "\\\\";
        case '\b':
          return stream << "\\b";
        case '\f':
          return stream << "\\f";
        case '\n':
          return stream << "\\n";
        case '\r':
          return stream << "\\r";
        case '\t':
          return stream << "\\t";
        default:
          if ('\x00' <= ch && ch <= '\x1f') { // Non-printable ASCII characters.
            return stream << "\\u" << std::hex << std::setw(4) << std::setfill('0') << (int)ch;
          } else {
            return stream << ch;
          }
      }
    };

    stream << '"';
    for (auto ch : value) {
      writeChar(stream, ch);
    }

    return stream << '"';
  }
};

struct BooleanConverter {
  static char const *ToString(bool value) noexcept {
    return value ? "true" : "false";
  }

  static bool FromString(std::string const &value) noexcept {
    return !value.empty();
  }

  static double ToDouble(bool value) noexcept {
    return value ? 1.0 : 0.0;
  }

  static int64_t ToInt64(bool value) noexcept {
    return value ? 1 : 0;
  }
};

struct Int64Converter {
  static std::string ToString(int64_t value) noexcept {
    return std::to_string(value);
  }

  static std::ostream &WriteAsString(std::ostream &stream, int64_t value) noexcept {
    return stream << value;
  }

  static int64_t FromDouble(double value) noexcept {
    if (std::isfinite(value)) {
      if (value > std::numeric_limits<int64_t>::max()) {
        return std::numeric_limits<int64_t>::max();
      } else if (value < std::numeric_limits<int64_t>::min()) {
        return std::numeric_limits<int64_t>::min();
      } else {
        return static_cast<int64_t>(value);
      }
    } else if (std::isnan(value)) {
      return 0;
    } else if (value == std::numeric_limits<double>::infinity()) {
      return std::numeric_limits<int64_t>::max();
    } else if (value == -std::numeric_limits<double>::infinity()) {
      return std::numeric_limits<int64_t>::min();
    } else {
      return 0;
    }
  }

  // Cast Int64 to a smaller signed type.
  // In case if value is bigger or smaller than the target type allows, use the type max or min values.
  template <
      class T,
      std::enable_if_t<std::is_same_v<T, int8_t> || std::is_same_v<T, int16_t> || std::is_same_v<T, int32_t>, int> = 1>
  static T To(int64_t value) noexcept {
    if (value > std::numeric_limits<T>::max()) {
      return std::numeric_limits<T>::max();
    } else if (value < std::numeric_limits<T>::min()) {
      return std::numeric_limits<T>::min();
    } else {
      return static_cast<T>(value);
    }
  }
};

struct UInt64Converter {
  // Cast UInt64 to a smaller unsigned type.
  // In case if value is bigger than the target type allows, use the type max value.
  template <
      class T,
      std::enable_if_t<std::is_same_v<T, uint8_t> || std::is_same_v<T, uint16_t> || std::is_same_v<T, uint32_t>, int> =
          1>
  static T To(uint64_t value) noexcept {
    if (value > std::numeric_limits<T>::max()) {
      return std::numeric_limits<T>::max();
    } else {
      return static_cast<T>(value);
    }
  }
};

struct DoubleConverter {
  static char const *NaNToString(double value) noexcept {
    if (std::isnan(value)) {
      return "NaN";
    } else if (value == std::numeric_limits<double>::infinity()) {
      return "Infinity";
    } else if (value == -std::numeric_limits<double>::infinity()) {
      return "-Infinity";
    } else {
      return nullptr;
    }
  }

  static std::string ToString(double value) noexcept {
    if (std::isfinite(value)) {
      return std::to_string(value);
    } else {
      return NaNToString(value);
    }
  }

  static std::ostream &WriteAsString(std::ostream &stream, double value) noexcept {
    if (std::isfinite(value)) {
      return stream << value;
    } else {
      return stream << NaNToString(value);
    }
  }

  static double FromString(std::string const &value) noexcept {
    auto trimmedStr = StringConverter::TrimString(value);
    if (trimmedStr.empty()) {
      return 0;
    }

    char *end;
    double result = strtod(trimmedStr.data(), &end);
    return (end == trimmedStr.data() + trimmedStr.size()) ? result : std::numeric_limits<double>::quiet_NaN();
  }
};

struct JSValueAsStringWriter {
  static constexpr char const *IndentString = "  ";

  JSValueAsStringWriter(std::ostream &stream) noexcept : m_stream{stream} {}

  static std::string ToString(JSValue const &value) noexcept {
    std::stringstream stream;
    JSValueAsStringWriter writer{stream};
    writer.WriteValue(value);
    return stream.str();
  }

  static std::string ToString(JSValueArray const &value) noexcept {
    std::stringstream stream;
    JSValueAsStringWriter writer{stream};
    writer.WriteArray(value);
    return stream.str();
  }

  std::ostream &WriteValue(JSValue const &value) noexcept {
    if (value.IsNull()) {
      return m_stream << NullConverter::NullString;
    } else if (value.TryGetObject()) {
      return m_stream << ObjectConverter::JavaScriptString;
    } else if (auto arrayPtr = value.TryGetArray()) {
      return WriteArray(*arrayPtr);
    } else if (auto stringPtr = value.TryGetString()) {
      return m_stream << *stringPtr;
    } else if (auto boolPtr = value.TryGetBoolean()) {
      return m_stream << BooleanConverter::ToString(*boolPtr);
    } else if (auto int64Ptr = value.TryGetInt64()) {
      return Int64Converter::WriteAsString(m_stream, *int64Ptr);
    } else if (auto doublePtr = value.TryGetDouble()) {
      return DoubleConverter::WriteAsString(m_stream, *doublePtr);
    } else {
      VerifyElseCrashSz(false, "Unexpected JSValue type");
    }
  }

  std::ostream &WriteArray(JSValueArray const &value) noexcept {
    bool start = true;
    for (auto const &item : value) {
      m_stream << (start ? (start = false, "") : ",");
      WriteValue(item);
    }

    return m_stream;
  }

 private:
  std::ostream &m_stream;
};

struct JSValueLogWriter {
  static constexpr char const *IndentString = "  ";

  JSValueLogWriter(std::ostream &stream) noexcept : m_stream{stream} {}

  static std::string ToString(JSValue const &value) noexcept {
    std::stringstream stream;
    JSValueLogWriter writer{stream};
    writer.WriteValue(value);
    return stream.str();
  }

  std::ostream &WriteIndent() noexcept {
    for (size_t i = 0; i < m_indent; ++i) {
      m_stream << IndentString;
    }

    return m_stream;
  }

  std::ostream &WriteValue(JSValue const &value) noexcept {
    if (value.IsNull()) {
      return m_stream << NullConverter::NullString;
    } else if (auto objectPtr = value.TryGetObject()) {
      return WriteObject(*objectPtr);
    } else if (auto arrayPtr = value.TryGetArray()) {
      return WriteArray(*arrayPtr);
    } else if (auto stringPtr = value.TryGetString()) {
      return StringConverter::WriteAsJsonString(m_stream, *stringPtr);
    } else if (auto boolPtr = value.TryGetBoolean()) {
      return m_stream << BooleanConverter::ToString(*boolPtr);
    } else if (auto int64Ptr = value.TryGetInt64()) {
      return Int64Converter::WriteAsString(m_stream, *int64Ptr);
    } else if (auto doublePtr = value.TryGetDouble()) {
      return DoubleConverter::WriteAsString(m_stream, *doublePtr);
    } else {
      VerifyElseCrashSz(false, "Unexpected JSValue type");
    }
  }

  std::ostream &WriteObject(JSValueObject const &value) noexcept {
    m_stream << "{";
    ++m_indent;
    bool start = true;
    for (auto const &prop : value) {
      m_stream << start ? (start = false, "") : ",\n";
      WriteIndent() << prop.first << ": ";
      WriteValue(prop.second);
    }

    --m_indent;
    if (!start) {
      m_stream << "\n";
      WriteIndent();
    }

    return m_stream << "}";
  }

  std::ostream &WriteArray(JSValueArray const &value) noexcept {
    m_stream << "[";
    ++m_indent;
    bool start = true;
    for (auto const &item : value) {
      m_stream << start ? (start = false, "") : ",\n";
      WriteValue(item);
    }

    --m_indent;
    if (!start) {
      m_stream << "\n";
      WriteIndent();
    }

    return m_stream << "]";
  }

 private:
  size_t m_indent{0};
  std::ostream &m_stream;
};

} // namespace

//===========================================================================
// JSValueObject implementation
//===========================================================================

JSValueObject::JSValueObject(std::initializer_list<JSValueObjectKeyValue> initObject) noexcept {
  for (auto const &item : initObject) {
    this->try_emplace(std::string(item.Key), std::move(*const_cast<JSValue *>(&item.Value)));
  }
}

JSValueObject::JSValueObject(std::map<std::string, JSValue, std::less<>> &&other) noexcept : map{std::move(other)} {}

JSValueObject JSValueObject::Copy() const noexcept {
  JSValueObject object;
  for (auto const &property : *this) {
    object.try_emplace(property.first, property.second.Copy());
  }

  return object;
}

JSValue &JSValueObject::operator[](std::string_view propertyName) noexcept {
  auto result = try_emplace(propertyName.data(), nullptr);
  return result.first->second;
}

JSValue const &JSValueObject::operator[](std::string_view propertyName) const noexcept {
  auto it = find(propertyName);
  if (it != end()) {
    return it->second;
  }

  return JSValue::Null;
}

bool JSValueObject::Equals(JSValueObject const &other) const noexcept {
  if (size() != other.size()) {
    return false;
  }

  // std::map keeps key-values in an ordered sequence.
  // Make sure that pairs are matching at the same position.
  auto otherIt = other.begin();
  for (auto const &property : *this) {
    auto it = otherIt++;
    if (property.first != it->first || !property.second.Equals(it->second)) {
      return false;
    }
  }

  return true;
}

bool JSValueObject::EqualsAfterConversion(JSValueObject const &other) const noexcept {
  if (size() != other.size()) {
    return false;
  }

  // std::map keeps key-values in an ordered sequence.
  // Make sure that pairs are matching at the same position.
  auto otherIt = other.begin();
  for (auto const &property : *this) {
    auto it = otherIt++;
    if (property.first != it->first || !property.second.EqualsAfterConversion(it->second)) {
      return false;
    }
  }

  return true;
}

/*static*/ JSValueObject JSValueObject::ReadFrom(IJSValueReader const &reader) noexcept {
  JSValueObject object;
  if (reader.ValueType() == JSValueType::Object) {
    hstring propertyName;
    while (reader.GetNextObjectProperty(/*ref*/ propertyName)) {
      object.try_emplace(to_string(propertyName), JSValue::ReadFrom(reader));
    }
  }

  return object;
}

void JSValueObject::WriteTo(IJSValueWriter const &writer) const noexcept {
  writer.WriteObjectBegin();
  for (auto const &property : *this) {
    writer.WritePropertyName(to_hstring(property.first));
    property.second.WriteTo(writer);
  }

  writer.WriteObjectEnd();
}

//===========================================================================
// JSValueArray implementation
//===========================================================================

JSValueArray::JSValueArray(size_type size) noexcept {
  reserve(size);
  for (size_type i = 0; i < size; ++i) {
    emplace_back(nullptr);
  }
}

JSValueArray::JSValueArray(size_type size, JSValue const &defaultValue) noexcept {
  reserve(size);
  for (size_type i = 0; i < size; ++i) {
    push_back(defaultValue.Copy());
  }
}

JSValueArray::JSValueArray(std::initializer_list<JSValueArrayItem> initArray) noexcept {
  for (auto const &item : initArray) {
    this->push_back(std::move(*const_cast<JSValue *>(&item.Item)));
  }
}

JSValueArray::JSValueArray(std::vector<JSValue> &&other) noexcept : vector{std::move(other)} {}

JSValueArray JSValueArray::Copy() const noexcept {
  JSValueArray array;
  array.reserve(size());
  for (auto const &item : *this) {
    array.push_back(item.Copy());
  }

  return array;
}

bool JSValueArray::Equals(JSValueArray const &other) const noexcept {
  if (size() != other.size()) {
    return false;
  }

  auto otherIt = other.begin();
  for (auto const &item : *this) {
    if (!item.Equals(*otherIt++)) {
      return false;
    }
  }

  return true;
}

bool JSValueArray::EqualsAfterConversion(JSValueArray const &other) const noexcept {
  if (size() != other.size()) {
    return false;
  }

  auto otherIt = other.begin();
  for (auto const &item : *this) {
    if (!item.EqualsAfterConversion(*otherIt++)) {
      return false;
    }
  }

  return true;
}

/*static*/ JSValueArray JSValueArray::ReadFrom(IJSValueReader const &reader) noexcept {
  JSValueArray array;
  if (reader.ValueType() == JSValueType::Array) {
    while (reader.GetNextArrayItem()) {
      array.push_back(JSValue::ReadFrom(reader));
    }
  }

  return array;
}

void JSValueArray::WriteTo(IJSValueWriter const &writer) const noexcept {
  writer.WriteArrayBegin();
  for (const JSValue &item : *this) {
    item.WriteTo(writer);
  }

  writer.WriteArrayEnd();
}

//===========================================================================
// JSValue implementation
//===========================================================================

/*static*/ const JSValue JSValue::Null;
/*static*/ const JSValueObject JSValue::EmptyObject;
/*static*/ const JSValueArray JSValue::EmptyArray;
/*static*/ const std::string JSValue::EmptyString;

JSValue::~JSValue() noexcept {
  switch (m_type) {
    case JSValueType::Object:
      m_object.~JSValueObject();
      break;
    case JSValueType::Array:
      m_array.~JSValueArray();
      break;
    case JSValueType::String:
      m_string.~basic_string();
      break;
  }

  m_type = JSValueType::Null;
  m_int64 = 0;
}

JSValue JSValue::Copy() const noexcept {
  switch (m_type) {
    case JSValueType::Object:
      return JSValue{m_object.Copy()};
    case JSValueType::Array:
      return JSValue{m_array.Copy()};
    case JSValueType::String:
      return JSValue{std::string(m_string)};
    case JSValueType::Boolean:
      return JSValue{m_bool};
    case JSValueType::Int64:
      return JSValue{m_int64};
    case JSValueType::Double:
      return JSValue{m_double};
    default:
      return JSValue{};
  }
}

#pragma warning(push)
#pragma warning(disable : 26495) // False positive for union member not initialized
JSValue::JSValue(JSValue &&other) noexcept : m_type{other.m_type} {
  switch (m_type) {
    case JSValueType::Object:
      new (&m_object) JSValueObject(std::move(other.m_object));
      break;
    case JSValueType::Array:
      new (&m_array) JSValueArray(std::move(other.m_array));
      break;
    case JSValueType::String:
      new (&m_string) std::string(std::move(other.m_string));
      break;
    case JSValueType::Boolean:
      m_bool = other.m_bool;
      break;
    case JSValueType::Int64:
      m_int64 = other.m_int64;
      break;
    case JSValueType::Double:
      m_double = other.m_double;
      break;
  }

  other.m_type = JSValueType::Null;
  other.m_int64 = 0;
}
#pragma warning(pop)

JSValue &JSValue::operator=(JSValue &&other) noexcept {
  if (this != &other) {
    this->~JSValue();
    new (this) JSValue(std::move(other));
  }

  return *this;
}

JSValueObject JSValue::MoveObject() noexcept {
  JSValueObject result;
  if (m_type == JSValueType::Object) {
    result = std::move(m_object);
    m_type = JSValueType::Null;
    m_int64 = 0;
  }

  return result;
}

JSValueArray JSValue::MoveArray() noexcept {
  JSValueArray result;
  if (m_type == JSValueType::Array) {
    result = std::move(m_array);
    m_type = JSValueType::Null;
    m_int64 = 0;
  }

  return result;
}

std::string JSValue::AsString() const noexcept {
  switch (m_type) {
    case JSValueType::Null:
      return NullConverter::NullString;
    case JSValueType::Object:
      return ObjectConverter::JavaScriptString;
    case JSValueType::Array:
      return JSValueAsStringWriter::ToString(m_array);
    case JSValueType::String:
      return m_string;
    case JSValueType::Boolean:
      return BooleanConverter::ToString(m_bool);
    case JSValueType::Int64:
      return Int64Converter::ToString(m_int64);
    case JSValueType::Double:
      return DoubleConverter::ToString(m_double);
    default:
      VerifyElseCrashSz(false, "Unexpected JSValue type");
  }
}

bool JSValue::AsBoolean() const noexcept {
  switch (m_type) {
    case JSValueType::Object:
    case JSValueType::Array:
      return true;
    case JSValueType::String:
      return BooleanConverter::FromString(m_string);
    case JSValueType::Boolean:
      return m_bool;
    case JSValueType::Int64:
      return m_int64 != 0;
    case JSValueType::Double:
      return !std::isnan(m_double) && m_double != 0;
    default:
      return false;
  }
}

int8_t JSValue::AsInt8() const noexcept {
  return Int64Converter::To<int8_t>(AsInt64());
}

int16_t JSValue::AsInt16() const noexcept {
  return Int64Converter::To<int16_t>(AsInt64());
}

int32_t JSValue::AsInt32() const noexcept {
  return Int64Converter::To<int32_t>(AsInt64());
}

int64_t JSValue::AsInt64() const noexcept {
  switch (m_type) {
    case JSValueType::Object:
    case JSValueType::Array:
    case JSValueType::String:
      return Int64Converter::FromDouble(AsDouble());
    case JSValueType::Boolean:
      return BooleanConverter::ToInt64(m_bool);
    case JSValueType::Int64:
      return m_int64;
    case JSValueType::Double:
      return Int64Converter::FromDouble(m_double);
    default:
      return 0;
  }
}

uint8_t JSValue::AsUInt8() const noexcept {
  return UInt64Converter::To<uint8_t>(AsUInt64());
}

uint16_t JSValue::AsUInt16() const noexcept {
  return UInt64Converter::To<uint16_t>(AsUInt64());
}

uint32_t JSValue::AsUInt32() const noexcept {
  return UInt64Converter::To<uint32_t>(AsUInt64());
}

uint64_t JSValue::AsUInt64() const noexcept {
  // Convert negative values to zero.
  int64_t value = AsInt64();
  return value >= 0 ? value : 0;
}

double JSValue::AsDouble() const noexcept {
  switch (m_type) {
    case JSValueType::Object:
      return std::numeric_limits<double>::quiet_NaN();
    case JSValueType::Array: {
      switch (m_array.size()) {
        case 0:
          return 0;
        case 1:
          return m_array[0].AsDouble();
        default:
          return std::numeric_limits<double>::quiet_NaN();
      }
    }
    case JSValueType::String:
      return DoubleConverter::FromString(m_string);
    case JSValueType::Boolean:
      return BooleanConverter::ToDouble(m_bool);
    case JSValueType::Int64:
      return static_cast<double>(m_int64);
    case JSValueType::Double:
      return m_double;
    default:
      return 0;
  }
}

float JSValue::AsFloat() const noexcept {
  return static_cast<float>(AsDouble());
}

std::string JSValue::ToString() const noexcept {
  switch (m_type) {
    case JSValueType::Null:
      return NullConverter::NullString;
    case JSValueType::Object:
    case JSValueType::Array:
      return JSValueLogWriter::ToString(*this);
    case JSValueType::String:
      return m_string;
    case JSValueType::Boolean:
      return BooleanConverter::ToString(m_bool);
    case JSValueType::Int64:
      return Int64Converter::ToString(m_int64);
    case JSValueType::Double:
      return DoubleConverter::ToString(m_double);
    default:
      VerifyElseCrashSz(false, "Unexpected JSValue type");
  }
}

size_t JSValue::PropertyCount() const noexcept {
  return (m_type == JSValueType::Object) ? m_object.size() : 0;
}

JSValue const &JSValue::GetObjectProperty(std::string_view propertyName) const noexcept {
  if (m_type == JSValueType::Object) {
    auto it = m_object.find(propertyName);
    if (it != m_object.end()) {
      return it->second;
    }
  }

  return Null;
}

size_t JSValue::ItemCount() const noexcept {
  return (m_type == JSValueType::Array) ? m_array.size() : 0;
}

JSValue const &JSValue::GetArrayItem(JSValueArray::size_type index) const noexcept {
  if (m_type == JSValueType::Array && index < m_array.size()) {
    return m_array[index];
  }

  return Null;
}

bool JSValue::Equals(const JSValue &other) const noexcept {
  if (m_type != other.m_type) {
    return false;
  }

  switch (m_type) {
    case JSValueType::Null:
      return true;
    case JSValueType::Object:
      return m_object.Equals(other.m_object);
    case JSValueType::Array:
      return m_array.Equals(other.m_array);
    case JSValueType::String:
      return m_string == other.m_string;
    case JSValueType::Boolean:
      return m_bool == other.m_bool;
    case JSValueType::Int64:
      return m_int64 == other.m_int64;
    case JSValueType::Double:
      return m_double == other.m_double;
    default:
      return false;
  }
}

bool JSValue::EqualsAfterConversion(const JSValue &other) const noexcept {
  switch (m_type) {
    case JSValueType::Null:
      return other.m_type == JSValueType::Null;
    case JSValueType::Object:
      switch (other.m_type) {
        case JSValueType::Object:
          return m_object.EqualsAfterConversion(other.m_object);
        case JSValueType::String:
          return ObjectConverter::JavaScriptString == other.m_string;
        default:
          return false;
      }
    case JSValueType::Array:
      switch (other.m_type) {
        case JSValueType::Array:
          return m_array.EqualsAfterConversion(other.m_array);
        case JSValueType::String:
          return JSValueAsStringWriter::ToString(m_array) == other.m_string;
        case JSValueType::Boolean:
          return DoubleConverter::FromString(JSValueAsStringWriter::ToString(m_array)) ==
              BooleanConverter::ToDouble(other.m_bool);
        case JSValueType::Int64:
          return DoubleConverter::FromString(JSValueAsStringWriter::ToString(m_array)) ==
              static_cast<double>(other.m_int64);
        case JSValueType::Double:
          return DoubleConverter::FromString(JSValueAsStringWriter::ToString(m_array)) == other.m_double;
        default:
          return false;
      }
    case JSValueType::String:
      switch (other.m_type) {
        case JSValueType::Object:
          return m_string == ObjectConverter::JavaScriptString;
        case JSValueType::Array:
          return m_string == JSValueAsStringWriter::ToString(other.m_array);
        case JSValueType::String:
          return m_string == other.m_string;
        case JSValueType::Boolean:
          return DoubleConverter::FromString(m_string) == BooleanConverter::ToDouble(other.m_bool);
        case JSValueType::Int64:
          return DoubleConverter::FromString(m_string) == static_cast<double>(other.m_int64);
        case JSValueType::Double:
          return DoubleConverter::FromString(m_string) == other.m_double;
        default:
          return false;
      }
    case JSValueType::Boolean:
      switch (other.m_type) {
        case JSValueType::Array:
          return BooleanConverter::ToDouble(m_bool) ==
              DoubleConverter::FromString(JSValueAsStringWriter::ToString(other.m_array));
        case JSValueType::String:
          return BooleanConverter::ToDouble(m_bool) == DoubleConverter::FromString(other.m_string);
        case JSValueType::Boolean:
          return m_bool == other.m_bool;
        case JSValueType::Int64:
          return BooleanConverter::ToInt64(m_bool) == other.m_int64;
        case JSValueType::Double:
          return BooleanConverter::ToDouble(m_bool) == other.m_double;
        default:
          return false;
      }
    case JSValueType::Int64:
      switch (other.m_type) {
        case JSValueType::Array:
          return static_cast<double>(m_int64) ==
              DoubleConverter::FromString(JSValueAsStringWriter::ToString(other.m_array));
        case JSValueType::String:
          return static_cast<double>(m_int64) == DoubleConverter::FromString(other.m_string);
        case JSValueType::Boolean:
          return m_int64 == BooleanConverter::ToInt64(other.m_bool);
        case JSValueType::Int64:
          return m_int64 == other.m_int64;
        case JSValueType::Double:
          return static_cast<double>(m_int64) == other.m_double;
        default:
          return false;
      }
    case JSValueType::Double:
      switch (other.m_type) {
        case JSValueType::Array:
          return m_double == DoubleConverter::FromString(JSValueAsStringWriter::ToString(other.m_array));
        case JSValueType::String:
          return m_double == DoubleConverter::FromString(other.m_string);
        case JSValueType::Boolean:
          return m_double == BooleanConverter::ToDouble(other.m_bool);
        case JSValueType::Int64:
          return m_double == static_cast<double>(other.m_int64);
        case JSValueType::Double:
          return m_double == other.m_double;
        default:
          return false;
      }
    default:
      return false;
  }
}

/*static*/ JSValue JSValue::ReadFrom(IJSValueReader const &reader) noexcept {
  switch (reader.ValueType()) {
    case JSValueType::Null:
      return JSValue();
    case JSValueType::Object:
      return JSValue(JSValueObject::ReadFrom(reader));
    case JSValueType::Array:
      return JSValue(JSValueArray::ReadFrom(reader));
    case JSValueType::String:
      return JSValue(to_string(reader.GetString()));
    case JSValueType::Boolean:
      return JSValue(reader.GetBoolean());
    case JSValueType::Int64:
      return JSValue(reader.GetInt64());
    case JSValueType::Double:
      return JSValue(reader.GetDouble());
    default:
      VerifyElseCrashSz(false, "Unexpected JSValue type");
  }
}

void JSValue::WriteTo(IJSValueWriter const &writer) const noexcept {
  switch (m_type) {
    case JSValueType::Null:
      return writer.WriteNull();
    case JSValueType::Object:
      return m_object.WriteTo(writer);
    case JSValueType::Array:
      return m_array.WriteTo(writer);
    case JSValueType::String:
      return writer.WriteString(to_hstring(m_string));
    case JSValueType::Boolean:
      return writer.WriteBoolean(m_bool);
    case JSValueType::Int64:
      return writer.WriteInt64(m_int64);
    case JSValueType::Double:
      return writer.WriteDouble(m_double);
    default:
      VerifyElseCrashSz(false, "Unexpected JSValue type");
  }
}

} // namespace winrt::Microsoft::ReactNative
