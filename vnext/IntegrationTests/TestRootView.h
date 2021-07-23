// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include <IReactRootView.h>

namespace Microsoft::React::Test {

class TestRootView : public facebook::react::IReactRootView {
  int64_t m_tag;
  std::string m_name;
  int64_t m_width;
  int64_t m_height;

 public:
  TestRootView(const std::string &name, int64_t width = 0, int64_t height = 0);

#pragma region IReactRootView members

  std::string JSComponentName() const noexcept override;
  int64_t GetActualHeight() const override;
  int64_t GetActualWidth() const override;
  int64_t GetTag() const override;
  void SetTag(int64_t tag) override;

#pragma endregion
};

} // namespace Microsoft::React::Test
