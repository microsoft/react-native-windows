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

<<<<<<< HEAD
#pragma region TestViewManager members

TestViewManager::TestViewManager(const char *name) : m_name{name} {}

const char *TestViewManager::GetName() const {
  return m_name.c_str();
}

folly::dynamic TestViewManager::GetExportedViewConstants() const {
  return folly::dynamic();
}

folly::dynamic TestViewManager::GetCommands() const {
  return folly::dynamic();
}

folly::dynamic TestViewManager::GetNativeProps() const {
  return folly::dynamic::object("dummyprop", "string");
}

ShadowNode *TestViewManager::createShadow() const {
  return new TestShadowNode();
}

void TestViewManager::destroyShadow(ShadowNode *) const {}

folly::dynamic TestViewManager::GetConstants() const {
  folly::dynamic constants = folly::dynamic::object("Constants", GetExportedViewConstants())("Commands", GetCommands())(
      "NativeProps", GetNativeProps());

  const auto bubblingEventTypesConstants = GetExportedCustomBubblingEventTypeConstants();
  if (!bubblingEventTypesConstants.empty())
    constants["bubblingEventTypes"] = bubblingEventTypesConstants;
  const auto directEventTypesConstants = GetExportedCustomDirectEventTypeConstants();
  if (!directEventTypesConstants.empty())
    constants["directEventTypes"] = directEventTypesConstants;

  return constants;
}

