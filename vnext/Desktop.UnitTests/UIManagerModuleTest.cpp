// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include <CppUnitTest.h>

#include <EmptyUIManagerModule.h>
#include <ViewManager.h>

using namespace facebook::react;
using namespace Microsoft::React::Test;
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace Microsoft::React::Test {

TEST_CLASS(UIManagerModuleTests) {
 protected:
  std::unique_ptr<EmptyUIManager> m_emptyUIManager;
  std::shared_ptr<NodeRegistry> m_nodeRegistry;

  std::shared_ptr<EmptyUINode> createSimpleTextHierarchy(
      EmptyUIManager * uiManager, std::string text) {
    int64_t rootTag = 1;
    auto rootView = uiManager->addRootView(rootTag);

    int64_t textTag = rootTag + 1;
    int64_t rawTextTag = textTag + 1;

    uiManager->createView(
        textTag,
        "RCTText",
        rootTag,
        folly::dynamic::object("collapsable", false));

    uiManager->createView(
        rawTextTag,
        "RCTRawText",
        rootTag,
        folly::dynamic::object("text", text)("collapsable", false));

    // TODO: use managechildren instead of setchildren after it is implemented
    // in EmptyUIManager.
    uiManager->setChildren(textTag, folly::dynamic::array(rawTextTag));
    uiManager->setChildren(rootTag, folly::dynamic::array(textTag));

    /*uiManager->manageChildren(
    textTag,
    nullptr,
    nullptr,
    folly::dynamic::array(rawTextTag),
    folly::dynamic::array(0),
    nullptr);
    uiManager->manageChildren(
    rootTag,
    nullptr,
    nullptr,
    folly::dynamic::array(textTag),
    folly::dynamic::array(0),
    nullptr);*/

    return rootView;
  }

 public:
  UIManagerModuleTests() {
    // Native views types
    auto viewmanagers =
        std::make_unique<std::vector<std::unique_ptr<IViewManager>>>();
    // TODO: Fix linking errors.
    // viewmanagers->push_back(std::make_unique<ViewManager>()); // RCTView
    // viewmanagers->push_back(std::make_unique<StubViewManager>("RCTText"));
    // viewmanagers->push_back(std::make_unique<StubViewManager>("RCTRawText"));

    m_nodeRegistry = std::make_shared<NodeRegistry>();

    // EmptyUIManager
    m_emptyUIManager = std::make_unique<EmptyUIManager>(
        std::move(viewmanagers), m_nodeRegistry);
  }

  ~UIManagerModuleTests() {
    m_emptyUIManager = nullptr;
    m_nodeRegistry = nullptr;
  }

  // Tests taken from
  // react-native\ReactAndroid\src\test\java\com\facebook\react\uimanager\UIManagerModuleTest.java
  TEST_METHOD(UIManagerModuleTests_CreateSimpleHierarchy) {
    auto rootView =
        createSimpleTextHierarchy(m_emptyUIManager.get(), "Some Text");
    Assert::AreEqual(static_cast<size_t>(1), rootView->m_children.size());

    auto textView = m_nodeRegistry->m_allNodes[rootView->m_children[0]];
    Assert::AreEqual(std::string("RCTText"), textView->m_className);
    Assert::AreEqual(static_cast<size_t>(1), textView->m_children.size());

    auto rawTextView = m_nodeRegistry->m_allNodes[textView->m_children[0]];
    Assert::AreEqual(std::string("RCTRawText"), rawTextView->m_className);
    // auto it = rawTextView->m_props.find("text");
    // EXPECT_NE(it, rawTextView->m_props.items().end());
    // EXPECT_STREQ("Some Text", it->second.c_str());
  }
};

} // namespace Microsoft::React::Test
