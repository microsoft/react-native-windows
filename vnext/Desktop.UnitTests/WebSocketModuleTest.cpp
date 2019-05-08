// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include <CppUnitTest.h>
#include <WebSocketModule.h>

using namespace facebook::react;
using namespace facebook::xplat::module;
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

TEST_CLASS(WebSocketModuleTest)
{
  enum class MethodId : size_t
  {
    Connect = 0,
    Close = 1,
    Send = 2,
    SendBinary = 3,
    Ping = 4,
    SIZE = 5
  };

  const char* MethodName[static_cast<size_t>(MethodId::SIZE)]
  {
    "connect",
    "close",
    "send",
    "sendBinary",
    "ping"
  };

  TEST_METHOD(WebSocketModuleTest_CreateModule)
  {
    auto module = std::make_unique<WebSocketModule>();

    Assert::IsFalse(module == nullptr);
    Assert::AreEqual(std::string("WebSocketModule"), module->getName());

    auto methods = module->getMethods();
    for (size_t i = 0; i < static_cast<size_t>(MethodId::SIZE); i++)
    {
      Assert::AreEqual(std::string(MethodName[i]), std::string(methods[i].name));
    }

    Assert::AreEqual(static_cast<size_t>(0), module->getConstants().size());
  }
};
