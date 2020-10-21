// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "TestInstance.h"

using namespace facebook::react;

using std::make_unique;
using std::map;
using std::move;
using std::shared_ptr;
using std::string;

namespace Microsoft::React::Test {

TestUIManager::TestUIManager() noexcept {}

TestUIManager ::~TestUIManager() noexcept {}

folly::dynamic GetGenericViewConfig() {
  return folly::dynamic::object("Constants", folly::dynamic::object())("Commands", folly::dynamic::object())(
      "NativeProps", folly::dynamic::object());
}

std::string TestUIManager::getName() {
  return "UIManager";
};
std::map<std::string, folly::dynamic> TestUIManager::getConstants() {
  std::map<std::string, folly::dynamic> map;
  map["RCTView"] = GetGenericViewConfig();
  map["RCTText"] = GetGenericViewConfig();
  map["RCTRawText"] = GetGenericViewConfig();
  map["RCTScrollView"] = GetGenericViewConfig();
  return std::move(map);
}
std::vector<facebook::xplat::module::CxxModule::Method> TestUIManager::getMethods() {
  return {
      Method(
          "getConstantsForViewManager",
          [](folly::dynamic argsJson) -> folly::dynamic { return GetGenericViewConfig(); },
          SyncTag),
      Method("removeRootView", [](folly::dynamic args) {}),
      Method("createView", [](folly::dynamic args) {}),
      Method(
          "configureNextLayoutAnimation", [](folly::dynamic args, Callback cbSuccess, Callback cbError) {}, AsyncTag),
      Method("setChildren", [](folly::dynamic args) {}),
      Method("updateView", [](folly::dynamic args) {}),
      Method("removeSubviewsFromContainerWithID", [](folly::dynamic args) {}),
      Method("manageChildren", [](folly::dynamic args) {}),
      Method("replaceExistingNonRootView", [](folly::dynamic args) {}),
      Method("dispatchViewManagerCommand", [](folly::dynamic args) {}),
      Method("measure", [](folly::dynamic args, Callback cb) {}),
      Method("measureInWindow", [](folly::dynamic args, Callback cb) {}),
      Method("measureLayout", [](folly::dynamic args, Callback cbError, Callback cbSuccess) {}, AsyncTag),
      Method("findSubviewIn", [](folly::dynamic args, Callback cb) {}),
      Method("focus", [](folly::dynamic args) {}),
      Method("blur", [](folly::dynamic args) {}),
      Method("setJSResponder", [](folly::dynamic args) {}),
      Method("clearJSResponder", []() {}),
  };
}

} // namespace Microsoft::React::Test
