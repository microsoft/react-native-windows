// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include <CppUnitTest.h>
#include <string>
#include <vector>
#include "../Chakra/Utf8DebugExtensions.h"
#include "UnicodeTestStrings.h"

using namespace facebook::react;
using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace std;

namespace Microsoft::React::Test {

TEST_CLASS(StringConversionTest_Desktop) {
 private:
  JsRuntimeHandle runtime;

  TEST_METHOD_INITIALIZE(Setup) {
    JsContextRef context;

    unsigned currentSourceContext = 0;

    // Create a runtime.
    JsCreateRuntime(JsRuntimeAttributeNone, nullptr, &runtime);
    // Create an execution context.
    JsCreateContext(runtime, &context);
    // Now set the current execution context.
    JsSetCurrentContext(context);
  }

  TEST_METHOD_CLEANUP(TearDown) {
    // Dispose runtime
    JsSetCurrentContext(JS_INVALID_REFERENCE);
    JsDisposeRuntime(runtime);
  }

  TEST_METHOD(StringConversionTest_WIN32Test) {
    JsValueRef value;
    string str;
    for (size_t i = 0; i < g_utf8TestStrings.size(); i++) {
      JsPointerToStringUtf8(
          g_utf8TestStrings[i].c_str(), g_utf8TestStrings[i].length(), &value);
      JsStringToStdStringUtf8(value, str);
      Assert::IsTrue(str.compare(g_utf8TestStrings[i]) == 0);
    }
  }
};

} // namespace Microsoft::React::Test
