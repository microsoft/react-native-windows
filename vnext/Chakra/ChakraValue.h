// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include <memory>
#include <sstream>
#include <unordered_map>
#include <vector>

#include <folly/dynamic.h>

/* definition to expand macro then apply to pragma message */
#define VALUE_TO_STRING(x) #x
#define VALUE(x) VALUE_TO_STRING(x)
#define VAR_NAME_VALUE(var) #var "=" VALUE(var)

#pragma message(VAR_NAME_VALUE(FOLLY_HAS_STRING_VIEW))
#pragma message(VAR_NAME_VALUE(FOLLY_MOBILE))
#pragma message(VAR_NAME_VALUE(FOLLY_SSE))
#pragma message(VAR_NAME_VALUE(FOLLY_SSE_MINOR))
#pragma message(VAR_NAME_VALUE(FOLLY_HAVE_MEMRCHR))
#pragma message(VAR_NAME_VALUE(FOLLY_NEON))

#if defined(USE_EDGEMODE_JSRT)
#include <jsrt.h>
#else
#include <ChakraCore.h>
#endif

#include "Utf8DebugExtensions.h"

namespace facebook {
namespace react {

class ChakraValue;
class ChakraObject;

enum JSPropertyAttributes
{
  kJSPropertyAttributeNone
};

class ChakraString
{
public:
  ChakraString& operator=(const ChakraString&) = delete;
  ChakraString() = default;

  explicit ChakraString(const char* utf8)
  {
    JsValueRef value = nullptr;
    // TODO verify return success
    if (JsNoError == JsPointerToStringUtf8(utf8, strlen(utf8), &value))
      JsAddRef(value, nullptr);
    m_string = value;
  }

  ChakraString(ChakraString&& other) :
    m_string(other.m_string)
  {
    other.m_string = nullptr;
  }

  ChakraString(const ChakraString& other) :
    m_string(other.m_string)
  {
    if (m_string)
    {
      JsAddRef(m_string, nullptr);
    }
  }

  ~ChakraString()
  {
    if (m_string)
    {
      JsRelease(m_string, nullptr);
    }
  }

  operator JsValueRef() const
  {
    return m_string;
  }

  // Length in characters
  size_t length() const
  {
    int length;
    JsGetStringLength(m_string, &length);
    return length;
  }

  /*
    * JavaScriptCore is built with strict utf16 -> utf8 conversion.
    * This means if JSC's built-in conversion function encounters a JavaScript
    * string which contains half of a 32-bit UTF-16 symbol, it produces an error
    * rather than returning a string.
    *
    * Instead of relying on this, we use our own utf16 -> utf8 conversion function
    * which is more lenient and always returns a string. When an invalid UTF-16
    * string is provided, it'll likely manifest as a rendering glitch in the app for
    * the invalid symbol.
    *
    * For details on JavaScript's unicode support see:
    * https://mathiasbynens.be/notes/javascript-unicode
    */
  std::string str() const
  {
    std::string str;
    /*auto result = */JsStringToStdStringUtf8(m_string, str);
    return str;
  }

  // Assumes that utf8 is null terminated
  bool equals(const char* utf8)
  {
    std::string str;

    /*auto result = */JsStringToStdStringUtf8(m_string, str);
    return str.compare(utf8) == 0;
  }

  // This assumes ascii is nul-terminated.
  static ChakraString createExpectingAscii(const char* ascii, size_t len)
  {
    return ChakraString(ascii);
  }

  static ChakraString createExpectingAscii(std::string const &ascii)
  {
    return createExpectingAscii(ascii.c_str(), ascii.size());
  }

  static ChakraString ref(JsValueRef string)
  {
    return ChakraString(string, false);
  }

  static ChakraString adopt(JsValueRef string)
  {
    return ChakraString(string, true);
  }

private:
  explicit ChakraString(JsValueRef string, bool adopt) :
    m_string(string)
  {
    if (!adopt && string)
    {
      JsAddRef(string, nullptr);
    }
  }

  JsValueRef m_string;
};


class ChakraObject
{
public:
  ChakraObject(const ChakraObject&) = delete;
  ChakraObject& operator=(const ChakraObject&) = delete;
  ChakraObject() = default;

  ChakraObject(JsValueRef obj) :
    m_obj(obj)
  {
    // TODO :: VEC is value is not object.
  }

  ChakraObject(ChakraObject&& other) :
    m_obj(other.m_obj),
    m_isProtected(other.m_isProtected)
  {
    other.m_obj = nullptr;
    other.m_isProtected = false;
  }

  ~ChakraObject()
  {
    if (m_isProtected && m_obj)
    {
      JsRelease(m_obj, nullptr);
    }
  }

