
/**
 * Copyright (c) Facebook, Inc. and its affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */
#pragma once

#include <react/components/view/ViewProps.h>
#include <react/graphics/Color.h>

namespace facebook {
namespace react {

enum class ActivityIndicatorViewSize { Small, Large };

static inline void fromRawValue(const RawValue &value, ActivityIndicatorViewSize &result) {
  auto string = (std::string)value;
  if (string == "small") { result = ActivityIndicatorViewSize::Small; return; }
  if (string == "large") { result = ActivityIndicatorViewSize::Large; return; }
  abort();
}

static inline std::string toString(const ActivityIndicatorViewSize &value) {
  switch (value) {
    case ActivityIndicatorViewSize::Small: return "small";
    case ActivityIndicatorViewSize::Large: return "large";
  }
}

class ActivityIndicatorViewProps final : public ViewProps {
 public:
  ActivityIndicatorViewProps() = default;
  ActivityIndicatorViewProps(const ActivityIndicatorViewProps &sourceProps, const RawProps &rawProps);

#pragma mark - Props

  bool hidesWhenStopped{false};
  bool animating{false};
  SharedColor color{};
  ActivityIndicatorViewSize size{ActivityIndicatorViewSize::Small};
};

} // namespace react
} // namespace facebook
