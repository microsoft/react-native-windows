// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.
#include "pch.h"

#include <CppUnitTest.h>

#include <Views/Image/ImageViewManager.h>
#include <Views/RawTextViewManager.h>
#include <Views/RootViewManager.h>
#include <Views/ScrollViewManager.h>
#include <Views/TextInputViewManager.h>
#include <Views/TextViewManager.h>
#include <Views/ViewViewManager.h>
#include <Views/WebViewManager.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace react::uwp;

TEST_CLASS(CreateViewManagers){
    TEST_METHOD(CreateViewManagers_Root){auto viewManager = std::make_unique<RootViewManager>(nullptr);

Assert::IsFalse(viewManager == nullptr);
}

TEST_METHOD(CreateViewManagers_View) {
  auto viewManager = std::make_unique<ViewViewManager>(nullptr);

  Assert::IsFalse(viewManager == nullptr);
}

TEST_METHOD(CreateViewManagers_Text) {
  auto viewManager = std::make_unique<TextViewManager>(nullptr);

  Assert::IsFalse(viewManager == nullptr);
}

TEST_METHOD(CreateViewManagers_TextInput) {
  auto viewManager = std::make_unique<TextInputViewManager>(nullptr);

  Assert::IsFalse(viewManager == nullptr);
}

TEST_METHOD(CreateViewManagers_Image) {
  auto viewManager = std::make_unique<ImageViewManager>(nullptr);

  Assert::IsFalse(viewManager == nullptr);
}

TEST_METHOD(CreateViewManagers_RawText) {
  auto viewManager = std::make_unique<RawTextViewManager>(nullptr);

  Assert::IsFalse(viewManager == nullptr);
}

TEST_METHOD(CreateViewManagers_Scroll) {
  auto viewManager = std::make_unique<ScrollViewManager>(nullptr);

  Assert::IsFalse(viewManager == nullptr);
}

TEST_METHOD(CreateViewManagers_Web) {
  auto viewManager = std::make_unique<WebViewManager>(nullptr);

  Assert::IsFalse(viewManager == nullptr);
}
}
;
