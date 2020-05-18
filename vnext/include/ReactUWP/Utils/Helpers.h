// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include <IReactInstance.h>
#include <folly/dynamic.h>
#include <stdint.h>
#include <winrt/Windows.UI.Xaml.h>

namespace winrt {
using namespace Windows::UI::Xaml;
}

namespace react {
namespace uwp {

using namespace std;

struct ReactId {
  int64_t tag{0};
  bool isValid{false};
};

template <typename T>
inline typename T asEnum(folly::dynamic const &obj) {
  return static_cast<T>(obj.asInt());
}

ReactId getViewId(_In_ IReactInstance *instance, winrt::FrameworkElement const &fe);
std::int32_t CountOpenPopups();

bool IsRS3OrHigher();
bool IsRS4OrHigher();
bool IsRS5OrHigher();
bool Is19H1OrHigher();

bool IsXamlIsland();
} // namespace uwp
} // namespace react
