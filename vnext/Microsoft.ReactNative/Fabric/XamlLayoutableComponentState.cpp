// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "XamlLayoutableComponentState.h"

namespace facebook {
namespace react {

LayoutConstraints const &XamlLayoutableComponentState::getLayoutConstraints() const {
  return layoutConstraints_;
}

Size const &XamlLayoutableComponentState::getSize() const {
  return desiredSize_;
}

} // namespace react
} // namespace facebook
