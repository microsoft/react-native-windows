// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include <jsi/jsi.h>
#include <CppUnitTest.h>

#include <functional>
#include <map>
#include <thread>

using namespace facebook::jsi;
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

using RuntimeFactory = std::function<std::unique_ptr<facebook::jsi::Runtime>()>;

namespace {

#pragma warning(push)
#pragma warning(disable : 4245)
unsigned countOccurences(const std::string &of, const std::string &in) {
  unsigned occurences = 0;
  std::string::size_type lastOccurence = -1;
  while ((lastOccurence = in.find(of, lastOccurence + 1)) != std::string::npos) {
    occurences++;
  }
  return occurences;
}
#pragma warning(pop)

} // namespace

TEST_CLASS(JsiRuntimeUnitTests) {

  // The order of these member variable declarations is important because they
  // need to be initialized in this order.
  RuntimeFactory m_factory;
  //std::unique_ptr<facebook::jsi::Runtime> runtime;
  //facebook::jsi::Runtime &rt;

  Value eval(const char* code)
  {
    auto &rt = *m_factory();

    return rt.global().getPropertyAsFunction(rt, "eval").call(rt, code);
  }

  Function function(const std::string& code) {
    auto &rt = *m_factory();

    return eval(("(" + code + ")").c_str()).getObject(rt).getFunction(rt);
  }

  bool checkValue(const Value& value, const std::string& jsValue)
  {
    auto &rt = *m_factory();

    // TODO (yicyao): Should we use === instead of == here?
    return function("function(value) { return value == " + jsValue + "; }").call(rt, value).getBool();
  }

  TEST_METHOD(JsiRuntimeUnitTests_PropNameIDTest) {
    // This is a little weird to test, because it doesn't really exist
    // in JS yet.  All I can do is create them, compare them, and
    // receive one as an argument to a HostObject.

    auto &rt = *m_factory();
    PropNameID quux = PropNameID::forAscii(rt, "quux1", 4);
    PropNameID movedQuux = std::move(quux);
    Assert::AreEqual(movedQuux.utf8(rt), {"quux"});
    movedQuux = PropNameID::forAscii(rt, "quux2");
    Assert::AreEqual(movedQuux.utf8(rt), {"quux2"});
    PropNameID copiedQuux = PropNameID(rt, movedQuux);
    Assert::IsTrue(PropNameID::compare(rt, movedQuux, copiedQuux));

    Assert::IsTrue(PropNameID::compare(rt, movedQuux, movedQuux));
    Assert::IsTrue(PropNameID::compare(rt, movedQuux, PropNameID::forAscii(rt, {"quux2"})));
    Assert::IsFalse(PropNameID::compare(rt, movedQuux, PropNameID::forAscii(rt, {"foo"})));
    uint8_t utf8[] = {0xF0, 0x9F, 0x86, 0x97};
    PropNameID utf8PropNameID = PropNameID::forUtf8(rt, utf8, sizeof(utf8));
    Assert::AreEqual(utf8PropNameID.utf8(rt), {u8"\U0001F197"});
    Assert::IsTrue(PropNameID::compare(rt, utf8PropNameID, PropNameID::forUtf8(rt, utf8, sizeof(utf8))));
    PropNameID nonUtf8PropNameID = PropNameID::forUtf8(rt, "meow");
    Assert::IsTrue(PropNameID::compare(rt, nonUtf8PropNameID, PropNameID::forAscii(rt, "meow")));
    Assert::AreEqual(nonUtf8PropNameID.utf8(rt), {"meow"});
    PropNameID strPropNameID = PropNameID::forString(rt, String::createFromAscii(rt, "meow"));
    Assert::IsTrue(PropNameID::compare(rt, nonUtf8PropNameID, strPropNameID));

    auto names = PropNameID::names(rt, "Ala", std::string("ma"), PropNameID::forAscii(rt, "kota"));
    Assert::AreEqual(static_cast<size_t>(3), names.size());
    Assert::IsTrue(PropNameID::compare(rt, names[0], PropNameID::forAscii(rt, "Ala")));
    Assert::IsTrue(PropNameID::compare(rt, names[1], PropNameID::forAscii(rt, "ma")));
    Assert::IsTrue(PropNameID::compare(rt, names[2], PropNameID::forAscii(rt, "kota")));
  }

  TEST_METHOD(JsiRuntimeUnitTests_StringTest)
  {
    auto &rt = *m_factory();

    Assert::IsTrue(checkValue(String::createFromAscii(rt, "foobar", 3), "'foo'"));
    Assert::IsTrue(checkValue(String::createFromAscii(rt, "foobar"), "'foobar'"));

    std::string baz = "baz";
    Assert::IsTrue(checkValue(String::createFromAscii(rt, baz), "'baz'"));

    uint8_t utf8[] = {0xF0, 0x9F, 0x86, 0x97};
    Assert::IsTrue(checkValue(String::createFromUtf8(rt, utf8, sizeof(utf8)), "'\\uD83C\\uDD97'"));

    Assert::AreEqual(eval("'quux'").getString(rt).utf8(rt), {"quux"});
    Assert::AreEqual(eval("'\\u20AC'").getString(rt).utf8(rt), {"\xe2\x82\xac"});

    String quux = String::createFromAscii(rt, "quux");
    String movedQuux = std::move(quux);
    Assert::AreEqual(movedQuux.utf8(rt), {"quux"});
    movedQuux = String::createFromAscii(rt, "quux2");
    Assert::AreEqual(movedQuux.utf8(rt), {"quux2"});
  }

  TEST_METHOD(JsiRuntimeUnitTests_ObjectTest)
  {
    auto &rt = *m_factory();

    eval("x = {1:2, '3':4, 5:'six', 'seven':['eight', 'nine']}");
    Object x = rt.global().getPropertyAsObject(rt, "x");
    Assert::AreEqual(static_cast<size_t>(4), x.getPropertyNames(rt).size(rt));
    Assert::IsTrue(x.hasProperty(rt, "1"));
    Assert::IsTrue(x.hasProperty(rt, PropNameID::forAscii(rt, "1")));
    Assert::IsFalse(x.hasProperty(rt, "2"));
    Assert::IsFalse(x.hasProperty(rt, PropNameID::forAscii(rt, "2")));
    Assert::IsTrue(x.hasProperty(rt, "3"));
    Assert::IsTrue(x.hasProperty(rt, PropNameID::forAscii(rt, "3")));
    Assert::IsTrue(x.hasProperty(rt, "seven"));
    Assert::IsTrue(x.hasProperty(rt, PropNameID::forAscii(rt, "seven")));
    Assert::AreEqual(2.0, x.getProperty(rt, "1").getNumber());
    Assert::AreEqual(2.0, x.getProperty(rt, PropNameID::forAscii(rt, "1")).getNumber());
    Assert::AreEqual(4.0, x.getProperty(rt, "3").getNumber());
    Value five = 5;
    Assert::AreEqual(x.getProperty(rt, PropNameID::forString(rt, five.toString(rt))).getString(rt).utf8(rt), {"six"});

    x.setProperty(rt, "ten", 11);
    Assert::AreEqual(static_cast<size_t>(5), x.getPropertyNames(rt).size(rt));
    Assert::IsTrue(eval("x.ten == 11").getBool());

    x.setProperty(rt, "e_as_float", 2.71f);
    Assert::IsTrue(eval("Math.abs(x.e_as_float - 2.71) < 0.001").getBool());

    x.setProperty(rt, "e_as_double", 2.71);
    Assert::IsTrue(eval("x.e_as_double == 2.71").getBool());

    uint8_t utf8[] = {0xF0, 0x9F, 0x86, 0x97};
    String nonAsciiName = String::createFromUtf8(rt, utf8, sizeof(utf8));
    x.setProperty(rt, PropNameID::forString(rt, nonAsciiName), "emoji");
    Assert::AreEqual(static_cast<size_t>(8), x.getPropertyNames(rt).size(rt));
    Assert::IsTrue(eval("x['\\uD83C\\uDD97'] == 'emoji'").getBool());

    Object seven = x.getPropertyAsObject(rt, "seven");
    Assert::IsTrue(seven.isArray(rt));
    Object evalf = rt.global().getPropertyAsObject(rt, "eval");
    Assert::IsTrue(evalf.isFunction(rt));

    Object movedX = Object(rt);
    movedX = std::move(x);
    Assert::AreEqual(static_cast<size_t>(8), movedX.getPropertyNames(rt).size(rt));
    Assert::AreEqual(2.0, movedX.getProperty(rt, "1").getNumber());

    Object obj = Object(rt);
    obj.setProperty(rt, "roses", "red");
    obj.setProperty(rt, "violets", "blue");
    Object oprop = Object(rt);
    obj.setProperty(rt, "oprop", oprop);
    obj.setProperty(rt, "aprop", Array(rt, 1));

    Assert::IsTrue(function("function (obj) { return "
                         "obj.roses == 'red' && "
                         "obj['violets'] == 'blue' && "
                         "typeof obj.oprop == 'object' && "
                         "Array.isArray(obj.aprop); }")
                    .call(rt, obj)
                    .getBool());

    // Check that getPropertyNames doesn't return non-enumerable
    // properties.
    obj = function(
              "function () {"
              "  obj = {};"
              "  obj.a = 1;"
              "  Object.defineProperty(obj, 'b', {"
              "    enumerable: false,"
              "    value: 2"
              "  });"
              "  return obj;"
              "}")
              .call(rt)
              .getObject(rt);
    Assert::AreEqual(1.0, obj.getProperty(rt, "a").getNumber());
    Assert::AreEqual(2.0, obj.getProperty(rt, "b").getNumber());
    Array names = obj.getPropertyNames(rt);
    Assert::AreEqual(static_cast<size_t>(1), names.size(rt));
    Assert::AreEqual(names.getValueAtIndex(rt, 0).getString(rt).utf8(rt), {"a"});
  }

  TEST_METHOD(JsiRuntimeUnitTests_FunctionTest)
  {
    auto &rt = *m_factory();

    // test move ctor
    Function fmove = function("function() { return 1 }");
    {
      Function g = function("function() { return 2 }");
      fmove = std::move(g);
    }
    Assert::AreEqual(2.0, fmove.call(rt).getNumber());

    // This tests all the function argument converters, and all the
    // non-lvalue overloads of call().
    Function f = function(
        "function(n, b, d, df, i, s1, s2, s3, o, a, f, v) { return "
        "n === null && "
        "b === true && "
        "d === 3.14 && "
        "Math.abs(df - 2.71) < 0.001 && "
        "i === 17 && "
        "s1 == 's1' && "
        "s2 == 's2' && "
        "s3 == 's3' && "
        "typeof o == 'object' && "
        "Array.isArray(a) && "
        "typeof f == 'function' && "
        "v == 42 }");
    std::string s3 = "s3";
    Assert::IsTrue(f.call(
                     rt,
                     nullptr,
                     true,
                     3.14,
                     2.71f,
                     17,
                     "s1",
                     String::createFromAscii(rt, "s2"),
                     s3,
                     Object(rt),
                     Array(rt, 1),
                     function("function(){}"),
                     Value(42))
                    .getBool());

    // lvalue overloads of call()
    Function flv = function(
        "function(s, o, a, f, v) { return "
        "s == 's' && "
        "typeof o == 'object' && "
        "Array.isArray(a) && "
        "typeof f == 'function' && "
        "v == 42 }");

    String s = String::createFromAscii(rt, "s");
    Object o = Object(rt);
    Array a = Array(rt, 1);
    Value v = 42;
    Assert::IsTrue(flv.call(rt, s, o, a, f, v).getBool());

    Function f1 = function("function() { return 1; }");
    Function f2 = function("function() { return 2; }");
    f2 = std::move(f1);
    Assert::AreEqual(1.0, f2.call(rt).getNumber());
  }

  TEST_METHOD(JsiRuntimeUnitTests_FunctionThisTest)
  {
    auto &rt = *m_factory();

    Function checkPropertyFunction = function("function() { return this.a === 'a_property' }");

    Object jsObject = Object(rt);
    jsObject.setProperty(rt, "a", String::createFromUtf8(rt, "a_property"));

    // class APropertyHostObject : public HostObject {
    //  Value get(Runtime& rt, const PropNameID& sym) override {
    //    return String::createFromUtf8(rt, "a_property");
    //  }

    //  void set(Runtime&, const PropNameID&, const Value&) override {}
    //};
    // Object hostObject =
    //  Object::createFromHostObject(rt, std::make_shared<APropertyHostObject>());

    Assert::IsTrue(checkPropertyFunction.callWithThis(rt, jsObject).getBool());
    // Assert::IsTrue(checkPropertyFunction.callWithThis(rt, hostObject).getBool());
    Assert::IsFalse(checkPropertyFunction.callWithThis(rt, Array(rt, 5)).getBool());
    Assert::IsFalse(checkPropertyFunction.call(rt).getBool());
  }

  TEST_METHOD(JsiRuntimeUnitTests_FunctionConstructorTest)
  {
    auto &rt = *m_factory();

    Function ctor = function(
        "function (a) {"
        "  if (typeof a !== 'undefined') {"
        "   this.pika = a;"
        "  }"
        "}");
    ctor.getProperty(rt, "prototype").getObject(rt).setProperty(rt, "pika", "chu");
    auto empty = ctor.callAsConstructor(rt);
    Assert::IsTrue(empty.isObject());
    auto emptyObj = std::move(empty).getObject(rt);
    Assert::AreEqual(emptyObj.getProperty(rt, "pika").getString(rt).utf8(rt), {"chu"});
    auto who = ctor.callAsConstructor(rt, "who");
    Assert::IsTrue(who.isObject());
    auto whoObj = std::move(who).getObject(rt);
    Assert::AreEqual(whoObj.getProperty(rt, "pika").getString(rt).utf8(rt), {"who"});

    auto instanceof = function("function (o, b) { return o instanceof b; }");
    Assert::IsTrue(instanceof.call(rt, emptyObj, ctor).getBool());
    Assert::IsTrue(instanceof.call(rt, whoObj, ctor).getBool());

    auto dateCtor = rt.global().getPropertyAsFunction(rt, "Date");
    auto date = dateCtor.callAsConstructor(rt);
    Assert::IsTrue(date.isObject());
    Assert::IsTrue(instanceof.call(rt, date, dateCtor).getBool());
    // Sleep for 50 milliseconds
    std::this_thread::sleep_for(std::chrono::milliseconds(50));

    // We use 48 here because sleep_for is not very precise,
    // and this test is not about timing precision.
    Assert::IsTrue(function("function (d) { return (new Date()).getTime() - d.getTime(); }").call(rt, date).getNumber() >= 48);
  }

  TEST_METHOD(JsiRuntimeUnitTests_InstanceOfTest)
  {
    auto &rt = *m_factory();

    auto ctor = function("function Rick() { this.say = 'wubalubadubdub'; }");
    auto newObj = function("function (ctor) { return new ctor(); }");
    auto instance = newObj.call(rt, ctor).getObject(rt);
    Assert::IsTrue(instance.instanceOf(rt, ctor));
    Assert::AreEqual(instance.getProperty(rt, "say").getString(rt).utf8(rt), {"wubalubadubdub"});
    Assert::IsFalse(Object(rt).instanceOf(rt, ctor));
    Assert::IsTrue(ctor.callAsConstructor(rt, nullptr, 0).getObject(rt).instanceOf(rt, ctor));
  }

  TEST_METHOD(JsiRuntimeUnitTests_ValueTest)
  {
    auto &rt = *m_factory();

    Assert::IsTrue(checkValue(Value::undefined(), "undefined"));
    Assert::IsTrue(checkValue(Value(), "undefined"));
    Assert::IsTrue(checkValue(Value::null(), "null"));
    Assert::IsTrue(checkValue(nullptr, "null"));

    Assert::IsTrue(checkValue(Value(false), "false"));
    Assert::IsTrue(checkValue(false, "false"));
    Assert::IsTrue(checkValue(true, "true"));

    Assert::IsTrue(checkValue(Value(1.5), "1.5"));
    Assert::IsTrue(checkValue(2.5, "2.5"));

    Assert::IsTrue(checkValue(Value(10), "10"));
    Assert::IsTrue(checkValue(20, "20"));
    Assert::IsTrue(checkValue(30, "30"));

    // rvalue implicit conversion
    Assert::IsTrue(checkValue(String::createFromAscii(rt, "one"), "'one'"));
    // lvalue explicit copy
    String s = String::createFromAscii(rt, "two");
    Assert::IsTrue(checkValue(Value(rt, s), "'two'"));

    {
      // rvalue assignment of trivial value
      Value v1 = 100;
      Value v2 = String::createFromAscii(rt, "hundred");
      v2 = std::move(v1);
      Assert::IsTrue(v2.isNumber());
      Assert::AreEqual(100.0, v2.getNumber());
    }

    {
      // rvalue assignment of js heap value
      Value v1 = String::createFromAscii(rt, "hundred");
      Value v2 = 100;
      v2 = std::move(v1);
      Assert::IsTrue(v2.isString());
      Assert::AreEqual(v2.getString(rt).utf8(rt), {"hundred"});
    }

    Object o = Object(rt);
    Assert::IsTrue(function("function(value) { return typeof(value) == 'object'; }").call(rt, Value(rt, o)).getBool());

    uint8_t utf8[] = "[null, 2, \"c\", \"emoji: \xf0\x9f\x86\x97\", {}]";

    Assert::IsTrue(function("function (arr) { return "
                         "Array.isArray(arr) && "
                         "arr.length == 5 && "
                         "arr[0] === null && "
                         "arr[1] == 2 && "
                         "arr[2] == 'c' && "
                         "arr[3] == 'emoji: \\uD83C\\uDD97' && "
                         "typeof arr[4] == 'object'; }")
                    .call(rt, Value::createFromJsonUtf8(rt, utf8, sizeof(utf8) - 1))
                    .getBool());

    Assert::IsTrue(eval("undefined").isUndefined());
    Assert::IsTrue(eval("null").isNull());
    Assert::IsTrue(eval("true").isBool());
    Assert::IsTrue(eval("false").isBool());
    Assert::IsTrue(eval("123").isNumber());
    Assert::IsTrue(eval("123.4").isNumber());
    Assert::IsTrue(eval("'str'").isString());
    // "{}" returns undefined.  empty code block?
    Assert::IsTrue(eval("({})").isObject());
    Assert::IsTrue(eval("[]").isObject());
    Assert::IsTrue(eval("(function(){})").isObject());

    Assert::AreEqual(123.0, eval("123").getNumber());
    Assert::AreEqual(eval("123.4").getNumber(), 123.4);
    Assert::AreEqual(eval("'str'").getString(rt).utf8(rt), {"str"});
    Assert::IsTrue(eval("[]").getObject(rt).isArray(rt));

    Assert::AreEqual(456.0, eval("456").asNumber());
    //TODO!
    //EXPECT_THROW(eval("'word'").asNumber(), JSIException);
    Assert::AreEqual(2.0, eval("({1:2, 3:4})").asObject(rt).getProperty(rt, "1").getNumber());
    //TODO!
    //EXPECT_THROW(eval("'oops'").asObject(rt), JSIException);

    Assert::AreEqual(eval("['zero',1,2,3]").toString(rt).utf8(rt), {"zero,1,2,3"});
  }

  TEST_METHOD(JsiRuntimeUnitTests_EqualsTest)
  {
    auto &rt = *m_factory();

    Assert::IsTrue(Object::strictEquals(rt, rt.global(), rt.global()));
    Assert::IsTrue(Value::strictEquals(rt, 1, 1));
    Assert::IsFalse(Value::strictEquals(rt, true, 1));
    Assert::IsFalse(Value::strictEquals(rt, true, false));
    Assert::IsTrue(Value::strictEquals(rt, false, false));
    Assert::IsFalse(Value::strictEquals(rt, nullptr, 1));
    Assert::IsTrue(Value::strictEquals(rt, nullptr, nullptr));
    Assert::IsTrue(Value::strictEquals(rt, Value::undefined(), Value()));
    Assert::IsTrue(Value::strictEquals(rt, rt.global(), Value(rt.global())));
    Assert::IsFalse(
        Value::strictEquals(rt, std::numeric_limits<double>::quiet_NaN(), std::numeric_limits<double>::quiet_NaN()));
    Assert::IsFalse(Value::strictEquals(
        rt, std::numeric_limits<double>::signaling_NaN(), std::numeric_limits<double>::signaling_NaN()));
    Assert::IsTrue(Value::strictEquals(rt, +0.0, -0.0));
    Assert::IsTrue(Value::strictEquals(rt, -0.0, +0.0));

    Function noop = Function::createFromHostFunction(
        rt, PropNameID::forAscii(rt, "noop"), 0, [](const Runtime &, const Value &, const Value *, size_t) {
          return Value();
        });
    auto noopDup = Value(rt, noop).getObject(rt);
    Assert::IsTrue(Object::strictEquals(rt, noop, noopDup));
    Assert::IsTrue(Object::strictEquals(rt, noopDup, noop));
    Assert::IsFalse(Object::strictEquals(rt, noop, rt.global()));
    Assert::IsTrue(Object::strictEquals(rt, noop, noop));
    Assert::IsTrue(Value::strictEquals(rt, Value(rt, noop), Value(rt, noop)));

    String str = String::createFromAscii(rt, "rick");
    String strDup = String::createFromAscii(rt, "rick");
    String otherStr = String::createFromAscii(rt, "morty");
    Assert::IsTrue(String::strictEquals(rt, str, str));
    Assert::IsTrue(String::strictEquals(rt, str, strDup));
    Assert::IsTrue(String::strictEquals(rt, strDup, str));
    Assert::IsFalse(String::strictEquals(rt, str, otherStr));
    Assert::IsTrue(Value::strictEquals(rt, Value(rt, str), Value(rt, str)));
    Assert::IsFalse(Value::strictEquals(rt, Value(rt, str), Value(rt, noop)));
    Assert::IsFalse(Value::strictEquals(rt, Value(rt, str), 1.0));
  }

  TEST_METHOD(JsiRuntimeUnitTests_ExceptionStackTraceTest)
  {
    auto &rt = *m_factory();

      static const char invokeUndefinedScript[] =
        "function hello() {"
        "  var a = {}; a.log(); }"
        "function world() { hello(); }"
        "world()";
    std::string stack;
    try {
      rt.evaluateJavaScript(std::make_unique<StringBuffer>(invokeUndefinedScript), "");
    } catch (JSError &e) {
      stack = e.getStack();
    }
    Assert::AreNotEqual(stack.find("world"), std::string::npos);
  }

  //TODO
  TEST_METHOD(JsiRuntimeUnitTests_JSErrorsArePropagatedNicely)
  {
    auto &rt = *m_factory();

    //unsigned callsBeforeError = 5;

    //Function sometimesThrows = function(
    //    "function sometimesThrows(shouldThrow, callback) {"
    //    "  if (shouldThrow) {"
    //    "    throw Error('Omg, what a nasty exception')"
    //    "  }"
    //    "  callback(callback);"
    //    "}");

    //Function callback = Function::createFromHostFunction(
    //    rt,
    //    PropNameID::forAscii(rt, "callback"),
    //    0,
    //    [&sometimesThrows, &callsBeforeError](Runtime &rt, const Value &thisVal, const Value *args, size_t count) {
    //      return sometimesThrows.call(rt, --callsBeforeError == 0, args[0]);
    //    });

    //try {
    //  sometimesThrows.call(rt, false, callback);
    //} catch (JSError &error) {
    //  // V8 will include the previous exception messages upon rethrow from native to JS
    //  EXPECT_PRED_FORMAT2(::testing::IsSubstring, "Omg, what a nasty exception", error.getMessage());
    //  EXPECT_GE(countOccurences("sometimesThrows", error.getStack()), 6);

    //  // system JSC JSI does not implement host function names
    //  // EXPECT_EQ(countOccurences("callback", error.getStack(rt)), 5);
    //}
  }

  TEST_METHOD(JsiRuntimeUnitTests_JSErrorsCanBeConstructedWithStack)
  {
    auto &rt = *m_factory();

    auto err = JSError(rt, "message", "stack");
    Assert::AreEqual(err.getMessage(), {"message"});
    Assert::AreEqual(err.getStack(), {"stack"});
  }

  TEST_METHOD(JsiRuntimeUnitTests_ScopeDoesNotCrashTest)
  {
    auto &rt = *m_factory();

    Scope scope(rt);
    Object o(rt);
  }

  TEST_METHOD(JsiRuntimeUnitTests_ScopeDoesNotCrashWhenValueEscapes)
  {
    auto &rt = *m_factory();

    Value v;
    Scope::callInNewScope(rt, [&]() {
      Object o(rt);
      o.setProperty(rt, "a", 5);
      v = std::move(o);
    });
    Assert::AreEqual(5.0, v.getObject(rt).getProperty(rt, "a").getNumber());
  }

  // Verifies you can have a host object that emulates a normal object
  TEST_METHOD(JsiRuntimeUnitTests_HostObjectWithValueMembers)
  {
    auto &rt = *m_factory();

    class Bag : public HostObject {
     public:
      Bag() = default;

      const Value &operator[](const std::string &name) const {
        auto iter = data_.find(name);
        if (iter == data_.end()) {
          return undef_;
        }
        return iter->second;
      }

     protected:
      Value get(Runtime &rt, const PropNameID &name) override {
        return Value(rt, (*this)[name.utf8(rt)]);
      }

      void set(Runtime &rt, const PropNameID &name, const Value &val) override {
        data_.emplace(name.utf8(rt), Value(rt, val));
      }

      Value undef_;
      std::map<std::string, Value> data_;
    };

    auto sharedBag = std::make_shared<Bag>();
    auto &bag = *sharedBag;
    Object jsbag = Object::createFromHostObject(rt, std::move(sharedBag));
    auto set = function(
        "function (o) {"
        "  o.foo = 'bar';"
        "  o.count = 37;"
        "  o.nul = null;"
        "  o.iscool = true;"
        "  o.obj = { 'foo': 'bar' };"
        "}");
    set.call(rt, jsbag);
    auto checkFoo = function("function (o) { return o.foo === 'bar'; }");
    auto checkCount = function("function (o) { return o.count === 37; }");
    auto checkNul = function("function (o) { return o.nul === null; }");
    auto checkIsCool = function("function (o) { return o.iscool === true; }");
    auto checkObj = function(
        "function (o) {"
        "  return (typeof o.obj) === 'object' && o.obj.foo === 'bar';"
        "}");
    // Check this looks good from js
    Assert::IsTrue(checkFoo.call(rt, jsbag).getBool());
    Assert::IsTrue(checkCount.call(rt, jsbag).getBool());
    Assert::IsTrue(checkNul.call(rt, jsbag).getBool());
    Assert::IsTrue(checkIsCool.call(rt, jsbag).getBool());
    Assert::IsTrue(checkObj.call(rt, jsbag).getBool());

    // Check this looks good from c++
    Assert::AreEqual(bag["foo"].getString(rt).utf8(rt), {"bar"});
    Assert::AreEqual(37.0, bag ["count"].getNumber());
    Assert::IsTrue(bag["nul"].isNull());
    Assert::IsTrue(bag["iscool"].getBool());
    Assert::AreEqual(bag["obj"].getObject(rt).getProperty(rt, "foo").getString(rt).utf8(rt), {"bar"});
  }
};
