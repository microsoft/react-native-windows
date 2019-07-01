// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "TestRootView.h"

namespace Microsoft::React::Test {

TestRootView::TestRootView(
    const std::string &name,
    int64_t width,
    int64_t height)
    : m_name{name}, m_width{width}, m_height{height} {}

void TestRootView::ResetView() {}

std::string TestRootView::JSComponentName() const noexcept {
  return m_name;
}

int64_t TestRootView::GetActualHeight() const {
  return m_height;
}

int64_t TestRootView::GetActualWidth() const {
  return m_width;
}

int64_t TestRootView::GetTag() const {
  return m_tag;
}

void TestRootView::SetTag(int64_t tag) {
  m_tag = tag;
}

} // namespace Microsoft::React::Test
