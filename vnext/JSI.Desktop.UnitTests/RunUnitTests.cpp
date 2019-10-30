// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "JSI/Shared/ChakraRuntimeArgs.h"
#include "JSI/Shared/ChakraRuntimeFactory.h"
#include "MemoryTracker.h"

#include <IntegrationTests/TestMessageQueueThread.h>

#include <gtest/gtest.h>
#include <windows.h>

using facebook::jsi::Runtime;
using facebook::react::CreateMemoryTracker;
using facebook::react::MessageQueueThread;
using Microsoft::JSI::ChakraRuntimeArgs;
using Microsoft::JSI::makeChakraRuntime;
using Microsoft::React::Test::TestMessageQueueThread;

int main(int argc, char **argv) {

  system("pause");

  ChakraRuntimeArgs args{};
  args.jsQueue = std::make_shared<TestMessageQueueThread>();

  std::shared_ptr<MessageQueueThread> memoryTrackerCallbackQueue = std::make_shared<TestMessageQueueThread>();

  args.memoryTracker = CreateMemoryTracker(std::move(memoryTrackerCallbackQueue));

  std::unique_ptr<Runtime> runtime = makeChakraRuntime(std::move(args));

  std::shared_ptr<facebook::jsi::StringBuffer> leoScript = std::make_shared<facebook::jsi::StringBuffer>(
      "(function () {\n"
      "	function Mammal() {\n"
      "		this.bloodTemp = 'warm';\n"
      "	}\n"
      "	function Carnivore() {}\n"
      "	function Lion(name) {\n"
      "		Mammal.call(this);\n"
      "		this.name = name;\n"
      "	}\n"
      "	Mammal.prototype.growHair = function() {\n"
      "		console.log('my hair is growing');\n"
      "	}\n"
      "	Carnivore.prototype = Object.create(Mammal.prototype);\n"
      "	Carnivore.prototype.eatMeat = function() {\n"
      "		console.log('Mmm.Meat');\n"
      "	};\n"
      "	Lion.prototype = Object.create(Carnivore.prototype);\n"
      "	Lion.prototype.pride = function() {\n"
      "		console.log('im king of the jungle');\n"
      "	};\n"
      "	var leo = new Lion('leo');\n"
      " Object.defineProperty(leo, 'son', {\n"
      "    enumerable: false,\n"
      "    value: 20\n"
      " });\n"
      "	return leo;\n"
      //"  function Animal (name) {\n"
      //"    this.name = name\n"
      //"  }\n"
      //"  Animal.prototype.sleep = function () {\n"
      //"    console.log('${this.name} is sleeping.')\n"
      //"  }\n"
      //"  var leo = new Animal('Leo')\n"
      //"  return leo\n"
      "})()");
  facebook::jsi::Object leo = runtime->evaluateJavaScript(leoScript, "").getObject(*runtime);
  std::cout << leo.getPropertyNames(*runtime).size(*runtime) << std::endl;

  std::shared_ptr<facebook::jsi::StringBuffer> strScript = std::make_shared<facebook::jsi::StringBuffer>(
      "(function () {\n"
      "  var str = new String('abc')\n"
      "  return str\n"
      "})()");
  facebook::jsi::Object str = runtime->evaluateJavaScript(strScript, "").getObject(*runtime);
  std::cout << str.getPropertyNames(*runtime).size(*runtime) << std::endl;

  std::shared_ptr<facebook::jsi::StringBuffer> arrScript = std::make_shared<facebook::jsi::StringBuffer>(
      "(function () {\n"
      "  var arr = [1, 2]\n"
      "  return arr\n"
      "})()");
  facebook::jsi::Object arr = runtime->evaluateJavaScript(arrScript, "").getObject(*runtime);
  std::cout << arr.getPropertyNames(*runtime).size(*runtime) << std::endl;



  //::testing::InitGoogleTest(&argc, argv);
  //return RUN_ALL_TESTS();
}
