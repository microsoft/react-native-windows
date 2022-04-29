// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include <react/renderer/core/LayoutConstraints.h>
#include <react/renderer/imagemanager/primitives.h>

namespace facebook {
namespace react {

/*
 * State for <TestXamlComponent> component.
 */
class XamlLayoutableComponentState final {
 public:
  XamlLayoutableComponentState(LayoutConstraints const &layoutConstraints, Size const &size)
      : layoutConstraints_(layoutConstraints), desiredSize_(size){};

  XamlLayoutableComponentState() = default;

  LayoutConstraints const &getLayoutConstraints() const;
  Size const &getSize() const;

 private:
  LayoutConstraints layoutConstraints_;
  Size desiredSize_;
};

} // namespace react
} // namespace facebook