  ChakraObject& operator=(ChakraObject&& other)
  {
    std::swap(m_obj, other.m_obj);
    std::swap(m_isProtected, other.m_isProtected);
    return *this;
  }

  operator JsValueRef() const
  {
    return m_obj;
  }

  bool isFunction() const
  {
    JsValueType type;
    JsGetValueType(m_obj, &type);
    return type == JsFunction;
  }

  ChakraValue callAsFunction(std::initializer_list<JsValueRef> args) const;
  ChakraValue callAsFunction(const ChakraObject& thisObj, std::initializer_list<JsValueRef> args) const;
  ChakraValue callAsFunction(int nArgs, const JsValueRef args[]) const;
  ChakraValue callAsFunction(const ChakraObject& thisObj, int nArgs, const JsValueRef args[]) const;

  ChakraObject callAsConstructor(std::initializer_list<JsValueRef> args) const;

  ChakraValue getProperty(const ChakraString& propName) const;
  ChakraValue getProperty(const char *propName) const;
  ChakraValue getPropertyAtIndex(unsigned index) const;
  void setProperty(const ChakraString& propName, const ChakraValue& value) const;
  void setProperty(const char *propName, const ChakraValue& value) const;
  std::unordered_map<std::string, std::string> toJSONMap() const;

  std::vector<ChakraString> getPropertyNames() const;

  void makeProtected()
  {
    if (!m_isProtected && m_obj)
    {
      JsAddRef(m_obj, nullptr);
      m_isProtected = true;
    }
  }

  static ChakraObject getGlobalObject()
  {
    JsValueRef value;
    JsGetGlobalObject(&value);
    return ChakraObject(value);
  }

  /**
    * Creates an instance of the default object class.
    */
  static ChakraObject create();

private:
  JsValueRef m_obj;
  bool m_isProtected = false;

  ChakraValue callAsFunction(JsValueRef thisObj, int nArgs, const JsValueRef args[]) const;
};

class ChakraValue
{
public:
  ChakraValue(const ChakraValue&) = delete;
  ChakraValue& operator=(const ChakraValue&) = delete;
  ChakraValue() = default;
  ChakraValue(JsValueRef value);
  ChakraValue(ChakraValue&&);

  operator JsValueRef() const
  {
    return m_value;
  }

  JsValueType getType() const
  {
    JsValueType type;
    JsGetValueType(m_value, &type);
    return type;
  }

  bool isBoolean() const
  {
    return getType() == JsBoolean;
  }

  bool asBoolean() const
  {
    bool value;
    JsBooleanToBool(m_value, &value);
    return value;
  }

  bool isNumber() const
  {
    return getType() == JsNumber;
  }

  bool isNull() const
  {
    return getType() == JsNull;
  }

  bool isUndefined() const
  {
    return getType() == JsUndefined;
  }

  double asNumber() const
  {
    if (isNumber())
    {
      double value;
      auto result = JsNumberToDouble(m_value, &value);
      if (result == JsErrorScriptException)
      {
        assert(false && "JsNumberToDouble() failed.");
        std::terminate();
      }
      return value;
    }
    else
    {
      return 0.0f;
    }
  }

  int32_t asInteger() const
  {
    return static_cast<int32_t>(asNumber());
  }

  uint32_t asUnsignedInteger() const
  {
    return static_cast<uint32_t>(asNumber());
  }

  bool isObject() const
  {
    return getType() == JsObject;
  }

  ChakraObject asObject();

  bool isString() const
  {
    return getType() == JsString;
  }

  ChakraString toString() noexcept
  {
    JsValueRef value;
    auto result = JsConvertValueToString(m_value, &value);
    if (JsNoError == result)
    {
      JsAddRef(value, nullptr); // TODO is this the right lifetime symantics?
    }
    else if (result == JsErrorScriptException)
    {
      assert(false && "JsConvertValueToString() failed().");
      std::terminate();
    }

    return ChakraString::adopt(value);
  }

  static ChakraValue makeError(const char *error);

  static ChakraValue makeNumber(double dbl)
  {
    JsValueRef value;
    JsDoubleToNumber(dbl, &value);
    return value;
  }

  static ChakraValue makeUndefined()
  {
    JsValueRef value;
    JsGetUndefinedValue(&value);
    return value;
  }

  std::string toJSONString(unsigned indent = 0) const;
  static ChakraValue fromJSON(const ChakraString& json);
  static JsValueRef fromDynamic(const folly::dynamic& value);
protected:
  JsValueRef m_value;
  // static JsValueRef fromDynamicInner(JsContextRef ctx, const folly::dynamic& obj);
};

}
}
