// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include <IReactRootView.h>

namespace facebook {
namespace react {
namespace test {

class TestRootView : public facebook::react::IReactRootView
{
  int64_t m_tag;
  std::string m_name;
  int64_t m_width;
  int64_t m_height;

public:
  TestRootView(const std::string& name, int64_t width = 0, int64_t height = 0);

  #pragma region IReactRootView members

  void ResetView() override;
  std::string JSComponentName() const noexcept override;
  int64_t GetActualHeight() const override;
  int64_t GetActualWidth() const override;
  int64_t GetTag() const override;
  void SetTag(int64_t tag) override;

  #pragma endregion
};

} } } // namespace facebook::react::test
