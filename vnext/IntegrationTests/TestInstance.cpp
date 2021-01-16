// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "TestInstance.h"

using namespace facebook::react;

using folly::dynamic;
using std::map;
using std::shared_ptr;
using std::string;

namespace Microsoft::React::Test {

TestUIManager::TestUIManager() noexcept {}

TestUIManager ::~TestUIManager() noexcept {}

dynamic GetGenericViewConfig() {
  return dynamic::object("Constants", dynamic::object())("Commands", dynamic::object())(
      "NativeProps", dynamic::object());
}

std::string TestUIManager::getName() {
  return "UIManager";
};
std::map<std::string, dynamic> TestUIManager::getConstants() {
  std::map<std::string, dynamic> map;
  map["RCTView"] = GetGenericViewConfig();
  map["RCTText"] = GetGenericViewConfig();
  map["RCTRawText"] = GetGenericViewConfig();
  map["RCTImageView"] = GetGenericViewConfig();
  map["RCTScrollView"] = GetGenericViewConfig();
  return std::move(map);
}
std::vector<facebook::xplat::module::CxxModule::Method> TestUIManager::getMethods() {
  return {
      {"getConstantsForViewManager", [](dynamic argsJson) -> dynamic { return GetGenericViewConfig(); }, SyncTag},
      {"removeRootView", [](dynamic args) {}},
      {"createView", [](dynamic args) {}},
      {"configureNextLayoutAnimation", [](dynamic args, Callback cbSuccess, Callback cbError) {}, AsyncTag},
      {"setChildren", [](dynamic args) {}},
      {"updateView", [](dynamic args) {}},
      {"removeSubviewsFromContainerWithID", [](dynamic args) {}},
      {"manageChildren", [](dynamic args) {}},
      {"replaceExistingNonRootView", [](dynamic args) {}},
      {"dispatchViewManagerCommand", [](dynamic args) {}},
      {"measure", [](dynamic args, Callback cb) {}},
      {"measureInWindow", [](dynamic args, Callback cb) {}},
      {"measureLayout", [](dynamic args, Callback cbError, Callback cbSuccess) {}, AsyncTag},
      {"findSubviewIn", [](dynamic args, Callback cb) {}},
      {"focus", [](dynamic args) {}},
      {"blur", [](dynamic args) {}},
      {"setJSResponder", [](dynamic args) {}},
      {"clearJSResponder", []() {}},
  };
}

} // namespace Microsoft::React::Test