folly::dynamic TestViewManager::GetExportedCustomBubblingEventTypeConstants() const {
  return folly::dynamic::object();
||||||| 811c767bf
#pragma region TestViewManager members

TestViewManager::TestViewManager(const char *name) : m_name{name} {}

const char *TestViewManager::GetName() const {
  return m_name.c_str();
}

dynamic TestViewManager::GetExportedViewConstants() const {
  return dynamic();
}

dynamic TestViewManager::GetCommands() const {
  return dynamic();
}

dynamic TestViewManager::GetNativeProps() const {
  return folly::dynamic::object("dummyprop", "string");
}

ShadowNode *TestViewManager::createShadow() const {
  return new TestShadowNode();
}

void TestViewManager::destroyShadow(ShadowNode *) const {}

dynamic TestViewManager::GetConstants() const {
  folly::dynamic constants = folly::dynamic::object("Constants", GetExportedViewConstants())("Commands", GetCommands())(
      "NativeProps", GetNativeProps());

  const auto bubblingEventTypesConstants = GetExportedCustomBubblingEventTypeConstants();
  if (!bubblingEventTypesConstants.empty())
    constants["bubblingEventTypes"] = bubblingEventTypesConstants;
  const auto directEventTypesConstants = GetExportedCustomDirectEventTypeConstants();
  if (!directEventTypesConstants.empty())
    constants["directEventTypes"] = directEventTypesConstants;

  return constants;
}

dynamic TestViewManager::GetExportedCustomBubblingEventTypeConstants() const {
  return dynamic::object();
=======
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
>>>>>>> 64b0f8706de05473456eae6340a4cbcd938baaaa
}

<<<<<<< HEAD
folly::dynamic TestViewManager::GetExportedCustomDirectEventTypeConstants() const {
  return folly::dynamic::object();
}

#pragma endregion TestViewManager members

#pragma region TestNativeUIManager members

void TestNativeUIManager::configureNextLayoutAnimation(
    folly::dynamic &&config,
    facebook::xplat::module::CxxModule::Callback success,
    facebook::xplat::module::CxxModule::Callback error) {}

void TestNativeUIManager::destroy() {}

ShadowNode *TestNativeUIManager::createRootShadowNode(IReactRootView *rootView) {
  return new TestShadowNode();
}

void TestNativeUIManager::destroyRootShadowNode(ShadowNode *node) {
  delete node;
}

void TestNativeUIManager::removeRootView(ShadowNode &rootNode) {}

void TestNativeUIManager::setHost(INativeUIManagerHost *host) {}

void TestNativeUIManager::onBatchComplete() {}

void TestNativeUIManager::ensureInBatch() {}

void TestNativeUIManager::measure(
    facebook::react::ShadowNode &shadowNode,
    facebook::react::ShadowNode &shadowRoot,
    facebook::xplat::module::CxxModule::Callback callback) {}

void TestNativeUIManager::measureInWindow(
    facebook::react::ShadowNode &shadowNode,
    facebook::xplat::module::CxxModule::Callback callback) {}

void TestNativeUIManager::measureLayout(
    facebook::react::ShadowNode &shadowNode,
    facebook::react::ShadowNode &ancestorShadowNode,
    facebook::xplat::module::CxxModule::Callback callbackFail,
    facebook::xplat::module::CxxModule::Callback callbackSuccess){};

void TestNativeUIManager::findSubviewIn(
    facebook::react::ShadowNode &shadowNode,
    float x,
    float y,
    facebook::xplat::module::CxxModule::Callback callback) {}

void TestNativeUIManager::AddRootView(
    facebook::react::ShadowNode &shadowNode,
    facebook::react::IReactRootView *pReactRootView) {}

void TestNativeUIManager::CreateView(facebook::react::ShadowNode &shadowNode, folly::dynamic /*ReadableMap*/ props) {}

void TestNativeUIManager::AddView(
    facebook::react::ShadowNode &parentShadowNode,
    facebook::react::ShadowNode &childShadowNode,
    uint64_t index) {}

void TestNativeUIManager::RemoveView(facebook::react::ShadowNode &shadowNode, bool removeChildren) {}

void TestNativeUIManager::ReplaceView(facebook::react::ShadowNode &shadowNode) {}

void TestNativeUIManager::UpdateView(facebook::react::ShadowNode &shadowNode, folly::dynamic /*ReadableMap*/ props) {}

void TestNativeUIManager::focus(int64_t reactTag) {}

void TestNativeUIManager::blur(int64_t reactTag) {}

#pragma endregion TestNativeUIManager members

#pragma region TestShadowNode members

void TestShadowNode::onDropViewInstance() {}

void TestShadowNode::removeAllChildren() {}

void TestShadowNode::AddView(ShadowNode &child, int64_t index) {}

void TestShadowNode::RemoveChildAt(int64_t indexToRemove) {}

void TestShadowNode::createView() {}

#pragma endregion

||||||| 811c767bf
dynamic TestViewManager::GetExportedCustomDirectEventTypeConstants() const {
  return dynamic::object();
}

#pragma endregion TestViewManager members

#pragma region TestNativeUIManager members

void TestNativeUIManager::configureNextLayoutAnimation(
    folly::dynamic &&config,
    facebook::xplat::module::CxxModule::Callback success,
    facebook::xplat::module::CxxModule::Callback error) {}

void TestNativeUIManager::destroy() {}

ShadowNode *TestNativeUIManager::createRootShadowNode(IReactRootView *rootView) {
  return new TestShadowNode();
}

void TestNativeUIManager::destroyRootShadowNode(ShadowNode *node) {
  delete node;
}

void TestNativeUIManager::removeRootView(ShadowNode &rootNode) {}

void TestNativeUIManager::setHost(INativeUIManagerHost *host) {}

void TestNativeUIManager::onBatchComplete() {}

void TestNativeUIManager::ensureInBatch() {}

void TestNativeUIManager::measure(
    facebook::react::ShadowNode &shadowNode,
    facebook::react::ShadowNode &shadowRoot,
    facebook::xplat::module::CxxModule::Callback callback) {}

void TestNativeUIManager::measureInWindow(
    facebook::react::ShadowNode &shadowNode,
    facebook::xplat::module::CxxModule::Callback callback) {}

void TestNativeUIManager::measureLayout(
    facebook::react::ShadowNode &shadowNode,
    facebook::react::ShadowNode &ancestorShadowNode,
    facebook::xplat::module::CxxModule::Callback callbackFail,
    facebook::xplat::module::CxxModule::Callback callbackSuccess){};

void TestNativeUIManager::findSubviewIn(
    facebook::react::ShadowNode &shadowNode,
    float x,
    float y,
    facebook::xplat::module::CxxModule::Callback callback) {}

void TestNativeUIManager::AddRootView(
    facebook::react::ShadowNode &shadowNode,
    facebook::react::IReactRootView *pReactRootView) {}

void TestNativeUIManager::CreateView(facebook::react::ShadowNode &shadowNode, folly::dynamic /*ReadableMap*/ props) {}

void TestNativeUIManager::AddView(
    facebook::react::ShadowNode &parentShadowNode,
    facebook::react::ShadowNode &childShadowNode,
    uint64_t index) {}

void TestNativeUIManager::RemoveView(facebook::react::ShadowNode &shadowNode, bool removeChildren) {}

void TestNativeUIManager::ReplaceView(facebook::react::ShadowNode &shadowNode) {}

void TestNativeUIManager::UpdateView(facebook::react::ShadowNode &shadowNode, folly::dynamic /*ReadableMap*/ props) {}

void TestNativeUIManager::focus(int64_t reactTag) {}

void TestNativeUIManager::blur(int64_t reactTag) {}

#pragma endregion TestNativeUIManager members

#pragma region TestShadowNode members

void TestShadowNode::onDropViewInstance() {}

void TestShadowNode::removeAllChildren() {}

void TestShadowNode::AddView(ShadowNode &child, int64_t index) {}

void TestShadowNode::RemoveChildAt(int64_t indexToRemove) {}

void TestShadowNode::createView() {}

#pragma endregion

=======
>>>>>>> 64b0f8706de05473456eae6340a4cbcd938baaaa
} // namespace Microsoft::React::Test
