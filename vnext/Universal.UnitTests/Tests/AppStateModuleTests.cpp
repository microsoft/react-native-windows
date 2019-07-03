// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.
#include "pch.h"

#include <CppUnitTest.h>

#include <Modules/AppStateModule.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace facebook::react;

typedef std::function<void(std::vector<folly::dynamic>)> Callback;

TEST_CLASS(AppStateModuleTest){TEST_METHOD(AppStateModule_Initialize){
    // Retreive the module and its JS methods
    auto appStateModule =
        std::make_unique<AppStateModule>(std::make_unique<AppState>());
auto methods = appStateModule -> getMethods();

// Setup callbacks from method
Callback first([&](std::vector<folly::dynamic> args) {
  Assert::AreEqual(args[0]["app_state"].asString().c_str(), "active");
});
Callback second;

// Invoke getCurrentAppState method
const auto &method = methods[0];
folly::dynamic params = folly::dynamic::array();
method.func(std::move(params), first, second);
}
}
;
